#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
