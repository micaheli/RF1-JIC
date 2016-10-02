#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stddef.h>


#ifdef stm32f405xx
#include <stm32f405xx.h>
#define _GPIOE GPIOE
#define _GPIOF GPIOF
#define _GPIOG GPIOG
#define _GPIOH GPIOH
#define _GPIOI GPIOI
#define _GPIOJ NULL
#define _GPIOK NULL
#endif



#define _GPIOA GPIOA
#define _GPIOB GPIOB
#define _GPIOC GPIOC
#define _GPIOD GPIOD


#include "../../low_level_driver/boarddef.h"
#include "mcu_include.h"
#include "general_functions.h"
#include "scheduler.h"
#include "flight.h"

#include "usbd_hid.h"
#include "usb_device.h"

//input
#include "input/gyro.h"

//all drivers
#include "drivers/serial.h"
#include "drivers/leds.h"
#include "drivers/buzzer.h"
#include "drivers/pid.h"
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
