package com.example.demo.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
public class LoginController {

    @GetMapping("/")
    public String showLoginPage() {
        return "login"; // Corresponds to src/main/resources/templates/login.html
    }

    @GetMapping("/guest-login")
    public String guestLogin() {
        // For guest login, simply redirect to the main index page or study notes page
        return "redirect:/index"; // Redirect to the main index.html
    }

    // You can add a POST mapping for actual login authentication here later
    // @PostMapping("/login")
    // public String processLogin(@RequestParam String username, @RequestParam String password) {
    //     // Authentication logic here
    //     return "redirect:/index";
    // }
} 