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
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.example.demo.study.model.StudyNote;
import com.example.demo.study.service.StudyNoteService;

@RestController
@RequestMapping("/api/study-notes")
public class StudyNoteController {

    @Autowired
    private StudyNoteService studyNoteService;

    @PostMapping
    public ResponseEntity<StudyNote> createNote(@RequestBody StudyNote note) {
        return ResponseEntity.ok(studyNoteService.createNote(note));
    }

    @GetMapping
    public ResponseEntity<List<StudyNote>> getAllNotes() {
        return ResponseEntity.ok(studyNoteService.getAllNotes());
    }

    @GetMapping("/{id}")
    public ResponseEntity<StudyNote> getNoteById(@PathVariable Long id) {
        return studyNoteService.getNoteById(id)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    @GetMapping("/category/{category}")
    public ResponseEntity<List<StudyNote>> getNotesByCategory(@PathVariable String category) {
        return ResponseEntity.ok(studyNoteService.getNotesByCategory(category));
    }

    @GetMapping("/search")
    public ResponseEntity<List<StudyNote>> searchNotes(@RequestParam String keyword) {
        return ResponseEntity.ok(studyNoteService.searchNotes(keyword));
    }

    @PutMapping("/{id}")
    public ResponseEntity<StudyNote> updateNote(@PathVariable Long id, @RequestBody StudyNote note) {
        try {
            return ResponseEntity.ok(studyNoteService.updateNote(id, note));
        } catch (RuntimeException e) {
            return ResponseEntity.notFound().build();
        }
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteNote(@PathVariable Long id) {
        studyNoteService.deleteNote(id);
        return ResponseEntity.ok().build();
    }
}
