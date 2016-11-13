#pragma once

extern void OutputActuators(volatile float motorOutputHere[], volatile float servoOutput[]);
extern void InitActuators(void);
void InitActuatorTimer(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef *timer, uint32_t timerChannel, uint32_t alternateFunction, uint32_t polarity, uint32_t pulseUs, uint32_t pwmHz, uint32_t timerHz);
extern void ZeroActuators(void);
