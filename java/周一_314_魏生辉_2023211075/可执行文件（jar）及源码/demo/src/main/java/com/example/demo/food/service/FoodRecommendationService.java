package com.example.demo.food.service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.PageRequest;
import org.springframework.stereotype.Service;

import com.example.demo.food.model.FoodRecommendation;
import com.example.demo.food.repository.FoodRecommendationRepository;

@Service
public class FoodRecommendationService {

    @Autowired
    private FoodRecommendationRepository foodRepository;

    public List<FoodRecommendation> getAllRecommendations() {
        return foodRepository.findAll();
    }

    public Optional<FoodRecommendation> getRecommendationById(Long id) {
        return foodRepository.findById(id);
    }

    public List<FoodRecommendation> getRecommendationsByCuisine(String cuisine) {
        return foodRepository.findByCuisine(cuisine);
    }

    public List<FoodRecommendation> searchRecommendationsByName(String name) {
        return foodRepository.findByNameContainingIgnoreCase(name);
    }

    public List<FoodRecommendation> getRecommendationsByMinRating(Double minRating) {
        return foodRepository.findByRatingGreaterThanEqual(minRating);
    }

    public List<FoodRecommendation> getMostPopularRecommendations(int limit) {
        return foodRepository.findAllOrderByViewCountDesc(PageRequest.of(0, limit));
    }

    public List<FoodRecommendation> getHighestRatedRecommendations(int limit) {
        return foodRepository.findAllOrderByRatingDesc(PageRequest.of(0, limit));
    }

    public FoodRecommendation createRecommendation(FoodRecommendation recommendation) {
        recommendation.setCreatedAt(LocalDateTime.now());
        recommendation.setViewCount(0);
        return foodRepository.save(recommendation);
    }

    public Optional<FoodRecommendation> updateRecommendation(Long id, FoodRecommendation recommendationDetails) {
        return foodRepository.findById(id).map(existingRecommendation -> {
            existingRecommendation.setName(recommendationDetails.getName());
            existingRecommendation.setDescription(recommendationDetails.getDescription());
            existingRecommendation.setCuisine(recommendationDetails.getCuisine());
            existingRecommendation.setLocation(recommendationDetails.getLocation());
            existingRecommendation.setImageUrl(recommendationDetails.getImageUrl());
            existingRecommendation.setRating(recommendationDetails.getRating());
            return foodRepository.save(existingRecommendation);
        });
    }

    public Optional<FoodRecommendation> incrementViewCount(Long id) {
        return foodRepository.findById(id).map(recommendation -> {
            recommendation.incrementViewCount();
            return foodRepository.save(recommendation);
        });
    }

    public boolean deleteRecommendation(Long id) {
        if (foodRepository.existsById(id)) {
            foodRepository.deleteById(id);
            return true;
        }
        return false;
    }
}
