#ifndef ESP_NOW_MODULE_H

#define ESP_NOW_MODULE_H
#include <Arduino.h>
#include "packet_structure.h"

extern const uint8_t MY_NODE_ID;

bool initEspNow();
bool sendAlertPacket(const AlertPacket &packet);

#endif