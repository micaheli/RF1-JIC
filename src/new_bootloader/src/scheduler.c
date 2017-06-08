#include "includes.h"

//this doesn't really belong here
uint8_t tInBuffer[HID_EPIN_SIZE], tOutBuffer[HID_EPOUT_SIZE-1];

inline void Scheduler(int32_t count)
{
	switch (count)
	{
		case 1:
			CheckRfblState();
			break;
		case 2:
			RfblUpdateLed(rfblLedSpeed1, rfblLedSpeed2);
			break;
		default:
			break;
	}

}

void ErrorHandler(uint32_t error)
{
	(void)(error);
    while (1)
    {
        DoLed(1, 1);
        DoLed(2, 0);
        DelayMs(40);
        DoLed(1, 0);
        DoLed(2, 1);
        DelayMs(40);
    }
}

//unused functions
void GyroExtiCallback (uint32_t callbackNumber)
{
	(void)(callbackNumber);
}

void GyroRxDmaCallback (uint32_t callbackNumber)
{
	(void)(callbackNumber);
}

void FlashDmaRxCallback (uint32_t callbackNumber)
{
	(void)(callbackNumber);
}

void MaxOsdDmaRxCallback (uint32_t callbackNumber)
{
	(void)(callbackNumber);
}
