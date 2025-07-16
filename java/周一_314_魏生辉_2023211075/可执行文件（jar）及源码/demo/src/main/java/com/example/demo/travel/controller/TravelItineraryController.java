package com.example.demo.travel.controller;

import java.util.List;

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

import com.example.demo.travel.model.TravelItinerary;
import com.example.demo.travel.service.TravelItineraryService;

@RestController
@RequestMapping("/api/travel-itineraries")
public class TravelItineraryController {

    @Autowired
    private TravelItineraryService travelService;

    @GetMapping
    public ResponseEntity<List<TravelItinerary>> getAllItineraries() {
        return ResponseEntity.ok(travelService.getAllItineraries());
    }

    @GetMapping("/{id}")
    public ResponseEntity<TravelItinerary> getItineraryById(@PathVariable Long id) {
        return travelService.getItineraryById(id)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    @GetMapping("/destination/{destination}")
    public ResponseEntity<List<TravelItinerary>> getItinerariesByDestination(@PathVariable String destination) {
        return ResponseEntity.ok(travelService.getItinerariesByDestination(destination));
    }

    @GetMapping("/duration")
    public ResponseEntity<List<TravelItinerary>> getItinerariesByMaxDuration(@RequestParam Integer maxDays) {
        return ResponseEntity.ok(travelService.getItinerariesByMaxDuration(maxDays));
    }

    @GetMapping("/budget")
    public ResponseEntity<List<TravelItinerary>> getItinerariesByMaxBudget(@RequestParam Double maxBudget) {
        return ResponseEntity.ok(travelService.getItinerariesByMaxBudget(maxBudget));
    }

    @PostMapping
    public ResponseEntity<TravelItinerary> createItinerary(@RequestBody TravelItinerary itinerary) {
        return new ResponseEntity<>(travelService.createItinerary(itinerary), HttpStatus.CREATED);
    }

    @PutMapping("/{id}")
    public ResponseEntity<TravelItinerary> updateItinerary(@PathVariable Long id, @RequestBody TravelItinerary itineraryDetails) {
        return travelService.updateItinerary(id, itineraryDetails)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteItinerary(@PathVariable Long id) {
        return travelService.deleteItinerary(id)
                ? ResponseEntity.noContent().build()
                : ResponseEntity.notFound().build();
    }
}
