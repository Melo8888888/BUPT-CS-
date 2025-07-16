package com.example.demo.controller;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.servlet.ModelAndView;

import com.example.demo.fashion.model.FashionOutfit;
import com.example.demo.fashion.service.FashionOutfitService;
import com.example.demo.food.model.FoodRecommendation;
import com.example.demo.food.service.FoodRecommendationService;
import com.example.demo.travel.service.TravelItineraryService;

@Controller
public class WebController {

    @Autowired
    private FoodRecommendationService foodService;

    @Autowired
    private FashionOutfitService fashionService;

    @Autowired
    private TravelItineraryService travelService;

    @GetMapping("/index")
    public ModelAndView index() {
        return new ModelAndView("index");
    }

    @GetMapping("/study-notes")
    public ModelAndView studyNotes() {
        return new ModelAndView("study-notes");
    }

    @GetMapping("/food-recommendations")
    public ModelAndView foodRecommendations() {
        ModelAndView mav = new ModelAndView("food-recommendations");
        List<FoodRecommendation> foodList = foodService.getAllRecommendations();
        mav.addObject("foodList", foodList);
        //System.out.println("为食物推荐页加载了 " + foodList.size() + " 条食物数据");
        return mav;
    }

    @GetMapping("/travel-itineraries")
    public ModelAndView travelItineraries() {
        ModelAndView modelAndView = new ModelAndView("travel-itineraries");
        // 不再添加行程数据
        return modelAndView;
    }

    @GetMapping("/fashion-outfits")
    public ModelAndView fashionOutfits() {
        ModelAndView mav = new ModelAndView("fashion-outfits");
        List<FashionOutfit> outfitList = fashionService.getAllOutfits();
        mav.addObject("outfitList", outfitList);
        //System.out.println("为穿搭推荐页加载了 " + outfitList.size() + " 条穿搭数据");

        // 添加过滤器选项
        mav.addObject("occasionOptions", new String[]{"casual", "formal", "party", "work"});
        mav.addObject("styleOptions", new String[]{"minimalist", "streetwear", "vintage", "korean", "sporty", "business"});

        return mav;
    }
}
