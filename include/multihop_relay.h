#ifndef MULTIHOP_RELAY_H
#define MULTIHOP_RELAY_H

#include <Arduino.h>
#include "packet_structure.h"

#ifndef MAX_HOPS
#define MAX_HOPS 3
#endif

namespace MultiHopRelay {
    uint32_t nextMessageId(uint8_t nodeId);
    bool rememberMessage(uint32_t messageId);
    bool shouldRelay(const AlertPacket &packet);
    AlertPacket makeRelayPacket(const AlertPacket &packet);
}

#endif
