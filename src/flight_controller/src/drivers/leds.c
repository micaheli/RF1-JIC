#include "includes.h"

#include <stdio.h>

ledStatus_t ledStatus;

//todo: Do we want to init LEDs like this? Maybe an array is a better method
void LedInit (void)
{
#if LEDn >= 1
    InitializeLED(LED1_GPIO_Port, LED1_GPIO_Pin);
#endif
#if LEDn >= 2
    InitializeLED(LED2_GPIO_Port, LED2_GPIO_Pin);
#endif
#if LEDn >= 3
    InitializeLED(LED3_GPIO_Port, LED3_GPIO_Pin);
#endif
}

void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
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

void UpdateLeds(void)
{
	uint32_t timeNow = millis();

	if (ledStatus.status != ledStatus.lastStatus)
	{
		ledStatus.timeStart = timeNow;
	}
	ledStatus.lastStatus = ledStatus.status;

	switch (ledStatus.status)
	{
		case LEDS_OFF:
			LED1_OFF;
			LED2_OFF;
			LED3_OFF;
			break;
		case LEDS_ON:
			LED1_ON;
			LED2_ON;
			LED3_ON;
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
			LED1_OFF;
			LED2_OFF;
			LED3_OFF;
			break;
	}
}

void BlinkAllLeds(uint32_t timeNow, uint16_t time1, uint16_t time2)
{
	if (((timeNow - ledStatus.timeStart) < time1) && (ledStatus.on) )
	{
		LED1_OFF;
		LED2_OFF;
		LED3_OFF;
		ledStatus.on = false;
	} else if (((timeNow - ledStatus.timeStart) > time1) && ((timeNow - ledStatus.timeStart) < time2) && (!ledStatus.on) )
	{
		LED1_ON;
		LED2_ON;
		LED3_ON;
		ledStatus.on = true;
	} else if ((timeNow - ledStatus.timeStart) > time2 )
	{
		ledStatus.timeStart = timeNow;
	}
}
