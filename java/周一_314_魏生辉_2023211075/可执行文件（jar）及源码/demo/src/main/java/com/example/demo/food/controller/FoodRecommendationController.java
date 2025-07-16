package com.example.demo.food.controller;

import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.example.demo.food.model.FoodRecommendation;
import com.example.demo.food.service.FoodRecommendationService;

@RestController
@RequestMapping("/api/food-recommendations")
public class FoodRecommendationController {

    @Autowired
    private FoodRecommendationService foodService;

    @GetMapping
    public ResponseEntity<List<FoodRecommendation>> getAllRecommendations() {
        List<FoodRecommendation> foodList = foodService.getAllRecommendations();
        System.out.println("API调用: 获取所有美食推荐, 返回 " + foodList.size() + " 条记录");
        return ResponseEntity.ok(foodList);
    }

    @GetMapping("/{id}")
    public ResponseEntity<FoodRecommendation> getRecommendationById(@PathVariable Long id) {
        System.out.println("API调用: 获取ID为 " + id + " 的美食推荐");
        return foodService.getRecommendationById(id)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    @GetMapping("/cuisine/{cuisine}")
    public ResponseEntity<List<FoodRecommendation>> getRecommendationsByCuisine(@PathVariable String cuisine) {
        List<FoodRecommendation> foodList = foodService.getRecommendationsByCuisine(cuisine);
        System.out.println("API调用: 获取菜系为 " + cuisine + " 的美食推荐, 返回 " + foodList.size() + " 条记录");
        return ResponseEntity.ok(foodList);
    }

    @GetMapping("/search")
    public ResponseEntity<List<FoodRecommendation>> searchRecommendations(@RequestParam String name) {
        List<FoodRecommendation> foodList = foodService.searchRecommendationsByName(name);
        System.out.println("API调用: 搜索名称包含 '" + name + "' 的美食推荐, 返回 " + foodList.size() + " 条记录");
        return ResponseEntity.ok(foodList);
    }

    @GetMapping("/rating")
    public ResponseEntity<List<FoodRecommendation>> getRecommendationsByMinRating(@RequestParam Double minRating) {
        List<FoodRecommendation> foodList = foodService.getRecommendationsByMinRating(minRating);
        System.out.println("API调用: 获取评分大于等于 " + minRating + " 的美食推荐, 返回 " + foodList.size() + " 条记录");
        return ResponseEntity.ok(foodList);
    }

    @GetMapping("/popular")
    public ResponseEntity<List<FoodRecommendation>> getMostPopularRecommendations(
            @RequestParam(defaultValue = "5") int limit) {
        List<FoodRecommendation> foodList = foodService.getMostPopularRecommendations(limit);
        System.out.println("API调用: 获取最受欢迎的 " + limit + " 条美食推荐, 返回 " + foodList.size() + " 条记录");
        return ResponseEntity.ok(foodList);
    }

    @GetMapping("/top-rated")
    public ResponseEntity<List<FoodRecommendation>> getHighestRatedRecommendations(
            @RequestParam(defaultValue = "5") int limit) {
        List<FoodRecommendation> foodList = foodService.getHighestRatedRecommendations(limit);
        System.out.println("API调用: 获取评分最高的 " + limit + " 条美食推荐, 返回 " + foodList.size() + " 条记录");
        return ResponseEntity.ok(foodList);
    }

    @PostMapping("/{id}/view")
    public ResponseEntity<FoodRecommendation> incrementViewCount(@PathVariable Long id) {
        System.out.println("API调用: 增加ID为 " + id + " 的美食推荐浏览次数");
        return foodService.incrementViewCount(id)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    @PostMapping
    public ResponseEntity<FoodRecommendation> createRecommendation(@RequestBody FoodRecommendation recommendation) {
        FoodRecommendation savedFood = foodService.createRecommendation(recommendation);
        System.out.println("API调用: 创建新的美食推荐 '" + savedFood.getName() + "', ID为 " + savedFood.getId());
        return new ResponseEntity<>(savedFood, HttpStatus.CREATED);
    }

    @PutMapping("/{id}")
    public ResponseEntity<FoodRecommendation> updateRecommendation(@PathVariable Long id, @RequestBody FoodRecommendation recommendationDetails) {
        System.out.println("API调用: 更新ID为 " + id + " 的美食推荐");
        return foodService.updateRecommendation(id, recommendationDetails)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteRecommendation(@PathVariable Long id) {
        System.out.println("API调用: 删除ID为 " + id + " 的美食推荐");
        return foodService.deleteRecommendation(id)
                ? ResponseEntity.noContent().build()
                : ResponseEntity.notFound().build();
    }

    @GetMapping("/test")
    public ResponseEntity<String> testEndpoint() {
        List<FoodRecommendation> allFoods = foodService.getAllRecommendations();
        StringBuilder report = new StringBuilder();

        report.append("数据库中共有 ").append(allFoods.size()).append(" 条美食数据\n\n");

        if (!allFoods.isEmpty()) {
            report.append("第一条数据示例：\n");
            FoodRecommendation sample = allFoods.get(0);
            report.append("ID: ").append(sample.getId()).append("\n");
            report.append("名称: ").append(sample.getName()).append("\n");
            report.append("菜系: ").append(sample.getCuisine()).append("\n");
            report.append("评分: ").append(sample.getRating()).append("\n");
            report.append("浏览次数: ").append(sample.getViewCount()).append("\n");
        } else {
            report.append("警告：数据库中没有美食数据！");
        }

        // 按菜系统计
        Map<String, Long> cuisineCount = allFoods.stream()
                .collect(Collectors.groupingBy(FoodRecommendation::getCuisine, Collectors.counting()));

        report.append("\n\n按菜系统计：\n");
        cuisineCount.forEach((cuisine, count) -> {
            report.append(cuisine).append(": ").append(count).append(" 条\n");
        });

        return ResponseEntity.ok(report.toString());
    }
}
