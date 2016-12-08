#pragma once


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <limits.h>

//low level drivers and driver based functions
#include "../../low_level_driver/boarddef.h"
#include "mcu_include.h"
#include "general_functions.h"
#include "exti.h"

//general includes
#include "pid.h"
#include "mixer.h"
#include "debug.h"
#include "flight.h"
#include "filter.h"
#include "watchdog.h"
#include "scheduler.h"
#include "flight_logger.h"
#include "rfbl_functions.h"

//telemetry includes
#include "sPortTelemetry.h"
#include "telemetry.h"
#include "mspTelemetry.h"
#include "spektrumTelemetry.h"


//input
#include "input/gyro.h"
#include "usbd_hid.h"
#include "usb_device.h"


//all drivers
#include "drivers/serial.h"
#include "drivers/leds.h"
#include "drivers/buzzer.h"
#include "drivers/rx.h"
#include "drivers/flash_chip.h"
#include "drivers/actuator_output.h"
#include "drivers/invensense_bus.h"
#include "drivers/invensense_device.h"
#include "drivers/invensense_register_map.h"
#include "drivers/dmaShenanigans.h"
#include "drivers/esc_1wire.h"


//config
#include "config.h"
#include "rf_math.h"

#include <arm_math.h>

void BoardInit(void);
