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

#define LEDn                    2

#define LED1_ENABLED			1
#define LED1_GPIO_Port          0
#define LED1_GPIO_Pin           GPIO_PIN_15
#define LED1_INVERTED			1

#define LED2_ENABLED			1
#define LED2_GPIO_Port          2
#define LED2_GPIO_Pin           GPIO_PIN_8
#define LED2_INVERTED			1

#define LED3_ENABLED			0
#define LED3_GPIO_Port          0
#define LED3_GPIO_Pin           0
#define LED3_INVERTED			0

//buzzer setup
#define BUZZER_GPIO_Port        2
#define BUZZER_GPIO_Pin         GPIO_PIN_2

#define SPEKTRUM_TELEM

//Gyro Config
#define GYRO_SPI                SPI2
#define GYRO_SPI_NUMBER			1		//starting from 0. 1 = SPI2
#define GYRO_SPI_CS_GPIO_Port   GPIOB
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_12

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_2
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     GPIOC
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_13
#define GYRO_EXTI_IRQn          EXTI15_10_IRQn
#define GYRO_EXTI_IRQHandler    EXTI15_10_IRQHandler

#define GYRO_TX_DMA_IRQn        DMA1_Stream4_IRQn
#define GYRO_TX_DMA_IRQHandler  DMA1_Stream4_IRQHandler
#define GYRO_RX_DMA_IRQn        DMA1_Stream3_IRQn
#define GYRO_RX_DMA_IRQHandler  DMA1_Stream3_IRQHandler


#define RECEIVER_UART			4 //UART number starting from 0. 4 = UART5
//Sbus inverter config
//#define USE_SBUS_HARDWARE_INVERTER
//#define SBUS_HARDWARE_GPIO			GPIOC
//#define SBUS_HARDWARE_PIN			GPIO_PIN_0
//#define SBUS_INVERTER_USART			USART1


//Serial Config
#define USE_VCP
#define VBUS_SENSING_GPIO			GPIOA
#define VBUS_SENSING_PIN			GPIO_PIN_9






//START SPI defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SPI1_ENABLE					1
#define SPI1_NSS_PIN				GPIO_PIN_4
#define SPI1_SCK_PIN				GPIO_PIN_5
#define SPI1_MISO_PIN				GPIO_PIN_6
#define SPI1_MOSI_PIN				GPIO_PIN_7
#define SPI1_NSS_GPIO_PORT			GPIOA
#define SPI1_SCK_GPIO_PORT			GPIOA
#define SPI1_MISO_GPIO_PORT			GPIOA
#define SPI1_MOSI_GPIO_PORT			GPIOA
#define SPI1_SCK_AF					GPIO_AF5_SPI1
#define SPI1_MISO_AF				GPIO_AF5_SPI1
#define SPI1_MOSI_AF				GPIO_AF5_SPI1
#define SPI1_SCK_AF					GPIO_AF5_SPI1					
#define SPI1_TX_DMA_STREAM			DMA2_Stream3
#define SPI1_TX_DMA_CHANNEL			DMA_CHANNEL_3
#define SPI1_TX_DMA_IRQn			DMA2_Stream3_IRQn
#define SPI1_TX_DMA_IRQHandler		DMA2_Stream3_IRQHandler
#define SPI1_RX_DMA_STREAM			DMA2_Stream0
#define SPI1_RX_DMA_CHANNEL			DMA_CHANNEL_3
#define SPI1_RX_DMA_IRQn			DMA2_Stream0_IRQn
#define SPI1_RX_DMA_IRQHandler		DMA2_Stream0_IRQHandler

#define SPI2_ENABLE					1
#define SPI2_NSS_PIN				GPIO_PIN_12
#define SPI2_SCK_PIN				GPIO_PIN_13
#define SPI2_MISO_PIN				GPIO_PIN_14
#define SPI2_MOSI_PIN				GPIO_PIN_15
#define SPI2_NSS_GPIO_PORT			GPIOB
#define SPI2_SCK_GPIO_PORT			GPIOB
#define SPI2_MISO_GPIO_PORT			GPIOB
#define SPI2_MOSI_GPIO_PORT			GPIOB
#define SPI2_SCK_AF					GPIO_AF5_SPI2
#define SPI2_MISO_AF				GPIO_AF5_SPI2
#define SPI2_MOSI_AF				GPIO_AF5_SPI2
#define SPI2_SCK_AF					GPIO_AF5_SPI2
#define SPI2_TX_DMA_STREAM			DMA1_Stream4
#define SPI2_TX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI2_TX_DMA_IRQn			DMA1_Stream4_IRQn
#define SPI2_TX_DMA_IRQHandler		DMA1_Stream4_IRQHandler
#define SPI2_RX_DMA_STREAM			DMA1_Stream3
#define SPI2_RX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI2_RX_DMA_IRQn			DMA1_Stream3_IRQn
#define SPI2_RX_DMA_IRQHandler		DMA1_Stream3_IRQHandler

#define SPI3_ENABLE					0
#define SPI3_NSS_PIN				0
#define SPI3_SCK_PIN				0
#define SPI3_MISO_PIN				0
#define SPI3_MOSI_PIN				0
#define SPI3_NSS_GPIO_PORT			0
#define SPI3_SCK_GPIO_PORT			0
#define SPI3_MISO_GPIO_PORT			0
#define SPI3_MOSI_GPIO_PORT			0
#define SPI3_SCK_AF					0
#define SPI3_MISO_AF				0
#define SPI3_MOSI_AF				0
#define SPI3_SCK_AF					0
#define SPI3_TX_DMA_STREAM			0
#define SPI3_TX_DMA_CHANNEL			0
#define SPI3_TX_DMA_IRQn			0
#define SPI3_TX_DMA_IRQHandler		0
#define SPI3_RX_DMA_STREAM			0
#define SPI3_RX_DMA_CHANNEL			0
#define SPI3_RX_DMA_IRQn			0
#define SPI3_RX_DMA_IRQHandler		0

//END SPI defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//START USART defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//USART Config
#define USARTx                     UART5
#define USARTx_TX_PIN              GPIO_PIN_12
#define USARTx_TX_GPIO_PORT        GPIOC
#define USARTx_TX_AF               GPIO_AF8_UART5
#define USARTx_RX_PIN              GPIO_PIN_12
#define USARTx_RX_GPIO_PORT        GPIOC
#define USARTx_RX_AF               GPIO_AF8_UART5
#define USARTx_IRQHandler          UART5_IRQHandler
#define USARTx_IRQn                UART5_IRQn
#define USARTx_TX_DMA_STREAM       DMA1_Stream7
#define USARTx_RX_DMA_STREAM       DMA1_Stream0
#define USARTx_TX_DMA_CHANNEL      DMA_CHANNEL_4
#define USARTx_RX_DMA_CHANNEL      DMA_CHANNEL_4
#define USARTx_TX_DMA_IRQn         DMA1_Stream7_IRQn
#define USARTx_RX_DMA_IRQn         DMA1_Stream0_IRQn
#define USARTx_TX_DMA_IRQHandler   DMA1_Stream7_IRQHandler
#define USARTx_RX_DMA_IRQHandler   DMA1_Stream0_IRQHandler

//USART 1 unused

#define USE_USART2
#define USART2_TX_PIN              GPIO_PIN_2
#define USART2_TX_GPIO_PORT        GPIOA
#define USART2_TX_AF               GPIO_AF7_USART2
#define USART2_RX_PIN              GPIO_PIN_3
#define USART2_RX_GPIO_PORT        GPIOA
#define USART2_RX_AF               GPIO_AF7_USART2
#define USART2_IRQHandler          USART2_IRQHandler
#define USART2_IRQn                USART2_IRQn
#define USART2_TX_DMA_STREAM       0				//No TX DMA on USART 2
#define USART2_TX_DMA_CHANNEL      0
#define USART2_TX_DMA_IRQn         0
#define USART2_TX_DMA_IRQHandler   0
#define USART2_RX_DMA_STREAM       0				//No RX DMA on USART 2
#define USART2_RX_DMA_CHANNEL      0
#define USART2_RX_DMA_IRQn         0
#define USART2_RX_DMA_IRQHandler   0

#define USE_USART3
#define USART3_TX_PIN              GPIO_PIN_10
#define USART3_TX_GPIO_PORT        GPIOC
#define USART3_TX_AF               GPIO_AF7_USART3
#define USART3_RX_PIN              GPIO_PIN_11
#define USART3_RX_GPIO_PORT        GPIOC
#define USART3_RX_AF               GPIO_AF7_USART3
#define USART3_IRQHandler          USART3_IRQHandler
#define USART3_IRQn                USART3_IRQn
#define USART3_TX_DMA_STREAM       0				//No TX DMA on USART 3
#define USART3_TX_DMA_CHANNEL      0
#define USART3_TX_DMA_IRQn         0
#define USART3_TX_DMA_IRQHandler   0
#define USART3_RX_DMA_STREAM       DMA1_Stream1
#define USART3_RX_DMA_CHANNEL      DMA_CHANNEL_4
#define USART3_RX_DMA_IRQn         DMA1_Stream1_IRQn
#define USART3_RX_DMA_IRQHandler   DMA1_Stream1_IRQHandler

#define USE_UART4
#define USART4_TX_PIN              GPIO_PIN_0
#define USART4_TX_GPIO_PORT        GPIOA
#define USART4_TX_AF               GPIO_AF8_UART4
#define USART4_RX_PIN              GPIO_PIN_1
#define USART4_RX_GPIO_PORT        GPIOA
#define USART4_RX_AF               GPIO_AF8_UART4
#define USART4_IRQHandler          UART4_IRQHandler
#define USART4_IRQn                UART4_IRQn
#define USART4_TX_DMA_STREAM       0				//No TX DMA on USART 4
#define USART4_TX_DMA_CHANNEL      0
#define USART4_TX_DMA_IRQn         0
#define USART4_TX_DMA_IRQHandler   0
#define USART4_RX_DMA_STREAM       0				//No RX DMA on USART 4
#define USART4_RX_DMA_CHANNEL      0
#define USART4_RX_DMA_IRQn         0
#define USART4_RX_DMA_IRQHandler   0

#define USE_UART5
#define USART5_TX_PIN              GPIO_PIN_12
#define USART5_TX_GPIO_PORT        GPIOC
#define USART5_TX_AF               GPIO_AF8_UART5
#define USART5_RX_PIN              GPIO_PIN_12
#define USART5_RX_GPIO_PORT        GPIOC
#define USART5_RX_AF               GPIO_AF8_UART5
#define USART5_IRQHandler          UART5_IRQHandler
#define USART5_IRQn                UART5_IRQn
#define USART5_TX_DMA_STREAM       DMA1_Stream7
#define USART5_TX_DMA_CHANNEL      DMA_CHANNEL_4
#define USART5_TX_DMA_IRQn         DMA1_Stream7_IRQn
#define USART5_TX_DMA_IRQHandler   DMA1_Stream7_IRQHandler
#define USART5_RX_DMA_STREAM       DMA1_Stream0
#define USART5_RX_DMA_CHANNEL      DMA_CHANNEL_4
#define USART5_RX_DMA_IRQn         DMA1_Stream0_IRQn
#define USART5_RX_DMA_IRQHandler   DMA1_Stream0_IRQHandler

#define USE_USART6
#define USART6_TX_PIN              GPIO_PIN_6
#define USART6_TX_GPIO_PORT        GPIOC
#define USART6_TX_AF               GPIO_AF8_USART6
#define USART6_RX_PIN              GPIO_PIN_6
#define USART6_RX_GPIO_PORT        GPIOC
#define USART6_RX_AF               GPIO_AF8_USART6
#define USART6_IRQHandler          USART6_IRQHandler
#define USART6_IRQn                USART6_IRQn
#define USART6_TX_DMA_STREAM       DMA2_Stream6
#define USART6_TX_DMA_CHANNEL      DMA_CHANNEL_4
#define USART6_TX_DMA_IRQn         DMA2_Stream6_IRQn
#define USART6_TX_DMA_IRQHandler   DMA2_Stream6_IRQHandler
#define USART6_RX_DMA_STREAM       DMA2_Stream2
#define USART6_RX_DMA_CHANNEL      DMA_CHANNEL_4
#define USART6_RX_DMA_IRQn         DMA2_Stream2_IRQn
#define USART6_RX_DMA_IRQHandler   DMA2_Stream2_IRQHandler
//END USART defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Motor defines
//motor Assignments
#define MOTOR1_TIM					TIM3
#define MOTOR1_PIN					GPIO_PIN_1
#define MOTOR1_GPIO					GPIOB
#define MOTOR1_ALTERNATE			GPIO_AF2_TIM3
#define MOTOR1_TIM_CH				TIM_CHANNEL_4
#define MOTOR1_TIM_CCR				MOTOR1_TIM->CCR4

#define MOTOR2_TIM					TIM3
#define MOTOR2_PIN					GPIO_PIN_0
#define MOTOR2_GPIO					GPIOB
#define MOTOR2_ALTERNATE			GPIO_AF2_TIM3
#define MOTOR2_TIM_CH				TIM_CHANNEL_3
#define MOTOR2_TIM_CCR				MOTOR2_TIM->CCR3

#define MOTOR3_TIM					TIM3
#define MOTOR3_PIN					GPIO_PIN_5
#define MOTOR3_GPIO					GPIOB
#define MOTOR3_ALTERNATE			GPIO_AF2_TIM3
#define MOTOR3_TIM_CH				TIM_CHANNEL_2
#define MOTOR3_TIM_CCR				MOTOR3_TIM->CCR2

#define MOTOR4_TIM					TIM3
#define MOTOR4_PIN					GPIO_PIN_4
#define MOTOR4_GPIO					GPIOB
#define MOTOR4_ALTERNATE			GPIO_AF2_TIM3
#define MOTOR4_TIM_CH				TIM_CHANNEL_1
#define MOTOR4_TIM_CCR				MOTOR4_TIM->CCR1
