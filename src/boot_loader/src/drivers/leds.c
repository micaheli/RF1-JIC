#include "includes.h"

#include <stdio.h>

ledStatus_t ledStatus;


//todo: Do we want to init LEDs like this? Maybe an array is a better method
void InitLeds (void)
{
	InitializeGpio(LED1_GPIO_Port, LED1_GPIO_Pin, 0);
}


void DoLed(uint32_t number, uint32_t on)
{

	(void)(number);

	if (on)
	{
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_GPIO_Pin, GPIO_PIN_RESET);
	}
	else
	{
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_GPIO_Pin, GPIO_PIN_SET);
	}

}
