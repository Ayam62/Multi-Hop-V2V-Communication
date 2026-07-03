#ifndef PACKET_STRUCTURE_H //if not defined 
#define PACKET_STRUCTURE_H // define

#include <Arduino.h>

struct __attribute__((__packed__)) AlertPacket{
    uint8_t nodeID;
    uint32_t msgID;
    float latitude;
    float longitude;
    float heading;
    uint8_t hopCount;
    uint8_t msgType;
};

#endif