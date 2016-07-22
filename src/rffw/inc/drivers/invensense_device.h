#pragma once

// all MPU device drivers (e.g. mpu9250) should implement the following
// functions
bool accgyroDeviceInit(void);
bool accgyroDeviceDetect(void);

void accgyroDeviceReadGyro(void);
void accgyroDeviceReadGyroComplete(void);
