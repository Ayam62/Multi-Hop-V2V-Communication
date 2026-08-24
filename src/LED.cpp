#include "LED.h"

namespace {
const uint8_t ledPins[] = {4, 2, 5, 18, 19, 21};
const uint8_t ledCount = sizeof(ledPins) / sizeof(ledPins[0]);
}

void setupLEDs() {
    for (uint8_t index = 0; index < ledCount; index++) {
        pinMode(ledPins[index], OUTPUT);
        digitalWrite(ledPins[index], LOW);
    }
}

void displayAlertLED(uint8_t alertType) {
    for (uint8_t index = 0; index < ledCount; index++) {
        digitalWrite(ledPins[index], LOW);
    }

    if (alertType >= 1 && alertType <= ledCount) {
        digitalWrite(ledPins[alertType - 1], HIGH);
    }
}