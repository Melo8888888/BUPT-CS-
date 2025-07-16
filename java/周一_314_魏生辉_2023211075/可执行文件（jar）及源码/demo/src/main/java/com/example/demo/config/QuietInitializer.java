package com.example.demo.config;

import org.springframework.context.annotation.Configuration;
import org.springframework.core.Ordered;
import org.springframework.core.annotation.Order;

import jakarta.annotation.PostConstruct;

/**
 * 禁用初始化器控制台输出的配置类
 */
@Configuration
@Order(Ordered.HIGHEST_PRECEDENCE)
public class QuietInitializer {

    @PostConstruct
    public void init() {
        // 仅设置系统日志级别，不直接修改Hibernate属性
        System.setProperty("logging.level.org.hibernate", "OFF");
        System.setProperty("logging.level.org.hibernate.SQL", "OFF");
        System.setProperty("logging.level.org.hibernate.type", "OFF");
        System.setProperty("logging.level.org.springframework.orm", "OFF");
    }
}
