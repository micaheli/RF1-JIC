//#include "includes.h"

#include <stdint.h>

#include "input/gyro.h"

#define CALIBRATION_CYCLES 1000

static uint16_t calibrationCycles = CALIBRATION_CYCLES;

static int16_t gyroCalibration[3];

static void updateCalibration(int16_t *rawGyro)
{
    static int32_t gyroSum[3];
    uint8_t axis;

    for (axis = 0; axis < 3; axis++) {
        gyroSum[axis] += rawGyro[axis];
    }

    if (--calibrationCycles == 0) {
        for (axis = 0; axis < 3; axis++) {
            // add what comes out to be 1/2 to improve rounding
            gyroCalibration[axis] = (gyroCalibration[axis] + (CALIBRATION_CYCLES / 2)) / CALIBRATION_CYCLES;
        }
    }
}

void updateGyro(int16_t *rawGyro, float scale)
{
    uint8_t axis;

    if (calibrationCycles != 0) {
        updateCalibration(rawGyro);
        return;
    }

    for (axis = 0; axis < 3; axis++) {
        rawGyro[axis] -= gyroCalibration[axis];
    }
}
