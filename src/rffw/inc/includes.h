#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "target.h"
#include "mcu_include.h"
#include "general_functions.h"
#include "scheduler.h"
#include "flight.h"

#include "usbd_hid.h"
#include "usb_device.h"
#include "includes.h"

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
