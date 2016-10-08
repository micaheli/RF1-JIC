//#include "includes.h"

#include <stdbool.h>
#include <stdint.h>

#include "includes.h"

#define CALIBRATION_CYCLES 1000

float dpsGyroArray[3] = {0.0f, 0.0f, 0.0f};

static uint32_t calibrationCycles = CALIBRATION_CYCLES * 1;

static inline void InlineUpdateCalibration(int16_t *rawGyro)
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

inline void InlineUpdateGyro(int16_t rawGyro[], int16_t rawGyroRotated[], float scale)
{

    if (calibrationCycles != 0) {
        InlineUpdateCalibration(rawGyro);
        return;
    }

    accgyroDeviceApplyCalibration(rawGyro);

    InlineApplyGyroRotation(rawGyro, rawGyroRotated);
    InlineApplyBoardRotationAndScale(rawGyroRotated, dpsGyroArray, scale);

    InlineFlightCode(dpsGyroArray);
}

inline void InlineApplyGyroRotation (int16_t rawGyro[], int16_t rawGyroRotated[]) {

	//from gyro, x, y, z (0, 1, 2)
	// x is roll, y is pitch, z is yaw

    switch (gyroConfig.gyroRotation) {

        case CW90:
        	rawGyroRotated[X] =  rawGyro[Y];
        	rawGyroRotated[Y] = -rawGyro[X];
        	rawGyroRotated[Z] =  rawGyro[Z];
            break;
        case CW180:
        	rawGyroRotated[X] = -rawGyro[X];
            rawGyroRotated[Y] = -rawGyro[Y];
            rawGyroRotated[Z] =  rawGyro[Z];
            break;
        case CW270:
        	rawGyroRotated[X] = -rawGyro[Y];
        	rawGyroRotated[Y] =  rawGyro[X];
        	rawGyroRotated[Z] =  rawGyro[Z];
            break;
        case CW0_INV:
        	rawGyroRotated[X] = -rawGyro[X];
        	rawGyroRotated[Y] =  rawGyro[Y];
        	rawGyroRotated[Z] = -rawGyro[Z];
            break;
        case CW90_INV:
        	rawGyroRotated[X] =  rawGyro[Y];
        	rawGyroRotated[Y] =  rawGyro[X];
        	rawGyroRotated[Z] = -rawGyro[Z];
            break;
        case CW180_INV:
        	rawGyroRotated[X] =  rawGyro[X];
        	rawGyroRotated[Y] = -rawGyro[Y];
            rawGyroRotated[Z] = -rawGyro[Z];
            break;
        case CW270_INV:
        	rawGyroRotated[X] = -rawGyro[Y];
        	rawGyroRotated[Y] = -rawGyro[X];
        	rawGyroRotated[Z] = -rawGyro[Z];
            break;
        case CW0:
        default:
        	rawGyroRotated[X] =  rawGyro[X];
        	rawGyroRotated[Y] =  rawGyro[Y];
        	rawGyroRotated[Z] =  rawGyro[Z];
            break;
    }

}

inline void InlineApplyBoardRotationAndScale ( int16_t rawGyroRotated[], float dpsGyroArray[], float scale ) {

	switch (gyroConfig.boardRotation) {
		case CW90:
			dpsGyroArray[ROLL]  = ((float)rawGyroRotated[Y]  * scale);
			dpsGyroArray[PITCH] = ((float)-rawGyroRotated[X] * scale);
			dpsGyroArray[YAW]   = ((float)rawGyroRotated[Z]  * scale);
			break;
		case CW180:
			dpsGyroArray[ROLL]  = ((float)-rawGyroRotated[X] * scale);
			dpsGyroArray[PITCH] = ((float)-rawGyroRotated[Y] * scale);
			dpsGyroArray[YAW]   = ((float)rawGyroRotated[Z]  * scale);
			break;
		case CW270:
			dpsGyroArray[ROLL]  = ((float)-rawGyroRotated[Y] * scale);
			dpsGyroArray[PITCH] = ((float)rawGyroRotated[X]  * scale);
			dpsGyroArray[YAW]   = ((float)rawGyroRotated[Z]  * scale);
			break;
		case CW0_INV:
			dpsGyroArray[ROLL]  = ((float)-rawGyroRotated[X] * scale);
			dpsGyroArray[PITCH] = ((float)rawGyroRotated[Y]  * scale);
			dpsGyroArray[YAW]   = ((float)-rawGyroRotated[Z] * scale);
			break;
		case CW90_INV:
			dpsGyroArray[ROLL]  = ((float)rawGyroRotated[Y]  * scale);
			dpsGyroArray[PITCH] = ((float)rawGyroRotated[X]  * scale);
			dpsGyroArray[YAW]   = ((float)-rawGyroRotated[Z] * scale);
			break;
		case CW180_INV:
			dpsGyroArray[ROLL]  = ((float)rawGyroRotated[X]  * scale);
			dpsGyroArray[PITCH] = ((float)-rawGyroRotated[Y] * scale);
			dpsGyroArray[YAW]   = ((float)-rawGyroRotated[Z] * scale);
			break;
		case CW270_INV:
			dpsGyroArray[ROLL]  = ((float)-rawGyroRotated[Y] * scale);
			dpsGyroArray[PITCH] = ((float)-rawGyroRotated[X] * scale);
			dpsGyroArray[YAW]   = ((float)-rawGyroRotated[Z] * scale);
			break;
		case CW0:
		default:
			dpsGyroArray[ROLL]  = ((float)rawGyroRotated[X]  * scale);
			dpsGyroArray[PITCH] = ((float)rawGyroRotated[Y]  * scale);
			dpsGyroArray[YAW]   = ((float)rawGyroRotated[Z]  * scale);
			break;
	}

}
