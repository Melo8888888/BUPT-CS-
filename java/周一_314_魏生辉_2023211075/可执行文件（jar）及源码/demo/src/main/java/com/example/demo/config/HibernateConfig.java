package com.example.demo.config;

import org.springframework.boot.autoconfigure.orm.jpa.HibernatePropertiesCustomizer;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

/**
 * Hibernate配置类，用于自定义Hibernate属性
 */
@Configuration
public class HibernateConfig {

    /**
     * 提供一个HibernatePropertiesCustomizer bean，用于配置Hibernate属性
     * 这种方式比直接在application.properties中设置属性更安全
     */
    @Bean
    public HibernatePropertiesCustomizer hibernatePropertiesCustomizer() {
        return hibernateProperties -> {
            // 禁用SQL日志 - 使用布尔值而不是字符串
            hibernateProperties.put("hibernate.show_sql", Boolean.FALSE);
            hibernateProperties.put("hibernate.format_sql", Boolean.FALSE);
            hibernateProperties.put("hibernate.use_sql_comments", Boolean.FALSE);

            // 禁用统计收集
            hibernateProperties.put("hibernate.generate_statistics", Boolean.FALSE);

            // 设置SQL语句日志参数（解决NumberFormatException）
            hibernateProperties.put("hibernate.log_slow_query", "-1");

            // 设置语句检查器
            hibernateProperties.put("hibernate.session.factory.statement_inspector",
                    "com.example.demo.config.HibernateSqlInterceptor");

            // 安全地设置引号标识符
            hibernateProperties.put("hibernate.globally_quoted_identifiers", Boolean.TRUE);
        };
    }
}
