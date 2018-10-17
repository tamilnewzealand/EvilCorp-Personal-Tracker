#include "Compass.h"

float calculateHeading(Vector3 accelerometer, Vector3 magnetometer) {
    // Compute direction of 'magnetic west' (Earth's y axis)
    const Vector3 magneticWest = VectorFastNormalise(VectorCrossProduct(accelerometer, magnetometer));

    // Compute direction of magnetic north (Earth's x axis)
    const Vector3 magneticNorth = VectorFastNormalise(VectorCrossProduct(magneticWest, accelerometer));

    // Calculate angular heading relative to magnetic north
    return RadiansToDegrees(atan2f(magneticWest.axis.x, magneticNorth.axis.x));
}