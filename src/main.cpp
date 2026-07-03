#include <Arduino.h>
#include "esp_now_module.h"
#include "packet_structure.h"
#include "gps_module.h"

const uint8_t MY_NODE_ID = 1; 
uint32_t messageSequenceCounter = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.printf("Initializing V2V Node #%d...\n", MY_NODE_ID);
    if (initEspNow()) {
        Serial.println("System initialization complete. Monitoring channel...");
    }
    setupGPSModule(); // Call the GPS module setup function
}

// MAKE SURE THIS EXACT BLOCK IS AT THE BOTTOM
void loop() {
    delay(5000);
    messageSequenceCounter++;
    Serial.printf("\n[Local Trigger] Generating Alert Serial #%u...\n", messageSequenceCounter);


    GPSCoordinates currentGPS = checkAndGetGPS();

    AlertPacket simulatedAlert;
    simulatedAlert.nodeID = MY_NODE_ID;
    simulatedAlert.msgID = messageSequenceCounter;
    simulatedAlert.latitude = currentGPS.latitude;  // Use the latitude from the GPS module
    simulatedAlert.longitude = currentGPS.longitude;  // Use the longitude from the GPS module
    simulatedAlert.heading = 184.50;      
    simulatedAlert.hopCount = 0;          
    simulatedAlert.msgType = 1;           

    // sendAlertPacket(simulatedAlert);
}