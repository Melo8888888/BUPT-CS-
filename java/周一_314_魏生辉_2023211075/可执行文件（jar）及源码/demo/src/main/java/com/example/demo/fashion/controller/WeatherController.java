package com.example.demo.fashion.controller;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.client.RestClientException;
import org.springframework.web.client.RestTemplate;

@RestController
public class WeatherController {

    private static final Logger logger = LoggerFactory.getLogger(WeatherController.class);

    @Value("${amap.weather.key}")
    private String amapKey;

    @Value("${weather.api.key}")
    private String openWeatherMapKey; // 保留原API密钥作为备用

    // 城市名称到城市编码的映射（常用城市）
    private static final Map<String, String> CITY_ADCODE_MAP = new ConcurrentHashMap<>();

    static {
        // 初始化城市编码映射
        CITY_ADCODE_MAP.put("beijing", "110000"); // 北京
        CITY_ADCODE_MAP.put("shanghai", "310000"); // 上海
        CITY_ADCODE_MAP.put("guangzhou", "440100"); // 广州
        CITY_ADCODE_MAP.put("shenzhen", "440300"); // 深圳
        CITY_ADCODE_MAP.put("chengdu", "510100"); // 成都
    }

    @GetMapping("/api/weather/today")
    public ResponseEntity<Map<String, Object>> getTodayWeather(
            @RequestParam(value = "city", defaultValue = "beijing") String city) {

        logger.info("Received weather request for city: {}", city);
        RestTemplate restTemplate = new RestTemplate();

        // 转换城市名称为小写，便于查找
        String lowerCaseCity = city.toLowerCase();

        // 获取城市编码
        String cityAdcode = CITY_ADCODE_MAP.getOrDefault(lowerCaseCity, "110000"); // 默认北京

        // 尝试使用高德地图天气API
        try {
            logger.info("Fetching weather data for city with adcode: {} using AMap API", cityAdcode);
            String url = "https://restapi.amap.com/v3/weather/weatherInfo?key=" + amapKey
                    + "&city=" + cityAdcode + "&extensions=base";

            Map<String, Object> response = restTemplate.getForObject(url, Map.class);
            logger.info("AMap weather API response received: {}", response);

            if (response != null && "1".equals(response.get("status")) && response.containsKey("lives")) {
                // 处理高德地图返回的数据格式，转换为与前端兼容的格式
                Object lives = response.get("lives");
                if (lives instanceof java.util.List && !((java.util.List<?>) lives).isEmpty()) {
                    Map<String, Object> weatherData = new HashMap<>();
                    Map<String, Object> weatherInfo = (Map<String, Object>) ((java.util.List<?>) lives).get(0);

                    // 创建与OpenWeatherMap格式类似的响应
                    Map<String, Object> main = new HashMap<>();
                    main.put("temp", weatherInfo.get("temperature"));
                    weatherData.put("main", main);

                    Map<String, Object> weather = new HashMap<>();
                    weather.put("description", weatherInfo.get("weather"));
                    // 根据天气现象确定id
                    String weatherDesc = (String) weatherInfo.get("weather");
                    int weatherId = getWeatherIdByDescription(weatherDesc);
                    weather.put("id", weatherId);

                    weatherData.put("weather", new Object[]{weather});
                    weatherData.put("name", weatherInfo.get("city"));

                    logger.info("Transformed weather data: {}", weatherData);
                    return ResponseEntity.ok(weatherData);
                }
            }

            logger.warn("Invalid response from AMap API, falling back to OpenWeatherMap");
            return getWeatherFromOpenWeatherMap(city);

        } catch (Exception e) {
            logger.error("Error fetching weather data from AMap: {}", e.getMessage());
            // 如果高德API失败，尝试使用OpenWeatherMap API作为备用
            return getWeatherFromOpenWeatherMap(city);
        }
    }

    /**
     * 根据天气描述确定天气ID（与OpenWeatherMap格式兼容）
     */
    private int getWeatherIdByDescription(String description) {
        if (description == null) {
            return 800; // 默认晴
        }

        if (description.contains("晴")) {
            return 800; // 晴天
        } else if (description.contains("多云") || description.contains("阴")) {
            return 801; // 多云
        } else if (description.contains("雨")) {
            if (description.contains("雷")) {
                return 201; // 雷阵雨
            } else if (description.contains("小")) {
                return 300; // 小雨
            } else if (description.contains("中")) {
                return 301; // 中雨
            } else if (description.contains("大")) {
                return 302; // 大雨
            } else {
                return 500; // 一般雨
            }
        } else if (description.contains("雪")) {
            return 600; // 雪
        } else if (description.contains("雾") || description.contains("霾")) {
            return 701; // 雾霾
        } else {
            return 800; // 默认晴天
        }
    }

    /**
     * 从OpenWeatherMap获取天气数据（备用方法）
     */
    private ResponseEntity<Map<String, Object>> getWeatherFromOpenWeatherMap(String city) {
        logger.info("Fetching weather data for city: {} with OpenWeatherMap API", city);
        RestTemplate restTemplate = new RestTemplate();

        try {
            String url = "https://api.openweathermap.org/data/2.5/weather?q=" + city
                    + "&appid=" + openWeatherMapKey + "&units=metric&lang=zh_cn";
            Map<String, Object> weatherData = restTemplate.getForObject(url, Map.class);
            logger.info("Weather data retrieved successfully from OpenWeatherMap");
            return ResponseEntity.ok(weatherData);
        } catch (RestClientException e) {
            logger.error("Error fetching weather data from OpenWeatherMap: {}", e.getMessage());

            // 返回错误响应
            Map<String, Object> errorResponse = new HashMap<>();
            errorResponse.put("error", "无法获取天气数据");
            errorResponse.put("message", "天气API服务暂时不可用，请稍后再试");

            return ResponseEntity.status(503).body(errorResponse);
        }
    }
}
