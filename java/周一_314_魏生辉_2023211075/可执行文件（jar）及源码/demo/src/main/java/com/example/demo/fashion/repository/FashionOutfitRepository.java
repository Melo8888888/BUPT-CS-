package com.example.demo.fashion.repository;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.example.demo.fashion.model.FashionOutfit;

@Repository
public interface FashionOutfitRepository extends JpaRepository<FashionOutfit, Long> {

    List<FashionOutfit> findBySeason(String season);

    List<FashionOutfit> findByOccasion(String occasion);

    List<FashionOutfit> findByStyle(String style);
}
