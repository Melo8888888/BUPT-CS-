package com.example.demo.config;

import java.util.regex.Pattern;

import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.filter.Filter;
import ch.qos.logback.core.spi.FilterReply;

/**
 * 过滤SQL语句的日志过滤器
 */
public class SqlPatternLogFilter extends Filter<ILoggingEvent> {

    private static final Pattern SQL_PATTERN = Pattern.compile(
            ".*(insert|update|delete|select|create|alter|drop).*",
            Pattern.CASE_INSENSITIVE);

    private static final Pattern HIBERNATE_PATTERN = Pattern.compile(
            ".*(Hibernate|HHH).*",
            Pattern.CASE_INSENSITIVE);

    @Override
    public FilterReply decide(ILoggingEvent event) {
        String loggerName = event.getLoggerName();
        String message = event.getMessage();

        // 过滤Hibernate日志
        if (loggerName != null && (loggerName.startsWith("org.hibernate")
                || loggerName.startsWith("org.springframework.orm")
                || loggerName.startsWith("org.springframework.jdbc")
                || loggerName.startsWith("org.springframework.transaction"))) {
            return FilterReply.DENY;
        }

        // 过滤SQL语句和Hibernate相关信息
        if (message != null && (SQL_PATTERN.matcher(message).matches()
                || HIBERNATE_PATTERN.matcher(message).matches())) {
            return FilterReply.DENY;
        }

        return FilterReply.NEUTRAL;
    }
}
