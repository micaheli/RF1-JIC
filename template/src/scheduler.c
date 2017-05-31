#include "includes.h"

inline void Scheduler(int32_t count)
{
	switch (count)
	{
		case 1:
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
