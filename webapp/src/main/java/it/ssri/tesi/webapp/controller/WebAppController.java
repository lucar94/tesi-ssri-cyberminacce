package it.ssri.tesi.webapp.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
public class WebAppController {

    @GetMapping(path = "/home")
    public String loginSuccess() {
        return "home";
    }

}
