package it.ssri.tesi.webapp.api;

import it.ssri.tesi.webapp.model.FormLogin;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@Slf4j
@RestController
@RequestMapping("/api")
public class WebAppApi {

    private static final String USER_AND_PASSWORD = "tesi";

    @PostMapping(
            path = "/login",
            consumes = {MediaType.APPLICATION_FORM_URLENCODED_VALUE})
    public ResponseEntity<Void> login(FormLogin form) {
        return USER_AND_PASSWORD.equals(form.getUsername()) && USER_AND_PASSWORD.equals(form.getPassword()) ?
                ResponseEntity.status(302).header("Location", "/home").build() :
                ResponseEntity.status(401).build();
    }

}
