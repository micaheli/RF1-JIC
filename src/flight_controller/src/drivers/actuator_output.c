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

//motor_output_array motorOutputArray[MAX_MOTOR_NUMBER];

void InitActuators(void) {

	float disarmUs;
	float idleUs;
	float walledUs;   //MAX PWM
	uint32_t pwmHz;   //max pwmHz
	uint32_t timerHz; //should be calculated based on CPU

	uint32_t walledPulseValue;

	switch (mainConfig.mixerConfig.escProtcol) {
		case ESC_PWM:
			disarmUs  = 1000;
			walledUs  = 2000;
			pwmHz     = 490;
			timerHz   = 1000000;
			break;
		case ESC_ONESHOT:
			disarmUs  = 125;
			walledUs  = 250;
			pwmHz     = 3900;
			timerHz   = 8000000;
			break;
		case ESC_ONESHOT42:
			disarmUs  = 41.66;
			walledUs  = 83.333;
			pwmHz     = 11500;
			timerHz   = 24000000;
			break;
		case ESC_MULTISHOT:
		default:
			disarmUs  = 5;
			walledUs  = 22;
			pwmHz     = 32000;
			timerHz   = 48000000;
			break;
	}

	pwmHz = CONSTRAIN(mainConfig.mixerConfig.escUpdateFrequency,50,pwmHz); //constrain pwmHz between the config value and the max allowed for the ESC protocol.

	idleUs = ((walledUs - disarmUs) * (mainConfig.mixerConfig.idlePercent * 0.01) ) + disarmUs;
	float period = ((float)timerHz/(float)pwmHz);
	float pwmUs = (1000000/(float)pwmHz); //31.25 / 1500
	float pwmUsPerTimerStep = (pwmUs / (period-1)); //us per timer step, 0.02083333333333333
	disarmPulseValue = (uint32_t) (disarmUs/pwmUsPerTimerStep);
	idlePulseValue   = (uint32_t) (idleUs/pwmUsPerTimerStep);
	walledPulseValue = (uint32_t) (walledUs/pwmUsPerTimerStep);
	pulseValueRange  = walledPulseValue - idlePulseValue; //throttle for motor output is float motorThrottle * pulseValueRange + idlePulseValue;

	for (uint32_t motorNum=0;motorNum<MAX_MOTOR_NUMBER;motorNum++) {
		if (board.motors[motorNum].enabled)
			InitActuatorTimer(ports[board.motors[motorNum].port], board.motors[motorNum].pin, timers[board.motors[motorNum].timer], board.motors[motorNum].timChannel, board.motors[motorNum].AF, board.motors[motorNum].polarity, disarmPulseValue, pwmHz, timerHz);
	}


}

void InitActuatorTimer(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef *timer, uint32_t timerChannel, uint32_t alternateFunction, uint32_t polarity, uint32_t pulseValue, uint32_t pwmHz, uint32_t timerHz) {

	uint16_t timerPrescaler = 0;

	if(timer == TIM1 || timer == TIM8 || timer == TIM9|| timer == TIM10|| timer == TIM11) {
		timerPrescaler = (uint16_t)(SystemCoreClock / timerHz) - 1;
	} else {
		timerPrescaler = (uint16_t)(SystemCoreClock / 2 / timerHz) - 1;
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
	sConfigOC.OCMode      = TIM_OCMODE_PWM2;
	sConfigOC.Pulse       = pulseValue;
	sConfigOC.OCPolarity  = polarity;
	sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_OC_ConfigChannel(&pwmTimer, &sConfigOC, timerChannel);
	HAL_TIM_Base_Start(&pwmTimer);
	HAL_TIM_PWM_Start(&pwmTimer, timerChannel);
}



inline void OutputActuators(volatile float motorOutput[], volatile float servoOutput[]) {

	uint32_t motorNum;

	if (boardArmed || calibrateMotors) {

		if (calibrateMotors) {
			if (motorOutput[0] < 0.1) {
				for (motorNum=0;motorNum<MAX_MOTOR_NUMBER;motorNum++) {
					if (board.motors[motorNum].enabled)
						*ccr[board.motors[motorNum].timCCR] = disarmPulseValue;
				}
			} else {
				for (motorNum=0;motorNum<MAX_MOTOR_NUMBER;motorNum++) {
					if (board.motors[motorNum].enabled)
						*ccr[board.motors[motorNum].timCCR] = (uint32_t)((float)1 * (float)pulseValueRange) + idlePulseValue;
				}
			}
		} else {
			for (motorNum=0;motorNum<MAX_MOTOR_NUMBER;motorNum++) {
				if (board.motors[motorNum].enabled)
					*ccr[board.motors[motorNum].timCCR] = (uint32_t)((float)motorOutput[motorNum] * (float)pulseValueRange) + idlePulseValue;
			}
		}

	} else {
		for (motorNum=0;motorNum<MAX_MOTOR_NUMBER;motorNum++) {
			if (board.motors[motorNum].enabled)
				*ccr[board.motors[motorNum].timCCR] = disarmPulseValue;
		}
	}

	(void)(servoOutput);
}

void ZeroActuators(void) {

	for (uint32_t motorNum=0;motorNum<MAX_MOTOR_NUMBER;motorNum++) {
		if (board.motors[motorNum].enabled)
			*ccr[board.motors[motorNum].timCCR] = disarmPulseValue;
	}

}
