#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "../../flight_controller/inc/rf_math.h"

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
void ErrorHandler(void);
