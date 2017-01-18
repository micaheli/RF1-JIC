#pragma once

// all MPU device drivers (e.g. mpu9250) should implement the following
// functions

typedef struct {
    uint8_t rateDiv;
    uint8_t gyroDlpf;
    uint8_t gyroDlpfBypass;
    uint8_t accDlpf;
    uint8_t accDlpfBypass;
    uint8_t accDenom;
} gyro_device_config;

extern gyro_device_config gyroConfig;
extern uint32_t deviceWhoAmI;

int AccGyroDeviceInit(loopCtrl_e gyroLoop);
int AccGyroDeviceDetect(void);

void accgyroDeviceReadGyro(void);
void accgyroDeviceReadAccGyro(void);
void accgyroDeviceReadComplete(void);

void accgyroDeviceCalibrate(int32_t *gyroData);
void accgyroDeviceApplyCalibration(int32_t *gyroData);
