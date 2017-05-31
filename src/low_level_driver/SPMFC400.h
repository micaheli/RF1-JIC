#pragma once

#include "mcu_include.h"

#define SPMFC400

//USB config
#define RFFW_HID_PRODUCT_STRING "RaceFlight FC"
#define RFBL_HID_PRODUCT_STRING "RaceFlight Boot Loader"
#define RFRC_HID_PRODUCT_STRING "RaceFlight Recovery"

#define FC_NAME "Spektrum SPMFCF400"

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
#define BUZZER_TIM				0
#define BUZZER_ALTERNATE		0
#define BUZZER_TIM_CH			0
#define BUZZER_TIM_CCR			0
#define BUZZER_POLARITY			TIM_OCPOLARITY_HIGH

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
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_13
#define GYRO_EXTI_IRQn          EXTI15_10_IRQn
#define GYRO_EXTI_IRQn_FP       FP_EXTI15_10

//Sbus inverter config
#undef  USE_SBUS_HARDWARE_INVERTER
#define SBUS_HARDWARE_GPIO			0
#define SBUS_HARDWARE_PIN			0
#define SBUS_INVERTER_USART			0

//Serial Config
#define VBUS_SENSING
#define VBUS_SENSING_GPIO			ENUM_PORTA
#define VBUS_SENSING_PIN			GPIO_PIN_9

//ADC Config
#define ADC0_TYPE					ADC_CURRENT
#define ADC0_PORT					ENUM_PORTC
#define ADC0_PIN					GPIO_PIN_3
#define ADC0_INSTANCE				ENUM_ADC3
#define ADC0_CHANNEL				ADC_CHANNEL_13

#define ADC1_TYPE					ADC_VOLTAGE
#define ADC1_PORT					ENUM_PORTC
#define ADC1_PIN					GPIO_PIN_0
#define ADC1_INSTANCE				ENUM_ADC3
#define ADC1_CHANNEL				ADC_CHANNEL_10

#define ADC2_TYPE					ADC_DISABLED
#define ADC2_PORT					0
#define ADC2_PIN					0
#define ADC2_INSTANCE				0
#define ADC2_CHANNEL				0

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
#undef USART_USED_ADVANCED

#define USART1_ENABLED					0
#define USART1_PINMODE					0
#define USART1_PINPULL					0
#define USART1_TXALT					0
#define USART1_TXPIN					0
#define USART1_TXPORT					0
#define USART1_TXDMA					0
#define USART1_TXDMA_IRQN				0
#define USART1_TXDMA_CHANNEL			0
#define USART1_RXALT					0
#define USART1_RXPIN					0
#define USART1_RXPORT					0
#define USART1_RXDMA					0
#define USART1_RXDMA_IRQN				0
#define USART1_RXDMA_CHANNEL			0

#define USART2_ENABLED					1
#define USART2_PINMODE					GPIO_MODE_AF_PP
#define USART2_PINPULL					GPIO_PULLUP
#define USART2_TXALT					GPIO_AF7_USART2
#define USART2_TXPIN					GPIO_PIN_2
#define USART2_TXPORT					ENUM_PORTA
#define USART2_TXDMA					ENUM_DMA1_STREAM_6
#define USART2_TXDMA_IRQN				DMA1_Stream6_IRQn
#define USART2_TXDMA_CHANNEL			DMA_CHANNEL_4
#define USART2_RXALT					GPIO_AF7_USART1
#define USART2_RXPIN					GPIO_PIN_3
#define USART2_RXPORT					ENUM_PORTA
#define USART2_RXDMA					ENUM_DMA1_STREAM_5
#define USART2_RXDMA_IRQN				DMA1_Stream5_IRQn
#define USART2_RXDMA_CHANNEL			DMA_CHANNEL_4

#define USART3_ENABLED					1
#define USART3_PINMODE					GPIO_MODE_AF_PP
#define USART3_PINPULL					GPIO_PULLUP
#define USART3_TXALT					GPIO_AF7_USART3
#define USART3_TXPIN					GPIO_PIN_10
#define USART3_TXPORT					ENUM_PORTC
#define USART3_TXDMA					0
#define USART3_TXDMA_IRQN				0
#define USART3_TXDMA_CHANNEL			0
#define USART3_RXALT					GPIO_AF7_USART3
#define USART3_RXPIN					GPIO_PIN_11
#define USART3_RXPORT					ENUM_PORTC
#define USART3_RXDMA					ENUM_DMA1_STREAM_1
#define USART3_RXDMA_IRQN				DMA1_Stream1_IRQn
#define USART3_RXDMA_CHANNEL			DMA_CHANNEL_4


#define USART4_ENABLED					1
#define USART4_PINMODE					GPIO_MODE_AF_PP
#define USART4_PINPULL					GPIO_PULLUP
#define USART4_TXALT					GPIO_AF8_UART4
#define USART4_TXPIN					GPIO_PIN_0
#define USART4_TXPORT					ENUM_PORTA
#define USART4_TXDMA					0
#define USART4_TXDMA_IRQN				0
#define USART4_TXDMA_CHANNEL			0
#define USART4_RXALT					GPIO_AF8_UART4
#define USART4_RXPIN					GPIO_PIN_1
#define USART4_RXPORT					ENUM_PORTA
#define USART4_RXDMA					ENUM_DMA1_STREAM_2
#define USART4_RXDMA_IRQN				DMA1_Stream2_IRQn
#define USART4_RXDMA_CHANNEL			DMA_CHANNEL_4

#define USART5_ENABLED					1
#define USART5_PINMODE					GPIO_MODE_AF_PP
#define USART5_PINPULL					GPIO_PULLUP
#define USART5_TXALT					GPIO_AF8_UART5
#define USART5_TXPIN					GPIO_PIN_12
#define USART5_TXPORT					ENUM_PORTC
#define USART5_TXDMA					ENUM_DMA1_STREAM_7
#define USART5_TXDMA_IRQN				DMA1_Stream7_IRQn
#define USART5_TXDMA_CHANNEL			DMA_CHANNEL_4
#define USART5_RXALT					GPIO_AF8_UART5
#define USART5_RXPIN					GPIO_PIN_2		//Rx pin isn't actually broken out. Just defined for autodetect to work.
#define USART5_RXPORT					ENUM_PORTD
#define USART5_RXDMA					ENUM_DMA1_STREAM_0
#define USART5_RXDMA_IRQN				DMA1_Stream0_IRQn
#define USART5_RXDMA_CHANNEL			DMA_CHANNEL_4

#define USART6_ENABLED					1
#define USART6_PINMODE					GPIO_MODE_AF_PP
#define USART6_PINPULL					GPIO_PULLUP
#define USART6_TXALT					GPIO_AF8_USART6
#define USART6_TXPIN					GPIO_PIN_6
#define USART6_TXPORT					ENUM_PORTC
#define USART6_TXDMA					ENUM_DMA2_STREAM_6
#define USART6_TXDMA_IRQN				DMA2_Stream6_IRQn
#define USART6_TXDMA_CHANNEL			DMA_CHANNEL_4
#define USART6_RXALT					GPIO_AF8_USART6
#define USART6_RXPIN					GPIO_PIN_6
#define USART6_RXPORT					ENUM_PORTC
#define USART6_RXDMA					ENUM_DMA2_STREAM_2
#define USART6_RXDMA_IRQN				DMA2_Stream2_IRQn
#define USART6_RXDMA_CHANNEL			DMA_CHANNEL_4
//END USART defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//General Timer Config
#define GENERAL_TIMER1					ENUM_TIM6
#define GENERAL_TIMER1_IRQN				TIM6_DAC_IRQn
#define GENERAL_TIMER2					ENUM_TIM7
#define GENERAL_TIMER2_IRQN				TIM7_IRQn

//Motor defines
//motor Assignments
#define ACTUATOR1_TYPE					ENUM_ACTUATOR_TYPE_MOTOR
#define ACTUATOR1_TIM					ENUM_TIM3
#define ACTUATOR1_PIN					GPIO_PIN_1
#define ACTUATOR1_GPIO					ENUM_PORTB
#define ACTUATOR1_ALTERNATE				GPIO_AF2_TIM5
#define ACTUATOR1_TIM_CH				TIM_CHANNEL_4
#define ACTUATOR1_TIM_CCR				ENUM_TIM3CCR4
#define ACTUATOR1_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR1_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_4
#define ACTUATOR1_DMA_ENABLED			1
#define ACTUATOR1_DMA					ENUM_DMA1_STREAM_2
#define ACTUATOR1_DMA_IRQN				DMA1_Stream2_IRQn
#define ACTUATOR1_DMA_CHANNEL			DMA_CHANNEL_5
#define ACTUATOR1_DMA_HANDLE			TIM_DMA_ID_CC4
#define ACTUATOR1_DMACALLBACK			FP_DMA1_S2
#define ACTUATOR1_IRQN					TIM3_IRQn
#define ACTUATOR1_EXTIN					EXTI1_IRQn
#define ACTUATOR1_EXTICALLBACK			FP_EXTI1

#define ACTUATOR2_TYPE					ENUM_ACTUATOR_TYPE_MOTOR
#define ACTUATOR2_TIM					ENUM_TIM3
#define ACTUATOR2_PIN					GPIO_PIN_0
#define ACTUATOR2_GPIO					ENUM_PORTB
#define ACTUATOR2_ALTERNATE				GPIO_AF2_TIM3
#define ACTUATOR2_TIM_CH				TIM_CHANNEL_3
#define ACTUATOR2_TIM_CCR				ENUM_TIM3CCR3
#define ACTUATOR2_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR2_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_3
#define ACTUATOR2_DMA_ENABLED			1
#define ACTUATOR2_DMA					ENUM_DMA1_STREAM_7
#define ACTUATOR2_DMA_IRQN				DMA1_Stream7_IRQn
#define ACTUATOR2_DMA_CHANNEL			DMA_CHANNEL_5
#define ACTUATOR2_DMA_HANDLE			TIM_DMA_ID_CC3
#define ACTUATOR2_DMACALLBACK			FP_DMA1_S7
#define ACTUATOR2_IRQN					TIM3_IRQn
#define ACTUATOR2_EXTIN					EXTI0_IRQn
#define ACTUATOR2_EXTICALLBACK			FP_EXTI0

#define ACTUATOR3_TYPE					ENUM_ACTUATOR_TYPE_MOTOR
#define ACTUATOR3_TIM					ENUM_TIM3
#define ACTUATOR3_PIN					GPIO_PIN_5
#define ACTUATOR3_GPIO					ENUM_PORTB
#define ACTUATOR3_ALTERNATE				GPIO_AF2_TIM3
#define ACTUATOR3_TIM_CH				TIM_CHANNEL_2
#define ACTUATOR3_TIM_CCR				ENUM_TIM3CCR2
#define ACTUATOR3_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR3_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_2
#define ACTUATOR3_DMA_ENABLED			1
#define ACTUATOR3_DMA					ENUM_DMA1_STREAM_5
#define ACTUATOR3_DMA_IRQN				DMA1_Stream5_IRQn
#define ACTUATOR3_DMA_CHANNEL			DMA_CHANNEL_5
#define ACTUATOR3_DMA_HANDLE			TIM_DMA_ID_CC2
#define ACTUATOR3_DMACALLBACK			FP_DMA1_S5
#define ACTUATOR3_IRQN					TIM3_IRQn
#define ACTUATOR3_EXTIN					EXTI9_5_IRQn
#define ACTUATOR3_EXTICALLBACK			FP_EXTI9_5

#define ACTUATOR4_TYPE					ENUM_ACTUATOR_TYPE_MOTOR
#define ACTUATOR4_TIM					ENUM_TIM3
#define ACTUATOR4_PIN					GPIO_PIN_4
#define ACTUATOR4_GPIO					ENUM_PORTB
#define ACTUATOR4_ALTERNATE				GPIO_AF2_TIM3
#define ACTUATOR4_TIM_CH				TIM_CHANNEL_1
#define ACTUATOR4_TIM_CCR				ENUM_TIM3CCR1
#define ACTUATOR4_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR4_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_1
#define ACTUATOR4_DMA_ENABLED			1
#define ACTUATOR4_DMA					ENUM_DMA1_STREAM_4
#define ACTUATOR4_DMA_IRQN				DMA1_Stream4_IRQn
#define ACTUATOR4_DMA_CHANNEL			DMA_CHANNEL_5
#define ACTUATOR4_DMA_HANDLE			TIM_DMA_ID_CC1
#define ACTUATOR4_DMACALLBACK			FP_DMA1_S4
#define ACTUATOR4_IRQN					TIM3_IRQn
#define ACTUATOR4_EXTIN					EXTI4_IRQn
#define ACTUATOR4_EXTICALLBACK			FP_EXTI4

#define ACTUATOR5_TYPE					0
#define ACTUATOR5_TIM					0
#define ACTUATOR5_PIN					0
#define ACTUATOR5_GPIO					0
#define ACTUATOR5_ALTERNATE				0
#define ACTUATOR5_TIM_CH				0
#define ACTUATOR5_TIM_CCR				0
#define ACTUATOR5_POLARITY				0
#define ACTUATOR5_ACTIVE_TIM			0
#define ACTUATOR5_DMA_ENABLED			0
#define ACTUATOR5_DMA					0
#define ACTUATOR5_DMA_IRQN				0
#define ACTUATOR5_DMA_CHANNEL			0
#define ACTUATOR5_DMA_HANDLE			0
#define ACTUATOR5_DMACALLBACK			0
#define ACTUATOR5_IRQN					0
#define ACTUATOR5_EXTIN					0
#define ACTUATOR5_EXTICALLBACK			0

#define ACTUATOR6_TYPE					ENUM_ACTUATOR_TYPE_SPMLAPTIMER
#define ACTUATOR6_TIM					ENUM_TIM1
#define ACTUATOR6_PIN					GPIO_PIN_10
#define ACTUATOR6_GPIO					ENUM_PORTA
#define ACTUATOR6_ALTERNATE				GPIO_AF1_TIM1
#define ACTUATOR6_TIM_CH				TIM_CHANNEL_3
#define ACTUATOR6_TIM_CCR				ENUM_TIM1CCR3
#define ACTUATOR6_POLARITY				GPIO_PULLUP
#define ACTUATOR6_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_3
#define ACTUATOR6_DMA_ENABLED			0
#define ACTUATOR6_DMA					ENUM_DMA2_STREAM_6
#define ACTUATOR6_DMA_IRQN				DMA2_Stream6_IRQn
#define ACTUATOR6_DMA_CHANNEL			DMA_CHANNEL_6
#define ACTUATOR6_DMA_HANDLE			TIM_DMA_ID_CC3
#define ACTUATOR6_DMACALLBACK			0
#define ACTUATOR6_IRQN					0
#define ACTUATOR6_IRQN_BRK				TIM1_BRK_TIM9_IRQn
#define ACTUATOR6_IRQN_UP				TIM1_UP_TIM10_IRQn
#define ACTUATOR6_IRQN_TRG				TIM1_TRG_COM_TIM11_IRQn
#define ACTUATOR6_IRQN_CC				TIM1_CC_IRQn
#define ACTUATOR6_EXTIN					0
#define ACTUATOR6_EXTICALLBACK			0

#define ACTUATOR7_TYPE					ENUM_ACTUATOR_TYPE_WS2812
#define ACTUATOR7_TIM					ENUM_TIM8
#define ACTUATOR7_PIN					GPIO_PIN_6
#define ACTUATOR7_GPIO					ENUM_PORTC
#define ACTUATOR7_ALTERNATE				GPIO_AF3_TIM8
#define ACTUATOR7_TIM_CH				TIM_CHANNEL_1
#define ACTUATOR7_TIM_CCR				ENUM_TIM8CCR1
#define ACTUATOR7_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR7_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_1
#define ACTUATOR7_DMA_ENABLED			1
#define ACTUATOR7_DMA					ENUM_DMA2_STREAM_2
#define ACTUATOR7_DMA_IRQN				DMA2_Stream2_IRQn
#define ACTUATOR7_DMA_CHANNEL			DMA_CHANNEL_7
#define ACTUATOR7_DMA_HANDLE			TIM_DMA_ID_CC1
#define ACTUATOR7_DMACALLBACK			0
#define ACTUATOR7_IRQN					0
#define ACTUATOR7_EXTIN					0
#define ACTUATOR7_EXTICALLBACK			0

#define ACTUATOR8_TYPE					ENUM_ACTUATOR_TYPE_SPORT
#define ACTUATOR8_TIM					ENUM_TIM4
#define ACTUATOR8_PIN					GPIO_PIN_6
#define ACTUATOR8_GPIO					ENUM_PORTB
#define ACTUATOR8_ALTERNATE				GPIO_AF2_TIM4
#define ACTUATOR8_TIM_CH				TIM_CHANNEL_1
#define ACTUATOR8_TIM_CCR				ENUM_TIM2CCR1
#define ACTUATOR8_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR8_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_1
#define ACTUATOR8_DMA_ENABLED			1
#define ACTUATOR8_DMA					ENUM_DMA1_STREAM_0
#define ACTUATOR8_DMA_IRQN				DMA1_Stream0_IRQn
#define ACTUATOR8_DMA_CHANNEL			DMA_CHANNEL_2
#define ACTUATOR8_DMA_HANDLE			TIM_DMA_ID_CC1
#define ACTUATOR8_DMACALLBACK			FP_DMA1_S0
#define ACTUATOR8_IRQN					TIM4_IRQn
#define ACTUATOR8_EXTIN					EXTI9_5_IRQn
#define ACTUATOR8_EXTICALLBACK			FP_EXTI9_5


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
