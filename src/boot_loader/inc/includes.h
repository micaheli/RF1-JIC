#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#include "../../low_level_driver/boarddef.h"
#include "mcu_include.h"
#include "general_functions.h"

#include "usbd_hid.h"
#include "usb_device.h"

#include "rfbl.h"

#include "drivers/leds.h"

void BoardInit(void);
void gpioClockInit(void);

//revolt and revo hard code
#ifdef STM32F745xx
#undef RFBL_GPIO1
#undef RFBL_PIN1
#undef RFBL_GPIO2
#undef RFBL_PIN2
#undef LED1_GPIO_Port
#undef LED1_GPIO_Pin
#define RFBL_GPIO1				GPIOC
#define RFBL_PIN1				GPIO_PIN_7
#define RFBL_GPIO2				GPIOC
#define RFBL_PIN2				GPIO_PIN_6
#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_5
#endif

#ifdef STM32F405xx
#undef RFBL_GPIO1
#undef RFBL_PIN1
#undef RFBL_GPIO2
#undef RFBL_PIN2
#undef LED1_GPIO_Port
#undef LED1_GPIO_Pin
#define RFBL_GPIO1				GPIOC
#define RFBL_PIN1				GPIO_PIN_7
#define RFBL_GPIO2				GPIOC
#define RFBL_PIN2				GPIO_PIN_6
#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_5

#define VBUS_SENSING_GPIO		GPIOC
#define VBUS_SENSING_PIN		GPIO_PIN_5
#endif

#ifdef STM32F446xx
#undef RFBL_GPIO1
#undef RFBL_PIN1
#undef RFBL_GPIO2
#undef RFBL_PIN2
#undef LED1_GPIO_Port
#undef LED1_GPIO_Pin
#define RFBL_GPIO1				GPIOC
#define RFBL_PIN1				GPIO_PIN_7
#define RFBL_GPIO2				GPIOC
#define RFBL_PIN2				GPIO_PIN_6
#define LED1_GPIO_Port          GPIOA
#define LED1_GPIO_Pin           GPIO_PIN_15

#define VBUS_SENSING_GPIO		GPIOA
#define VBUS_SENSING_PIN		GPIO_PIN_9
#endif
