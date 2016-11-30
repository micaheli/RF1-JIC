#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#include "../../low_level_driver/boarddef.h"
#include "../../recovery_loader/inc/drivers/leds.h"
#include "../../recovery_loader/inc/rfbl.h"
#include "../../recovery_loader/inc/usb/usb_device.h"
#include "mcu_include.h"
#include "general_functions.h"

#include "usbd_hid.h"
#include "rfbl.h"


void BoardInit(void);
void gpioClockInit(void);

//revolt and revo hard code
#ifdef STM32F405xx
#define RFBL_GPIO1				GPIOC
#define RFBL_PIN1				GPIO_PIN_7
#define RFBL_GPIO2				GPIOC
#define RFBL_PIN2				GPIO_PIN_6
#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_5
#endif#ifdef STM32F446xx
#define RFBL_GPIO1				GPIOC
#define RFBL_PIN1				GPIO_PIN_7
#define RFBL_GPIO2				GPIOC
#define RFBL_PIN2				GPIO_PIN_6
#define LED1_GPIO_Port          GPIOA
#define LED1_GPIO_Pin           GPIO_PIN_15
#endif
