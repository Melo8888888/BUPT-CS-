package com.example.demo.travel.controller;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.client.RestTemplate;

import com.example.demo.travel.model.TravelItinerary;
import com.example.demo.travel.service.TravelItineraryService;

@RestController
@RequestMapping("/api/travel")
public class TravelApiController {

    @Autowired
    private TravelItineraryService travelService;

    private final RestTemplate restTemplate = new RestTemplate();

    @Value("${amap.web.key}")
    private String AMAP_WEB_KEY;

    @Value("${amap.web.security-code}")
    private String AMAP_SECURITY_CODE;

    @Value("${amap.route.key}")
    private String AMAP_ROUTE_KEY;

    @GetMapping("/itineraries")
    public List<TravelItinerary> getAllItineraries() {
        return travelService.getAllItineraries();
    }

    @GetMapping("/walking-route")
    public ResponseEntity<Object> getWalkingRoute(
            @RequestParam String origin,
            @RequestParam String destination) {

        try {
            String url = String.format(
                    "https://restapi.amap.com/v3/direction/walking?key=%s&origin=%s&destination=%s&extensions=all",
                    AMAP_ROUTE_KEY, origin, destination
            );

            System.out.println("请求高德步行路线API: " + url);

            // 发送请求到高德API并返回结果
            Object response = restTemplate.getForObject(url, Object.class);

            return ResponseEntity.ok(response);
        } catch (Exception e) {
            System.err.println("获取步行路线错误: " + e.getMessage());
            e.printStackTrace();
            return ResponseEntity.status(500).body("{\"status\":\"0\",\"info\":\"服务器内部错误: " + e.getMessage() + "\"}");
        }
    }

    @GetMapping("/driving-route")
    public ResponseEntity<Object> getDrivingRoute(
            @RequestParam String origin,
            @RequestParam String destination) {

        try {
            String url = String.format(
                    "https://restapi.amap.com/v3/direction/driving?key=%s&origin=%s&destination=%s&extensions=all",
                    AMAP_ROUTE_KEY, origin, destination
            );

            System.out.println("请求高德驾车路线API: " + url);

            // 发送请求到高德API并返回结果
            Object response = restTemplate.getForObject(url, Object.class);

            return ResponseEntity.ok(response);
        } catch (Exception e) {
            System.err.println("获取驾车路线错误: " + e.getMessage());
            e.printStackTrace();
            return ResponseEntity.status(500).body("{\"status\":\"0\",\"info\":\"服务器内部错误: " + e.getMessage() + "\"}");
        }
    }

    @GetMapping("/transit-route")
    public ResponseEntity<Object> getTransitRoute(
            @RequestParam String origin,
            @RequestParam String destination,
            @RequestParam(required = false, defaultValue = "北京") String city) {

        try {
            String url = String.format(
                    "https://restapi.amap.com/v3/direction/transit/integrated?key=%s&origin=%s&destination=%s&city=%s&extensions=all",
                    AMAP_ROUTE_KEY, origin, destination, city
            );

            System.out.println("请求高德公交路线API: " + url);

            // 发送请求到高德API并返回结果
            Object response = restTemplate.getForObject(url, Object.class);

            return ResponseEntity.ok(response);
        } catch (Exception e) {
            System.err.println("获取公交路线错误: " + e.getMessage());
            e.printStackTrace();
            return ResponseEntity.status(500).body("{\"status\":\"0\",\"info\":\"服务器内部错误: " + e.getMessage() + "\"}");
        }
    }

    @GetMapping("/bicycling-route")
    public ResponseEntity<Object> getBicyclingRoute(
            @RequestParam String origin,
            @RequestParam String destination) {

        try {
            String url = String.format(
                    "https://restapi.amap.com/v4/direction/bicycling?key=%s&origin=%s&destination=%s",
                    AMAP_ROUTE_KEY, origin, destination
            );

            System.out.println("请求高德骑行路线API: " + url);

            // 发送请求到高德API并返回结果
            Object response = restTemplate.getForObject(url, Object.class);

            return ResponseEntity.ok(response);
        } catch (Exception e) {
            System.err.println("获取骑行路线错误: " + e.getMessage());
            e.printStackTrace();
            return ResponseEntity.status(500).body("{\"status\":\"0\",\"info\":\"服务器内部错误: " + e.getMessage() + "\"}");
        }
    }

    @GetMapping("/geocode")
    public ResponseEntity<Object> geocodeAddress(
            @RequestParam String address) {

        try {
            String url = String.format(
                    "https://restapi.amap.com/v3/geocode/geo?key=%s&address=%s&city=北京",
                    AMAP_ROUTE_KEY, address
            );

            System.out.println("请求高德地理编码API: " + url);

            // 发送请求到高德API并返回结果
            Object response = restTemplate.getForObject(url, Object.class);

            return ResponseEntity.ok(response);
        } catch (Exception e) {
            System.err.println("地理编码错误: " + e.getMessage());
            return ResponseEntity.status(500).body("{\"status\":\"0\",\"info\":\"服务器内部错误\"}");
        }
    }

    @GetMapping("/test-key")
    public ResponseEntity<Object> testApiKey() {
        try {
            // 测试两个不同的API密钥
            String webKeyUrl = String.format(
                    "https://restapi.amap.com/v3/ip?key=%s&ip=114.247.50.2",
                    AMAP_WEB_KEY
            );

            String routeKeyUrl = String.format(
                    "https://restapi.amap.com/v3/ip?key=%s&ip=114.247.50.2",
                    AMAP_ROUTE_KEY
            );

            System.out.println("测试Web API密钥: " + webKeyUrl);
            System.out.println("测试Route API密钥: " + routeKeyUrl);

            // 发送请求到高德API并返回结果
            Object webResponse = restTemplate.getForObject(webKeyUrl, Object.class);
            Object routeResponse = restTemplate.getForObject(routeKeyUrl, Object.class);

            // 返回结果
            return ResponseEntity.ok(
                    Map.of(
                            "message", "API密钥测试",
                            "web_key", AMAP_WEB_KEY,
                            "web_security_code", AMAP_SECURITY_CODE,
                            "web_response", webResponse,
                            "route_key", AMAP_ROUTE_KEY,
                            "route_response", routeResponse
                    )
            );
        } catch (Exception e) {
            System.err.println("测试API密钥错误: " + e.getMessage());
            e.printStackTrace();
            return ResponseEntity.status(500).body(Map.of(
                    "error", e.getMessage(),
                    "web_key", AMAP_WEB_KEY,
                    "route_key", AMAP_ROUTE_KEY
            ));
        }
    }
}
