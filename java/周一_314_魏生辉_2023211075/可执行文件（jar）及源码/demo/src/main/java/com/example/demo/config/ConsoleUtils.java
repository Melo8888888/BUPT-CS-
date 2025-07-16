package com.example.demo.config;

import org.springframework.stereotype.Component;

/**
 * 控制台操作工具类
 */
@Component
public class ConsoleUtils {

    /**
     * 清除控制台内容
     */
    public static void clearConsole() {
        try {
            // 针对不同操作系统
            if (System.getProperty("os.name").toLowerCase().contains("win")) {
                // Windows系统
                new ProcessBuilder("cmd", "/c", "cls").inheritIO().start().waitFor();
            } else {
                // Linux/Unix/Mac系统
                System.out.print("\033[H\033[2J");
                System.out.flush();
            }
        } catch (Exception e) {
            // 如果清除失败，不影响程序正常运行
            System.out.println("\n\n\n");  // 至少输出几个空行
        }
    }

    /**
     * 显示应用程序启动成功信息
     *
     * @param baseUrl 应用基础URL
     * @param port 应用端口
     * @param contextPath 应用上下文路径
     */
    public static void printStartupBanner(String baseUrl, String port, String contextPath) {
        System.out.println();
        System.out.println("============================================");
        System.out.println("      邮驿四方应用启动成功");
        System.out.println("      邮驿四方——衣食学行全指南");
        System.out.println("============================================");
        System.out.println("- 本地访问: " + baseUrl + "/");
        System.out.println("- 端口号: " + port);
        System.out.println("- 项目路径: " + contextPath);
        System.out.println("============================================");
        System.out.println("- 衣——穿搭推荐页面: " + baseUrl + "/fashion-outfits");
        System.out.println("- 食——美食推荐页面: " + baseUrl + "/food-recommendations");
        System.out.println("- 学——学习笔记页面: " + baseUrl + "/study-notes");
        System.out.println("- 行——行程推荐页面: " + baseUrl + "/travel-itineraries");
        System.out.println("============================================");
        System.out.println();
    }
}
