#pragma once

void EXTI_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority, uint32_t gpioModeIt);
void EXTI_Deinit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, IRQn_Type IRQn);
