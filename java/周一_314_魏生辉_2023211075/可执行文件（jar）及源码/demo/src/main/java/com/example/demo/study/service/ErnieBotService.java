package com.example.demo.study.service;

import java.util.HashMap;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;

import com.example.demo.study.config.ErnieBotConfig;
import com.fasterxml.jackson.databind.ObjectMapper;

@Service
public class ErnieBotService {

    @Autowired
    private ErnieBotConfig ernieBotConfig;
    
    @Autowired
    private RestTemplate restTemplate;
    
    @Autowired
    private ObjectMapper objectMapper;

    public String getStudyAssistantResponse(String userQuery) {
        String apiUrl = ernieBotConfig.getApiUrl();
        
        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_JSON);
        headers.set("Authorization", "Bearer " + ernieBotConfig.getApiKey());
        
        Map<String, Object> requestBody = new HashMap<>();
        requestBody.put("messages", new Object[]{
            Map.of("role", "user", "content", userQuery)
        });
        
        HttpEntity<Map<String, Object>> requestEntity = new HttpEntity<>(requestBody, headers);
        
        try {
            ResponseEntity<Map> response = restTemplate.exchange(
                apiUrl,
                HttpMethod.POST,
                requestEntity,
                Map.class
            );
            
            if (response.getStatusCode() == HttpStatus.OK && response.getBody() != null) {
                Map<String, Object> responseBody = response.getBody();
                
                // Handle the response based on ERNIE Bot API structure
                if (responseBody.containsKey("result")) {
                    return responseBody.get("result").toString();
                } else if (responseBody.containsKey("response")) {
                    return responseBody.get("response").toString();
                } else if (responseBody.containsKey("content")) {
                    return responseBody.get("content").toString();
                } else {
                    // Log the response structure for debugging
                    System.out.println("Unexpected response structure: " + responseBody);
                    return "抱歉，我无法理解API的响应格式。";
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
            return "抱歉，我暂时无法回答这个问题。请稍后再试。错误信息: " + e.getMessage();
        }
        
        return "抱歉，我暂时无法回答这个问题。请稍后再试。";
    }
} 