package com.example.demo.fashion.controller;

import java.util.List;
import java.util.HashMap;

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
import org.springframework.web.bind.annotation.RestController;

import com.example.demo.fashion.model.FashionOutfit;
import com.example.demo.fashion.service.FashionOutfitService;

@RestController
@RequestMapping("/api/fashion-outfits")
public class FashionOutfitController {

    @Autowired
    private FashionOutfitService fashionService;

    @GetMapping
    public ResponseEntity<List<FashionOutfit>> getAllOutfits() {
        return ResponseEntity.ok(fashionService.getAllOutfits());
    }

    @GetMapping("/{id}")
    public ResponseEntity<FashionOutfit> getOutfitById(@PathVariable Long id) {
        return fashionService.getOutfitById(id)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    @GetMapping("/season/{season}")
    public ResponseEntity<List<FashionOutfit>> getOutfitsBySeason(@PathVariable String season) {
        return ResponseEntity.ok(fashionService.getOutfitsBySeason(season));
    }

    @GetMapping("/occasion/{occasion}")
    public ResponseEntity<List<FashionOutfit>> getOutfitsByOccasion(@PathVariable String occasion) {
        return ResponseEntity.ok(fashionService.getOutfitsByOccasion(occasion));
    }

    @GetMapping("/style/{style}")
    public ResponseEntity<List<FashionOutfit>> getOutfitsByStyle(@PathVariable String style) {
        return ResponseEntity.ok(fashionService.getOutfitsByStyle(style));
    }

    @PostMapping
    public ResponseEntity<FashionOutfit> createOutfit(@RequestBody FashionOutfit outfit) {
        return new ResponseEntity<>(fashionService.createOutfit(outfit), HttpStatus.CREATED);
    }

    @PutMapping("/{id}")
    public ResponseEntity<FashionOutfit> updateOutfit(@PathVariable Long id, @RequestBody FashionOutfit outfitDetails) {
        return fashionService.updateOutfit(id, outfitDetails)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteOutfit(@PathVariable Long id) {
        return fashionService.deleteOutfit(id)
                ? ResponseEntity.noContent().build()
                : ResponseEntity.notFound().build();
    }

    @GetMapping("/test")
    public ResponseEntity<Object> testOutfitApi() {
        List<FashionOutfit> outfits = fashionService.getAllOutfits();

        // 创建测试响应对象
        var response = new HashMap<String, Object>();
        response.put("success", true);
        response.put("message", "穿搭推荐API测试成功");
        response.put("timestamp", new java.util.Date().toString());
        response.put("count", outfits.size());

        if (!outfits.isEmpty()) {
            response.put("sampleOutfit", outfits.get(0));
        }

        return ResponseEntity.ok(response);
    }
}
