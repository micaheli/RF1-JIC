#pragma once

void OutputActuators(float motorOutput[], float servoOutput[]);
void InitActuators(void);
void InitActuatorTimer(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef *timer, uint32_t timerChannel, uint32_t alternateFunction);
