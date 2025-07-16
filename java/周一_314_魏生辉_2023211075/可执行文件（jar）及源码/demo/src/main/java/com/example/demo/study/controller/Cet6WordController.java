package com.example.demo.study.controller;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import com.example.demo.study.model.Cet6Word;
import com.example.demo.study.service.Cet6WordService;

@RestController
@RequestMapping("/api/cet6words")
public class Cet6WordController {

    @Autowired
    private Cet6WordService cet6WordService;

    @GetMapping
    public List<Cet6Word> getAllWords() {
        return cet6WordService.getAllWords();
    }

    @GetMapping("/{id}")
    public ResponseEntity<Cet6Word> getWordById(@PathVariable Long id) {
        return cet6WordService.getWordById(id)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    @PostMapping
    public Cet6Word createWord(@RequestBody Cet6Word word) {
        return cet6WordService.saveWord(word);
    }

    @PutMapping("/{id}")
    public ResponseEntity<Cet6Word> updateWord(@PathVariable Long id, @RequestBody Cet6Word wordDetails) {
        return cet6WordService.getWordById(id)
                .map(word -> {
                    word.setWord(wordDetails.getWord());
                    word.setDefinition(wordDetails.getDefinition());
                    word.setPronunciation(wordDetails.getPronunciation());
                    word.setCategory(wordDetails.getCategory());
                    word.setDifficulty(wordDetails.getDifficulty());
                    return ResponseEntity.ok(cet6WordService.saveWord(word));
                })
                .orElse(ResponseEntity.notFound().build());
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteWord(@PathVariable Long id) {
        if (cet6WordService.getWordById(id).isPresent()) {
            cet6WordService.deleteWord(id);
            return ResponseEntity.noContent().build();
        } else {
            return ResponseEntity.notFound().build();
        }
    }

    @GetMapping("/daily/{limit}")
    public List<Cet6Word> getDailyWords(@PathVariable int limit) {
        return cet6WordService.getDailyWords(limit);
    }
} 