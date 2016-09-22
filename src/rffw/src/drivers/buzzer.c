#include "includes.h"
buzzerStatus_t buzzerStatus;


void InitializeBuzzer(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(GPIOx, GPIO_Pin);

    GPIO_InitStructure.Pin = GPIO_Pin;

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);

    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
}

void UpdateBuzzer(void)
{
    uint32_t timeNow = millis();
    buzzerStatus.on = false;

    switch(buzzerStatus.status)
    {

    case STATE_BUZZER_ON:
    	BUZZER_ON;
    case STATE_BUZZER_OFF:
        BUZZER_OFF;
    case STATE_BUZZER_LOST:
    	Buzz(timeNow,33, 66);
     }

    //Buzz(timeNow, 1, 1);

}

void Buzz(uint32_t timeNow, uint16_t time1, uint16_t time2) {
	if (((timeNow - buzzerStatus.timeStart) < time1) && (!buzzerStatus.on) )
	{
		BUZZER_ON;
		buzzerStatus.on = true;
	}
	else if (((timeNow - buzzerStatus.timeStart) > time1) && ((timeNow - buzzerStatus.timeStart) < time2) && (buzzerStatus.on) )
	{
		BUZZER_OFF;
		buzzerStatus.on = false;
	}
	else if ((timeNow - buzzerStatus.timeStart) > time2 )
	{
		buzzerStatus.timeStart = timeNow;
	}
}
