#include "gps_module.h"
#include <SPIFFS.h>

GPSCoordinates latestData = {0.0, 0.0, false};
File locationFile;
unsigned long lastLocationUpdateMillis = 0;
const unsigned long LOCATION_UPDATE_INTERVAL_MILLIS = 3000UL;

bool readNextLocation() {
  if (!locationFile || !locationFile.available()) {
    locationFile.seek(0);
  }

  while (locationFile.available()) {
    String locationLine = locationFile.readStringUntil('\n');
    locationLine.trim();

    int commaIndex = locationLine.indexOf(',');
    if (commaIndex <= 0) {
      Serial.printf("[GPS DEBUG] Skipping bad line: \"%s\"\n", locationLine.c_str());
      continue;
    }

    String latitudeText = locationLine.substring(0, commaIndex);
    String longitudeText = locationLine.substring(commaIndex + 1);
    latestData.latitude = latitudeText.toDouble();
    latestData.longitude = longitudeText.toDouble();
    latestData.newDataAvailable = true;
    return true;
  }

  return false;
}

void setupGPSModule() {
  if (!SPIFFS.begin(true)) {
    Serial.println("CRITICAL: Failed to mount SPIFFS for location replay.");
    return;
  }

  locationFile = SPIFFS.open("/location_data.txt", "r");
  if (!locationFile) {
    Serial.println("CRITICAL: Could not open /location_data.txt.");
    return;
  }

  Serial.printf("[GPS DEBUG] File opened. Size: %d bytes\n", locationFile.size());

  Serial.println("Location replay ready. Reading /location_data.txt every 3 seconds.");
  bool gotFirstLine = readNextLocation();
  Serial.printf("[GPS DEBUG] First line read: %s | Lat: %.7f, Lng: %.7f\n",
                gotFirstLine ? "OK" : "FAILED",
                latestData.latitude, latestData.longitude);
  lastLocationUpdateMillis = millis() - LOCATION_UPDATE_INTERVAL_MILLIS;
}

GPSCoordinates checkAndGetGPS() {
  latestData.newDataAvailable = false;

  unsigned long now = millis();
  if (now - lastLocationUpdateMillis >= LOCATION_UPDATE_INTERVAL_MILLIS) {
    if (readNextLocation()) {
      lastLocationUpdateMillis = now;
      Serial.printf("[Location Replay] Lat: %.12f, Lng: %.12f\n",
                    latestData.latitude,
                    latestData.longitude);
    }
  }

  return latestData; 
}

GPSCoordinates getLatestGPS() {
  return latestData;
}