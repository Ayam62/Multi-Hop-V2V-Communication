#include "multihop_relay.h"
#include <esp_system.h>

namespace {
    constexpr size_t MESSAGE_CACHE_SIZE = 32;
    uint32_t seenMessageIds[MESSAGE_CACHE_SIZE] = {};
    bool cacheEntryUsed[MESSAGE_CACHE_SIZE] = {};
    size_t nextCacheIndex = 0;
    uint32_t messageSequence = esp_random() & 0x00FFFFFF;
}

namespace MultiHopRelay {
    uint32_t nextMessageId(uint8_t nodeId) {
        const uint32_t messageId = (static_cast<uint32_t>(nodeId) << 24) |
                                   (messageSequence & 0x00FFFFFF);
        messageSequence = (messageSequence + 1) & 0x00FFFFFF;
        rememberMessage(messageId);
        Serial.printf("[TX] New messageId=%u, hopCount=0\n", messageId);
        return messageId;
    }

    bool rememberMessage(uint32_t messageId) {
        for (size_t index = 0; index < MESSAGE_CACHE_SIZE; ++index) {
            if (cacheEntryUsed[index] && seenMessageIds[index] == messageId) {
                Serial.printf("[RX] Duplicate messageId=%u detected; suppressing packet.\n",
                              messageId);
                return false;
            }
        }

        seenMessageIds[nextCacheIndex] = messageId;
    cacheEntryUsed[nextCacheIndex] = true;
        nextCacheIndex = (nextCacheIndex + 1) % MESSAGE_CACHE_SIZE;
        return true;
    }

    bool shouldRelay(const AlertPacket &packet) {
        if (packet.hopCount >= MAX_HOPS) {
            Serial.printf("[RELAY] messageId=%u stopped at hopCount=%u (MAX_HOPS=%u).\n",
                          packet.msgID, packet.hopCount, MAX_HOPS);
            return false;
        }

        Serial.printf("[RELAY] messageId=%u will be relayed from hopCount=%u.\n",
                      packet.msgID, packet.hopCount);
        return true;
    }

    AlertPacket makeRelayPacket(const AlertPacket &packet) {
        AlertPacket relayPacket = packet;
        ++relayPacket.hopCount;
        Serial.printf("[RELAY] Forwarding messageId=%u with hopCount=%u.\n",
                      relayPacket.msgID, relayPacket.hopCount);
        return relayPacket;
    }
}
