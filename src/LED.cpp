#include "LED.h"

namespace {
const uint8_t ledPins[] = {4, 2, 5, 18, 19, 21};
const uint8_t ledCount = sizeof(ledPins) / sizeof(ledPins[0]);
const unsigned long ledOnDurationMillis = 3000UL;
unsigned long ledOffAt[ledCount] = {};
}

void setupLEDs() {
    for (uint8_t index = 0; index < ledCount; index++) {
        pinMode(ledPins[index], OUTPUT);
        digitalWrite(ledPins[index], LOW);
    }
}

void displayAlertLED(uint8_t alertType) {
    if (alertType >= 1 && alertType <= ledCount) {
        const uint8_t ledIndex = alertType - 1;
        digitalWrite(ledPins[ledIndex], HIGH);
        ledOffAt[ledIndex] = millis() + ledOnDurationMillis;
    }
}

void updateLEDs() {
    const unsigned long now = millis();

    for (uint8_t index = 0; index < ledCount; index++) {
        if (ledOffAt[index] != 0 &&
            static_cast<long>(now - ledOffAt[index]) >= 0) {
            digitalWrite(ledPins[index], LOW);
            ledOffAt[index] = 0;
        }
    }
}