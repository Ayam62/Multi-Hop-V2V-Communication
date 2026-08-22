#include "esp_now_module.h"
#include "directional_awareness.h"
#include "gps_module.h"
#include "multihop_relay.h"
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

    Serial.printf("[RX] messageId=%u from node #%u, hopCount=%u.\n",
                  incomingPacket.msgID, incomingPacket.nodeID, incomingPacket.hopCount);
    if (!MultiHopRelay::rememberMessage(incomingPacket.msgID)) {
        return;
    }

    
    const GPSCoordinates receiverLocation = getLatestGPS();
    const float receiverHeading = DirectionalAwareness::getLocalVehicleHeading();
    
    const bool relevantAlert = DirectionalAwareness::isLocationalAlertRelevant(
        incomingPacket.msgType,
        receiverHeading,
        incomingPacket.heading,
        receiverLocation.latitude,
        receiverLocation.longitude,
        incomingPacket.latitude,
        incomingPacket.longitude,
        60.0f
    );

    if (!relevantAlert) {
        Serial.printf("[RELEVANCE] messageId=%u from node #%u not relevant; ignoring and not relaying. Sender=%0.1f°, Receiver=%0.1f°, Alert=%s\n",
                      incomingPacket.msgID,
                      incomingPacket.nodeID,
                      incomingPacket.heading,
                      receiverHeading,
                      getAlertDescription(incomingPacket.msgType));
        return;
    }

    Serial.printf("[RELEVANCE] messageId=%u is relevant; processing alert.\n",
                  incomingPacket.msgID);

    Serial.println("\n===== [ NEW ESP-NOW PACKET RECEIVED ] =====");
    Serial.printf("Sender Node ID: %d\n", incomingPacket.nodeID);
    Serial.printf("Message Sequence ID: %u\n", incomingPacket.msgID);
    Serial.printf("Coordinates   : Lat: %f, Lng: %f\n", incomingPacket.latitude, incomingPacket.longitude);
    Serial.printf("Sender Heading : %f°\n", incomingPacket.heading);
    Serial.printf("Receiver Heading: %f°\n", receiverHeading);
    Serial.printf("Current Hop    : %d\n", incomingPacket.hopCount);
    Serial.printf("Alert Type Code: %d\n", incomingPacket.msgType);
    Serial.printf("Alert Message  : %s\n", getAlertDescription(incomingPacket.msgType));
    Serial.println("===========================================");

    if (MultiHopRelay::shouldRelay(incomingPacket)) {
        const AlertPacket relayPacket = MultiHopRelay::makeRelayPacket(incomingPacket);
        if (!sendAlertPacket(relayPacket)) {
            Serial.printf("[RELAY] Failed to broadcast messageId=%u.\n", relayPacket.msgID);
        }
    }
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