#include "includes.h"

void InitActuators(void) {

	InitActuatorTimer(GPIOE, GPIO_PIN_12, GPIO_AF1_TIM1 )
}

void InitActuatorTimer(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t alternateFunction) {

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = alternateFunction;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

	TIM_HandleTypeDef pwmTimer;

	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
	TIM_OC_InitTypeDef sConfigOC;

	pwmTimer.Instance = TIM1;
	pwmTimer.Init.Prescaler = 1000;
	pwmTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	pwmTimer.Init.Period = 8399;
	pwmTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	pwmTimer.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&pwmTimer);


	sConfigOC.OCMode = TIM_OCMODE_PWM2;
	sConfigOC.Pulse = 4000;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;
	HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3);
}

void OutputActuators(float motorOutput[], float servoOutput[]) {
	(void)(motorOutput);
	(void)(servoOutput);
}
