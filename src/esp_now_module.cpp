#include "esp_now_module.h"
#include <WiFi.h>
#include <esp_now.h>

// Broadcast MAC Address to target all surrounding nodes
uint8_t broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Callback triggered whenever a transmission attempt completes
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("-> Packet Transmit Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAIL");
}

// Callback triggered whenever a raw packet drops out of the sky
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    // Check if the payload size matches our exact structural expectations
    if (len != sizeof(AlertPacket)) {
        Serial.println("Received packet with invalid structure size! Ignoring.");
        return;
    }

    // DESERIALIZATION: Cast raw byte array memory block directly into our C Struct
    AlertPacket incomingPacket;
    memcpy(&incomingPacket, incomingData, sizeof(AlertPacket));

    Serial.println("\n===== [ NEW ESP-NOW PACKET RECEIVED ] =====");
    Serial.printf("Sender Node ID: %d\n", incomingPacket.nodeID);
    Serial.printf("Message Sequence ID: %u\n", incomingPacket.msgID);
    Serial.printf("Coordinates   : Lat: %f, Lng: %f\n", incomingPacket.latitude, incomingPacket.longitude);
    Serial.printf("Sender Heading : %f°\n", incomingPacket.heading);
    Serial.printf("Current Hop    : %d\n", incomingPacket.hopCount);
    Serial.printf("Alert Type Code: %d\n", incomingPacket.msgType);
    Serial.println("===========================================");
}

bool initEspNow() {
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("CRITICAL: Failed to initialize ESP-NOW");
        return false;
    }

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

    // Register generic broadcast peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastMac, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to bind Broadcast peer configuration");
        return false;
    }
    return true;
}

bool sendAlertPacket(const AlertPacket &packet) {
    // SERIALIZATION: Pass the pointer to our memory block and its true size
    esp_err_t result = esp_now_send(broadcastMac, (uint8_t *) &packet, sizeof(AlertPacket));
    return (result == ESP_OK);
}