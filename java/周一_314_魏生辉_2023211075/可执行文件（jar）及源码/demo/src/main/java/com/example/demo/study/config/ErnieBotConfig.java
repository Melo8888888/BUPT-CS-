package com.example.demo.study.config;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;

@Configuration
public class ErnieBotConfig {
    
    @Value("${ernie.bot.api-key}")
    private String apiKey;
    
    @Value("${ernie.bot.secret-key}")
    private String secretKey;
    
    @Value("${ernie.bot.api-url}")
    private String apiUrl;
    
    public String getApiKey() {
        return apiKey;
    }
    
    public String getSecretKey() {
        return secretKey;
    }
    
    public String getApiUrl() {
        return apiUrl;
    }
} 