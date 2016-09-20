//#include "includes.h"

#include <stdbool.h>
#include <stdint.h>

#include "includes.h"

#include "usbd_hid.h"
#include "usb_device.h"

#include "input/gyro.h"

#include "drivers/invensense_device.h"

#define CALIBRATION_CYCLES 1000

extern uint8_t tInBuffer[];

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

    return;
    // HID stuff
    LED1_TOGGLE;
	tInBuffer[0] = 1;
	tInBuffer[1]=(int8_t)(rawGyro[0] * scale);;
	tInBuffer[2]=(int8_t)(rawGyro[1] * scale);;
	tInBuffer[3]=(int8_t)(rawGyro[2] * scale);;
	tInBuffer[4]=0x65;
	tInBuffer[5]=0x46;
	tInBuffer[6]=0x6c;
	tInBuffer[7]=0x69;
	tInBuffer[8]=0x67;
	tInBuffer[9]=0x68;
	tInBuffer[10]=0x74;
	tInBuffer[11]=0x20;
	tInBuffer[12]=0x46;
	tInBuffer[13]=0x54;
	tInBuffer[14]=0x57;
	tInBuffer[15]=0x21;
	tInBuffer[16]=0x21;
    USBD_HID_SendReport (&hUsbDeviceFS, tInBuffer, HID_EPIN_SIZE);

}
