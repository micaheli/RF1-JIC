#include "includes.h"
buzzerStatus_t buzzerStatus;


void InitBuzzer(void) {
	InitializeBuzzerPin(BUZZER_GPIO_Port, BUZZER_GPIO_Pin);
}

void InitializeBuzzerPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(GPIOx, GPIO_Pin);

    GPIO_InitStructure.Pin = GPIO_Pin;

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);

    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);

    buzzerStatus.status = STATE_BUZZER_OFF;

}

void UpdateBuzzer(void)
{
	uint32_t timeNow = millis();
	//checking if time needs to be updated
	if (buzzerStatus.status != buzzerStatus.lastStatus)
	{
		buzzerStatus.timeStart = timeNow;
	}
	buzzerStatus.lastStatus = buzzerStatus.status;

	//different states for buzzer
    switch(buzzerStatus.status)
    {

    default:
    case STATE_BUZZER_OFF:
        BUZZER_OFF;
        break;
    case STATE_BUZZER_ON:
    	BUZZER_ON;
    	break;
    case STATE_BUZZER_LOST:
    	Buzz(timeNow,40, 500);
    	break;
    case STATE_BUZZER_SIMPLE:
    	Buzz(timeNow,300,400);
    	break;

     }

    //Buzz(timeNow, 1, 1);

}
//function to make the buzzer buzz
void Buzz(uint32_t timeNow, uint16_t time1, uint16_t time2) {
	//does this for the amount of time for time1
	if (((timeNow - buzzerStatus.timeStart) < time1) && (!buzzerStatus.on) )
	{
		BUZZER_ON;
		buzzerStatus.on = true;
	}
	//does this for the amount of time2
	else if (((timeNow - buzzerStatus.timeStart) > time1) && ((timeNow - buzzerStatus.timeStart) < time2) && (buzzerStatus.on) )
	{
		BUZZER_OFF;
		buzzerStatus.on = false;
	}
	//if greater than time time2 reset timestart
	else if ((timeNow - buzzerStatus.timeStart) > time2 )
	{
		buzzerStatus.timeStart = timeNow;
	}
}
