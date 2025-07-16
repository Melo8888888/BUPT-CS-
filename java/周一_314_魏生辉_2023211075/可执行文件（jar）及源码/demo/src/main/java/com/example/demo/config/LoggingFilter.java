package com.example.demo.config;

import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Configuration;
import org.springframework.core.Ordered;
import org.springframework.core.annotation.Order;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.Logger;
import ch.qos.logback.classic.LoggerContext;
import jakarta.annotation.PostConstruct;

/**
 * 禁用Hibernate SQL日志的配置类
 */
@Configuration
@Order(Ordered.HIGHEST_PRECEDENCE)
public class LoggingFilter {

    @PostConstruct
    public void init() {
        // 配置Logback日志
        try {
            // 获取Logback日志上下文
            LoggerContext loggerContext = (LoggerContext) LoggerFactory.getILoggerFactory();

            // 设置Hibernate及相关日志级别为OFF
            String[] loggersToDisable = {
                // Hibernate核心日志
                "org.hibernate",
                "org.hibernate.SQL",
                "org.hibernate.type",
                "org.hibernate.type.descriptor.sql",
                "org.hibernate.type.descriptor.sql.BasicBinder",
                "org.hibernate.orm.jdbc.bind",
                "org.hibernate.engine.jdbc",
                "org.hibernate.engine.transaction",
                "org.hibernate.stat",
                "org.hibernate.event",
                // Spring数据库相关日志
                "org.springframework.orm",
                "org.springframework.jdbc",
                "org.springframework.transaction",
                "org.springframework.orm.jpa",
                "org.springframework.orm.jpa.JpaTransactionManager"
            };

            // 禁用所有指定的日志记录器
            for (String loggerName : loggersToDisable) {
                Logger logger = loggerContext.getLogger(loggerName);
                if (logger != null) {
                    logger.setLevel(Level.OFF);
                    logger.setAdditive(false);
                }
            }

            // 设置系统属性，进一步禁用Hibernate日志
            System.setProperty("org.hibernate.SQL.level", "OFF");
            System.setProperty("org.hibernate.type.descriptor.sql.BasicBinder.level", "OFF");
            System.setProperty("hibernate.show_sql", "false");
            System.setProperty("spring.jpa.show-sql", "false");

        } catch (Exception e) {
            // 忽略异常
            System.err.println("无法配置日志: " + e.getMessage());
        }
    }
}
