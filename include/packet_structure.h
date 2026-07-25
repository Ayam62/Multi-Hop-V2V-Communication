#ifndef PACKET_STRUCTURE_H
#define PACKET_STRUCTURE_H

#include <Arduino.h>
#include "alert_type.h"

struct AlertPacket {
    uint8_t nodeID;
    uint32_t msgID;
    float latitude;
    float longitude;
    float heading;
    uint8_t hopCount;
    uint8_t msgType; 
};

#endif