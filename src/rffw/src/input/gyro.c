//#include "includes.h"

#include <stdbool.h>
#include <stdint.h>

#include "includes.h"

#define CALIBRATION_CYCLES 1000

int16_t dpsGyroArray[3] = {0, 0, 0};

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

    dpsGyroArray[0] = (rawGyro[0] * scale);
    dpsGyroArray[1] = (rawGyro[1] * scale);
    dpsGyroArray[2] = (rawGyro[2] * scale);

}
