package com.example.demo.food.repository;

import java.util.List;

import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;

import com.example.demo.food.model.FoodRecommendation;

@Repository
public interface FoodRecommendationRepository extends JpaRepository<FoodRecommendation, Long> {

    List<FoodRecommendation> findByCuisine(String cuisine);

    List<FoodRecommendation> findByNameContainingIgnoreCase(String name);

    List<FoodRecommendation> findByRatingGreaterThanEqual(Double rating);

    @Query("SELECT f FROM FoodRecommendation f ORDER BY f.viewCount DESC")
    List<FoodRecommendation> findAllOrderByViewCountDesc(Pageable pageable);

    @Query("SELECT f FROM FoodRecommendation f ORDER BY f.rating DESC")
    List<FoodRecommendation> findAllOrderByRatingDesc(Pageable pageable);
}
