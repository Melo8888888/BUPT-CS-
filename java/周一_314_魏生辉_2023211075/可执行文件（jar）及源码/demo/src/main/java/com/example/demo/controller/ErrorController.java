package com.example.demo.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.servlet.ModelAndView;

import jakarta.servlet.http.HttpServletRequest;

@Controller
@RequestMapping("/error")
public class ErrorController {

    @GetMapping("/404")
    public ModelAndView handleNotFound() {
        ModelAndView modelAndView = new ModelAndView("error/error");
        modelAndView.addObject("message", "Page not found");
        return modelAndView;
    }

    @GetMapping("/500")
    public ModelAndView handleServerError() {
        ModelAndView modelAndView = new ModelAndView("error/error");
        modelAndView.addObject("message", "Internal server error");
        return modelAndView;
    }

    @GetMapping("/error")
    public ModelAndView handleError(HttpServletRequest request) {
        ModelAndView modelAndView = new ModelAndView("error/error");

        // Get the error details from request
        Integer statusCode = (Integer) request.getAttribute("javax.servlet.error.status_code");
        Exception exception = (Exception) request.getAttribute("javax.servlet.error.exception");

        if (statusCode != null) {
            modelAndView.addObject("statusCode", statusCode);
        }

        if (exception != null) {
            modelAndView.addObject("exception", exception);
        } else {
            modelAndView.addObject("message", "An unknown error occurred");
        }

        return modelAndView;
    }
}
