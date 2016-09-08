#pragma once

void EXTI_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
