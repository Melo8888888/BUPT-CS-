package com.example.demo.travel.model;

import java.util.List;

public class Route {

    private String origin;
    private String destination;
    private String transportType;
    private Double distance; // 单位：米
    private Integer duration; // 单位：秒
    private List<RouteStep> steps;

    // 内部类，用于表示路线中的每个步骤
    public static class RouteStep {

        private String instruction;
        private Double distance; // 单位：米
        private Integer duration; // 单位：秒
        private String roadName;

        public RouteStep() {
        }

        public RouteStep(String instruction, Double distance, Integer duration, String roadName) {
            this.instruction = instruction;
            this.distance = distance;
            this.duration = duration;
            this.roadName = roadName;
        }

        // Getters and Setters
        public String getInstruction() {
            return instruction;
        }

        public void setInstruction(String instruction) {
            this.instruction = instruction;
        }

        public Double getDistance() {
            return distance;
        }

        public void setDistance(Double distance) {
            this.distance = distance;
        }

        public Integer getDuration() {
            return duration;
        }

        public void setDuration(Integer duration) {
            this.duration = duration;
        }

        public String getRoadName() {
            return roadName;
        }

        public void setRoadName(String roadName) {
            this.roadName = roadName;
        }
    }

    // 构造函数
    public Route() {
    }

    public Route(String origin, String destination, String transportType, Double distance, Integer duration, List<RouteStep> steps) {
        this.origin = origin;
        this.destination = destination;
        this.transportType = transportType;
        this.distance = distance;
        this.duration = duration;
        this.steps = steps;
    }

    // Getters and Setters
    public String getOrigin() {
        return origin;
    }

    public void setOrigin(String origin) {
        this.origin = origin;
    }

    public String getDestination() {
        return destination;
    }

    public void setDestination(String destination) {
        this.destination = destination;
    }

    public String getTransportType() {
        return transportType;
    }

    public void setTransportType(String transportType) {
        this.transportType = transportType;
    }

    public Double getDistance() {
        return distance;
    }

    public void setDistance(Double distance) {
        this.distance = distance;
    }

    public Integer getDuration() {
        return duration;
    }

    public void setDuration(Integer duration) {
        this.duration = duration;
    }

    public List<RouteStep> getSteps() {
        return steps;
    }

    public void setSteps(List<RouteStep> steps) {
        this.steps = steps;
    }
}
