#include "includes.h"

TIM_HandleTypeDef ledTimer;


void ledtimer(uint32_t pwmHz, uint32_t timerHz)
{
	uint16_t timerPrescaler = 0;

	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_OC_InitTypeDef sConfigOC;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_ClockConfigTypeDef sClockSourceConfig;

	timerPrescaler = (uint16_t)(SystemCoreClock / timerHz) - 1;

	// Initialize GPIO
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;

	GPIO_InitStruct.Pin       = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	// Initialize timer
	ledTimer.Instance           = TIM2;
	ledTimer.Init.Prescaler     = timerPrescaler;
	ledTimer.Init.CounterMode   = TIM_COUNTERMODE_UP;
	ledTimer.Init.Period        = (timerHz / pwmHz) - 1;
	ledTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&ledTimer);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&ledTimer, &sClockSourceConfig);

	HAL_TIM_PWM_Init(&ledTimer);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&ledTimer, &sMasterConfig);

	// Initialize timer pwm channel

	sConfigOC.OCMode      = TIM_OCMODE_PWM2;
	sConfigOC.Pulse       = 0;
	sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_PWM_ConfigChannel(&ledTimer, &sConfigOC, TIM_CHANNEL_1);

	HAL_TIM_Base_Start(&ledTimer);
	HAL_TIM_PWM_Start(&ledTimer, TIM_CHANNEL_1);

}


int main(void)
{

	int32_t count;
	int32_t x;

	count = 16;

	VectorIrqInit(0x08000000);
    BoardInit();

	//InitLeds();
	//DelayMs(200);
	//InitFets();
	//DelayMs(5);

	//for (x = 0; x < 5; x++) {
	//	Beep(2000 + x * 6000, 10, 10);
	//	DelayMs(10);
	//}

	ZeroMotor();
	// set timer to output 2000 Hz tone
	InitFetTimerGpios(2000, SystemCoreClock / 2);

	//turn on low fet

	//InitFets();
	Beep(3000, 3000, 10);

	ledtimer(32000, SystemCoreClock);
	TIM2->CCR1 = (uint16_t)lrintf(0.5f * (float)((SystemCoreClock) / 32000));


    while (1)
    {

		TIM2->CCR1 = (uint16_t)lrintf(0.0f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.1f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.2f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.3f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.4f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.5f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.6f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.7f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.8f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.9f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(1.0f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.9f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.8f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.7f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.6f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.5f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.4f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.3f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.2f * (float)((SystemCoreClock) / 32000));DelayMs(100); 
		TIM2->CCR1 = (uint16_t)lrintf(0.1f * (float)((SystemCoreClock) / 32000));DelayMs(100); 

		uint16_t ccr = (uint16_t)lrintf(0.5f * (float)((SystemCoreClock / 2) / 2000));

		//set volume for low fet
		AFetLoOn();
		A_FET_HI_CCR = ccr;
		B_FET_HI_CCR = 0;
		C_FET_HI_CCR = 0;
		DelayMs(100); //let timer run for 100 ms
		A_FET_HI_CCR = 0; // turn off pwm
		B_FET_HI_CCR = 0; // turn off pwm
		C_FET_HI_CCR = 0; // turn off pwm
		DelayMs(100); //let timer run for 100 ms

		BFetLoOn();
		A_FET_HI_CCR = 0;
		B_FET_HI_CCR = ccr;
		C_FET_HI_CCR = 0;
		DelayMs(100); //let timer run for 100 ms
		A_FET_HI_CCR = 0; // turn off pwm
		B_FET_HI_CCR = 0; // turn off pwm
		C_FET_HI_CCR = 0; // turn off pwm
		DelayMs(100); //let timer run for 100 ms

		CFetLoOn();
		A_FET_HI_CCR = 0;
		B_FET_HI_CCR = 0;
		C_FET_HI_CCR = ccr;
		DelayMs(100); //let timer run for 100 ms
		A_FET_HI_CCR = 0; // turn off pwm
		B_FET_HI_CCR = 0; // turn off pwm
		C_FET_HI_CCR = 0; // turn off pwm
		DelayMs(100); //let timer run for 100 ms
		//inlineDigitalHi(LED0_GPIO, LED0_PIN);
		//DelayMs(100);
		//inlineDigitalHi(LED1_GPIO, LED1_PIN);
		//DelayMs(100);
		//inlineDigitalHi(LED2_GPIO, LED2_PIN);
		//DelayMs(100);

		//inlineDigitalLo(LED0_GPIO, LED0_PIN);
		//DelayMs(100);
		//inlineDigitalLo(LED1_GPIO, LED1_PIN);
		//DelayMs(100);
		//inlineDigitalLo(LED2_GPIO, LED2_PIN);
		//DelayMs(100);

    	//Scheduler(count--);

    	if (count == -1)
    		count = 16;
    }

}
