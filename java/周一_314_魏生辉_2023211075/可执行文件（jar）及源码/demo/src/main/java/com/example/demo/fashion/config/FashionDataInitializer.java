package com.example.demo.fashion.config;

import java.time.LocalDateTime;
import java.util.Arrays;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import com.example.demo.fashion.model.FashionOutfit;
import com.example.demo.fashion.repository.FashionOutfitRepository;

@Configuration
public class FashionDataInitializer {

    private static final Logger logger = LoggerFactory.getLogger(FashionDataInitializer.class);

    @Autowired
    private FashionOutfitRepository fashionRepository;

    @Bean
    public CommandLineRunner initFashionData() {
        return args -> {
            // 如果已经有数据，则不再初始化
            if (fashionRepository.count() > 0) {
                return;
            }

            // 休闲风格的穿搭
            List<FashionOutfit> casualOutfits = Arrays.asList(
                    createOutfit(
                            "基础休闲风",
                            "简单舒适的日常穿搭，适合周末出行、逛街或与朋友聚会。",
                            "spring", "casual", "minimalist",
                            "上衣：白色基础T恤\n裤子：蓝色直筒牛仔裤\n鞋子：白色运动鞋\n配饰：简约手表"
                    ),
                    createOutfit(
                            "街头休闲风",
                            "融合街头元素的休闲穿搭，展现活力与个性。",
                            "summer", "casual", "streetwear",
                            "上衣：oversized宽松T恤\n外套：复古棒球夹克\n裤子：工装裤\n鞋子：高帮帆布鞋\n配饰：棒球帽，链条项链"
                    ),
                    createOutfit(
                            "韩系休闲风",
                            "简约清新的韩系穿搭，尽显温柔与时尚。",
                            "autumn", "casual", "korean",
                            "上衣：白色宽松针织衫\n外套：驼色长款大衣\n裤子：直筒牛仔裤\n鞋子：小白鞋\n配饰：简约耳环，小包包"
                    )
            );

            // 正式场合的穿搭
            List<FashionOutfit> formalOutfits = Arrays.asList(
                    createOutfit(
                            "商务正装",
                            "专业大气的商务正装，适合重要会议和正式商务场合。",
                            "winter", "formal", "business",
                            "上衣：白色衬衫\n外套：深蓝色西装外套\n裤子：深蓝色西装裤\n鞋子：黑色皮鞋\n配饰：领带，皮带，手表"
                    ),
                    createOutfit(
                            "复古正式风",
                            "融合复古元素的正式穿搭，优雅而有格调。",
                            "autumn", "formal", "vintage",
                            "上衣：丝绒西装外套\n内搭：白色衬衫\n裤子：高腰西装裤\n鞋子：牛津鞋\n配饰：胸针，复古手表"
                    ),
                    createOutfit(
                            "极简主义正装",
                            "简约利落的正式穿搭，展现现代都市感。",
                            "spring", "formal", "minimalist",
                            "上衣：黑色无领西装外套\n内搭：白色高领衫\n裤子：黑色直筒裤\n鞋子：黑色乐福鞋\n配饰：极简手环"
                    )
            );

            // 派对穿搭
            List<FashionOutfit> partyOutfits = Arrays.asList(
                    createOutfit(
                            "时尚派对风",
                            "潮流前卫的派对穿搭，成为派对焦点。",
                            "summer", "party", "streetwear",
                            "上衣：印花短袖衬衫\n外套：皮夹克\n裤子：黑色破洞牛仔裤\n鞋子：马丁靴\n配饰：多层次项链，黑色墨镜"
                    ),
                    createOutfit(
                            "复古派对风",
                            "带有复古元素的派对穿搭，独特而吸睛。",
                            "autumn", "party", "vintage",
                            "上衣：复古印花衬衫\n裤子：高腰阔腿裤\n鞋子：厚底皮鞋\n配饰：贝雷帽，圆形墨镜，复古腰带"
                    ),
                    createOutfit(
                            "韩系派对风",
                            "融合韩系元素的派对穿搭，时尚而不失甜美。",
                            "spring", "party", "korean",
                            "上衣：亮片吊带上衣\n外套：短款夹克\n裤子/裙子：高腰A字裙\n鞋子：厚底运动鞋\n配饰：发箍，小挎包"
                    )
            );

            // 工作穿搭
            List<FashionOutfit> workOutfits = Arrays.asList(
                    createOutfit(
                            "商务休闲风",
                            "介于正式与休闲之间的工作穿搭，专业又舒适。",
                            "autumn", "work", "business",
                            "上衣：白色衬衫\n外套：灰色针织开衫\n裤子：黑色休闲裤\n鞋子：乐福鞋\n配饰：简约腕表，皮带"
                    ),
                    createOutfit(
                            "极简工作风",
                            "简约大气的工作穿搭，展现专业与高效。",
                            "winter", "work", "minimalist",
                            "上衣：黑色高领毛衣\n外套：米色风衣\n裤子：黑色直筒西装裤\n鞋子：黑色尖头平底鞋\n配饰：简约耳钉，细腰带"
                    ),
                    createOutfit(
                            "运动商务风",
                            "融合运动元素的工作穿搭，活力四射又不失专业。",
                            "spring", "work", "sporty",
                            "上衣：白色Polo衫\n外套：深蓝色休闲西装外套\n裤子：卡其色休闲裤\n鞋子：休闲皮鞋\n配饰：帆布表带手表，帆布腰带"
                    )
            );

            // 保存所有穿搭数据
            fashionRepository.saveAll(casualOutfits);
            fashionRepository.saveAll(formalOutfits);
            fashionRepository.saveAll(partyOutfits);
            fashionRepository.saveAll(workOutfits);

            logger.debug("已成功初始化 {} 条穿搭数据", fashionRepository.count());
        };
    }

    // 创建穿搭数据的辅助方法
    private FashionOutfit createOutfit(String title, String description, String season,
            String occasion, String style, String items) {
        FashionOutfit outfit = new FashionOutfit();
        outfit.setTitle(title);
        outfit.setDescription(description);
        outfit.setSeason(season);
        outfit.setOccasion(occasion);
        outfit.setStyle(style);
        outfit.setItems(items);

        // 设置示例图片URL
        String imageUrl = getImageUrlForOutfit(occasion, style);
        outfit.setImageUrl(imageUrl);

        outfit.setCreatedAt(LocalDateTime.now());
        return outfit;
    }

    // 获取穿搭图片URL的辅助方法（使用样式和场合生成不同图片）
    private String getImageUrlForOutfit(String occasion, String style) {
        // 使用免费图片网站的图片，根据场合和风格生成不同图片URL
        switch (occasion) {
            case "casual":
                switch (style) {
                    case "minimalist":
                        return "https://images.unsplash.com/photo-1516762689617-e1cffcef479d";
                    case "streetwear":
                        return "https://images.unsplash.com/photo-1523380677598-64d85d4f3fa0";
                    case "korean":
                        return "https://images.unsplash.com/photo-1509631179647-0177331693ae";
                    default:
                        return "https://images.unsplash.com/photo-1515886657613-9f3515b0c78f";
                }
            case "formal":
                switch (style) {
                    case "minimalist":
                        return "https://images.unsplash.com/photo-1622519407650-3df9883f76a5";
                    case "vintage":
                        return "https://images.unsplash.com/photo-1507680434567-5739c80be1ac";
                    case "business":
                        return "https://images.unsplash.com/photo-1497366811353-6870744d04b2";
                    default:
                        return "https://images.unsplash.com/photo-1506634064465-7dab4de896ed";
                }
            case "party":
                switch (style) {
                    case "streetwear":
                        return "https://images.unsplash.com/photo-1529139574466-a303027c1d8b";
                    case "vintage":
                        return "https://images.unsplash.com/photo-1552374196-1ab2a1c593e8";
                    case "korean":
                        return "https://images.unsplash.com/photo-1581044777550-4cfa60707c03";
                    default:
                        return "https://images.unsplash.com/photo-1542838132-92c53300491e";
                }
            case "work":
                switch (style) {
                    case "business":
                        return "https://images.unsplash.com/photo-1487222477894-8943e31ef7b2";
                    case "minimalist":
                        return "https://images.unsplash.com/photo-1556159317-6c26aa9e3a0f";
                    case "sporty":
                        return "https://images.unsplash.com/photo-1489987707025-afc232f7ea0f";
                    default:
                        return "https://images.unsplash.com/photo-1580651315530-69c8e0026377";
                }
            default:
                return "https://images.unsplash.com/photo-1490481651871-ab68de25d43d";
        }
    }
}
