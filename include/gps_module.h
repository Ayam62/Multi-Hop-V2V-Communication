#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>

// Define a structure to hold both coordinates together
struct GPSCoordinates {
  double latitude;
  double longitude;
  bool newDataAvailable; // A flag to tell main if the data just updated
};

// Replay the next coordinate from /location_data.txt when its interval expires.
void setupGPSModule();
GPSCoordinates checkAndGetGPS();
GPSCoordinates getLatestGPS();

#endif