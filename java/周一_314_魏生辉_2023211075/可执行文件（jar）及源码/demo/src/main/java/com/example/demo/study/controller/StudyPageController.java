package com.example.demo.study.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;

@Controller
@RequestMapping("/study")
public class StudyPageController {

    @GetMapping("/cet6-game")
    public String getCet6GamePage() {
        return "study/cet6-game"; // Corresponds to src/main/resources/templates/study/cet6-game.html
    }

    @GetMapping("/study-progress")
    public String getStudyProgressPage() {
        return "study/study-progress"; // Corresponds to src/main/resources/templates/study/study-progress.html
    }
} 