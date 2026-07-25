#ifndef SWITCH_INPUT_H
#define SWITCH_INPUT_H

#include <Arduino.h>

extern const int buttonPins[];
extern const int numButtons;
extern int switch_value;

void setup_switches();
void update_switches();

#endif // SWITCH_INPUT_H
