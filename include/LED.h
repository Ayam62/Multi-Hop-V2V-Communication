#ifndef LED_H
#define LED_H

#include <Arduino.h>

void setupLEDs();
void displayAlertLED(uint8_t alertType);
void updateLEDs();

#endif // LED_H
