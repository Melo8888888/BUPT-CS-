package com.example.demo.travel.service;

import java.time.LocalDateTime;
import java.time.temporal.ChronoUnit;
import java.util.List;
import java.util.Optional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.example.demo.travel.model.TravelItinerary;
import com.example.demo.travel.repository.TravelItineraryRepository;

@Service
public class TravelItineraryService {

    @Autowired
    private TravelItineraryRepository travelRepository;

    public List<TravelItinerary> getAllItineraries() {
        return travelRepository.findAll();
    }

    public Optional<TravelItinerary> getItineraryById(Long id) {
        return travelRepository.findById(id);
    }

    public List<TravelItinerary> getItinerariesByDestination(String destination) {
        return travelRepository.findByDestination(destination);
    }

    public List<TravelItinerary> getItinerariesByMaxDuration(Integer maxDays) {
        return travelRepository.findByDurationDaysLessThanEqual(maxDays);
    }

    public List<TravelItinerary> getItinerariesByMaxBudget(Double maxBudget) {
        return travelRepository.findByBudgetLessThanEqual(maxBudget);
    }

    public TravelItinerary createItinerary(TravelItinerary itinerary) {
        if (itinerary.getStartDate() != null && itinerary.getEndDate() != null && itinerary.getDurationDays() == null) {
            int days = (int) ChronoUnit.DAYS.between(itinerary.getStartDate(), itinerary.getEndDate()) + 1;
            itinerary.setDurationDays(days);
        }
        itinerary.setCreatedAt(LocalDateTime.now());
        return travelRepository.save(itinerary);
    }

    public Optional<TravelItinerary> updateItinerary(Long id, TravelItinerary itineraryDetails) {
        return travelRepository.findById(id).map(existingItinerary -> {
            existingItinerary.setTitle(itineraryDetails.getTitle());
            existingItinerary.setDestination(itineraryDetails.getDestination());
            existingItinerary.setDescription(itineraryDetails.getDescription());
            existingItinerary.setStartDate(itineraryDetails.getStartDate());
            existingItinerary.setEndDate(itineraryDetails.getEndDate());

            if (itineraryDetails.getStartDate() != null && itineraryDetails.getEndDate() != null) {
                int days = (int) ChronoUnit.DAYS.between(itineraryDetails.getStartDate(), itineraryDetails.getEndDate()) + 1;
                existingItinerary.setDurationDays(days);
            } else {
                existingItinerary.setDurationDays(itineraryDetails.getDurationDays());
            }

            existingItinerary.setImageUrl(itineraryDetails.getImageUrl());
            existingItinerary.setBudget(itineraryDetails.getBudget());
            return travelRepository.save(existingItinerary);
        });
    }

    public boolean deleteItinerary(Long id) {
        if (travelRepository.existsById(id)) {
            travelRepository.deleteById(id);
            return true;
        }
        return false;
    }
}
