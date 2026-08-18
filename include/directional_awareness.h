#ifndef DIRECTIONAL_AWARENESS_H
#define DIRECTIONAL_AWARENESS_H

#include <Arduino.h>
#include "alert_type.h"

namespace DirectionalAwareness {
    enum RelativeTrafficPosition {
        POSITION_FRONT,
        POSITION_REAR,
        POSITION_OPPOSITE_APPROACHING,
        POSITION_OPPOSITE_AWAY,
        POSITION_SIDE
    };

    void setLocalVehicleHeading(float headingDegrees);
    float getLocalVehicleHeading();
    float normalizeHeading(float headingDegrees);
    float calculateHeadingDelta(float referenceHeading, float targetHeading);
    float calculateHeadingFromCoordinates(float previousLatitude,
                                         float previousLongitude,
                                         float currentLatitude,
                                         float currentLongitude);
    float calculateBearingToTarget(float receiverLatitude,
                                  float receiverLongitude,
                                  float targetLatitude,
                                  float targetLongitude);
    RelativeTrafficPosition getRelativeTrafficPosition(float receiverHeading,
                                                      float senderHeading,
                                                      float receiverLatitude,
                                                      float receiverLongitude,
                                                      float senderLatitude,
                                                      float senderLongitude,
                                                      float headingToleranceDegrees = 60.0f);
    bool isSameDirection(float headingA, float headingB, float toleranceDegrees = 60.0f);
    bool isDirectionalAlertRelevant(uint8_t msgType,
                                   float senderHeading,
                                   float receiverHeading,
                                   float toleranceDegrees = 60.0f);
    bool isLocationalAlertRelevant(uint8_t msgType,
                                  float receiverHeading,
                                  float senderHeading,
                                  float receiverLatitude,
                                  float receiverLongitude,
                                  float senderLatitude,
                                  float senderLongitude,
                                  float headingToleranceDegrees = 60.0f);
}

#endif // DIRECTIONAL_AWARENESS_H
