#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>
#include "BluetoothSerial.h"

// Define a structure to hold both coordinates together
struct GPSCoordinates {
  double latitude;
  double longitude;
  bool newDataAvailable; // A flag to tell main if the data just updated
};

// Declare the functions that main.cpp is allowed to call
void setupGPSModule();
GPSCoordinates checkAndGetGPS();

#endif