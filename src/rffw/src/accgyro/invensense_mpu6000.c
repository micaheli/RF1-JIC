#include <stdbool.h>

#include "includes.h"

#include "accgyro/invensense_bus.h"
#include "accgyro/invensense_device.h"
#include "accgyro/invensense_register_map.h"

#include "input/gyro.h"

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
    //uint8_t address; // needed to start rx/tx transfer when sending address
    uint8_t gyroX_H;
    uint8_t gyroX_L;
    uint8_t gyroY_H;
    uint8_t gyroY_L;
    uint8_t gyroZ_H;
    uint8_t gyroZ_L;
} gyroFrame_t;

static gyroFrame_t gyroRxFrame;
//static gyroFrame_t gyroTxFrame;

int16_t gyroData[3];

bool accgyroDeviceInit(void)
{
    // reset gyro
    accgyroWriteRegister(INVENS_RM_PWR_MGMT_1, INVENS_CONST_H_RESET);
    HAL_Delay(150);

    // set gyro clock to Z axis gyro
    if (!accgyroVerifyWriteRegister(INVENS_RM_PWR_MGMT_1, INVENS_CONST_CLK_Z)) {
        return false;
    }

    // clear low power states
    accgyroWriteRegister(INVENS_RM_PWR_MGMT_2, 0);

    // disable I2C Interface, clear fifo, and reset sensor signal paths
    accgyroWriteRegister(INVENS_RM_USER_CTRL, INVENS_CONST_I2C_IF_DIS | INVENS_CONST_FIFO_RESET | INVENS_CONST_SIG_COND_RESET);

    // set gyro full scale to +/- 2000 deg / sec
    if (!accgyroVerifyWriteRegister(INVENS_RM_GYRO_CONFIG, INVENS_CONST_FSR_2000DPS << 3)) {
        return false;
    }

    // set accel full scale to +/- 8g
    if (!accgyroVerifyWriteRegister(INVENS_RM_ACCEL_CONFIG, INVENS_CONST_FSR_8G << 3)) {
        return false;
    }

    // set gyro sample divider rate
    accgyroWriteRegister(INVENS_RM_SMPLRT_DIV, 0); // currently denom of 1

    // set interrupt pin PP, 50uS pulse, status cleared on INT_STATUS read
    if (!accgyroVerifyWriteRegister(INVENS_RM_INT_PIN_CFG, INVENS_CONST_INT_RD_CLEAR)) {
        return false;
    }

#ifdef GYRO_EXTI_GPIO_Port
    // enable data ready interrupt
    if (!accgyroVerifyWriteRegister(INVENS_RM_INT_ENABLE, INVENS_CONST_DATA_RDY_EN)) {
        return false;
    }
#endif

    // gyro DLPF config
    if (!accgyroVerifyWriteRegister(INVENS_RM_CONFIG, 0)) { // 8kHz
        return false;
    }

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

        accgyroReadRegister(INVENS_RM_WHO_AM_I, 1, &data);
        if (data == MPU6000_WHO_AM_I) {
            break;
        }
    }
    if (attempt == 20) {
        return false;
    }

    // read the product id
    accgyroReadRegister(INVENS_RM_PRODUCT_ID, 1, &data);

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

void accgyroDeviceReadGyro(void)
{
    //gyroTxFrame.address = INVENS_RM_GYRO_XOUT_H | 0x80;
    uint8_t address = INVENS_RM_GYRO_XOUT_H;

    accgyroDMAReadRegister(address, (uint8_t *)&gyroRxFrame, 6);

    //updateGyro(gyroData, 1.f / 16.4f);
}



void accgyroDeviceReadGyroComplete(void)
{
    gyroData[0] = (int16_t)((gyroRxFrame.gyroX_H << 8) | gyroRxFrame.gyroX_L);
    gyroData[1] = (int16_t)((gyroRxFrame.gyroY_H << 8) | gyroRxFrame.gyroY_L);
    gyroData[2] = (int16_t)((gyroRxFrame.gyroZ_H << 8) | gyroRxFrame.gyroZ_L);

    updateGyro(gyroData, 1.f / 16.4f);
}
