#include "directional_awareness.h"
#include <math.h>

namespace DirectionalAwareness {

static float localVehicleHeading = 0.0f;

void setLocalVehicleHeading(float headingDegrees) {
    // FIXED: Removed the `fabsf(headingDegrees) < 1e-6f` check. 
    // A heading of 0.0 (North) is perfectly valid and should not be ignored.
    localVehicleHeading = normalizeHeading(headingDegrees);
}

float getLocalVehicleHeading() {
    return localVehicleHeading;
}

float normalizeHeading(float headingDegrees) {
    while (headingDegrees < 0.0f) {
        headingDegrees += 360.0f;
    }
    while (headingDegrees >= 360.0f) {
        headingDegrees -= 360.0f;
    }
    return headingDegrees;
}

float calculateHeadingDelta(float referenceHeading, float targetHeading) {
    float normalizedReference = normalizeHeading(referenceHeading);
    float normalizedTarget = normalizeHeading(targetHeading);

    float delta = normalizedTarget - normalizedReference;

    if (delta > 180.0f) {
        delta -= 360.0f;
    } else if (delta < -180.0f) {
        delta += 360.0f;
    }

    return delta;
}

float calculateHeadingFromCoordinates(float previousLatitude,
                                     float previousLongitude,
                                     float currentLatitude,
                                     float currentLongitude) {
    float latDelta = currentLatitude - previousLatitude;
    float lonDelta = currentLongitude - previousLongitude;

    if (fabsf(latDelta) < 1e-7f && fabsf(lonDelta) < 1e-7f) {
        return 0.0f;
    }

    // Geographic approximation: x=latDelta (North/South), y=lonDelta (East/West)
    float heading = atan2f(lonDelta, latDelta) * 180.0f / PI;
    heading = normalizeHeading(heading);
    return heading;
}

float calculateBearingToTarget(float receiverLatitude,
                              float receiverLongitude,
                              float targetLatitude,
                              float targetLongitude) {
    float latDelta = targetLatitude - receiverLatitude;
    float lonDelta = targetLongitude - receiverLongitude;

    if (fabsf(latDelta) < 1e-7f && fabsf(lonDelta) < 1e-7f) {
        return 0.0f;
    }

    float bearing = atan2f(lonDelta, latDelta) * 180.0f / PI;
    return normalizeHeading(bearing);
}

RelativeTrafficPosition getRelativeTrafficPosition(float receiverHeading,
                                                  float senderHeading,
                                                  float receiverLatitude,
                                                  float receiverLongitude,
                                                  float senderLatitude,
                                                  float senderLongitude,
                                                  float headingToleranceDegrees) {
    const float sameDirectionDelta = fabsf(calculateHeadingDelta(receiverHeading, senderHeading));
    const float bearingToSender = calculateBearingToTarget(receiverLatitude, receiverLongitude,
                                                          senderLatitude, senderLongitude);
    const float relativeToReceiver = calculateHeadingDelta(receiverHeading, bearingToSender);

    const bool sameLane = sameDirectionDelta <= headingToleranceDegrees;
    const bool oppositeLane = sameDirectionDelta >= (180.0f - headingToleranceDegrees);

    if (sameLane) {
        if (relativeToReceiver >= -90.0f && relativeToReceiver <= 90.0f) {
            return POSITION_FRONT;
        }
        return POSITION_REAR;
    }

    if (oppositeLane) {
        if (relativeToReceiver >= -90.0f && relativeToReceiver <= 90.0f) {
            return POSITION_OPPOSITE_APPROACHING;
        }
        return POSITION_OPPOSITE_AWAY;
    }

    return POSITION_SIDE;
}

bool isSameDirection(float headingA, float headingB, float toleranceDegrees) {
    float delta = fabsf(calculateHeadingDelta(headingA, headingB));
    return delta <= toleranceDegrees;
}

bool isDirectionalAlertRelevant(uint8_t msgType,
                               float senderHeading,
                               float receiverHeading,
                               float toleranceDegrees) {
    bool relevant = true;

    // UPDATED: Aligned directional relevance with the new locational rules
    switch (msgType) {
        case ALERT_EMERGENCY:
        case ALERT_VISIBILITY:
        case ALERT_OBSTACLE:
        case ALERT_HARD_BRAKE:
            // These alerts require the sender to be in the same direction (front or rear)
            relevant = isSameDirection(senderHeading, receiverHeading, toleranceDegrees);
            break;

        case ALERT_ACCIDENT:
        case ALERT_HAZARD:
        default:
            // Receive from all locations/directions
            relevant = true;
            break;
    }

    return relevant;
}

bool isLocationalAlertRelevant(uint8_t msgType,
                              float receiverHeading,
                              float senderHeading,
                              float receiverLatitude,
                              float receiverLongitude,
                              float senderLatitude,
                              float senderLongitude,
                              float headingToleranceDegrees) {
    const RelativeTrafficPosition relativePosition = getRelativeTrafficPosition(
        receiverHeading,
        senderHeading,
        receiverLatitude,
        receiverLongitude,
        senderLatitude,
        senderLongitude,
        headingToleranceDegrees
    );

    // UPDATED: Applied your exact filtering rules
    switch (msgType) {
        case ALERT_EMERGENCY:
            // Receive the alert if sender is behind
            return relativePosition == POSITION_REAR;

        case ALERT_ACCIDENT:
        case ALERT_HAZARD:
            // Receive from all locations
            return true;

        case ALERT_VISIBILITY:
        case ALERT_OBSTACLE:
        case ALERT_HARD_BRAKE:
            // Receive the alert if sender is in front
            return relativePosition == POSITION_FRONT;

        default:
            return true;
    }
}

} // namespace DirectionalAwareness