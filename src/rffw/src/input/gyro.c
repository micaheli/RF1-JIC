//#include "includes.h"

#include <stdbool.h>
#include <stdint.h>

#include "usbd_hid.h"
#include "usb_device.h"

#include "drivers/invensense_device.h"

#include "input/gyro.h"

#define CALIBRATION_CYCLES 1000

static int8_t hidBuffer[4];

static uint32_t calibrationCycles = CALIBRATION_CYCLES * 100;

static void updateCalibration(int16_t *rawGyro)
{
    static int32_t gyroSum[3] = {0, 0, 0};
    int16_t gyroCalibration[3];
    uint8_t axis;

    if (calibrationCycles <= CALIBRATION_CYCLES) {
        for (axis = 0; axis < 3; axis++) {
            gyroSum[axis] += rawGyro[axis];
        }
    }

    if (--calibrationCycles == 0) {
        for (axis = 0; axis < 3; axis++) {
            // add what comes out to be 1/2 to improve rounding
            gyroCalibration[axis] = (int16_t)(-(gyroSum[axis] + (CALIBRATION_CYCLES / 2)) / CALIBRATION_CYCLES);
        }

        accgyroDeviceCalibrate(gyroCalibration);
    }
}

void updateGyro(int16_t *rawGyro, float scale)
{
    if (calibrationCycles != 0) {
        updateCalibration(rawGyro);
        return;
    }

    accgyroDeviceApplyCalibration(rawGyro);

    // HID stuff
    hidBuffer[0] = 0;
    hidBuffer[1] = (int8_t)(rawGyro[0] * scale);
    hidBuffer[2] = (int8_t)(rawGyro[1] * scale);
    hidBuffer[3] = 0;

    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidBuffer, 4);
}
