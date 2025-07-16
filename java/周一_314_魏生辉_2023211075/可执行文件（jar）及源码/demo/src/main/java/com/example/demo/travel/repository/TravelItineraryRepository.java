package com.example.demo.travel.repository;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.example.demo.travel.model.TravelItinerary;

@Repository
public interface TravelItineraryRepository extends JpaRepository<TravelItinerary, Long> {

    List<TravelItinerary> findByDestination(String destination);

    List<TravelItinerary> findByDurationDaysLessThanEqual(Integer maxDays);

    List<TravelItinerary> findByBudgetLessThanEqual(Double maxBudget);
}
