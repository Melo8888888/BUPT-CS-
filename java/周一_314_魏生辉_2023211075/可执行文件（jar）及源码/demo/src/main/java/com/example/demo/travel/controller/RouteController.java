package com.example.demo.travel.controller;

import java.util.ArrayList;
import java.util.List;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.example.demo.travel.model.Route;
import com.example.demo.travel.model.Route.RouteStep;

@RestController
@RequestMapping("/api/routes")
public class RouteController {

    // 北邮坐标位置常量
    private static final String DEFAULT_ORIGIN = "北京邮电大学(西土城路校区)";

    /**
     * 获取路线规划信息，默认以北邮为起点
     *
     * @param origin 起点地址，如果为空则默认为北邮
     * @param destination 目的地地址，必填
     * @param type 交通方式：driving（驾车）, walking（步行）, transit（公交）, bicycling（骑行）
     * @return 路线信息
     */
    @GetMapping("/directions")
    public ResponseEntity<Route> getDirections(
            @RequestParam(required = false) String origin,
            @RequestParam String destination,
            @RequestParam(defaultValue = "walking") String type) {

        // 如果未提供起点，使用默认起点（北邮）
        String startPoint = (origin == null || origin.isEmpty()) ? DEFAULT_ORIGIN : origin;

        // 在实际应用中，这里应该调用高德地图API获取真实路线数据
        // 现在我们返回模拟数据用于前端测试
        Route route = generateMockRoute(startPoint, destination, type);

        return ResponseEntity.ok(route);
    }

    /**
     * 生成模拟路线数据用于测试
     */
    private Route generateMockRoute(String origin, String destination, String type) {
        List<RouteStep> steps = new ArrayList<>();
        Double totalDistance = 0.0;
        Integer totalDuration = 0;

        // 根据不同交通方式生成不同的路线步骤
        switch (type) {
            case "driving":
                steps.add(new RouteStep("从" + origin + "出发", 0.0, 0, ""));
                steps.add(new RouteStep("沿西土城路向南行驶", 800.0, 180, "西土城路"));
                steps.add(new RouteStep("右转进入学院路", 1200.0, 300, "学院路"));
                steps.add(new RouteStep("左转进入中关村北大街", 1500.0, 360, "中关村北大街"));
                steps.add(new RouteStep("到达" + destination, 0.0, 0, ""));

                totalDistance = 3500.0;
                totalDuration = 840;
                break;

            case "walking":
                steps.add(new RouteStep("从" + origin + "出发", 0.0, 0, ""));
                steps.add(new RouteStep("向西步行至西土城路", 300.0, 240, "校内道路"));
                steps.add(new RouteStep("沿西土城路向南步行", 1000.0, 720, "西土城路"));
                steps.add(new RouteStep("左转进入学院路", 800.0, 600, "学院路"));
                steps.add(new RouteStep("到达" + destination, 0.0, 0, ""));

                totalDistance = 2100.0;
                totalDuration = 1560;
                break;

            case "transit":
                steps.add(new RouteStep("从" + origin + "出发", 0.0, 0, ""));
                steps.add(new RouteStep("步行至西土城路北站", 300.0, 240, "校内道路"));
                steps.add(new RouteStep("乘坐718路公交车", 3000.0, 600, "718路"));
                steps.add(new RouteStep("在中关村站下车", 0.0, 60, ""));
                steps.add(new RouteStep("步行至" + destination, 500.0, 360, "中关村大街"));

                totalDistance = 3800.0;
                totalDuration = 1260;
                break;

            case "bicycling":
                steps.add(new RouteStep("从" + origin + "出发", 0.0, 0, ""));
                steps.add(new RouteStep("骑行至西土城路", 200.0, 60, "校内道路"));
                steps.add(new RouteStep("沿西土城路向南骑行", 1000.0, 300, "西土城路"));
                steps.add(new RouteStep("右转进入学院路", 1200.0, 360, "学院路"));
                steps.add(new RouteStep("到达" + destination, 0.0, 0, ""));

                totalDistance = 2400.0;
                totalDuration = 720;
                break;
        }

        return new Route(origin, destination, type, totalDistance, totalDuration, steps);
    }
}
