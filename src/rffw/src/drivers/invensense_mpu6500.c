#include <stdbool.h>

#include "includes.h"

#include "drivers/invensense_bus.h"
#include "drivers/invensense_device.h"
#include "drivers/invensense_register_map.h"

#include "input/gyro.h"

// value returned on WHO_AM_I register
#define MPU6500_WHO_AM_I    0x70
#define MPU6555_WHO_AM_I    0x7C
#define MPU9250_WHO_AM_I    0x71

typedef struct __attribute__((__packed__)) {
    uint8_t address; // needed to start rx/tx transfer when sending address
    uint8_t gyroX_H;
    uint8_t gyroX_L;
    uint8_t gyroY_H;
    uint8_t gyroY_L;
    uint8_t gyroZ_H;
    uint8_t gyroZ_L;
} gyroFrame_t;

static gyroFrame_t gyroRxFrame;
static gyroFrame_t gyroTxFrame;

static int16_t gyroData[3];
static int16_t gyroCal[3];

bool accgyroDeviceInit(void)
{
    // reset gyro
    accgyroWriteRegister(INVENS_RM_PWR_MGMT_1, INVENS_CONST_H_RESET);
    HAL_Delay(150);

    // set gyro clock to Z axis gyro
    if (!accgyroVerifyWriteRegister(INVENS_RM_PWR_MGMT_1, INVENS_CONST_CLK_PLL)) {
        return false;
    }

    // clear low power states
    accgyroWriteRegister(INVENS_RM_PWR_MGMT_2, 0);

    // disable I2C Interface, clear fifo, and reset sensor signal paths
    accgyroWriteRegister(INVENS_RM_USER_CTRL, INVENS_CONST_I2C_IF_DIS | INVENS_CONST_FIFO_RESET | INVENS_CONST_SIG_COND_RESET);

    // set gyro full scale to +/- 2000 deg / sec
    if (!accgyroVerifyWriteRegister(INVENS_RM_GYRO_CONFIG, INVENS_CONST_FSR_2000DPS << 3 | FCB_DISABLE)) { // 8kHz
        return false;
    }

    // set accel full scale to +/- 8g
    if (!accgyroVerifyWriteRegister(INVENS_RM_ACCEL_CONFIG, INVENS_CONST_FSR_8G << 3)) {
        return false;
    }

    // gyro DLPF config
    if (!accgyroVerifyWriteRegister(INVENS_RM_CONFIG, 0)) { // 8kHz
        return false;
    }

    // set gyro sample divider rate
    accgyroWriteRegister(INVENS_RM_SMPLRT_DIV, 0); // currently denom of 1

    // set interrupt pin PP, 50uS pulse, status cleared on read, i2c bypass
    if (!accgyroVerifyWriteRegister(INVENS_RM_INT_PIN_CFG, INVENS_CONST_INT_RD_CLEAR | INVENS_CONST_BYPASS_EN)) {
        return false;
    }

#ifdef GYRO_EXTI
    // enable data ready interrupt
    if (!accgyroVerifyWriteRegister(INVENS_RM_INT_ENABLE, INVENS_CONST_DATA_RDY_EN)) {
        return false;
    }
#endif

    return true;
}

bool accgyroDeviceDetect(void)
{
    uint8_t attempt, data;

    // reset gyro
    accgyroWriteRegister(INVENS_RM_PWR_MGMT_1, INVENS_CONST_H_RESET);

    // poll for the who am i register while device resets
    for (attempt = 0; attempt < 20; attempt++) {
        HAL_Delay(100);

        accgyroReadData(INVENS_RM_WHO_AM_I, &data, 1);
        if (data == MPU6500_WHO_AM_I || data == MPU6555_WHO_AM_I || data == MPU9250_WHO_AM_I) {
            break;
        }
    }
    if (attempt == 20) {
        return false;
    }

    return true;
}

void accgyroDeviceReadGyro(void)
{
    gyroTxFrame.address = INVENS_RM_GYRO_XOUT_H | 0x80;

    accgyroDMAReadWriteData((uint8_t *)&gyroTxFrame, (uint8_t *)&gyroRxFrame, 7);
}

void accgyroDeviceReadGyroComplete(void)
{
    gyroData[0] = (int16_t)((gyroRxFrame.gyroX_H << 8) | gyroRxFrame.gyroX_L);
    gyroData[1] = (int16_t)((gyroRxFrame.gyroY_H << 8) | gyroRxFrame.gyroY_L);
    gyroData[2] = (int16_t)((gyroRxFrame.gyroZ_H << 8) | gyroRxFrame.gyroZ_L);

    updateGyro(gyroData, 1.f / 16.4f);
}

void accgyroDeviceCalibrate(int16_t *gyroData)
{
    // TODO - load this into the gyro offset register
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
