#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "target.h"
#include "mcu_include.h"

#include "usbd_hid.h"
#include "usb_device.h"

#include "rfbl.h"

#include "drivers/leds.h"

void BoardInit(void);
void gpioClockInit(void);
void inlineDigitalHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void inlineDigitalLo(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
bool inlineIsPinStatusHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void ErrorHandler(void);
