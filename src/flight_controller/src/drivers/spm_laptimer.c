#include "includes.h"

STRU_TELE_LAPTIMER lap_timer;
uint32_t lastTimeLap;
uint32_t lastTimegate;
uint32_t currentTime;
uint32_t captureTime;
uint32_t pulseIndex;
uint32_t pulseDuration[8];
uint8_t pulseCode;
uint32_t hits;
uint8_t lapDelay = 0;
void laptimerInit(void)
{
	lap_timer.identifier = TELE_DEVICE_LAPTIMER;
	lap_timer.sID = 0x00;
	lap_timer.lapNumber = 0;
	lap_timer.gateNumber = 0;
	lap_timer.lastLapTime = 0;
	lap_timer.gateTime = 0;

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	uint16_t prescalerValue;

	    //GPIOA Configuration: TIM5 Channel 1 as alternate function push-pull
	GPIO_InitStructure.Pin = SPM_LAPTIMER_PIN;
	GPIO_InitStructure.Mode = GPIO_Mode_AF;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.OType = GPIO_OType_PP;
	GPIO_InitStructure.Speed = GPIO_Speed_100MHz;
	GPIO_Init(SPM_LAPTIMER_GPIO, &GPIO_InitStructure);

	GPIO_PinAFConfig(SPM_LAPTIMER_GPIO, SPM_LAPTIMER_PINSOURCE, SPM_LAPTIMER_TIM_AF);


	    // Stop timer
	TIM_Cmd(SPM_LAPTIMER_TIM, DISABLE);

	    // Compute the prescaler value 
	prescalerValue = (uint16_t)(SystemCoreClock / 1000000) - 1;
	// Time base configuration 
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = SEQUENCE_TIMEOUT;
	TIM_TimeBaseStructure.TIM_Prescaler = prescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(SPM_LAPTIMER_TIM, &TIM_TimeBaseStructure);

	// PWM1 Mode configuration: Channel1
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = SPM_LAPTIMER_CHANNEL;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x00;
	TIM_ICInit(SPM_LAPTIMER_TIM, &TIM_ICInitStructure);

  
  // Configure interrupts
	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = SPM_LAPTIMER_TIM_CC_IRQN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x08;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x08;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = SPM_LAPTIMER_TIM_UP_IRQN;
	NVIC_Init(&NVIC_InitStructure);
	/*
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_TRG_COM_TIM11_IRQn;
	NVIC_Init(&NVIC_InitStructure);
  */
	// Enable timer and interupts
	TIM_Cmd(SPM_LAPTIMER_TIM, ENABLE);
	TIM_ITConfig(SPM_LAPTIMER_TIM, SPM_LAPTIMER_IT_CC, ENABLE);
	//TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	

}

/*
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line9)){
		captureTime = micros();
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
	

}
*/
uint8_t gpioStatus;
uint8_t fallingEdges;
uint8_t risingEdges;
uint8_t pulseCodeDebug;
LAP_STATE lapState = READING_PULSES;

extern void SPM_LAPTIMER_TIM_CC_IRQ_HANDLER(void)
{
	SPM_LAPTIMER_TIM->CNT = 0;
	TIM_ITConfig(SPM_LAPTIMER_TIM, SPM_LAPTIMER_IT_CC, DISABLE);
	captureTime = TIM_GetCapture3(SPM_LAPTIMER_TIM);
	
		if (pulseIndex >= CODE_SIZE)
		{
			pulseIndex = 0;
			pulseCode = 0;
			TIM_ClearITPendingBit(SPM_LAPTIMER_TIM, SPM_LAPTIMER_IT_CC);
			TIM_ITConfig(SPM_LAPTIMER_TIM, SPM_LAPTIMER_IT_CC, ENABLE);
			return;
		}
		if (pulseIndex == 0)
		{
			TIM_ITConfig(SPM_LAPTIMER_TIM, TIM_IT_Update, ENABLE);
			pulseCode = 0;
			memset(pulseDuration, 0, 16);
		}

		if ((captureTime > SHORT_MIN_PULSE + OFF_DURATION) && (captureTime < SHORT_MAX_PULSE + OFF_DURATION))	//0
		{
			pulseDuration[pulseIndex] = captureTime;
			pulseCode &= ~(1 << pulseIndex);
			pulseIndex++;
		}
		else if ((captureTime >= LONG_MIN_PULSE + OFF_DURATION) && (captureTime < LONG_MAX_PULSE + OFF_DURATION)) //1
		{
			pulseDuration[pulseIndex] = captureTime;
			pulseCode |= 1 << pulseIndex;
			pulseIndex++;
		}

	TIM_ClearITPendingBit(SPM_LAPTIMER_TIM, SPM_LAPTIMER_IT_CC);
	TIM_ITConfig(SPM_LAPTIMER_TIM, SPM_LAPTIMER_IT_CC, ENABLE);
}



void SPM_LAPTIMER_TIM_UP_IRQ_HANDLER(void)
{
	TIM_ITConfig(SPM_LAPTIMER_TIM, TIM_IT_Update, DISABLE);
	
	
	if (pulseIndex == CODE_SIZE 
#ifdef START_PULSES
            && (pulseCode & 3) == 1 
#endif
#ifdef END_PULSES
            && (pulseCode >> (CODE_SIZE - 2) & 3) == 1
#endif
	)
	{
		LED0_ON;
		lapDelay = 0;
		pulseCodeDebug = pulseCode;
		currentTime = millis();
		if ((pulseCode>>PULSE_SHIFT & 0x0F) == 0)	//lap captured. Disable CC and wait for enough update interrupts 
		{
			lap_timer.lapNumber++;
			lap_timer.lastLapTime = currentTime;
			lastTimeLap = currentTime;
		}
		else	//no lap. Disable update interrupt until a starting pulse is detected once again
		{
			lap_timer.gateNumber = (pulseCode >> PULSE_SHIFT & 0x0F); 
			lap_timer.gateTime = currentTime;
		}
		
	
		TIM_ClearITPendingBit(SPM_LAPTIMER_TIM, SPM_LAPTIMER_IT_CC);
		TIM_ClearITPendingBit(SPM_LAPTIMER_TIM, TIM_IT_Update);
		TIM_ITConfig(SPM_LAPTIMER_TIM, TIM_IT_Update, ENABLE);
		TIM_ITConfig(SPM_LAPTIMER_TIM, SPM_LAPTIMER_IT_CC, DISABLE);
		SPM_LAPTIMER_TIM->ARR = DISABLE_PERIOD;
		lapState = GATE_PASSED;
	}
	else if (lapDelay > DISABLE_TIME)
	{
		LED0_OFF;
		lapDelay = 0;
		SPM_LAPTIMER_TIM->ARR = SEQUENCE_TIMEOUT;
		TIM_ClearITPendingBit(SPM_LAPTIMER_TIM, SPM_LAPTIMER_IT_CC);
		TIM_ClearITPendingBit(SPM_LAPTIMER_TIM, TIM_IT_Update);
		TIM_ITConfig(SPM_LAPTIMER_TIM, TIM_IT_Update, DISABLE);
		TIM_ITConfig(SPM_LAPTIMER_TIM, SPM_LAPTIMER_IT_CC, ENABLE);
		lapState = READING_PULSES;
		pulseIndex = 0;
		pulseCode = 0;
		return;
	}
	else if(lapState == GATE_PASSED)
	{
		lapDelay++;
		TIM_ClearITPendingBit(SPM_LAPTIMER_TIM, TIM_IT_Update);
		TIM_ITConfig(SPM_LAPTIMER_TIM, TIM_IT_Update, ENABLE);
	}
	else
	{
		TIM_ClearITPendingBit(SPM_LAPTIMER_TIM, TIM_IT_Update);
		TIM_ITConfig(SPM_LAPTIMER_TIM, TIM_IT_Update, DISABLE);
	}

	pulseIndex = 0;
	pulseCode = 0;
}
