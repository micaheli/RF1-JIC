#include "../../../recovery_loader/inc/includes.h"


//todo: Do we want to init LEDs like this? Maybe an array is a better method
void LedInit (void) {
#if LEDn >= 1
    InitializeLED(LED1_GPIO_Port, LED1_GPIO_Pin);
#endif
#if LEDn >= 2
    InitializeLED(LED2_GPIO_Port, LED2_GPIO_Pin);
#endif
#if LEDn >= 3
    InitializeLED(LED3_GPIO_Port, LED3_GPIO_Pin);
#endif
}

void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(GPIOx, GPIO_Pin);

    GPIO_InitStructure.Pin = GPIO_Pin;

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);

    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
}
