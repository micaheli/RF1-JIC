#pragma once

#include "mcu_include.h"



//DMA1 St1  USART3 RX

//DMA2 St0 SPI1 RX
//DMA2 St3 SPI1 RX

//DMA2 St7 USART1 TX
//DMA2 St2 USART1 RX

//DMA1 St0 M4/PA2
//DMA1 St6 M3/PA3
//DMA1 St M2/PA3





//DMA1 Ch7, St4, USART3 TX
//DMA1 Ch4, St1, USART3 RX

//DMA2 Ch3, St0 GYRO SPI1 RX
//DMA2 Ch3, St3 GYRO SPI1 TX

//DMA1 Ch0, St3 ESC SPI2 RX
//DMA1 Ch4, St5 ESC SPI2 TX

//DMA1 Ch0, St0 FLASH SPI3 RX
//DMA1 Ch0, St5 FLASH SPI3 TX

//DMA1 Ch6, St4 LED

//DMA1 Ch5, St7 M1
//DMA1 Ch5, St2 M2
//DMA1 Ch3, St6 M3
//DMA1 Ch3, St1 M4


//USB config
#define RFFW_HID_PRODUCT_STRING "RaceFlight FC"
#define RFBL_HID_PRODUCT_STRING "RaceFlight Boot Loader"
#define RFRC_HID_PRODUCT_STRING "RaceFlight Recovery"

//MCU config
#define FC_PLLM	8
#define FC_PLLN	384
#define FC_PLLP	2
#define FC_PLLQ	8


//LED config

#define LEDn                    2

#define LED1_ENABLED			1
#define LED1_GPIO_Port          _PORTB
#define LED1_GPIO_Pin           GPIO_PIN_5
#define LED1_INVERTED			0

#define LED2_ENABLED			0
#define LED2_GPIO_Port          0
#define LED2_GPIO_Pin           0
#define LED2_INVERTED			0

#define LED3_ENABLED			0
#define LED3_GPIO_Port          0
#define LED3_GPIO_Pin           0
#define LED3_INVERTED			0

//buzzer setup
#define BUZZER_GPIO_Port        _PORTB
#define BUZZER_GPIO_Pin         GPIO_PIN_4

//ws2812
//PB6
#define WS2812_LED_Port			_PORTB
#define WS2812_LED_Pin			GPIO_PIN_6

//#define SPEKTRUM_TELEM

//Gyro Config
#define GYRO_SPI                SPI1
#define GYRO_SPI_NUMBER			0		//starting from 0. 1 = SPI2
#define GYRO_SPI_CS_GPIO_Port   GPIOA
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_4

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_2
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     GPIOC
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_4
#define GYRO_EXTI_IRQn          EXTI4_IRQn
#define GYRO_EXTI_IRQHandler    EXTI4_IRQHandler

#define GYRO_TX_DMA_IRQn        DMA2_Stream3_IRQn
#define GYRO_TX_DMA_IRQHandler  DMA2_Stream3_IRQHandler
#define GYRO_RX_DMA_IRQn        DMA2_Stream0_IRQn
#define GYRO_RX_DMA_IRQHandler  DMA2_Stream0_IRQHandler


#define RECEIVER_UART			2 //UART number starting from 0. 4 = UART5

//Sbus inverter config
#define USE_SBUS_HARDWARE_INVERTER
#define SBUS_HARDWARE_GPIO			GPIOC
#define SBUS_HARDWARE_PIN			GPIO_PIN_0
#define SBUS_INVERTER_USART			USART1


//Serial Config
#define USE_VCP
#define VBUS_SENSING_GPIO			GPIOC
#define VBUS_SENSING_PIN			GPIO_PIN_5






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

#define SPI2_ENABLE					0
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

#define SPI3_ENABLE					1
#define SPI3_NSS_PIN				GPIO_PIN_3
#define SPI3_SCK_PIN				GPIO_PIN_10
#define SPI3_MISO_PIN				GPIO_PIN_11
#define SPI3_MOSI_PIN				GPIO_PIN_12
#define SPI3_NSS_GPIO_PORT			GPIOB
#define SPI3_SCK_GPIO_PORT			GPIOC
#define SPI3_MISO_GPIO_PORT			GPIOC
#define SPI3_MOSI_GPIO_PORT			GPIOC
#define SPI3_SCK_AF					GPIO_AF6_SPI3
#define SPI3_MISO_AF				GPIO_AF6_SPI3
#define SPI3_MOSI_AF				GPIO_AF6_SPI3
#define SPI3_SCK_AF					GPIO_AF6_SPI3
#define SPI3_TX_DMA_STREAM			DMA1_Stream5
#define SPI3_TX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI3_TX_DMA_IRQn			DMA1_Stream5_IRQn
#define SPI3_TX_DMA_IRQHandler		DMA1_Stream5_IRQHandler
#define SPI3_RX_DMA_STREAM			DMA1_Stream0
#define SPI3_RX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI3_RX_DMA_IRQn			DMA1_Stream0_IRQn
#define SPI3_RX_DMA_IRQHandler		DMA1_Stream0_IRQHandler

//END SPI defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//START USART defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//USART Config
/*
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
*/

//USART 1 unused
/*
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
*/
/*
#define USE_USART3
#define USARTx					   USART3
#define USARTx_TX_PIN              GPIO_PIN_10
#define USARTx_TX_GPIO_PORT        GPIOB
#define USARTx_TX_AF               GPIO_AF7_USART3
#define USARTx_RX_PIN              GPIO_PIN_11
#define USARTx_RX_GPIO_PORT        GPIOB
#define USARTx_RX_AF               GPIO_AF7_USART3
#define USARTx_IRQHandler          USART3_IRQHandler
#define USARTx_IRQn                USART3_IRQn
#define USARTx_TX_DMA_STREAM       DMA1_Stream3
#define USARTx_TX_DMA_CHANNEL      DMA_CHANNEL_4
#define USARTx_TX_DMA_IRQn         DMA1_Stream3_IRQn
#define USARTx_TX_DMA_IRQHandler   DMA1_Stream3_IRQHandler
#define USARTx_RX_DMA_STREAM       DMA1_Stream3
#define USARTx_RX_DMA_CHANNEL      DMA_CHANNEL_4
#define USARTx_RX_DMA_IRQn         DMA1_Stream1_IRQn
#define USARTx_RX_DMA_IRQHandler   DMA1_Stream1_IRQHandler


*/
/*
*/
#define USE_SPEKTRUM_SERIAL

//spektrum
#ifdef USE_SPEKTRUM_SERIAL
	#define USE_USART1
	#define USARTx					   USART1
	#define USARTx_TX_PIN              GPIO_PIN_9
	#define USARTx_TX_GPIO_PORT        GPIOA
	#define USARTx_TX_AF               GPIO_AF7_USART1
	#define USARTx_RX_PIN              GPIO_PIN_9
	#define USARTx_RX_GPIO_PORT        GPIOA
	#define USARTx_RX_AF               GPIO_AF7_USART1
	#define USARTx_IRQHandler          UART1_IRQHandler
	#define USARTx_IRQn                UART1_IRQn
	#define USARTx_TX_DMA_STREAM       DMA2_Stream7
	#define USARTx_TX_DMA_CHANNEL      DMA_CHANNEL_4
	#define USARTx_TX_DMA_IRQn         DMA2_Stream7_IRQn
	#define USARTx_TX_DMA_IRQHandler   DMA2_Stream7_IRQHandler
	#define USARTx_RX_DMA_STREAM       DMA2_Stream2
	#define USARTx_RX_DMA_CHANNEL      DMA_CHANNEL_4
	#define USARTx_RX_DMA_IRQn         DMA2_Stream2_IRQn
	#define USARTx_RX_DMA_IRQHandler   DMA2_Stream2_IRQHandler

	#define USARTx_BAUDRATE            (uint32_t)115200
	#define USARTx_WORDLENGTH          (uint32_t)UART_WORDLENGTH_8B
	#define USARTx_STOPBITS            (uint32_t)UART_STOPBITS_1
	#define USARTx_PARITY              (uint32_t)UART_PARITY_NONE
	#define USARTx_HWFLOWCTRL          (uint32_t)UART_HWCONTROL_NONE
	#define USARTx_MODE                (uint32_t)UART_MODE_TX_RX
#else
	//frsky
	#define USE_SBUS_SERIAL
	#define USE_USART1
	#define USARTx					   USART1
	#define USARTx_TX_PIN              GPIO_PIN_9
	#define USARTx_TX_GPIO_PORT        GPIOA
	#define USARTx_TX_AF               GPIO_AF7_USART1
	#define USARTx_RX_PIN              GPIO_PIN_10
	#define USARTx_RX_GPIO_PORT        GPIOA
	#define USARTx_RX_AF               GPIO_AF7_USART1
	#define USARTx_IRQHandler          USART1_IRQHandler
	#define USARTx_IRQn                USART1_IRQn
	#define USARTx_TX_DMA_STREAM       DMA2_Stream7
	#define USARTx_TX_DMA_CHANNEL      DMA_CHANNEL_4
	#define USARTx_TX_DMA_IRQn         DMA2_Stream7_IRQn
	#define USARTx_TX_DMA_IRQHandler   DMA2_Stream7_IRQHandler
	#define USARTx_RX_DMA_STREAM       DMA2_Stream2
	#define USARTx_RX_DMA_CHANNEL      DMA_CHANNEL_4
	#define USARTx_RX_DMA_IRQn         DMA2_Stream2_IRQn
	#define USARTx_RX_DMA_IRQHandler   DMA2_Stream2_IRQHandler

	#define USARTx_BAUDRATE            (uint32_t)100000
	#define USARTx_WORDLENGTH          (uint32_t)UART_WORDLENGTH_8B
	#define USARTx_STOPBITS            (uint32_t)UART_STOPBITS_2
	#define USARTx_PARITY              (uint32_t)UART_PARITY_EVEN
	#define USARTx_HWFLOWCTRL          (uint32_t)UART_HWCONTROL_NONE
	#define USARTx_MODE                (uint32_t)UART_MODE_TX_RX
#endif
/*
*/

/*
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
*/

//#define USE_UART5
#define USART5_TX_PIN              0
#define USART5_TX_GPIO_PORT        0
#define USART5_TX_AF               0
#define USART5_RX_PIN              0
#define USART5_RX_GPIO_PORT        0
#define USART5_RX_AF               0
#define USART5_IRQHandler          0
#define USART5_IRQn                0
#define USART5_TX_DMA_STREAM       0
#define USART5_TX_DMA_CHANNEL      0
#define USART5_TX_DMA_IRQn         0
#define USART5_TX_DMA_IRQHandler   0
#define USART5_RX_DMA_STREAM       0
#define USART5_RX_DMA_CHANNEL      0
#define USART5_RX_DMA_IRQn         0
#define USART5_RX_DMA_IRQHandler   0

/*
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
*/
//END USART defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Motor defines
//motor Assignments
#define MOTOR1_TIM					ENUMTIM3
#define MOTOR1_PIN					GPIO_PIN_0
#define MOTOR1_GPIO					_PORTB
#define MOTOR1_ALTERNATE			GPIO_AF2_TIM3
#define MOTOR1_TIM_CH				TIM_CHANNEL_3
#define MOTOR1_TIM_CCR				TIM3CCR3
#define MOTOR1_TIM_CCR1				CCR3
#define MOTOR1_POLARITY				TIM_OCPOLARITY_LOW

#define MOTOR2_TIM					ENUMTIM3
#define MOTOR2_PIN					GPIO_PIN_1
#define MOTOR2_GPIO					_PORTB
#define MOTOR2_ALTERNATE			GPIO_AF2_TIM3
#define MOTOR2_TIM_CH				TIM_CHANNEL_4
#define MOTOR2_TIM_CCR				TIM3CCR4
#define MOTOR2_TIM_CCR1				CCR4
#define MOTOR2_POLARITY				TIM_OCPOLARITY_LOW

#define MOTOR3_TIM					ENUMTIM2
#define MOTOR3_PIN					GPIO_PIN_3
#define MOTOR3_GPIO					_PORTA
#define MOTOR3_ALTERNATE			GPIO_AF1_TIM2
#define MOTOR3_TIM_CH				TIM_CHANNEL_4
#define MOTOR3_TIM_CCR				TIM2CCR4
#define MOTOR3_TIM_CCR1				CCR4
#define MOTOR3_POLARITY				TIM_OCPOLARITY_LOW

#define MOTOR4_TIM					ENUMTIM2
#define MOTOR4_PIN					GPIO_PIN_2
#define MOTOR4_GPIO					_PORTA
#define MOTOR4_ALTERNATE			GPIO_AF1_TIM2
#define MOTOR4_TIM_CH				TIM_CHANNEL_3
#define MOTOR4_TIM_CCR				TIM2CCR3
#define MOTOR4_POLARITY				TIM_OCPOLARITY_LOW


//Flash Config
#define FLASH_SPI					SPI3
#define FLASH_SPI_CS_GPIO_Pin		SPI3_NSS_PIN
#define FLASH_SPI_CS_GPIO_Port		SPI3_NSS_GPIO_PORT
#define FLASH_SPI_SCK_PIN			SPI3_SCK_PIN
#define FLASH_SPI_SCK_GPIO_PORT		SPI3_SCK_GPIO_PORT
#define FLASH_SPI_SCK_AF			SPI3_SCK_AF
#define FLASH_SPI_MISO_PIN			SPI3_MISO_PIN
#define FLASH_SPI_MISO_GPIO_PORT	SPI3_MISO_GPIO_PORT
#define FLASH_SPI_MISO_AF			SPI3_MISO_AF
#define FLASH_SPI_MOSI_PIN			SPI3_MOSI_PIN
#define FLASH_SPI_MOSI_GPIO_PORT	SPI3_MOSI_GPIO_PORT
#define FLASH_SPI_MOSI_AF			SPI3_MOSI_AF

#define FLASH_SPI_BAUD				SPI_BAUDRATEPRESCALER_2

#define FLASH_DMA_TX_IRQn			SPI3_TX_DMA_IRQn
#define FLASH_DMA_TX_IRQHandler		SPI3_TX_DMA_IRQHandler
#define FLASH_DMA_RX_IRQn			SPI3_RX_DMA_IRQn
#define FLASH_DMA_RX_IRQHandler		SPI3_RX_DMA_IRQHandler