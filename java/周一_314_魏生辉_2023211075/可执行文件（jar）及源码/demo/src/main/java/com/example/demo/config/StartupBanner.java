package com.example.demo.config;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.ApplicationArguments;
import org.springframework.boot.ApplicationRunner;
import org.springframework.stereotype.Component;

/**
 * 应用启动后显示美观的启动信息
 */
@Component
public class StartupBanner implements ApplicationRunner {

    @Value("${app.server.port:8080}")
    private String serverPort;

    @Value("${app.server.address:127.0.0.1}")
    private String serverAddress;

    @Value("${app.server.context-path:/}")
    private String contextPath;

    @Value("${app.startup.show-access-url:true}")
    private boolean showAccessUrl;

    @Override
    public void run(ApplicationArguments args) throws Exception {
        if (showAccessUrl) {
            // 清除控制台
            clearConsole();

            // 处理上下文路径格式
            String path = contextPath;
            if (!path.startsWith("/")) {
                path = "/" + path;
            }
            if (path.equals("/")) {
                path = "";
            }

            // 构建应用基础URL
            String baseUrl = "http://" + serverAddress + ":" + serverPort + path;

            // 显示应用启动信息
            printBanner(baseUrl);
        }
    }

    /**
     * 清除控制台
     */
    private void clearConsole() {
        try {
            // 多输出几行空行，简单清屏
            for (int i = 0; i < 50; i++) {
                System.out.println();
            }
        } catch (Exception e) {
            // 忽略异常
        }
    }

    /**
     * 打印启动信息
     */
    private void printBanner(String baseUrl) {
        System.out.println("============================================");
        System.out.println("      邮驿四方应用启动成功");
        System.out.println("============================================");
        System.out.println("- 本地访问: " + baseUrl + "/");
        System.out.println("- 端口号: " + serverPort);
        System.out.println("- 项目路径: " + contextPath);
        System.out.println("============================================");
        System.out.println("- 裁云作裳-风尚驿程: " + baseUrl + "/fashion-outfits");
        System.out.println("- 读书万卷-文思驿站: " + baseUrl + "/study-notes");
        System.out.println("- 寻味九州-风味驿馆: " + baseUrl + "/food-recommendations");
        System.out.println("- 行者无疆-山河驿路: " + baseUrl + "/travel-itineraries");
        System.out.println("============================================");
        System.out.println("代码量——5000+");
        System.out.println("内容丰富度——html数量12个，可交互界面30+");
    }
}
