package com.example.demo.food.config;

import java.time.LocalDateTime;
import java.util.Arrays;
import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import com.example.demo.food.model.FoodRecommendation;
import com.example.demo.food.repository.FoodRecommendationRepository;

@Configuration
public class FoodDataInitializer {

    @Autowired
    private FoodRecommendationRepository foodRepository;

    @Bean
    public CommandLineRunner initFoodData() {
        return args -> {
            // 如果已经有数据，则不再初始化
            if (foodRepository.count() > 0) {
                return;
            }

            // 准备中餐数据
            List<FoodRecommendation> chineseFoods = Arrays.asList(
                    createFood("红烧肉", "经典东坡肉，肥而不腻，入口即化", "chinese",
                            "上海市黄浦区南京东路123号",
                            "https://img.zcool.cn/community/01f5ac58b4db2fa801219c77b9a106.jpg", 4.8),
                    createFood("小笼包", "鲜汁四溢的上海经典小吃", "chinese",
                            "南京路步行街附近",
                            "https://img.zcool.cn/community/01639e58afb2c3a801219c77e22fbb.jpg", 4.7),
                    createFood("北京烤鸭", "皮酥肉嫩，香气四溢的北京传统名菜", "chinese",
                            "北京市东城区王府井大街88号",
                            "https://img.zcool.cn/community/01f9aa5cd5675ba801214168ea6c5c.jpg", 4.9),
                    createFood("麻婆豆腐", "麻辣鲜香的川菜经典", "chinese",
                            "成都市锦江区春熙路99号",
                            "https://img.zcool.cn/community/0138e45cd5681ea801214168289c9c.jpg", 4.6),
                    createFood("宫保鸡丁", "甜辣酸香，口感丰富的传统菜肴", "chinese",
                            "重庆市渝中区解放碑67号",
                            "https://img.zcool.cn/community/01d5855e1147a9a80120a895c55a47.jpg", 4.5)
            );

            // 准备日餐数据
            List<FoodRecommendation> japaneseFoods = Arrays.asList(
                    createFood("寿司拼盘", "新鲜食材，精美摆盘，口感丰富", "japanese",
                            "上海市长宁区虹桥路1665号",
                            "https://img.zcool.cn/community/01db4558afab05a801219c77b0a9e3.jpg", 4.7),
                    createFood("拉面", "汤头浓郁，面条筋道的日式拉面", "japanese",
                            "北京市朝阳区三里屯太古里",
                            "https://img.zcool.cn/community/01c3b25cd567b2a8012141688d61d6.jpg", 4.6),
                    createFood("天妇罗", "外酥里嫩，口感轻盈的日式炸物", "japanese",
                            "广州市天河区天河路385号",
                            "https://img.zcool.cn/community/016de55cd567aca801214168afbd91.jpg", 4.5)
            );

            // 准备韩餐数据
            List<FoodRecommendation> koreanFoods = Arrays.asList(
                    createFood("韩式烤肉", "现点现烤，肉质鲜嫩多汁", "korean",
                            "上海市徐汇区衡山路45号",
                            "https://img.zcool.cn/community/01365e5cd567dba801214168c5a9e1.jpg", 4.7),
                    createFood("石锅拌饭", "营养均衡，色彩丰富的韩国料理", "korean",
                            "北京市朝阳区工体北路13号",
                            "https://img.zcool.cn/community/01c1a65cd567faa801214168097e62.jpg", 4.5),
                    createFood("部队火锅", "香辣可口，食材丰富的韩式火锅", "korean",
                            "深圳市南山区海岸城购物中心",
                            "https://img.zcool.cn/community/01822a5e112bf9a80120a8950e9c6c.jpg", 4.6)
            );

            // 准备西餐数据
            List<FoodRecommendation> westernFoods = Arrays.asList(
                    createFood("牛排", "精选谷饲牛肉，完美火候烹制", "western",
                            "上海市浦东新区陆家嘴环路",
                            "https://img.zcool.cn/community/018e115cd5683ca801214168302095.jpg", 4.8),
                    createFood("意大利面", "手工制作，口感绝佳的经典意面", "western",
                            "北京市朝阳区三里屯Village",
                            "https://img.zcool.cn/community/0171785cd56887a801214168d41fc1.jpg", 4.6),
                    createFood("披萨", "选材考究，烘焙香脆的传统披萨", "western",
                            "广州市越秀区北京路232号",
                            "https://img.zcool.cn/community/013ee25cd5688ea801214168b0cf8d.jpg", 4.5),
                    createFood("法式鹅肝", "入口即化，奢华享受的经典法餐", "western",
                            "上海市静安区南京西路1376号",
                            "https://img.zcool.cn/community/016fee5cd56893a801214168a40f7e.jpg", 4.9)
            );

            // 准备其他美食数据
            List<FoodRecommendation> otherFoods = Arrays.asList(
                    createFood("泰式冬阴功汤", "酸辣鲜香，滋味丰富的泰国名汤", "other",
                            "深圳市福田区华强北",
                            "https://img.zcool.cn/community/01bdaa5cd568a0a801214168e45bef.jpg", 4.7),
                    createFood("印度咖喱", "香料丰富，风味独特的印度美食", "other",
                            "上海市静安区南京西路1788号",
                            "https://img.zcool.cn/community/01af715cd568a6a8012141683b9fc3.jpg", 4.5),
                    createFood("越南河粉", "清爽可口，营养丰富的越南代表性美食", "other",
                            "广州市越秀区文明路",
                            "https://img.zcool.cn/community/0163d55cd568aca801214168cc0eac.jpg", 4.6)
            );

            // 保存所有数据
            foodRepository.saveAll(chineseFoods);
            foodRepository.saveAll(japaneseFoods);
            foodRepository.saveAll(koreanFoods);
            foodRepository.saveAll(westernFoods);
            foodRepository.saveAll(otherFoods);

            System.out.println("已成功初始化 " + foodRepository.count() + " 条美食数据");
        };
    }

    // 创建美食数据的辅助方法
    private FoodRecommendation createFood(String name, String description, String cuisine,
            String location, String imageUrl, Double rating) {
        FoodRecommendation food = new FoodRecommendation();
        food.setName(name);
        food.setDescription(description);
        food.setCuisine(cuisine);
        food.setLocation(location);
        food.setImageUrl(imageUrl);
        food.setRating(rating);
        food.setCreatedAt(LocalDateTime.now());
        food.setViewCount(0);
        return food;
    }
}
