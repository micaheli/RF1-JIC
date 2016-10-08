#include "../../recovery_loader/inc/includes.h"

void EXTI_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
    HAL_GPIO_DeInit(GPIOx, GPIO_Pin);
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin = GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);

    /* EXTI interrupt init */
    HAL_NVIC_SetPriority(IRQn, PreemptPriority, SubPriority);
    HAL_NVIC_EnableIRQ(IRQn);
}
