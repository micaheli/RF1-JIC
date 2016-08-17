#pragma once

// all MPU device drivers (e.g. mpu9250) should implement the following
// functions
bool accgyroDeviceInit(loopCtrl_e gyroLoop);
bool accgyroDeviceDetect(void);

void accgyroDeviceReadGyro(void);
void accgyroDeviceReadAccGyro(void);
void accgyroDeviceReadComplete(void);

void accgyroDeviceCalibrate(int16_t *gyroData);
void accgyroDeviceApplyCalibration(int16_t *gyroData);
