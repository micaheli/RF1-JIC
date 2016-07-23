#pragma once

// all MPU device drivers (e.g. mpu9250) should implement the following
// functions
bool accgyroDeviceInit(void);
bool accgyroDeviceDetect(void);

void accgyroDeviceReadGyro(void);
void accgyroDeviceReadGyroComplete(void);

void accgyroDeviceCalibrate(int16_t *gyroData);
void accgyroDeviceApplyCalibration(int16_t *gyroData);
