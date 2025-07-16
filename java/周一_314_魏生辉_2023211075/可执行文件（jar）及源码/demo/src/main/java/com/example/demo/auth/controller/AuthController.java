package com.example.demo.auth.controller;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;

@Controller
public class AuthController {

    // 移除/login映射，因为LoginController已经处理了根路径"/"并返回login视图
    // 如果需要单独的登录页面路由，可以使用不同的路径
    
    @GetMapping("/auth/register")
    public String register() {
        return "auth/register";
    }
    
    @GetMapping("/auth/forgot-password")
    public String forgotPassword() {
        return "auth/forgot-password";
    }
    
    @GetMapping("/auth/reset-password")
    public String resetPassword(@RequestParam(required = false) String token, Model model) {
        // 在实际应用中，这里应该验证token的有效性
        model.addAttribute("token", token);
        return "auth/reset-password";
    }
    
    // 移除/guest-login映射，因为LoginController已经处理了这个路径
} 