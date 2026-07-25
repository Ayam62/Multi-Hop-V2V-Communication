#ifndef ESP_NOW_MODULE_H

#define ESP _NOW_MODULE_H
#include <Arduino.h>
#include "packet_structure.h"

bool initEspNow();
bool sendAlertPacket(const AlertPacket &packet);


#endif