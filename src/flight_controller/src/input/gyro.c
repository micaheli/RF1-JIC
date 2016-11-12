//#include "includes.h"

#include <stdbool.h>
#include <stdint.h>

#include "includes.h"

float dpsGyroArray[3] = {0.0f, 0.0f, 0.0f};
float geeForceAccArray[3] = {0.0f, 0.0f, 0.0f};
static int32_t gyroSum[3] = {0, 0, 0};

volatile uint32_t gyroCalibrationCycles = GYRO_CALIBRATION_CYCLES * 1;

void ResetGyroCalibration(void) {

	bzero(gyroSum,sizeof(gyroSum));
	gyroCalibrationCycles = GYRO_CALIBRATION_CYCLES * 1;

}

static inline void InlineUpdateCalibration(int32_t *rawGyro)
{

	int32_t gyroCalibration[3];
	int32_t axis;

    if (gyroCalibrationCycles <= GYRO_CALIBRATION_CYCLES) {
        for (axis = 2; axis >= 0; axis--) {
            gyroSum[axis] += rawGyro[axis];
        }
    }

    if (--gyroCalibrationCycles == 0) {
        for (axis = 2; axis >= 0; axis--) {
            // add what comes out to be 1/2 to improve rounding
            gyroCalibration[axis] = (int32_t)(-(gyroSum[axis] + (GYRO_CALIBRATION_CYCLES / 2)) / GYRO_CALIBRATION_CYCLES);
        }

        accgyroDeviceCalibrate(gyroCalibration);
    }
}

inline void InlineUpdateAcc(int32_t rawAcc[], float scale)
{

    InlineApplyGyroAccRotationAndScale(rawAcc, geeForceAccArray, scale);

    InlineUpdateAttitude(geeForceAccArray);
}

inline void InlineUpdateGyro(int32_t rawGyro[], float scale)
{

    if (gyroCalibrationCycles != 0) {
        InlineUpdateCalibration(rawGyro);
        return;
    }

    accgyroDeviceApplyCalibration(rawGyro);

    InlineApplyGyroAccRotationAndScale(rawGyro, dpsGyroArray, scale);

    InlineFlightCode(dpsGyroArray);
}

inline void InlineApplyGyroAccRotationAndScale (int32_t rawData[], float dataArray[], float scale ) {

	//from gyro, x, y, z (0, 1, 2)
	// x is roll, y is pitch, z is yaw

    switch (mainConfig.gyroConfig.gyroRotation) {

        case CW90:
        	dataArray[Y] = ((float)rawData[Y] * scale);
        	dataArray[Z] = ((float)rawData[X] * -scale);
        	dataArray[X] = ((float)rawData[Z] * scale);
            break;
        case CW180:
        	dataArray[Y] = ((float)rawData[X] * -scale);
        	dataArray[Z] = ((float)rawData[Y] * -scale);
        	dataArray[X] = ((float)rawData[Z] * scale);
            break;
        case CW270:
        	dataArray[Y] = ((float)rawData[Y] * -scale);
        	dataArray[Z] = ((float)rawData[X] * scale);
        	dataArray[X] = ((float)rawData[Z] * scale);
            break;
        case CW0_INV:
        	dataArray[Y] = ((float)rawData[X] * -scale);
        	dataArray[Z] = ((float)rawData[Y] * scale);
        	dataArray[X] = ((float)rawData[Z] * -scale);
            break;
        case CW90_INV:
        	dataArray[Y] = ((float)rawData[Y] * scale);
        	dataArray[Z] = ((float)rawData[X] * scale);
        	dataArray[X] = ((float)rawData[Z] * -scale);
            break;
        case CW180_INV:
        	dataArray[Y] = ((float)rawData[X] * scale);
        	dataArray[Z] = ((float)rawData[Y] * -scale);
        	dataArray[X] = ((float)rawData[Z] * -scale);
            break;
        case CW270_INV:
        	dataArray[Y] = ((float)rawData[Y] * -scale);
        	dataArray[Z] = ((float)rawData[X] * -scale);
        	dataArray[X] = ((float)rawData[Z] * -scale);
            break;
        case CW0:
        default:
        	dataArray[Y] = ((float)rawData[X] * scale);
        	dataArray[Z] = ((float)rawData[Y] * scale);
        	dataArray[X] = ((float)rawData[Z] * scale);
            break;
    }

}
