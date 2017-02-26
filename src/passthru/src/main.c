#include "includes.h"

int main(void)
{

	VectorIrqInit(0x08060000);

	GetBoardHardwareDefs();
    InitializeMCUSettings();
    BoardInit();
    InitLeds();

	while(1)
	{

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
