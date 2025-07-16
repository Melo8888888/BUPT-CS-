package com.example.demo.study.service;

import java.util.List;
import java.util.Optional;
import java.util.Random;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.example.demo.study.model.Cet6Word;
import com.example.demo.study.repository.Cet6WordRepository;

@Service
public class Cet6WordService {

    @Autowired
    private Cet6WordRepository cet6WordRepository;

    public List<Cet6Word> getAllWords() {
        return cet6WordRepository.findAll();
    }

    public Optional<Cet6Word> getWordById(Long id) {
        return cet6WordRepository.findById(id);
    }

    public Cet6Word saveWord(Cet6Word word) {
        return cet6WordRepository.save(word);
    }

    public void deleteWord(Long id) {
        cet6WordRepository.deleteById(id);
    }

    public List<Cet6Word> getDailyWords(int limit) {
        // For now, return a random selection of words.
        // In a real application, this would involve more complex logic
        // based on user progress and learning algorithms.
        List<Cet6Word> allWords = cet6WordRepository.findAll();
        if (allWords.size() <= limit) {
            return allWords;
        }

        Random random = new Random();
        return random.ints(limit, 0, allWords.size())
                .mapToObj(allWords::get)
                .collect(java.util.stream.Collectors.toList());
    }
} 