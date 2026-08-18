#ifndef ALERT_TYPES_H
#define ALERT_TYPES_H

#include <Arduino.h>

// 1. Strongly typed Enum mapping hardware numbers (1–6) to meaningful constants
enum AlertType : uint8_t {
    ALERT_EMERGENCY        = 1,
    ALERT_ACCIDENT         = 2,
    ALERT_HAZARD           = 3,
    ALERT_VISIBILITY       = 4,
    ALERT_OBSTACLE         = 5,
    ALERT_HARD_BRAKE       = 6
};

// 2. Helper function to map alert numbers back to human-readable strings
inline const char* getAlertDescription(uint8_t msgType) {
    switch (msgType) {
        case ALERT_EMERGENCY:      return "Emergency";
        case ALERT_ACCIDENT:       return "Accident ahead";
        case ALERT_HAZARD:         return  "Hazard Signal";
        case ALERT_VISIBILITY:     return "Low Visibility ahead";
        case ALERT_OBSTACLE:       return "Potholes/Obstacle ahead";
        case ALERT_HARD_BRAKE:     return "Hard brake";
        default:                   return "Unknown Alert";
    }
}

#endif // ALERT_TYPES_H