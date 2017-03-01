#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#include "../../low_level_driver/boarddef.h"
#include "mcu_include.h"
#include "general_functions.h"

#include "usbd_hid.h"
#include "usb_device.h"

#include "rfbl.h"

#include "drivers/leds.h"


enum {
	GYRO_INIT_FAILIURE                =  (1 << 0),
	SERIAL_INIT_FAILIURE              =  (1 << 1),
	DMA_INIT_FAILIURE                 =  (1 << 2),
	FLASH_INIT_FAILIURE               =  (1 << 3),
	WS2812_LED_INIT_FAILIURE          =  (1 << 4),
	FLASH_SPI_INIT_FAILIURE           =  (1 << 5),
	GYRO_SETUP_COMMUNICATION_FAILIURE =  (1 << 6),
	SERIAL_HALF_DUPLEX_INIT_FAILURE   =  (1 << 7),
	SERIAL_INIT_FAILURE               =  (1 << 8),
	MSP_DMA_GYRO_RX_INIT_FAILIURE     =  (1 << 9),
	MSP_DMA_GYRO_TX_INIT_FAILIURE     =  (1 << 10),
	MSP_DMA_SPI_RX_INIT_FAILIURE      =  (1 << 11),
	MSP_DMA_SPI_TX_INIT_FAILIURE      =  (1 << 12),
	NOT_USED1					      =  (1 << 13),
	NOT_USED2					      =  (1 << 14),
	NOT_USED3					      =  (1 << 15),
	NOT_USED4					      =  (1 << 16),
	HARD_FAULT                        =  (1 << 17),
	MEM_FAULT                         =  (1 << 18),
	BUS_FAULT                         =  (1 << 19),
	USAGE_FAULT                       =  (1 << 20),
	GYRO_SPI_INIT_FAILIURE            =  (1 << 21),
	TIMER_INPUT_INIT_FAILIURE         =  (1 << 22),
	ADC_INIT_FAILIURE                 =  (1 << 23),
	ADC_DMA_INIT_FAILIURE             =  (1 << 24),
};


void BoardInit(void);
void gpioClockInit(void);

//revolt and revo hard code
#ifdef STM32F745xx
#undef RFBL_GPIO1
#undef RFBL_PIN1
#undef RFBL_GPIO2
#undef RFBL_PIN2
#undef LED1_GPIO_Port
#undef LED1_GPIO_Pin
#define RFBL_GPIO1				GPIOC
#define RFBL_PIN1				GPIO_PIN_7
#define RFBL_GPIO2				GPIOC
#define RFBL_PIN2				GPIO_PIN_6
#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_5
#endif

#ifdef STM32F405xx
#undef RFBL_GPIO1
#undef RFBL_PIN1
#undef RFBL_GPIO2
#undef RFBL_PIN2
#undef LED1_GPIO_Port
#undef LED1_GPIO_Pin
#define RFBL_GPIO1				GPIOC
#define RFBL_PIN1				GPIO_PIN_7
#define RFBL_GPIO2				GPIOC
#define RFBL_PIN2				GPIO_PIN_6
#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_5

#define VBUS_SENSING_GPIO		GPIOC
#define VBUS_SENSING_PIN		GPIO_PIN_5
#endif

#ifdef STM32F446xx
#undef RFBL_GPIO1
#undef RFBL_PIN1
#undef RFBL_GPIO2
#undef RFBL_PIN2
#undef LED1_GPIO_Port
#undef LED1_GPIO_Pin
#define RFBL_GPIO1				GPIOC
#define RFBL_PIN1				GPIO_PIN_7
#define RFBL_GPIO2				GPIOC
#define RFBL_PIN2				GPIO_PIN_6
#define LED1_GPIO_Port          GPIOA
#define LED1_GPIO_Pin           GPIO_PIN_15

#define VBUS_SENSING_GPIO		GPIOA
#define VBUS_SENSING_PIN		GPIO_PIN_9
#endif
