#pragma once

#include "target.h"
#include "mcu_include.h"

#include "usbd_hid.h"
#include "usb_device.h"

#if LEDn >= 1
#define LED1_ON     HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_GPIO_Pin, GPIO_PIN_RESET)
#define LED1_OFF    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_GPIO_Pin, GPIO_PIN_SET)
#define LED1_TOGGLE HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_GPIO_Pin)
#else
#define LED1_ON
#define LED1_OFF
#define LED1_TOGGLE
#endif

#if LEDn >= 2
#define LED2_ON     HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_GPIO_Pin, GPIO_PIN_RESET)
#define LED2_OFF    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_GPIO_Pin, GPIO_PIN_SET)
#define LED2_TOGGLE HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_GPIO_Pin)
#else
#define LED2_ON
#define LED2_OFF
#define LED2_TOGGLE
#endif

#if LEDn >= 3
#define LED3_ON     HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_GPIO_Pin, GPIO_PIN_RESET)
#define LED3_OFF    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_GPIO_Pin, GPIO_PIN_SET)
#define LED3_TOGGLE HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_GPIO_Pin)
#else
#define LED3_ON
#define LED3_OFF
#define LED3_TOGGLE
#endif

void BoardInit(void);
void ErrorHandler(void);
