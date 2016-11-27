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


//input
#include "input/gyro.h"
#include "usbd_hid.h"
#include "usb_device.h"


//all drivers
#include "drivers/spektrumTelemetry.h"
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
void ErrorHandler(uint32_t error);

#define GYRO_INIT_FAILIURE                0
#define SERIAL_INIT_FAILIURE              1
#define DMA_INIT_FAILIURE                 2
#define FLASH_INIT_FAILIURE               3
#define WS2812_LED_INIT_FAILIURE          4
#define FLASH_SPI_INIT_FAILIURE           5
#define GYRO_SETUP_COMMUNICATION_FAILIURE 6
#define SERIAL_HALF_DUPLEX_INIT_FAILURE   7
#define SERIAL_INIT_FAILURE               8
#define MSP_DMA_GYRO_RX_INIT_FAILIURE     9
#define MSP_DMA_GYRO_TX_INIT_FAILIURE     10
#define MSP_DMA_SPI1_RX_INIT_FAILIURE     11
#define MSP_DMA_SPI1_TX_INIT_FAILIURE     12
#define MSP_DMA_SPI2_RX_INIT_FAILIURE     13
#define MSP_DMA_SPI2_TX_INIT_FAILIURE     14
#define MSP_DMA_SPI3_RX_INIT_FAILIURE     15
#define MSP_DMA_SPI3_TX_INIT_FAILIURE     16
#define HARD_FAULT                        17
#define MEM_FAULT                         18
#define BUS_FAULT                         19
#define USAGE_FAULT                       20
#define GYRO_SPI_INIT_FAILIURE            21
#define TIMER_INPUT_INIT_FAILIURE         22

