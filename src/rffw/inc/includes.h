#pragma once

#include "target.h"
#include "mcu_include.h"

#include "usbd_hid.h"
#include "usb_device.h"

#include "drivers/leds.h"

void BoardInit(void);
void ErrorHandler(void);
