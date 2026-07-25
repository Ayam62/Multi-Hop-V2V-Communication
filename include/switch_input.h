#ifndef SWITCH_INPUT_H
#define SWITCH_INPUT_H

#include <Arduino.h>

// Define the pins and state as static so this header can be included safely
static const int buttonPins[] = {13, 12, 14, 27, 26, 25};
static const int numButtons = 6;

// Global variable that will be available in main.cpp
static int switch_value = 0;

// Call this inside your setup() function
inline void setup_switches() {
    for (int i = 0; i < numButtons; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }
}

// Call this inside your loop() function
inline void update_switches() {
    for (int i = 0; i < numButtons; i++) {
        // With INPUT_PULLUP, LOW means the button is pressed
        if (digitalRead(buttonPins[i]) == LOW) {
            
            // Set switch_value to 1 through 6
            switch_value = i + 1; 
            
            // Simple debounce delay
            delay(250); 
        }
    }
}

#endif // SWITCH_INPUT_H
