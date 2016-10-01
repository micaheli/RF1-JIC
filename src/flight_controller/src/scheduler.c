#include "includes.h"

extern uint8_t tOutBuffer[];
extern uint8_t tInBuffer[];

void scheduler(uint8_t count)
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

void taskHandlePcComm(void)
{

	if (tOutBuffer[0]==2) { //we have a usb report

		ledStatus.status = tOutBuffer[1];
		buzzerStatus.status = tOutBuffer[1];
		tInBuffer[0] = 1;
		tInBuffer[1]=(int8_t)dpsGyroArray[0];
		tInBuffer[2]=(int8_t)dpsGyroArray[1];
		tInBuffer[3]=(int8_t)dpsGyroArray[2];
		tInBuffer[4]=0x67;
		tInBuffer[5]=0x67;
		tInBuffer[6]=0x67;
		tInBuffer[7]=0x67;
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

	    tOutBuffer[0] = 0; //clear buffer id, only need to clear first byte.

	}

}

void taskLed(void)
{
	UpdateLeds();
}

void taskBuzzer(void)
{
	UpdateBuzzer();
}
