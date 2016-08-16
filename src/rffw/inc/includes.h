#pragma once

#include "target.h"
#include "mcu_include.h"

#include "usbd_hid.h"
#include "usb_device.h"

//all drivers
#include "drivers/leds.h"
#include "drivers/pid.h"
#include "drivers/rx.h"
#include "drivers/filter.h"
#include "drivers/math.h"

void BoardInit(void);
void ErrorHandler(void);
