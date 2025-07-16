package com.example.demo.fashion.service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.example.demo.fashion.model.FashionOutfit;
import com.example.demo.fashion.repository.FashionOutfitRepository;

@Service
public class FashionOutfitService {

    @Autowired
    private FashionOutfitRepository fashionRepository;

    public List<FashionOutfit> getAllOutfits() {
        return fashionRepository.findAll();
    }

    public Optional<FashionOutfit> getOutfitById(Long id) {
        return fashionRepository.findById(id);
    }

    public List<FashionOutfit> getOutfitsBySeason(String season) {
        return fashionRepository.findBySeason(season);
    }

    public List<FashionOutfit> getOutfitsByOccasion(String occasion) {
        return fashionRepository.findByOccasion(occasion);
    }

    public List<FashionOutfit> getOutfitsByStyle(String style) {
        return fashionRepository.findByStyle(style);
    }

    public FashionOutfit createOutfit(FashionOutfit outfit) {
        outfit.setCreatedAt(LocalDateTime.now());
        return fashionRepository.save(outfit);
    }

    public Optional<FashionOutfit> updateOutfit(Long id, FashionOutfit outfitDetails) {
        return fashionRepository.findById(id).map(existingOutfit -> {
            existingOutfit.setTitle(outfitDetails.getTitle());
            existingOutfit.setDescription(outfitDetails.getDescription());
            existingOutfit.setSeason(outfitDetails.getSeason());
            existingOutfit.setOccasion(outfitDetails.getOccasion());
            existingOutfit.setStyle(outfitDetails.getStyle());
            existingOutfit.setImageUrl(outfitDetails.getImageUrl());
            existingOutfit.setItems(outfitDetails.getItems());
            return fashionRepository.save(existingOutfit);
        });
    }

    public boolean deleteOutfit(Long id) {
        if (fashionRepository.existsById(id)) {
            fashionRepository.deleteById(id);
            return true;
        }
        return false;
    }
}
