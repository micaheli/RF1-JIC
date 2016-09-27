#include <stdbool.h>

#include "includes.h"

#include "input/gyro.h"

#include "drivers/invensense_bus.h"
#include "drivers/invensense_device.h"
#include "drivers/invensense_register_map.h"

// value returned on WHO_AM_I register
#define MPU6000_WHO_AM_I    0x68

// Product ID Description for MPU6000
// Product Name Product Revision
#define MPU6000ES_REV_C4    0x14
#define MPU6000ES_REV_C5    0x15
#define MPU6000ES_REV_D6    0x16
#define MPU6000ES_REV_D7    0x17
#define MPU6000ES_REV_D8    0x18
#define MPU6000_REV_C4      0x54
#define MPU6000_REV_C5      0x55
#define MPU6000_REV_D6      0x56
#define MPU6000_REV_D7      0x57
#define MPU6000_REV_D8      0x58
#define MPU6000_REV_D9      0x59
#define MPU6000_REV_D10     0x5A

typedef struct __attribute__((__packed__)) {
    uint8_t accAddress;  // needed to start rx/tx transfer when sending address
    uint8_t accelX_H;
    uint8_t accelX_L;
    uint8_t accelY_H;
    uint8_t accelY_L;
    uint8_t accelZ_H;
    uint8_t accelZ_L;
    uint8_t dummy;        // otherwise TEMP_H
    uint8_t gyroAddress;  // otherwise TEMP_L
    uint8_t gyroX_H;
    uint8_t gyroX_L;
    uint8_t gyroY_H;
    uint8_t gyroY_L;
    uint8_t gyroZ_H;
    uint8_t gyroZ_L;
} gyroFrame_t;

static gyroFrame_t gyroRxFrame;
static gyroFrame_t gyroTxFrame;

typedef struct {
    uint8_t rateDiv;
    uint8_t gyroDlpf;
} gyro6000Config_t;

gyro6000Config_t mpu6000GyroConfig[] = {
    [LOOP_L1] = {1, INVENS_CONST_GYRO_DLPF_188},
    [LOOP_M1] = {8, INVENS_CONST_GYRO_DLPF_256},
    [LOOP_M2] = {4, INVENS_CONST_GYRO_DLPF_256},
    [LOOP_M4] = {2, INVENS_CONST_GYRO_DLPF_256},
    [LOOP_M8] = {1, INVENS_CONST_GYRO_DLPF_256},
    [LOOP_H1] = {8, INVENS_CONST_GYRO_DLPF_3600},
    [LOOP_H2] = {4, INVENS_CONST_GYRO_DLPF_3600},
    [LOOP_H4] = {2, INVENS_CONST_GYRO_DLPF_3600},
    [LOOP_H8] = {1, INVENS_CONST_GYRO_DLPF_3600},
};

static bool accelUpdate = false;
static int16_t gyroRotated[3];
static int16_t gyroData[3];
static int16_t accelData[3];
static int16_t gyroCal[3];

bool accgyroDeviceInit(loopCtrl_e gyroLoop)
{
    gyro6000Config_t gyroConfig;

    // the mpu6000 caps out at 8khz
    if (gyroLoop > LOOP_H8) {
        gyroLoop = LOOP_H8;
    }

    gyroConfig = mpu6000GyroConfig[gyroLoop];

    // reset gyro
    accgyroWriteRegister(INVENS_RM_PWR_MGMT_1, INVENS_CONST_H_RESET);
    HAL_Delay(150);

    // set gyro clock to Z axis gyro
    accgyroVerifyWriteRegister(INVENS_RM_PWR_MGMT_1, INVENS_CONST_CLK_Z);

    // clear low power states
    accgyroWriteRegister(INVENS_RM_PWR_MGMT_2, 0);

    // disable I2C Interface, clear fifo, and reset sensor signal paths
    // TODO: shouldn't disable i2c on non-spi
    accgyroWriteRegister(INVENS_RM_USER_CTRL, INVENS_CONST_I2C_IF_DIS | INVENS_CONST_FIFO_RESET | INVENS_CONST_SIG_COND_RESET);

    // set gyro sample divider rate
    accgyroVerifyWriteRegister(INVENS_RM_SMPLRT_DIV, gyroConfig.rateDiv - 1);

    // gyro DLPF config
    accgyroVerifyWriteRegister(INVENS_RM_CONFIG, gyroConfig.gyroDlpf);

    // set gyro full scale to +/- 2000 deg / sec
    accgyroVerifyWriteRegister(INVENS_RM_GYRO_CONFIG, INVENS_CONST_GYRO_FSR_2000DPS << 3);

    // set accel full scale to +/- 16g
    accgyroVerifyWriteRegister(INVENS_RM_ACCEL_CONFIG, INVENS_CONST_ACC_FSR_16G << 3);

    // set interrupt pin PP, 50uS pulse, status cleared on INT_STATUS read
    accgyroVerifyWriteRegister(INVENS_RM_INT_PIN_CFG, INVENS_CONST_INT_RD_CLEAR);

#ifdef GYRO_EXTI_GPIO_Port
    // enable data ready interrupt
    accgyroVerifyWriteRegister(INVENS_RM_INT_ENABLE, INVENS_CONST_DATA_RDY_EN);
#endif

    return true;
}

bool accgyroDeviceDetect(void)
{
    uint8_t attempt, data;

    // reset gyro
    accgyroWriteRegister(INVENS_RM_PWR_MGMT_1, INVENS_CONST_H_RESET);
    HAL_Delay(151);
    accgyroWriteRegister(INVENS_RM_PWR_MGMT_1, INVENS_CONST_H_RESET);

    // poll for the who am i register while device resets
    for (attempt = 0; attempt < 100; attempt++) {
        HAL_Delay(151);

        accgyroReadData(INVENS_RM_WHO_AM_I, &data, 1);
        if (data == MPU6000_WHO_AM_I) {
            break;
        }
    }
    if (attempt == 100) {
        return false;
    }

    // read the product id
    accgyroReadData(INVENS_RM_PRODUCT_ID, &data, 1);

    // if who am i and id match, return true
    switch (data) {
        case MPU6000ES_REV_C4:
        case MPU6000ES_REV_C5:
        case MPU6000_REV_C4:
        case MPU6000_REV_C5:
        case MPU6000ES_REV_D6:
        case MPU6000ES_REV_D7:
        case MPU6000ES_REV_D8:
        case MPU6000_REV_D6:
        case MPU6000_REV_D7:
        case MPU6000_REV_D8:
        case MPU6000_REV_D9:
        case MPU6000_REV_D10:
            return true;
    }

    return false;
}

void accgyroDeviceReadAccGyro(void)
{
    // start read from accel, set high bit to read
    gyroTxFrame.accAddress = INVENS_RM_ACCEL_XOUT_H | 0x80;

    accelUpdate = true;
    accgyroDMAReadWriteData(&gyroTxFrame.accAddress, &gyroRxFrame.accAddress, 15);
}

void accgyroDeviceReadGyro(void)
{
    // start read from gyro, set high bit to read
    gyroTxFrame.gyroAddress = INVENS_RM_GYRO_XOUT_H | 0x80;

    accelUpdate = false;
    accgyroDMAReadWriteData(&gyroTxFrame.gyroAddress, &gyroRxFrame.gyroAddress, 7);
}

void accgyroDeviceReadComplete(void)
{
    if (accelUpdate) {
        accelData[0] = (int16_t)((gyroRxFrame.accelX_H << 8) | gyroRxFrame.accelX_L);
        accelData[1] = (int16_t)((gyroRxFrame.accelY_H << 8) | gyroRxFrame.accelY_L);
        accelData[2] = (int16_t)((gyroRxFrame.accelZ_H << 8) | gyroRxFrame.accelZ_L);

        // TODO: updateAccel(accelData, 1.f / 2048.f);
    }

    gyroData[0] = (int16_t)((gyroRxFrame.gyroX_H << 8) | gyroRxFrame.gyroX_L);
    gyroData[1] = (int16_t)((gyroRxFrame.gyroY_H << 8) | gyroRxFrame.gyroY_L);
    gyroData[2] = (int16_t)((gyroRxFrame.gyroZ_H << 8) | gyroRxFrame.gyroZ_L);

    InlineUpdateGyro(gyroData, gyroRotated, 1.f / 16.4f);
}

void accgyroDeviceCalibrate(int16_t *gyroData)
{
    uint8_t idx;

    for (idx = 0; idx < 3; idx++) {
        gyroCal[idx] = gyroData[idx];
    }
}

void accgyroDeviceApplyCalibration(int16_t *gyroData)
{
    uint8_t idx;

    for (idx = 0; idx < 3; idx++) {
        gyroData[idx] += gyroCal[idx];
    }
}
