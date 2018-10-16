#ifndef ALGORITHMS_AHRS_H_
#define ALGORITHMS_AHRS_H_

// Implementation of Madgwick's IMU and AHRS algorithms.

#include <math.h>
#include <stdint.h>
#include "Drivers/includes.h"

void AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void AHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);

float getRoll();
float getPitch();
float getYaw();

#endif /* ALGORITHMS_AHRS_H_ */
