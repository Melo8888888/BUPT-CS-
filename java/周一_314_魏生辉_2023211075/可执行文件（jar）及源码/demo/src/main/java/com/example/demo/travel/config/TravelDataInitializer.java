package com.example.demo.travel.config;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.Arrays;
import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import com.example.demo.travel.model.TravelItinerary;
import com.example.demo.travel.repository.TravelItineraryRepository;

@Configuration
public class TravelDataInitializer {

    @Autowired
    private TravelItineraryRepository travelRepository;

    @Bean
    public CommandLineRunner initTravelData() {
        return args -> {
            // 只有当数据库中没有数据时才初始化
            if (travelRepository.count() == 0) {
                //System.out.println("正在初始化旅行路线推荐示例数据...");

                // 定义示例行程
                List<TravelItinerary> itineraries = Arrays.asList(
                        // 北京校园周边半日游
                        createItinerary(
                                "北邮周边文化探索",
                                "北京",
                                "从北邮出发，探索海淀区周边历史与文化景点，包括中国科学技术馆、中关村创业大街和清华大学校园，适合学生周末休闲。",
                                LocalDate.now(),
                                LocalDate.now(),
                                1,
                                "https://images.unsplash.com/photo-1584490087441-75aa5ccdfb77",
                                50.0
                        ),
                        // 北京古都一日游
                        createItinerary(
                                "古都印象一日游",
                                "北京",
                                "游览故宫、天坛、颐和园等北京著名景点，体验传统皇家文化与建筑艺术，品尝老北京特色小吃。",
                                LocalDate.now(),
                                LocalDate.now().plusDays(1),
                                2,
                                "https://images.unsplash.com/photo-1519683268254-f377c0ded7a2",
                                300.0
                        ),
                        // 科技创新之旅
                        createItinerary(
                                "科技创新之旅",
                                "北京",
                                "探访中关村科技园区及周边科技馆和创新中心，了解中国现代科技发展历程，体验前沿科技成果。",
                                LocalDate.now().plusDays(1),
                                LocalDate.now().plusDays(1),
                                1,
                                "https://images.unsplash.com/photo-1620064881869-28db329a5535",
                                100.0
                        ),
                        // 长城文化探秘
                        createItinerary(
                                "长城文化探秘",
                                "北京",
                                "前往八达岭或慕田峪长城，深入了解中国古代建筑智慧与军事防御体系，感受历史文化底蕴。",
                                LocalDate.now().plusDays(2),
                                LocalDate.now().plusDays(2),
                                1,
                                "https://images.unsplash.com/photo-1508804185872-d7badad00f7d",
                                200.0
                        ),
                        // 798艺术区体验
                        createItinerary(
                                "798艺术区文艺之旅",
                                "北京",
                                "游览798艺术区，欣赏现代艺术展览，体验文艺氛围，参观特色工业风格建筑，品尝创意咖啡。",
                                LocalDate.now().plusDays(3),
                                LocalDate.now().plusDays(3),
                                1,
                                "https://images.unsplash.com/photo-1567013127542-490d757e6aa2",
                                150.0
                        ),
                        // 北京美食探索
                        createItinerary(
                                "京城美食探索",
                                "北京",
                                "从早餐的豆汁儿、豆腐脑到烤鸭、涮羊肉，一日尝遍北京特色美食，感受地道老北京饮食文化。",
                                LocalDate.now().plusDays(4),
                                LocalDate.now().plusDays(4),
                                1,
                                "https://images.unsplash.com/photo-1563245372-f21724e3856d",
                                300.0
                        ),
                        // 香山红叶观赏
                        createItinerary(
                                "香山红叶之旅",
                                "北京",
                                "秋季游览香山公园，欣赏红叶美景，登顶俯瞰北京城景，参观双清别墅，体验京郊自然风光。",
                                LocalDate.now().plusDays(5),
                                LocalDate.now().plusDays(5),
                                1,
                                "https://images.unsplash.com/photo-1605000797499-36c69e479ed7",
                                80.0
                        ),
                        // 北京夜游
                        createItinerary(
                                "北京城市夜景游",
                                "北京",
                                "欣赏国家大剧院、CBD商圈、奥林匹克公园等地夜景，感受现代北京的繁华与活力，体验不同的城市魅力。",
                                LocalDate.now().plusDays(6),
                                LocalDate.now().plusDays(6),
                                1,
                                "https://images.unsplash.com/photo-1577706552081-119d654a1f24",
                                120.0
                        )
                );

                // 保存所有示例行程
                travelRepository.saveAll(itineraries);

                //System.out.println("旅行路线推荐示例数据初始化完成，共添加 " + itineraries.size() + " 条数据。");
            }
        };
    }

    /**
     * 创建行程辅助方法
     */
    private TravelItinerary createItinerary(String title, String destination, String description,
            LocalDate startDate, LocalDate endDate, Integer durationDays,
            String imageUrl, Double budget) {
        TravelItinerary itinerary = new TravelItinerary();
        itinerary.setTitle(title);
        itinerary.setDestination(destination);
        itinerary.setDescription(description);
        itinerary.setStartDate(startDate);
        itinerary.setEndDate(endDate);
        itinerary.setDurationDays(durationDays);
        itinerary.setImageUrl(imageUrl);
        itinerary.setBudget(budget);
        itinerary.setCreatedAt(LocalDateTime.now());
        return itinerary;
    }
}
