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
#include "includes.h"

//all drivers
#include "drivers/leds.h"
#include "drivers/pid.h"
#include "drivers/rx.h"
#include "drivers/filter.h"
#include "drivers/math.h"
#include "drivers/config.h"
#include "drivers/actuator_output.h"

void BoardInit(void);
void ErrorHandler(void);
