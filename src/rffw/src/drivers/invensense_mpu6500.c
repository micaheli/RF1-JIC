#include <stdbool.h>

#include "includes.h"

#include "input/gyro.h"

#include "drivers/invensense_bus.h"
#include "drivers/invensense_device.h"
#include "drivers/invensense_register_map.h"

// value returned on WHO_AM_I register
#define MPU6500_WHO_AM_I    0x70
#define MPU6555_WHO_AM_I    0x7C
#define MPU9250_WHO_AM_I    0x71

typedef struct __attribute__((__packed__)) {
    uint8_t accelAddress; // needed to start read/write transfer to send address
    uint8_t accelX_H;
    uint8_t accelX_L;
    uint8_t accelY_H;
    uint8_t accelY_L;
    uint8_t accelZ_H;
    uint8_t accelZ_L;
    uint8_t dummy;       // otherwise TEMP_H
    uint8_t gyroAddress; // location to start gyro read/writes, otherwise TEMP_L
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
    uint8_t gyroDlpfBypass;
    uint8_t accDlpf;
    uint8_t accDlpfBypass;
    uint8_t accDenom;
} gyro6500Config_t;

// TODO: check what the acc actually updates at when sample rate divider != 0
// we have assumed that sample divider rate effects the accelerometer update rate
// currently: L1 uses 1khz base rate for gyro and acc, gyro updates same as
//            acc update
//            everything else uses 8khz or 32khz gyro rate, so set acc to 4khz
//            acc update, get acc update every 2nd or 8th gyro update
static gyro6500Config_t mpu6500GyroConfig[] = {
    [LOOP_L1] = {1, INVENS_CONST_GYRO_DLPF_188, INVENS_CONST_GYRO_FCB_DISABLE, INVENS_CONST_ACC_DLPF_460, INVENS_CONST_ACC_FCB_DISABLE, 1},
    [LOOP_M1] = {8, INVENS_CONST_GYRO_DLPF_256, INVENS_CONST_GYRO_FCB_DISABLE, 0, INVENS_CONST_ACC_FCB_ENABLE, 2},
    [LOOP_M2] = {4, INVENS_CONST_GYRO_DLPF_256, INVENS_CONST_GYRO_FCB_DISABLE, 0, INVENS_CONST_ACC_FCB_ENABLE, 2},
    [LOOP_M4] = {2, INVENS_CONST_GYRO_DLPF_256, INVENS_CONST_GYRO_FCB_DISABLE, 0, INVENS_CONST_ACC_FCB_ENABLE, 2},
    [LOOP_M8] = {1, INVENS_CONST_GYRO_DLPF_256, INVENS_CONST_GYRO_FCB_DISABLE, 0, INVENS_CONST_ACC_FCB_ENABLE, 2},
    [LOOP_H1] = {8, INVENS_CONST_GYRO_DLPF_3600, INVENS_CONST_GYRO_FCB_DISABLE, 0, INVENS_CONST_ACC_FCB_ENABLE, 2},
    [LOOP_H2] = {4, INVENS_CONST_GYRO_DLPF_3600, INVENS_CONST_GYRO_FCB_DISABLE, 0, INVENS_CONST_ACC_FCB_ENABLE, 2},
    [LOOP_H4] = {2, INVENS_CONST_GYRO_DLPF_3600, INVENS_CONST_GYRO_FCB_DISABLE, 0, INVENS_CONST_ACC_FCB_ENABLE, 2},
    [LOOP_H8] = {1, INVENS_CONST_GYRO_DLPF_3600, INVENS_CONST_GYRO_FCB_DISABLE, 0, INVENS_CONST_ACC_FCB_ENABLE, 2},
    [LOOP_H16] = {2, 0, INVENS_CONST_GYRO_FCB_32_3600, 0, INVENS_CONST_ACC_FCB_ENABLE, 8},
    [LOOP_H32] = {1, 0, INVENS_CONST_GYRO_FCB_32_3600, 0, INVENS_CONST_ACC_FCB_ENABLE, 8},
    [LOOP_UH1] = {32, 0, INVENS_CONST_GYRO_FCB_32_8800, 0, INVENS_CONST_ACC_FCB_ENABLE, 8},
    [LOOP_UH2] = {16, 0, INVENS_CONST_GYRO_FCB_32_8800, 0, INVENS_CONST_ACC_FCB_ENABLE, 8},
    [LOOP_UH4] = {8, 0, INVENS_CONST_GYRO_FCB_32_8800, 0, INVENS_CONST_ACC_FCB_ENABLE, 8},
    [LOOP_UH8] = {4, 0, INVENS_CONST_GYRO_FCB_32_8800, 0, INVENS_CONST_ACC_FCB_ENABLE, 8},
    [LOOP_UH16] = {2, 0, INVENS_CONST_GYRO_FCB_32_8800, 0, INVENS_CONST_ACC_FCB_ENABLE, 8},
    [LOOP_UH32] = {1, 0, INVENS_CONST_GYRO_FCB_32_8800, 0, INVENS_CONST_ACC_FCB_ENABLE, 8},
};

static bool accelUpdate = false;
static int16_t gyroData[3];
static int16_t accelData[3];

bool accgyroDeviceInit(loopCtrl_e gyroLoop)
{
    gyro6500Config_t gyroConfig;

    // don't overflow array
    if (gyroLoop > LOOP_UH32) {
        gyroLoop = LOOP_UH32;
    }

    gyroConfig = mpu6500GyroConfig[gyroLoop];

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
    // TODO: shouldn't disable i2c on non-spi
    accgyroWriteRegister(INVENS_RM_USER_CTRL, INVENS_CONST_I2C_IF_DIS | INVENS_CONST_FIFO_RESET | INVENS_CONST_SIG_COND_RESET);

    // set gyro sample divider rate
    accgyroWriteRegister(INVENS_RM_SMPLRT_DIV, gyroConfig.rateDiv - 1);

    // gyro DLPF config
    if (!accgyroVerifyWriteRegister(INVENS_RM_CONFIG, gyroConfig.gyroDlpf)) {
        return false;
    }

    // set gyro full scale to +/- 2000 deg / sec and DLPF bypass
    if (!accgyroVerifyWriteRegister(INVENS_RM_GYRO_CONFIG, INVENS_CONST_GYRO_FSR_2000DPS << 3 | gyroConfig.gyroDlpfBypass)) {
        return false;
    }

    // set accel full scale to +/- 16g
    if (!accgyroVerifyWriteRegister(INVENS_RM_ACCEL_CONFIG, INVENS_CONST_ACC_FSR_16G << 3)) {
        return false;
    }

    // set the accelerometer dlpf
    if (!accgyroVerifyWriteRegister(INVENS_RM_ACCEL_CONFIG2, gyroConfig.accDlpfBypass << 3 | gyroConfig.accDlpf)) {
        return false;
    }

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
    for (attempt = 0; attempt < 100; attempt++) {
        HAL_Delay(100);

        accgyroReadData(INVENS_RM_WHO_AM_I, &data, 1);
        if (data == MPU6500_WHO_AM_I || data == MPU6555_WHO_AM_I || data == MPU9250_WHO_AM_I) {
            break;
        }
    }

    if (attempt == 100) {
        return false;
    }

    return true;
}

void accgyroDeviceReadAccGyro(void)
{
    // start read from accel, set high bit to read
    gyroTxFrame.accelAddress = INVENS_RM_ACCEL_XOUT_H | 0x80;

    accelUpdate = true;
    accgyroDMAReadWriteData(&gyroTxFrame.accelAddress, &gyroRxFrame.accelAddress, 15);
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

    updateGyro(gyroData, 1.f / 16.4f);
}

// TODO: this is broken - fix it
void accgyroDeviceCalibrate(int16_t *gyroData)
{
    skipGyro = true;

    if (!accgyroVerifyWriteRegister(INVENS_RM_XG_OFFSET_H, (uint8_t)(gyroData[0] >> 8))) {
        ErrorHandler();
    }

    if (!accgyroVerifyWriteRegister(INVENS_RM_XG_OFFSET_L, (uint8_t)(gyroData[0] & 0xFF))) {
        ErrorHandler();
    }

    if (!accgyroVerifyWriteRegister(INVENS_RM_YG_OFFSET_H, (uint8_t)(gyroData[1] >> 8))) {
        ErrorHandler();
    }

    if (!accgyroVerifyWriteRegister(INVENS_RM_YG_OFFSET_L, (uint8_t)(gyroData[1] & 0xFF))) {
        ErrorHandler();
    }

    if (!accgyroVerifyWriteRegister(INVENS_RM_ZG_OFFSET_H, (uint8_t)(gyroData[2] >> 8))) {
        ErrorHandler();
    }

    if (!accgyroVerifyWriteRegister(INVENS_RM_ZG_OFFSET_L, (uint8_t)(gyroData[2] & 0xFF))) {
        ErrorHandler();
    }

    skipGyro = false;
}

// because of offset registers, the gyro reading will already have calibration applied
void accgyroDeviceApplyCalibration(int16_t *gyroData)
{
    (void)gyroData;
}
