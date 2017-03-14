#include "includes.h"

volatile uint32_t disarmPulseValue3d;
volatile uint32_t disarmPulseValue;
volatile uint32_t calibratePulseValue;
volatile uint32_t idlePulseValue;
volatile uint32_t pulseValueRange;


static void InitActuatorTimer(motor_type actuator, uint32_t pwmHz, uint32_t timerHz);
static void ThrottleToDshot(uint8_t *serialOutBuffer, float throttle, float idle);

//motor_output_array motorOutputArray[MAX_MOTOR_NUMBER];

void DeInitActuators(void)  {
	uint32_t motorNum;
	uint32_t outputNumber;
	for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++)
	{
		outputNumber = mainConfig.mixerConfig.motorOutput[motorNum];

		if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
		{
			HAL_GPIO_DeInit(ports[board.motors[outputNumber].port], board.motors[outputNumber].pin);
			HAL_TIM_PWM_DeInit(&pwmTimers[board.motors[outputNumber].actuatorArrayNum]);
			HAL_TIM_Base_DeInit(&pwmTimers[board.motors[outputNumber].actuatorArrayNum]);
		}
	}
}

void InitActuators(void) {

	float disarmUs;    // shortest pulse width (disarmed)
	float disarmUs3d;  // shortest pulse width (disarmed)
	float calibrateUs; // shortest pulse width (calibrate)
	float walledUs;    // longest pulse width (full throttle)
	float idleUs;      // idle pulse width (armed, zero throttle)
	uint32_t motorNum;
	uint32_t outputNumber;

	// timerHz *must* be a proper divisor of the timer frequency
	// REVOLT - 48 MHz (overclocked from 42 MHz)
	uint32_t timerHz; // frequency of the timer
	uint32_t pwmHz;   // max update frequency for protocol
	uint32_t walledPulseValue;

	switch (mainConfig.mixerConfig.escProtcol) {
		case ESC_PWM:
			disarmUs3d  = 1500;
			disarmUs    = 990;
			calibrateUs = 1000;
			walledUs    = 2000;
			pwmHz       = 490;
			timerHz     = 1000000;
			break;
		case ESC_ONESHOT:
			disarmUs3d  = 187.5;
			disarmUs    = 120;
			calibrateUs = 125;
			walledUs    = 250;
			pwmHz       = 3900;
			timerHz     = 8000000;
			break;
		case ESC_ONESHOT42:
			disarmUs    = 40;
			calibrateUs = 41.667;
			walledUs    = 83.334; // round up for int math
			pwmHz       = 11500;
			timerHz     = 24000000;
			break;
		case ESC_DSHOT150:
		case ESC_DSHOT300:
		case ESC_DSHOT600:
			disarmUs    = 0;
			calibrateUs = 0;
			walledUs    = 2000;
			idleUs      = 48;
			pwmHz       = 0;
			timerHz     = 0;
			disarmPulseValue = 0;
			idlePulseValue   = 48;
			walledPulseValue = 2048;
			pulseValueRange  = 2000;
			InitDshotOutputOnMotors(mainConfig.mixerConfig.escProtcol);
			return;
			break;
		case ESC_MULTISHOT25:
			disarmUs3d  = 6.25;
			disarmUs    = 2.400;
			calibrateUs = 2.500;
			walledUs    = 10.00;
			pwmHz       = 32000;
			timerHz     = 48000000; // full resolution
			//timerHz   = 12000000; // 1/4 resolution
			break;
		case ESC_MULTISHOT125:
			disarmUs3d  = 3.125;
			disarmUs    = 1.220;
			calibrateUs = 1.250;
			walledUs    = 5.000;
			pwmHz       = 32000;
			timerHz     = 48000000; // full resolution
			//timerHz   = 12000000; // 1/4 resolution
			break;
		case ESC_MULTISHOT:
		default:
			disarmUs3d  = 13.75;
			disarmUs    = 4.900;
			calibrateUs = 5.000;
			walledUs    = 22.50;
			pwmHz       = 32000;
			timerHz     = 48000000; // full resolution
			//timerHz     = 54000000; // full resolution
			//timerHz   = 12000000; // 1/4 resolution
			break;
	}


	// constrain motor update frequency between the 50Hz and the max allowed for the ESC protocol
	pwmHz = CONSTRAIN(mainConfig.mixerConfig.escUpdateFrequency, 50, pwmHz);
	// compute idle PWM width from idlePercent
	idleUs = ((walledUs - disarmUs) * (mainConfig.mixerConfig.idlePercent * 0.01) ) + disarmUs;

	disarmPulseValue3d  = ((uint32_t)(disarmUs3d * timerHz))  / 1000000;
	disarmPulseValue    = ((uint32_t)(disarmUs * timerHz))    / 1000000;
	calibratePulseValue = ((uint32_t)(calibrateUs * timerHz)) / 1000000;
	idlePulseValue      = ((uint32_t)(idleUs * timerHz))      / 1000000;
	walledPulseValue    = ((uint32_t)(walledUs * timerHz))    / 1000000;

	pulseValueRange  = walledPulseValue - idlePulseValue; //throttle for motor output is float motorThrottle * pulseValueRange + idlePulseValue;

	for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
		outputNumber = mainConfig.mixerConfig.motorOutput[motorNum];
		if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
			InitActuatorTimer(board.motors[outputNumber], pwmHz, timerHz);
		}
	}

	DelayMs(2); //give timer time to stabilize.

	// Start the timers
	for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
		outputNumber = mainConfig.mixerConfig.motorOutput[motorNum];
		if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
			HAL_TIM_Base_Start(&pwmTimers[board.motors[outputNumber].actuatorArrayNum]);
			HAL_TIM_PWM_Start(&pwmTimers[board.motors[outputNumber].actuatorArrayNum], board.motors[outputNumber].timChannel);
		}
	}

	DelayMs(2); //give timer time to stabilize.

}

void InitActuatorTimer(motor_type actuator, uint32_t pwmHz, uint32_t timerHz)
{
	uint16_t timerPrescaler = 0;

	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_OC_InitTypeDef sConfigOC;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_ClockConfigTypeDef sClockSourceConfig;

	timerPrescaler = (uint16_t)(SystemCoreClock / TimerPrescalerDivisor(actuator.timer) / timerHz) - 1;

	// Initialize GPIO
	HAL_GPIO_DeInit(ports[actuator.port], actuator.pin);

	GPIO_InitStruct.Pin       = actuator.pin;
	GPIO_InitStruct.Pull      = (actuator.polarity == TIM_OCPOLARITY_LOW) ? GPIO_PULLDOWN : GPIO_PULLUP;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = actuator.AF;

	HAL_GPIO_Init(ports[actuator.port], &GPIO_InitStruct);

	// Initialize timer
	pwmTimers[actuator.actuatorArrayNum].Instance           = timers[actuator.timer];
	pwmTimers[actuator.actuatorArrayNum].Init.Prescaler     = timerPrescaler;
	pwmTimers[actuator.actuatorArrayNum].Init.CounterMode   = TIM_COUNTERMODE_UP;
	pwmTimers[actuator.actuatorArrayNum].Init.Period        = (timerHz / pwmHz) - 1;
	pwmTimers[actuator.actuatorArrayNum].Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&pwmTimers[actuator.actuatorArrayNum]);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&pwmTimers[actuator.actuatorArrayNum], &sClockSourceConfig);

	HAL_TIM_PWM_Init(&pwmTimers[actuator.actuatorArrayNum]);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&pwmTimers[actuator.actuatorArrayNum], &sMasterConfig);

	// Initialize timer pwm channel

	sConfigOC.OCMode      = TIM_OCMODE_PWM2;
	sConfigOC.Pulse       = 0;
	sConfigOC.OCPolarity  = actuator.polarity;
	sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_PWM_ConfigChannel(&pwmTimers[actuator.actuatorArrayNum], &sConfigOC, actuator.timChannel);

}

inline void ThrottleToDshot(uint8_t *serialOutBuffer, float throttle, float idle) {
	uint32_t digitalThrottle;
	int32_t  checksum = 0;
	int32_t  checksumData;
	uint32_t currThrottle;

	if (idle > 0) {

		currThrottle = (uint32_t)InlineChangeRangef(throttle, 1.0, 0.0, 2047, (2000.0 * idle * 0.01)+48.0);

		digitalThrottle = ( ( (uint32_t)( currThrottle ) << 1 ) | 0); //0 is no telem request, 1 is telem request

	} else {

		digitalThrottle = 0;

	}

	checksumData = digitalThrottle;

    for (uint32_t i = 0; i < 3; i++) {
    	checksum ^=  checksumData;   // xor data by nibbles
    	checksumData >>= 4;
    }
    checksum &= 0xf;
    // append checksum
    digitalThrottle = ((digitalThrottle << 4) | checksum);

    serialOutBuffer[0] = (uint8_t)(digitalThrottle >> 8);
    serialOutBuffer[1] = (uint8_t)(digitalThrottle & 0x00ff);

}

void OutputActuators(volatile float motorOutput[], volatile float servoOutput[])
//inline void OutputActuators(volatile float motorOutput[], volatile float servoOutput[])
{
	(void)servoOutput;

	uint32_t outputNumber;
	uint32_t motorNum;
	uint8_t serialOutBuffer[2];

	if (boardArmed) {
		for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
			outputNumber = mainConfig.mixerConfig.motorOutput[motorNum];
			if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
				if ( IsDshotEnabled() ) {
					ThrottleToDshot(serialOutBuffer, motorOutput[motorNum], mainConfig.mixerConfig.idlePercent);
					OutputSerialDmaByte(serialOutBuffer, 2, board.motors[outputNumber], 1, 0, 1);
				} else {
					*ccr[board.motors[outputNumber].timCCR] = (uint16_t)(motorOutput[motorNum] * (float)pulseValueRange) + idlePulseValue;
				}
			}
		}
	} else if (calibrateMotors) {
		if (motorOutput[0] < 0.1) {
			for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
				outputNumber = mainConfig.mixerConfig.motorOutput[motorNum];
				if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
					if ( !IsDshotEnabled() ) {
						*ccr[board.motors[outputNumber].timCCR] = calibratePulseValue;
					}
				}
			}
		} else {
			for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
				outputNumber = mainConfig.mixerConfig.motorOutput[motorNum];
				if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
					if ( !IsDshotEnabled() ) {
						*ccr[board.motors[outputNumber].timCCR] = pulseValueRange + idlePulseValue;
					}
				}
			}
		}
	} else {
		for (motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
			outputNumber = mainConfig.mixerConfig.motorOutput[motorNum];
			if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
				if ( IsDshotEnabled() ) {
					ThrottleToDshot(serialOutBuffer, 0, 0);
					OutputSerialDmaByte(serialOutBuffer, 2, board.motors[outputNumber], 1, 0, 1);
				} else {
					if (threeDeeMode)
						*ccr[board.motors[outputNumber].timCCR] = disarmPulseValue3d;
					else
						*ccr[board.motors[outputNumber].timCCR] = disarmPulseValue;
				}
			}
		}
	}
}


void ZeroActuators(uint32_t delayUs)
{

	uint32_t outputNumber;
	uint8_t serialOutBuffer[2];

	if ( (mainConfig.mixerConfig.escProtcol != ESC_DSHOT600) && (mainConfig.mixerConfig.escProtcol != ESC_DSHOT300) && (mainConfig.mixerConfig.escProtcol != ESC_DSHOT150) )
		__disable_irq();

	for (uint32_t motorNum=0; motorNum < MAX_MOTOR_NUMBER; motorNum++)
	{
		outputNumber = mainConfig.mixerConfig.motorOutput[motorNum];
		if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
		{
			if ( (mainConfig.mixerConfig.escProtcol == ESC_DSHOT600) || (mainConfig.mixerConfig.escProtcol == ESC_DSHOT300) || (mainConfig.mixerConfig.escProtcol == ESC_DSHOT150) )
			{
				ThrottleToDshot(serialOutBuffer, 0, 0);
				OutputSerialDmaByte(serialOutBuffer, 2, board.motors[outputNumber], 1, 0, 1); //buffer with data, number of bytes, actuator to output on, msb, no serial frame
			}
			else
			{
				if (threeDeeMode)
					*ccr[board.motors[outputNumber].timCCR] = disarmPulseValue3d;
				else
					*ccr[board.motors[outputNumber].timCCR] = disarmPulseValue;
			}
		}
	}

	if (delayUs)
		simpleDelay_ASM(delayUs);

	if ( (mainConfig.mixerConfig.escProtcol != ESC_DSHOT600) && (mainConfig.mixerConfig.escProtcol != ESC_DSHOT300) && (mainConfig.mixerConfig.escProtcol != ESC_DSHOT150) )
		__enable_irq();

}

void IdleActuator(uint32_t motorNum)
{

	uint32_t outputNumber;
	uint8_t serialOutBuffer[2];

	outputNumber = mainConfig.mixerConfig.motorOutput[motorNum];

	if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
	{
		if ( IsDshotEnabled() )
		{
			ThrottleToDshot(serialOutBuffer, motorOutput[outputNumber], mainConfig.mixerConfig.idlePercent);
			OutputSerialDmaByte(serialOutBuffer, 2, board.motors[outputNumber], 1, 0, 1);
		}
		else
		{
			if (threeDeeMode)
				*ccr[board.motors[outputNumber].timCCR] = (uint16_t)disarmPulseValue3d; //idle is handled by mixer in 3D mode
			else
				*ccr[board.motors[outputNumber].timCCR] = (uint16_t)idlePulseValue;
		}
	}

}

