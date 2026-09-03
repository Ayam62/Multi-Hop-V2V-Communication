#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>

// Define a structure to hold both coordinates together
struct GPSCoordinates {
  double latitude;
  double longitude;
  bool newDataAvailable; // A flag to tell main if the data just updated
};

// Read newline-terminated latitude,longitude values from the phone over Bluetooth.
void setupGPSModule();
GPSCoordinates checkAndGetGPS();
GPSCoordinates getLatestGPS();

#endif