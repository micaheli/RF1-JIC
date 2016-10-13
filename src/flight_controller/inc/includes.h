#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stddef.h>


#include "../../low_level_driver/boarddef.h"
#include "mcu_include.h"
#include "general_functions.h"
#include "scheduler.h"
#include "flight.h"
#include "debug.h"
#include "rfbl_functions.h"

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
#include "filter.h"
#include "math.h"
#include "mixer.h"
#include "config.h"
#include "drivers/actuator_output.h"
#include "drivers/invensense_bus.h"
#include "drivers/invensense_device.h"
#include "drivers/invensense_register_map.h"

void BoardInit(void);
void ErrorHandler(void);
