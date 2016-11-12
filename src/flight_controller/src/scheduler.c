#include "includes.h"

extern uint8_t tOutBuffer[];
extern uint8_t tInBuffer[];
uint32_t failsafeStage = 0;

void scheduler(int32_t count)
{

	switch (count) {

		case 0:
			taskHandlePcComm();
			break;
		case 1:
			taskLed();
			break;
		case 2:
			taskBuzzer();
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		default:
			break;

	}

}

inline void taskHandlePcComm(void)
{

	if (tOutBuffer[0]==2) { //we have a usb report

		if (tOutBuffer[1]==1) {
			tInBuffer[0] = 1;
			tInBuffer[1] = (int8_t)pitchAttitude;
			tInBuffer[2] = (int8_t)rollAttitude;
			tInBuffer[3] = (int8_t)yawAttitude;
			USBD_HID_SendReport (&hUsbDeviceFS, tInBuffer, HID_EPIN_SIZE);
			bzero(tOutBuffer, HID_EPIN_SIZE);
		} else {

			if (tOutBuffer[4]==0) {
				tOutBuffer[4]=0x20;
			}
			ProcessCommand((char *)tOutBuffer);
			bzero(tOutBuffer, HID_EPIN_SIZE);
		}

	}

}

inline void taskLed(void)
{
	UpdateLeds();
}

inline void taskBuzzer(void)
{
	UpdateBuzzer();
}
