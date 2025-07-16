package com.example.demo.study.repository;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.example.demo.study.model.StudyNote;

@Repository
public interface StudyNoteRepository extends JpaRepository<StudyNote, Long> {

    List<StudyNote> findByCategory(String category);

    List<StudyNote> findByTitleContainingOrContentContaining(String title, String content);
}
