#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stddef.h>



#define _TIM1 TIM1
#define _TIM2 TIM2
#define _TIM3 TIM3
#define _TIM4 TIM4
#define _TIM5 TIM5
#define _TIM6 TIM6
#define _TIM7 TIM7
#define _TIM8 TIM8
#define _TIM9 TIM9
#define _TIM10 TIM10
#define _TIM11 TIM11
#define _TIM12 TIM12
#define _TIM13 TIM13
#define _TIM14 TIM14

#define _GPIOA GPIOA
#define _GPIOB GPIOB
#define _GPIOC GPIOC
#define _GPIOD GPIOD

#ifdef stm32f405xx

#include <stm32f405xx.h>

#define _GPIOE GPIOE
#define _GPIOF GPIOF
#define _GPIOG GPIOG
#define _GPIOH GPIOH
#define _GPIOI GPIOI
#define _GPIOJ NULL
#define _GPIOK NULL

#define _USART1 USART1
#define _USART1s 1
#define _USART2 USART2
#define _USART2s 1
#define _USART3 USART3
#define _USART3s 1
#define _USART4 UART4
#define _USART4s 0
#define _USART5 UART5
#define _USART5s 0
#define _USART6 USART6
#define _USART6s 1
#define _USART7 USART7
#define _USART7s 1


//STM32F4 UID address
#define DEVICE_ID1					0x1FFF7A10
#define DEVICE_ID2					0x1FFF7A14
#define DEVICE_ID3					0x1FFF7A18


#define USE_RFBL
#define ADDRESS_RFBL_START		(0x08000000)
#define ADDRESS_CONFIG_START	(0x0800C000)
#define ADDRESS_FLASH_START		(0x08020000)
#define ADDRESS_FLASH_END		(0x080FFFF0)

#endif



#include "../../low_level_driver/boarddef.h"
#include "mcu_include.h"
#include "general_functions.h"
#include "scheduler.h"
#include "flight.h"
#include "debug.h"

#include "usbd_hid.h"
#include "usb_device.h"

//input
#include "input/gyro.h"

//all drivers
#include "drivers/serial.h"
#include "drivers/leds.h"
#include "drivers/buzzer.h"
#include "pid.h"
#include "drivers/rx.h"
#include "drivers/filter.h"
#include "drivers/math.h"
#include "drivers/config.h"
#include "drivers/mixer.h"
#include "drivers/actuator_output.h"
#include "drivers/invensense_device.h"
#include "drivers/invensense_bus.h"

void BoardInit(void);
void ErrorHandler(void);
