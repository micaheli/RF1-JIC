#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <limits.h>

#define CONSTRAIN(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#include "board.h"
#include "bldc.h"

//low level drivers and driver based functions
#include "../../low_level_driver/boarddef.h"
#include "mcu_include.h"
#include "general_functions.h"
#include "led.h"
#include "exti.h"
#include "power.h"
#include "sound.h"
#include "scheduler.h"

//#include "usbd_hid.h"
//#include "usb_device.h"


#include "drivers/leds.h"

void BoardInit(void);
void gpioClockInit(void);


//unused functions reference by board defs
extern void GyroExtiCallback(uint32_t callbackNumber);
extern void GyroRxDmaCallback(uint32_t callbackNumber);
extern void FlashDmaRxCallback(uint32_t callbackNumber);
extern void MaxOsdDmaRxCallback(uint32_t callbackNumber);
