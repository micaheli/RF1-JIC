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
void ErrorHandler(void);

//revolt and revo hard code
#define RFBL_GPIO1				GPIOC
#define RFBL_PIN1				GPIO_PIN_7
#define RFBL_GPIO2				GPIOC
#define RFBL_PIN2				GPIO_PIN_6
#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_5

#define VBUS_SENSING_GPIO		GPIOC
#define VBUS_SENSING_PIN		GPIO_PIN_5
