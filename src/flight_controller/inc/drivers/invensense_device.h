#pragma once

// all MPU device drivers (e.g. mpu9250) should implement the following
// functions
int accgyroDeviceInit(loopCtrl_e gyroLoop);
int accgyroDeviceDetect(void);

void accgyroDeviceReadGyro(void);
void accgyroDeviceReadAccGyro(void);
void accgyroDeviceReadComplete(void);

void accgyroDeviceCalibrate(int16_t *gyroData);
void accgyroDeviceApplyCalibration(int16_t *gyroData);
