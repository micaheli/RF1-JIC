#include "includes.h"

extern uint8_t tOutBuffer[];
extern uint8_t tInBuffer[];
uint32_t skipTaskHandlePcComm   = 0;
uint32_t failsafeStage          = 0;
uint32_t autoSaveTimer          = 0;


//soft serial buffer handling. TODO: make a structure
volatile uint32_t softSerialEnabled = 0;
volatile uint32_t softSerialBuf[2][SOFT_SERIAL_BIT_TIME_ARRAY_SIZE];
volatile uint32_t softSerialInd[2];
volatile uint32_t softSerialCurBuf;
volatile uint32_t softSerialLastByteProcessedLocation;
volatile uint32_t softSerialSwitchBuffer;

uint8_t    proccesedSoftSerial[25]; //25 byte buffer enough?
uint32_t   proccesedSoftSerialIdx = 0;
uint32_t   softSerialLineIdleSensed = 0;
uint32_t   lastBitFound = 0;


static void TaskProcessSoftSerial(void);
static void TaskTelemtry(void);
static void TaskWizard(void);
static void TaskHandlePcComm(void);
static void TaskLed(void);
static void TaskBuzzer(void);
static void TaskAdc(void);

inline void scheduler(int32_t count)
{

	switch (count) {

		case 0:
			TaskHandlePcComm();
			break;
		case 1:
			TaskLed();
			break;
		case 2:
			TaskBuzzer();
			break;
		case 3:
			TaskAdc();
			break;
		case 4:
			TaskProcessSoftSerial();
			break;
		case 5:
			TaskTelemtry();
			break;
		case 6:
			TaskWizard();
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

inline void TaskAdc(void)
{
	PollAdc();
	CheckBatteryCellCount();
}

inline void TaskProcessSoftSerial(void)
{

	 if (oneWireActive)
		 FeedTheDog();
}

inline void TaskWizard(void) {

	switch(wizardStatus.currentWizard)
	{

		case WIZ_RC:
			if (wizardStatus.currentStep == 1) //step three needs to be polled by user/gui
				HandleWizRc();
			break;
		case 0:
		default:
			return;
	}
}

inline void TaskTelemtry(void)
{
	ProcessTelemtry();
}

inline void TaskHandlePcComm(void)
{
	if (skipTaskHandlePcComm)
		return;

	if (tOutBuffer[0]==2) { //we have a usb report

		ProcessCommand((char *)tOutBuffer);
		bzero(tOutBuffer, HID_EPIN_SIZE);

	}

}

inline void TaskLed(void)
{
	UpdateLeds(); //update status LEDs
	UpdateWs2812Leds();
}

inline void TaskBuzzer(void)
{
	UpdateBuzzer();
}
