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

		ProcessCommand((char *)tOutBuffer);
		bzero(tOutBuffer, HID_EPIN_SIZE);

/*
		if ((tOutBuffer[1] == 'D') && (tOutBuffer[2] == 'F') && (tOutBuffer[3] == 'U')) {
			SystemResetToDfuBootloader();
		}

		if ((tOutBuffer[1] == 'S') && (tOutBuffer[2] == 'A') && (tOutBuffer[3] == 'V') && (tOutBuffer[4] == 'E')) {
			SaveConfig (ADDRESS_CONFIG_START);
		}

		if ((tOutBuffer[1] == 'P') && (tOutBuffer[2] == 'O') && (tOutBuffer[3] == 'O')) {
			tInBuffer[0] = 1;
			tInBuffer[6] = mainConfig.pidConfig[YAW].kp;
			tInBuffer[7] = mainConfig.pidConfig[YAW].kp;
			tInBuffer[8] = mainConfig.pidConfig[YAW].kp;
			tInBuffer[9] = mainConfig.pidConfig[YAW].kp;
			GenerateConfig();
			tInBuffer[10] = mainConfig.pidConfig[YAW].kp;
			tInBuffer[11] = mainConfig.pidConfig[YAW].kp;
			tInBuffer[12] = mainConfig.pidConfig[YAW].kp;
			tInBuffer[13] = mainConfig.pidConfig[YAW].kp;
			USBD_HID_SendReport (&hUsbDeviceFS, tInBuffer, HID_EPIN_SIZE);

		}

		if (tOutBuffer[1] == 6) {
			calibrateMotors = 1;
			motorOutput[0] = 1;
			motorOutput[1] = 1;
			motorOutput[2] = 1;
			motorOutput[3] = 1;
			motorOutput[4] = 1;
			motorOutput[5] = 1;
			motorOutput[6] = 1;
			motorOutput[7] = 1;
			motorOutput[8] = 1;
			OutputActuators(motorOutput, servoOutput);
		} else if (tOutBuffer[1] == 7) {
			motorOutput[0] = 0;
			motorOutput[1] = 0;
			motorOutput[2] = 0;
			motorOutput[3] = 0;
			motorOutput[4] = 0;
			motorOutput[5] = 0;
			motorOutput[6] = 0;
			motorOutput[7] = 0;
			motorOutput[8] = 0;
			OutputActuators(motorOutput, servoOutput);
			calibrateMotors = 0;
		} else if (tOutBuffer[1] == 8) {
			boardArmed = 1;
		} else if (tOutBuffer[1] == 9) {
			boardArmed = 0;
			motorOutput[0] = 0;
			motorOutput[1] = 0;
			motorOutput[2] = 0;
			motorOutput[3] = 0;
			motorOutput[4] = 0;
			motorOutput[5] = 0;
			motorOutput[6] = 0;
			motorOutput[7] = 0;
			motorOutput[8] = 0;
			OutputActuators(motorOutput, servoOutput);
		} else {
			ledStatus.status = tOutBuffer[1];
			buzzerStatus.status = tOutBuffer[2];
		}
		tInBuffer[0] = 1;
		tInBuffer[1]=(int8_t)dpsGyroArray[0];
		tInBuffer[2]=(int8_t)dpsGyroArray[1];
		tInBuffer[3]=(int8_t)dpsGyroArray[2];
		tInBuffer[4]=(uint8_t)debugU32[0];
		tInBuffer[5]=(uint8_t)debugU32[1];
		tInBuffer[6]=(uint8_t)debugU32[2];
		tInBuffer[7]=(uint8_t)debugU32[3];
		tInBuffer[8]=(uint8_t)debugU32[4];
		tInBuffer[9]=(uint8_t)debugU32[4];
		tInBuffer[10]=(uint8_t)debugU32[5];
		tInBuffer[11]=(uint8_t)debugU32[6];
		tInBuffer[12]=(uint8_t)debugU32[7];
		tInBuffer[13]=(uint8_t)debugU32[8];
		tInBuffer[14]=(uint8_t)debugU32[9];

//		tInBuffer[9]=Rx_Buffer[0];
//		tInBuffer[10]=Rx_Buffer[1];
//		tInBuffer[11]=Rx_Buffer[2];
//		tInBuffer[12]=Rx_Buffer[3];
//		tInBuffer[13]=Rx_Buffer[4];
//		tInBuffer[14]=Rx_Buffer[5];

		tInBuffer[15]=0x21;
		tInBuffer[16]=0x21;
	    USBD_HID_SendReport (&hUsbDeviceFS, tInBuffer, HID_EPIN_SIZE);

	    tOutBuffer[0] = 0; //clear buffer id, only need to clear first byte.
*/
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
