package com.example.demo.config;

import org.hibernate.resource.jdbc.spi.StatementInspector;
import org.springframework.stereotype.Component;

/**
 * Hibernate SQL拦截器，用于拦截和监控SQL执行但不输出到控制台
 */
@Component
public class HibernateSqlInterceptor implements StatementInspector {

    private static final long serialVersionUID = 1L;

    @Override
    public String inspect(String sql) {
        // 只返回原始SQL，不做额外处理
        return sql;
    }
}
