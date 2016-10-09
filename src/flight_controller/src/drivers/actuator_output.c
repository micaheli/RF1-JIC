#include "includes.h"

volatile uint32_t disarmPulseValue;
volatile uint32_t idlePulseValue;
volatile uint32_t pulseValueRange;

typedef struct {
	unsigned char active;
	uint32_t timer;
	uint32_t timerChannel;
	uint32_t ccr;
} motor_output_array;

motor_output_array motorOutputArray[MAX_MOTOR_NUMBER];

void InitActuators(void) {

	bzero(motorOutputArray, sizeof(motorOutputArray));

	motorOutputArray[0].active = 1;
	motorOutputArray[1].active = 1;
	motorOutputArray[2].active = 1;
	motorOutputArray[3].active = 1;

	//motorOutputArray[0].timer = _TIM3;
	//motorOutputArray[1].timer = _TIM3;
	//motorOutputArray[2].timer = _TIM9;
	//motorOutputArray[3].timer = _TIM2;

	//these calues come from config
	//this is for multishot at 32KHz on STM32F4 running at 192MHz
	float disarmUs  = 5;
	float idleUs  = 6.5;
	float walledUs  = 20;
	uint32_t pwmHz   = 32000;
	uint32_t timerHz = 48000000;

	uint32_t walledPulseValue;

	float period = ((float)timerHz/(float)pwmHz);
	float pwmUs = (1000000/(float)pwmHz); //31.25 / 1500
	float pwmUsPerTimerStep = (pwmUs / (period-1)); //us per timer step, 0.02083333333333333
	disarmPulseValue = (uint32_t) (disarmUs/pwmUsPerTimerStep);
	idlePulseValue   = (uint32_t) (idleUs/pwmUsPerTimerStep);
	walledPulseValue = (uint32_t) (walledUs/pwmUsPerTimerStep);
	pulseValueRange  = walledPulseValue - idlePulseValue; //throttle for motor output is float motorThrottle * pulseValueRange + idlePulseValue;

	InitActuatorTimer(GPIOB, GPIO_PIN_0, TIM3, TIM_CHANNEL_3, GPIO_AF2_TIM3, TIM_OCPOLARITY_LOW, disarmPulseValue, pwmHz, timerHz );
	InitActuatorTimer(GPIOB, GPIO_PIN_1, TIM3, TIM_CHANNEL_4, GPIO_AF2_TIM3, TIM_OCPOLARITY_LOW, disarmPulseValue, pwmHz, timerHz );
	InitActuatorTimer(GPIOA, GPIO_PIN_3, TIM9, TIM_CHANNEL_2, GPIO_AF3_TIM9, TIM_OCPOLARITY_LOW, disarmPulseValue, pwmHz, timerHz );
	InitActuatorTimer(GPIOA, GPIO_PIN_2, TIM2, TIM_CHANNEL_3, GPIO_AF1_TIM2, TIM_OCPOLARITY_LOW, disarmPulseValue, pwmHz, timerHz );

}

void InitActuatorTimer(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef *timer, uint32_t timerChannel, uint32_t alternateFunction, uint32_t polarity, uint32_t pulseValue, uint32_t pwmHz, uint32_t timerHz) {

	uint32_t timerPrescaler = 0;

	if(timer == TIM1 || timer == TIM8 || timer == TIM9|| timer == TIM10|| timer == TIM11) {
		timerPrescaler = (SystemCoreClock / timerHz) - 1;
	} else {
		timerPrescaler = (SystemCoreClock / 2 / timerHz) - 1;
	}

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = alternateFunction;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

	TIM_HandleTypeDef pwmTimer;

	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;
	TIM_ClockConfigTypeDef sClockSourceConfig;

	pwmTimer.Instance = timer;
	pwmTimer.Init.Prescaler = timerPrescaler;
	pwmTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	pwmTimer.Init.Period = (timerHz/pwmHz)-1;
	pwmTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&pwmTimer);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&pwmTimer, &sClockSourceConfig);

	HAL_TIM_PWM_Init(&pwmTimer);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&pwmTimer, &sMasterConfig);

	/*
	 sConfigOC.OCMode = TIM_OCMODE_PWM2;
	sConfigOC.Pulse = 4000;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;
	 */
	sConfigOC.OCMode = TIM_OCMODE_PWM2;
	sConfigOC.Pulse = pulseValue;
	sConfigOC.OCPolarity = polarity;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_OC_ConfigChannel(&pwmTimer, &sConfigOC, timerChannel);
	HAL_TIM_Base_Start(&pwmTimer);
	HAL_TIM_PWM_Start(&pwmTimer, timerChannel);
}

void OutputActuators(float motorOutput[], float servoOutput[]) {
	int x;
	uint32_t pulseValue;
	if (boardArmed || calibrateMotors) {

		if (calibrateMotors) {
			pulseValue = disarmPulseValue;
		} else {
			pulseValue = idlePulseValue;
		}
		for(x=7; x>=0; x--)
		{
			motorOutput[x] = InlineConstrainf(motorOutput[x], 0.0, 1.0);
		}

		TIM2->CCR3 = (uint32_t)((float)motorOutput[0] * (float)pulseValueRange) + pulseValue;
		TIM3->CCR4 = (uint32_t)((float)motorOutput[1] * (float)pulseValueRange) + pulseValue;
		TIM3->CCR3 = (uint32_t)((float)motorOutput[2] * (float)pulseValueRange) + pulseValue;
		TIM9->CCR2 = (uint32_t)((float)motorOutput[3] * (float)pulseValueRange) + pulseValue;

	} else {

		TIM3->CCR3 = disarmPulseValue;
		TIM3->CCR4 = disarmPulseValue;
		TIM9->CCR2 = disarmPulseValue;
		TIM2->CCR3 = disarmPulseValue;

	}

	(void)(servoOutput);
}
