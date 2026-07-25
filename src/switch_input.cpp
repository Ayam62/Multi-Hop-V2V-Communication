#include <Arduino.h>
#include "switch_input.h"

const int buttonPins[] = {13, 12, 14, 27, 26, 25};
const int numButtons = 6;

int switch_value = 0;

void setup_switches() {
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void update_switches() {
  for (int i = 0; i < numButtons; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      switch_value = i + 1;
      delay(250);
    }
  }
}