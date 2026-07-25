#include <Arduino.h>

// Array of your specific GPIO pins
const int buttonPins[] = {13, 12, 14, 27, 26, 25};
const int numButtons = 6;

// Variable to store which switch was pressed (0 means none have been pressed yet)
int switch_value = 0; 

void setup_switches() {
  Serial.begin(115200);
  
  // Set all pins in the array to use internal pull-up resistors
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void update_switches() {
  // Check each button one by one
  for (int i = 0; i < numButtons; i++) {
    
    // Remember: With INPUT_PULLUP, a pressed button connected to GND reads as LOW
    if (digitalRead(buttonPins[i]) == LOW) {
      
      // i is the array index (0 to 5). We add 1 to make the switch_value 1 to 6.
      switch_value = i + 1; 
      
      Serial.print("Switch pressed! switch_value = ");
      Serial.println(switch_value);
      
      // A small delay acts as a "debounce" so a single press isn't read 50 times in a millisecond
      delay(250); 
    }
  }
}