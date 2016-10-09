#include "includes.h"

#include <stdio.h>

ledStatus_t ledStatus;


void InitializeLed(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(GPIOx, GPIO_Pin);

    GPIO_InitStructure.Pin = GPIO_Pin;

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);

    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);

    ledStatus.status = LEDS_OFF;
}


//todo: Do we want to init LEDs like this? Maybe an array is a better method
void InitLeds (void)
{
	for (int x;x<3;x++)
	{
		if (board.internalLeds[x].enabled)
		{
			InitializeLed(ports[board.internalLeds[x].port], board.internalLeds[x].pin);
		}
	}
}


void DoLed(uint32_t number, uint32_t on)
{
	if (on)
	{
		if (board.internalLeds[number].enabled)
			HAL_GPIO_WritePin(ports[board.internalLeds[number].port], board.internalLeds[number].pin, GPIO_PIN_RESET);
	}
	else
	{
		if (board.internalLeds[number].enabled)
			HAL_GPIO_WritePin(ports[board.internalLeds[number].port], board.internalLeds[number].pin, GPIO_PIN_SET);
	}
}

void UpdateLeds(void)
{
	uint32_t timeNow = InlineMillis();

	if (ledStatus.status != ledStatus.lastStatus)
	{
		ledStatus.timeStart = timeNow;
	}
	ledStatus.lastStatus = ledStatus.status;

	switch (ledStatus.status)
	{
		case LEDS_OFF:
			DoLed(0, 0);
			DoLed(1, 0);
			DoLed(2, 0);
			break;
		case LEDS_ON:
			DoLed(0, 1);
			DoLed(1, 1);
			DoLed(2, 1);
			break;
		case LEDS_SLOW_BLINK:
			BlinkAllLeds(timeNow, 999, 1998);
			break;
		case LEDS_MED_BLINK:
			BlinkAllLeds(timeNow, 333, 666);
			break;
		case LEDS_FAST_BLINK:
			BlinkAllLeds(timeNow, 100, 200);
			break;
		default:
			DoLed(0, 0);
			DoLed(1, 0);
			DoLed(2, 0);
			break;
	}
}

void BlinkAllLeds(uint32_t timeNow, uint16_t time1, uint16_t time2)
{
	if (((timeNow - ledStatus.timeStart) < time1) && (ledStatus.on) )
	{
		DoLed(0, 0);
		DoLed(1, 0);
		DoLed(2, 0);
		ledStatus.on = false;
	} else if (((timeNow - ledStatus.timeStart) > time1) && ((timeNow - ledStatus.timeStart) < time2) && (!ledStatus.on) )
	{
		DoLed(0, 1);
		DoLed(1, 1);
		DoLed(2, 1);
		ledStatus.on = true;
	} else if ((timeNow - ledStatus.timeStart) > time2 )
	{
		ledStatus.timeStart = timeNow;
	}
}
