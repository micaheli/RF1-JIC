#include "includes.h"

TIM_HandleTypeDef ledTimer;


#define MUS_C_1 523
#define MUS_Cs_1 554
#define MUS_D_1 587
#define MUS_Ds_1 622
#define MUS_E_1 659
#define MUS_F_1 698
#define MUS_Fs_1 740
#define MUS_G_1 784
#define MUS_Gs_1 831
#define MUS_A_1 880
#define MUS_As_1 932
#define MUS_B_1 988

#define MUS_C_2 1046
#define MUS_Cs_2 1109
#define MUS_D_2 1175
#define MUS_Ds_2 1244
#define MUS_E_2 1328
#define MUS_F_2 1397
#define MUS_Fs_2 1480
#define MUS_G_2 1568
#define MUS_Gs_2 1661
#define MUS_A_2 1760
#define MUS_As_2 1865
#define MUS_B_2 1975

#define MUS_C_3 2093
#define MUS_Cs_3 2217
#define MUS_D_3 2349
#define MUS_Ds_3 2489
#define MUS_E_3 2637
#define MUS_F_3 2794
#define MUS_Fs_3 2960
#define MUS_G_3 3136
#define MUS_Gs_3 3322
#define MUS_A_3 3520
#define MUS_As_3 3729
#define MUS_B_3 3951

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

void RunTimerSound(uint32_t frequency, uint32_t ms)
{
	uint16_t ccr = (uint16_t)lrintf(0.015f * (float)((SystemCoreClock / 2) / frequency));
	AFetLoOff();
    BFetLoOff();
    CFetLoOff();
	InitFetTimerGpios(frequency, SystemCoreClock / 2);
	BFetLoOn();
	C_FET_HI_CCR = ccr;
	DelayMs(ms);
	C_FET_HI_CCR = 0;
	BFetLoOff();
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
	//	delayUs(1000);
	//}

	ZeroMotor();
	// set timer to output 2000 Hz tone
	InitFetTimerGpios(2093, SystemCoreClock / 2);

	//turn on low fet

	//InitFets();
	//Beep(3000, 3000, 10);

	ledtimer(32000, SystemCoreClock);
	TIM2->CCR1 = (uint16_t)lrintf(0.5f * (float)((SystemCoreClock) / 32000));


    while (1)
    {

		RunTimerSound(MUS_C_3, 125);

		TIM2->CCR1 = (uint16_t)lrintf(0.0f * (float)((SystemCoreClock) / 32000)); 
		TIM2->CCR1 = (uint16_t)lrintf(0.2f * (float)((SystemCoreClock) / 32000)); 
		TIM2->CCR1 = (uint16_t)lrintf(0.4f * (float)((SystemCoreClock) / 32000)); 
		TIM2->CCR1 = (uint16_t)lrintf(0.6f * (float)((SystemCoreClock) / 32000)); 
		TIM2->CCR1 = (uint16_t)lrintf(0.8f * (float)((SystemCoreClock) / 32000)); 
		TIM2->CCR1 = (uint16_t)lrintf(1.0f * (float)((SystemCoreClock) / 32000)); 
		TIM2->CCR1 = (uint16_t)lrintf(0.8f * (float)((SystemCoreClock) / 32000)); 
		TIM2->CCR1 = (uint16_t)lrintf(0.6f * (float)((SystemCoreClock) / 32000)); 
		TIM2->CCR1 = (uint16_t)lrintf(0.4f * (float)((SystemCoreClock) / 32000)); 
		TIM2->CCR1 = (uint16_t)lrintf(0.01f * (float)((SystemCoreClock) / 32000)); 

		uint16_t ccr = (uint16_t)lrintf(0.015f * (float)((SystemCoreClock / 2) / 2093));

		//set volume for low fet
		//AFetLoOn();
		//BFetLoOn();
		//AFetLoOn();

		//AFetLoOff();
		//AFetLoOff();
		//B_FET_HI_CCR = 0;

/*
		AFetLoOn();
		C_FET_HI_CCR = ccr;
		delayUs(1000);
		//AFetLoOff();
		AFetLoOff();
		C_FET_HI_CCR = 0;

		BFetLoOn();
		C_FET_HI_CCR = ccr;
		delayUs(1000);
		//AFetLoOff();
		BFetLoOff();
		C_FET_HI_CCR = 0;
	
		BFetLoOn();
		A_FET_HI_CCR = ccr;
		delayUs(1000);
		//AFetLoOff();
		BFetLoOff();
		A_FET_HI_CCR = 0;

		CFetLoOn();
		A_FET_HI_CCR = ccr;
		delayUs(1000);
		//AFetLoOff();
		CFetLoOff();
		A_FET_HI_CCR = 0;

		CFetLoOn();
		B_FET_HI_CCR = ccr;
		delayUs(1000);
		//AFetLoOff();
		CFetLoOff();
		B_FET_HI_CCR = 0;
*/
		//B_FET_HI_CCR = ccr;
		//C_FET_HI_CCR = ccr;
		//DelayMs(100); //let timer run for 100 ms
		//A_FET_HI_CCR = 0; // turn off pwm
		//B_FET_HI_CCR = 0; // turn off pwm
		//C_FET_HI_CCR = 0; // turn off pwm
		//DelayMs(100); //let timer run for 100 ms

		//BFetLoOn();
		//A_FET_HI_CCR = 0;
		//B_FET_HI_CCR = ccr;
		//C_FET_HI_CCR = 0;
		//DelayMs(100); //let timer run for 100 ms
		//A_FET_HI_CCR = 0; // turn off pwm
		//B_FET_HI_CCR = 0; // turn off pwm
		//C_FET_HI_CCR = 0; // turn off pwm
		//DelayMs(100); //let timer run for 100 ms
		//BFetLoOff();

		//CFetLoOn();
		//A_FET_HI_CCR = 0;
		//B_FET_HI_CCR = 0;
		//C_FET_HI_CCR = ccr;
		//DelayMs(100); //let timer run for 100 ms
		//A_FET_HI_CCR = 0; // turn off pwm
		//B_FET_HI_CCR = 0; // turn off pwm
		//C_FET_HI_CCR = 0; // turn off pwm
		//DelayMs(100); //let timer run for 100 ms
		//CFetLoOff();

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
