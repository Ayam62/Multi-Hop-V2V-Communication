#include "esp_now_module.h"
#include <WiFi.h>
#include <esp_now.h>

// Broadcast MAC Address to target all surrounding nodes
uint8_t broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

const uint8_t MAX_HOPS = 6;
const uint32_t PACKET_TTL_MS = 10000UL; // remember each packet for 10 seconds

struct SeenPacket {
    uint8_t nodeID;
    uint32_t msgID;
    uint32_t firstSeenMs;
};

SeenPacket seenPackets[32];
uint8_t seenPacketCount = 0;

void purgeExpiredPackets() {
    uint32_t now = millis();
    uint8_t writeIndex = 0;

    for (uint8_t i = 0; i < seenPacketCount; ++i) {
        if ((now - seenPackets[i].firstSeenMs) <= PACKET_TTL_MS) {
            if (writeIndex != i) {
                seenPackets[writeIndex] = seenPackets[i];
            }
            writeIndex++;
        }
    }

    seenPacketCount = writeIndex;
}

bool isPacketSeen(uint8_t nodeID, uint32_t msgID) {
    purgeExpiredPackets();

    for (uint8_t i = 0; i < seenPacketCount; ++i) {
        if (seenPackets[i].nodeID == nodeID && seenPackets[i].msgID == msgID) {
            return true;
        }
    }
    return false;
}

void rememberPacket(uint8_t nodeID, uint32_t msgID) {
    purgeExpiredPackets();

    if (seenPacketCount < 32) {
        seenPackets[seenPacketCount].nodeID = nodeID;
        seenPackets[seenPacketCount].msgID = msgID;
        seenPackets[seenPacketCount].firstSeenMs = millis();
        seenPacketCount++;
    }
}

// Callback triggered whenever a transmission attempt completes
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("-> Packet Transmit Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAIL");
}

// Callback triggered whenever a raw packet drops out of the sky
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    if (len != sizeof(AlertPacket)) {
        Serial.println("Received packet with invalid structure size! Ignoring.");
        return;
    }

    AlertPacket incomingPacket;
    memcpy(&incomingPacket, incomingData, sizeof(AlertPacket));

    if (incomingPacket.nodeID == MY_NODE_ID) {
        Serial.println("Ignoring own packet to prevent self-loop.");
        return;
    }

    if (isPacketSeen(incomingPacket.nodeID, incomingPacket.msgID)) {
        Serial.println("Duplicate packet detected. Ignoring.");
        return;
    }
    rememberPacket(incomingPacket.nodeID, incomingPacket.msgID);

    Serial.println("\n===== [ NEW ESP-NOW PACKET RECEIVED ] =====");
    Serial.printf("Sender Node ID: %d\n", incomingPacket.nodeID);
    Serial.printf("Message Sequence ID: %u\n", incomingPacket.msgID);
    Serial.printf("Coordinates   : Lat: %f, Lng: %f\n", incomingPacket.latitude, incomingPacket.longitude);
    Serial.printf("Sender Heading : %f°\n", incomingPacket.heading);
    Serial.printf("Current Hop    : %d\n", incomingPacket.hopCount);
    Serial.printf("Alert Type Code: %d\n", incomingPacket.msgType);
    Serial.printf("Alert Message  : %s\n", getAlertDescription(incomingPacket.msgType));
    Serial.println("===========================================");

    if (incomingPacket.hopCount >= MAX_HOPS) {
        Serial.printf("Max hop count reached (%d). Not relaying further.\n", MAX_HOPS);
        return;
    }

    AlertPacket relayPacket = incomingPacket;
    relayPacket.hopCount++;

    Serial.printf("Relaying packet from node %d with new hop count %d\n",
                  relayPacket.nodeID, relayPacket.hopCount);

    esp_err_t result = esp_now_send(broadcastMac, (uint8_t *)&relayPacket, sizeof(relayPacket));
    Serial.println(result == ESP_OK ? "Relay broadcast queued." : "Relay broadcast failed.");
}

bool initEspNow() {
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("CRITICAL: Failed to initialize ESP-NOW");
        return false;
    }

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

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
    esp_err_t result = esp_now_send(broadcastMac, (uint8_t *)&packet, sizeof(AlertPacket));
    return (result == ESP_OK);
}