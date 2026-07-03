#include "gps_module.h"

// Create the Bluetooth object here
BluetoothSerial SerialBT;

// Create a private variable to hold the latest coordinates
GPSCoordinates latestData = {0.0, 0.0, false};

void setupGPSModule() {
  // Start the Bluetooth device
  SerialBT.begin("ESP32_V2V_Node"); 
  Serial.println("Bluetooth is ready! Pair your phone now.");
}

// This function checks for new data, updates the struct, and returns it
GPSCoordinates checkAndGetGPS() {
  // Reset the flag every time we check
  latestData.newDataAvailable = false; 

  if (SerialBT.available()) {
    String incomingGPS = SerialBT.readStringUntil('\n');
    incomingGPS.trim();
    
    int commaIndex = incomingGPS.indexOf(',');

    if (commaIndex > 0) {
      String latString = incomingGPS.substring(0, commaIndex);
      String lonString = incomingGPS.substring(commaIndex + 1);

      // Update our internal struct with the new math-ready doubles
      latestData.latitude = latString.toDouble();
      latestData.longitude = lonString.toDouble();
      latestData.newDataAvailable = true; // Flag that we got fresh data!
    }
  }
  
  // Return the struct back to wherever called this function
  return latestData; 
}