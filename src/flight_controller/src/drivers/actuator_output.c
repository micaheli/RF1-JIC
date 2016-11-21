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

	float disarmUs;   // shortest pulse width (disarmed)
	float walledUs;   // longest pulse width (full throttle)
	float idleUs;     // idle pulse width (armed, zero throttle)

	// timerHz *must* be a proper divisor of the timer frequency
	//     REVOLT - 48 MHz (overclocked from 42 MHz)
	uint32_t timerHz; // frequency of the timer
	uint32_t pwmHz;   // max update frequency for protocol
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
			disarmUs  = 41.667;
			walledUs  = 83.334; // round up for int math
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

	// constrain motor update frequency between the 50Hz and the max allowed for the ESC protocol
	pwmHz = CONSTRAIN(mainConfig.mixerConfig.escUpdateFrequency, 50, pwmHz);
	// compute idle PWM width from idlePercent
	idleUs = ((walledUs - disarmUs) * (mainConfig.mixerConfig.idlePercent * 0.01) ) + disarmUs;

	disarmPulseValue = ((uint32_t)(disarmUs * timerHz)) / 1000000;
	idlePulseValue   = ((uint32_t)(idleUs * timerHz)) / 1000000;
	walledPulseValue = ((uint32_t)(walledUs * timerHz)) / 1000000;

	pulseValueRange  = walledPulseValue - idlePulseValue; //throttle for motor output is float motorThrottle * pulseValueRange + idlePulseValue;

	for (uint32_t motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
		if (board.motors[motorNum].enabled) {
			InitActuatorTimer(board.motors[motorNum], pwmHz, timerHz);
		}
	}

	DelayMs(5); //give timer time to stabilize.

}

void InitActuatorTimer(motor_type actuator, uint32_t pwmHz, uint32_t timerHz)
{
	uint16_t timerPrescaler = 0;

	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_HandleTypeDef pwmTimer;
	TIM_OC_InitTypeDef sConfigOC;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_ClockConfigTypeDef sClockSourceConfig;

	switch (actuator.timer) {
		case ENUMTIM1:
		case ENUMTIM8:
		case ENUMTIM9:
		case ENUMTIM10:
		case ENUMTIM11:
			timerPrescaler = (uint16_t)(SystemCoreClock / timerHz) - 1;
			break;
		default:
			timerPrescaler = (uint16_t)(SystemCoreClock / 2 / timerHz) - 1;
			break;
	}

	// Initialize GPIO

	GPIO_InitStruct.Pin = actuator.pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = actuator.AF;
	HAL_GPIO_Init(ports[actuator.port], &GPIO_InitStruct);

	// Initialize timer

	// TIM_Handle's _should_ be preserved in a global state, since we don't,
	// and to ensure initialization happens correctly, zero the handle
	memset(&pwmTimer, 0, sizeof(pwmTimer));

	pwmTimer.Instance = timers[actuator.timer];
	pwmTimer.Init.Prescaler = timerPrescaler;
	pwmTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	pwmTimer.Init.Period = (timerHz / pwmHz) - 1;
	pwmTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&pwmTimer);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&pwmTimer, &sClockSourceConfig);

	HAL_TIM_PWM_Init(&pwmTimer);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&pwmTimer, &sMasterConfig);

	// Initialize timer pwm channel

	sConfigOC.OCMode      = TIM_OCMODE_PWM2;
	sConfigOC.Pulse       = 0;
	sConfigOC.OCPolarity  = actuator.polarity;
	sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_PWM_ConfigChannel(&pwmTimer, &sConfigOC, actuator.timChannel);

	// Start the timer

	HAL_TIM_Base_Start(&pwmTimer);
	HAL_TIM_PWM_Start(&pwmTimer, actuator.timChannel);
}


inline void OutputActuators(volatile float motorOutput[], volatile float servoOutput[])
{
	(void)servoOutput;

	uint32_t motorNum;

	if (boardArmed) {
		for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
			if (board.motors[motorNum].enabled) {
				*ccr[board.motors[motorNum].timCCR] = (uint16_t)(motorOutput[motorNum] * (float)pulseValueRange) + idlePulseValue;
			}
		}
	} else if (calibrateMotors) {
		if (motorOutput[0] < 0.1) {
			for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
				if (board.motors[motorNum].enabled) {
					*ccr[board.motors[motorNum].timCCR] = disarmPulseValue;
				}
			}
		} else {
			for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
				if (board.motors[motorNum].enabled) {
					*ccr[board.motors[motorNum].timCCR] = pulseValueRange + idlePulseValue;
				}
			}
		}
	} else {
		for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
			if (board.motors[motorNum].enabled) {
				*ccr[board.motors[motorNum].timCCR] = disarmPulseValue;
			}
		}
	}
}

void ZeroActuators(void) {
	for (uint32_t motorNum=0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
		if (board.motors[motorNum].enabled) {
			*ccr[board.motors[motorNum].timCCR] = disarmPulseValue;
		}
	}

}
