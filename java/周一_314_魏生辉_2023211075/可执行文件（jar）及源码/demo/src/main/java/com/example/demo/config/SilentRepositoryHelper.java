package com.example.demo.config;

import java.util.Collection;
import java.util.function.Supplier;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.data.repository.CrudRepository;
import org.springframework.stereotype.Component;

/**
 * 无日志输出的仓库操作辅助工具 用于在数据初始化过程中禁止SQL日志输出
 */
@Component
public class SilentRepositoryHelper {

    private static final Logger logger = LoggerFactory.getLogger(SilentRepositoryHelper.class);

    /**
     * 静默保存单个实体
     *
     * @param <T> 实体类型
     * @param <R> 仓库类型
     * @param repository 仓库实例
     * @param entity 待保存实体
     * @return 保存后的实体
     */
    public <T, R extends CrudRepository<T, ?>> T saveSilently(R repository, T entity) {
        return executeSilently(() -> repository.save(entity));
    }

    /**
     * 静默保存多个实体
     *
     * @param <T> 实体类型
     * @param <R> 仓库类型
     * @param repository 仓库实例
     * @param entities 待保存实体集合
     * @return 保存后的实体集合
     */
    public <T, R extends CrudRepository<T, ?>> Iterable<T> saveAllSilently(R repository, Collection<T> entities) {
        return executeSilently(() -> repository.saveAll(entities));
    }

    /**
     * 静默执行任意仓库操作
     *
     * @param <T> 返回值类型
     * @param operation 要执行的操作
     * @return 操作结果
     */
    public <T> T executeSilently(Supplier<T> operation) {
        // 临时保存日志级别
        String previousLevel = System.getProperty("logging.level.org.hibernate.SQL");

        try {
            // 设置临时系统属性禁用SQL日志
            System.setProperty("logging.level.org.hibernate.SQL", "OFF");
            System.setProperty("spring.jpa.show-sql", "false");
            System.setProperty("hibernate.show_sql", "false");

            // 执行操作
            return operation.get();
        } finally {
            // 恢复之前的日志级别
            if (previousLevel != null) {
                System.setProperty("logging.level.org.hibernate.SQL", previousLevel);
            } else {
                System.clearProperty("logging.level.org.hibernate.SQL");
            }
        }
    }
}
