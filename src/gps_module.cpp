#include "gps_module.h"
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
GPSCoordinates latestData = {0.0, 0.0, false};

void setupGPSModule() {
  SerialBT.begin("ESP32_V2V_Node");
  Serial.println("Bluetooth is ready! Pair your phone now.");
}

GPSCoordinates checkAndGetGPS() {
  latestData.newDataAvailable = false;

  if (SerialBT.available()) {
    String incomingGPS = SerialBT.readStringUntil('\n');
    incomingGPS.trim();

    int commaIndex = incomingGPS.indexOf(',');
    if (commaIndex > 0) {
      String latitudeText = incomingGPS.substring(0, commaIndex);
      String longitudeText = incomingGPS.substring(commaIndex + 1);

      latestData.latitude = latitudeText.toDouble();
      latestData.longitude = longitudeText.toDouble();
      latestData.newDataAvailable = true;
      Serial.printf("[Bluetooth GPS] Lat: %.12f, Lng: %.12f\n",
                    latestData.latitude,
                    latestData.longitude);
    } else {
      Serial.printf("[GPS DEBUG] Ignoring malformed location: \"%s\"\n",
                    incomingGPS.c_str());
    }
  }

  return latestData;
}

GPSCoordinates getLatestGPS() {
  return latestData;
}