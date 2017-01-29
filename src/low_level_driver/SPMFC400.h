#pragma once

#include "mcu_include.h"

//USB config
#define RFFW_HID_PRODUCT_STRING "RaceFlight FC"
#define RFBL_HID_PRODUCT_STRING "RaceFlight Boot Loader"
#define RFRC_HID_PRODUCT_STRING "RaceFlight Recovery"

//MCU config
#define FC_PLLM	12
#define FC_PLLN	384
#define FC_PLLP	2
#define FC_PLLQ	8

//LED config
#define LED1_ENABLED			1
#define LED1_GPIO_Port          ENUM_PORTA
#define LED1_GPIO_Pin           GPIO_PIN_15
#define LED1_INVERTED			1

#define LED2_ENABLED			1
#define LED2_GPIO_Port          ENUM_PORTC
#define LED2_GPIO_Pin           GPIO_PIN_8
#define LED2_INVERTED			1

#define LED3_ENABLED			0
#define LED3_GPIO_Port          0
#define LED3_GPIO_Pin           0
#define LED3_INVERTED			0

//buzzer setup
#define BUZZER_GPIO_Port        ENUM_PORTC
#define BUZZER_GPIO_Pin         GPIO_PIN_2

//Gyro Config
#define GYRO_SPI_NUMBER			ENUM_SPI2
#define GYRO_SPI_TX_ENUM		ENUM_DMA1_STREAM_4
#define GYRO_SPI_RX_ENUM		ENUM_DMA1_STREAM_3
#define GYRO_SPI_NUMBER			ENUM_SPI2
#define GYRO_SPI_CS_GPIO_Port   ENUM_PORTB
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_12
#define GYRO_RX_DMA_FP          FP_DMA1_S3

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_2
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     ENUM_PORTC
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_4
#define GYRO_EXTI_IRQn          EXTI15_10_IRQn
#define GYRO_EXTI_IRQn_FP       FP_EXTI15_10

//Sbus inverter config
#undef  USE_SBUS_HARDWARE_INVERTER
#define SBUS_HARDWARE_GPIO			0
#define SBUS_HARDWARE_PIN			0
#define SBUS_INVERTER_USART			0

//Serial Config
#undef  USE_VCP
#define VBUS_SENSING_GPIO			0
#define VBUS_SENSING_PIN			0

//Serial Config
#define VBUS_SENSING_GPIO			GPIOA
#define VBUS_SENSING_PIN			GPIO_PIN_9

//START SPI defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SPI1_ENABLE					1
#define SPI1_NSS_PIN				GPIO_PIN_4
#define SPI1_SCK_PIN				GPIO_PIN_5
#define SPI1_MISO_PIN				GPIO_PIN_6
#define SPI1_MOSI_PIN				GPIO_PIN_7
#define SPI1_NSS_GPIO_PORT			ENUM_PORTA
#define SPI1_SCK_GPIO_PORT			ENUM_PORTA
#define SPI1_MISO_GPIO_PORT			ENUM_PORTA
#define SPI1_MOSI_GPIO_PORT			ENUM_PORTA
#define SPI1_SCK_AF					GPIO_AF5_SPI1
#define SPI1_MISO_AF				GPIO_AF5_SPI1
#define SPI1_MOSI_AF				GPIO_AF5_SPI1
#define SPI1_SCK_AF					GPIO_AF5_SPI1
#define SPI1_PRIORITY				4
#define SPI1_TX_DMA_STREAM			ENUM_DMA2_STREAM_3
#define SPI1_TX_DMA_CHANNEL			DMA_CHANNEL_3
#define SPI1_TX_DMA_IRQn			DMA2_Stream3_IRQn
#define SPI1_TX_DMA_PRIORITY		4
#define SPI1_RX_DMA_STREAM			ENUM_DMA2_STREAM_0
#define SPI1_RX_DMA_CHANNEL			DMA_CHANNEL_3
#define SPI1_RX_DMA_IRQn			DMA2_Stream0_IRQn
#define SPI1_RX_DMA_PRIORITY		4
#define SPI1_SCK_PULL				GPIO_PULLDOWN

#define SPI2_ENABLE					1
#define SPI2_NSS_PIN				GPIO_PIN_12
#define SPI2_SCK_PIN				GPIO_PIN_13
#define SPI2_MISO_PIN				GPIO_PIN_14
#define SPI2_MOSI_PIN				GPIO_PIN_15
#define SPI2_NSS_GPIO_PORT			ENUM_PORTB
#define SPI2_SCK_GPIO_PORT			ENUM_PORTB
#define SPI2_MISO_GPIO_PORT			ENUM_PORTB
#define SPI2_MOSI_GPIO_PORT			ENUM_PORTB
#define SPI2_SCK_AF					GPIO_AF5_SPI2
#define SPI2_MISO_AF				GPIO_AF5_SPI2
#define SPI2_MOSI_AF				GPIO_AF5_SPI2
#define SPI2_SCK_AF					GPIO_AF5_SPI2
#define SPI2_PRIORITY				1
#define SPI2_TX_DMA_STREAM			ENUM_DMA1_STREAM_4
#define SPI2_TX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI2_TX_DMA_IRQn			DMA1_Stream4_IRQn
#define SPI2_TX_DMA_PRIORITY		1
#define SPI2_RX_DMA_STREAM			ENUM_DMA1_STREAM_3
#define SPI2_RX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI2_RX_DMA_IRQn			DMA1_Stream3_IRQn
#define SPI2_RX_DMA_PRIORITY		1
#define SPI2_SCK_PULL				GPIO_PULLDOWN

#define SPI3_ENABLE					0
#define SPI3_NSS_PIN				GPIO_PIN_0
#define SPI3_SCK_PIN				GPIO_PIN_3
#define SPI3_MISO_PIN				GPIO_PIN_4
#define SPI3_MOSI_PIN				GPIO_PIN_5
#define SPI3_NSS_GPIO_PORT			ENUM_PORTB
#define SPI3_SCK_GPIO_PORT			ENUM_PORTB
#define SPI3_MISO_GPIO_PORT			ENUM_PORTB
#define SPI3_MOSI_GPIO_PORT			ENUM_PORTB
#define SPI3_SCK_AF					GPIO_AF6_SPI3
#define SPI3_MISO_AF				GPIO_AF6_SPI3
#define SPI3_MOSI_AF				GPIO_AF6_SPI3
#define SPI3_SCK_AF					GPIO_AF6_SPI3
#define SPI3_PRIORITY				8
#define SPI3_TX_DMA_STREAM			ENUM_DMA1_STREAM_5
#define SPI3_TX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI3_TX_DMA_IRQn			DMA1_Stream5_IRQn
#define SPI3_TX_DMA_PRIORITY		8
#define SPI3_RX_DMA_STREAM			ENUM_DMA1_STREAM_2
#define SPI3_RX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI3_RX_DMA_IRQn			DMA1_Stream2_IRQn
#define SPI3_RX_DMA_PRIORITY		8
#define SPI3_SCK_PULL				GPIO_PULLDOWN
//END SPI defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//START USART defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//END USART defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Motor defines
//motor Assignments
#define MOTOR3_TIM					ENUM_TIM3
#define MOTOR3_PIN					GPIO_PIN_1
#define MOTOR3_GPIO					ENUM_PORTB
#define MOTOR3_ALTERNATE			GPIO_AF2_TIM3
#define MOTOR3_TIM_CH				TIM_CHANNEL_4
#define MOTOR3_TIM_CCR				ENUM_TIM3CCR4
#define MOTOR3_TIM_CCR1				CCR4
#define MOTOR3_POLARITY				TIM_OCPOLARITY_LOW

#define MOTOR2_TIM					ENUM_TIM3
#define MOTOR2_PIN					GPIO_PIN_0
#define MOTOR2_GPIO					ENUM_PORTB
#define MOTOR2_ALTERNATE			GPIO_AF2_TIM3
#define MOTOR2_TIM_CH				TIM_CHANNEL_3
#define MOTOR2_TIM_CCR				ENUM_TIM3CCR3
#define MOTOR2_TIM_CCR1				CCR3
#define MOTOR2_POLARITY				TIM_OCPOLARITY_LOW

#define MOTOR4_TIM					ENUM_TIM3
#define MOTOR4_PIN					GPIO_PIN_5
#define MOTOR4_GPIO					ENUM_PORTB
#define MOTOR4_ALTERNATE			GPIO_AF2_TIM3
#define MOTOR4_TIM_CH				TIM_CHANNEL_2
#define MOTOR4_TIM_CCR				ENUM_TIM3CCR2
#define MOTOR4_TIM_CCR1				CCR2
#define MOTOR4_POLARITY				TIM_OCPOLARITY_LOW

#define MOTOR1_TIM					ENUM_TIM3
#define MOTOR1_PIN					GPIO_PIN_4
#define MOTOR1_GPIO					ENUM_PORTB
#define MOTOR1_ALTERNATE			GPIO_AF2_TIM3
#define MOTOR1_TIM_CH				TIM_CHANNEL_1
#define MOTOR1_TIM_CCR				ENUM_TIM3CCR1
#define MOTOR1_TIM_CCR1				CCR1
#define MOTOR1_POLARITY				TIM_OCPOLARITY_LOW


//Flash Config
#define FLASH_SPI_NUMBER			ENUM_SPI1
#define FLASH_SPI_CS_GPIO_Pin		SPI1_NSS_PIN
#define FLASH_SPI_CS_GPIO_Port		SPI1_NSS_GPIO_PORT
#define FLASH_SPI_SCK_PIN			SPI1_SCK_PIN
#define FLASH_SPI_SCK_GPIO_PORT		SPI1_SCK_GPIO_PORT
#define FLASH_SPI_SCK_AF			SPI1_SCK_AF
#define FLASH_SPI_MISO_PIN			SPI1_MISO_PIN
#define FLASH_SPI_MISO_GPIO_PORT	SPI1_MISO_GPIO_PORT
#define FLASH_SPI_MISO_AF			SPI1_MISO_AF
#define FLASH_SPI_MOSI_PIN			SPI1_MOSI_PIN
#define FLASH_SPI_MOSI_GPIO_PORT	SPI1_MOSI_GPIO_PORT
#define FLASH_SPI_MOSI_AF			SPI1_MOSI_AF
#define FLASH_RX_DMA_FP				FP_DMA2_S0

#define FLASH_SPI_FAST_BAUD			SPI_BAUDRATEPRESCALER_4
#define FLASH_SPI_SLOW_BAUD			SPI_BAUDRATEPRESCALER_4

#define FLASH_DMA_TX_IRQn			SPI1_TX_DMA_IRQn
#define FLASH_DMA_TX_IRQHandler		SPI1_TX_DMA_IRQHandler
#define FLASH_DMA_RX_IRQn			SPI1_RX_DMA_IRQn
#define FLASH_DMA_RX_IRQHandler		SPI1_RX_DMA_IRQHandler
