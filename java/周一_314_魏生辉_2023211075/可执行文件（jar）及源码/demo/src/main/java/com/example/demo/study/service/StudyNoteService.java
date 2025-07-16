package com.example.demo.study.service;

import java.util.List;
import java.util.Optional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.example.demo.study.model.StudyNote;
import com.example.demo.study.repository.StudyNoteRepository;

@Service
public class StudyNoteService {

    @Autowired
    private StudyNoteRepository studyNoteRepository;

    public List<StudyNote> getAllNotes() {
        return studyNoteRepository.findAll();
    }

    public Optional<StudyNote> getNoteById(Long id) {
        return studyNoteRepository.findById(id);
    }

    public List<StudyNote> getNotesByCategory(String category) {
        return studyNoteRepository.findByCategory(category);
    }

    public List<StudyNote> searchNotes(String keyword) {
        return studyNoteRepository.findByTitleContainingOrContentContaining(keyword, keyword);
    }

    public StudyNote createNote(StudyNote note) {
        return studyNoteRepository.save(note);
    }

    public StudyNote updateNote(Long id, StudyNote updatedNote) {
        return studyNoteRepository.findById(id)
                .map(note -> {
                    note.setTitle(updatedNote.getTitle());
                    note.setContent(updatedNote.getContent());
                    note.setCategory(updatedNote.getCategory());
                    return studyNoteRepository.save(note);
                })
                .orElseThrow(() -> new RuntimeException("Note not found with id: " + id));
    }

    public void deleteNote(Long id) {
        studyNoteRepository.deleteById(id);
    }
}
