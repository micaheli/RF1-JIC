#pragma once

#include "mcu_include.h"

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
#define BUZZER_TIM				ENUMTIM3
#define BUZZER_ALTERNATE		GPIO_AF2_TIM3
#define BUZZER_TIM_CH			TIM_CHANNEL_1
#define BUZZER_TIM_CCR			TIM3CCR1
#define BUZZER_POLARITY			TIM_OCPOLARITY_LOW

//ws2812
//PB6
#define WS2812_LED_Port			_PORTB
#define WS2812_LED_Pin			GPIO_PIN_6

//#define SPEKTRUM_TELEM

//Gyro Config
#define GYRO_SPI_NUMBER			ENUM_SPI1
#define GYRO_SPI_TX_ENUM		ENUM_DMA2_STREAM_3
#define GYRO_SPI_RX_ENUM		ENUM_DMA2_STREAM_0
#define GYRO_SPI_NUMBER			ENUM_SPI1
#define GYRO_SPI_CS_GPIO_Port   _PORTA
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_4
#define GYRO_RX_DMA_FP          FP_DMA2_S0

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_2
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     _PORTC
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_4
#define GYRO_EXTI_IRQn          EXTI4_IRQn
#define GYRO_EXTI_IRQn_FP       FP_EXTI4

//Sbus inverter config
#undef USE_SBUS_HARDWARE_INVERTER
#define SBUS_HARDWARE_GPIO			0
#define SBUS_HARDWARE_PIN			0
#define SBUS_INVERTER_USART			0

//Serial Config
#define VBUS_SENSING_GPIO			GPIOC
#define VBUS_SENSING_PIN			GPIO_PIN_5

//START SPI defines------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SPI1_ENABLE					1
#define SPI1_NSS_PIN				GPIO_PIN_4
#define SPI1_SCK_PIN				GPIO_PIN_5
#define SPI1_MISO_PIN				GPIO_PIN_6
#define SPI1_MOSI_PIN				GPIO_PIN_7
#define SPI1_NSS_GPIO_PORT			_PORTA
#define SPI1_SCK_GPIO_PORT			_PORTA
#define SPI1_MISO_GPIO_PORT			_PORTA
#define SPI1_MOSI_GPIO_PORT			_PORTA
#define SPI1_SCK_AF					GPIO_AF5_SPI1
#define SPI1_MISO_AF				GPIO_AF5_SPI1
#define SPI1_MOSI_AF				GPIO_AF5_SPI1
#define SPI1_SCK_AF					GPIO_AF5_SPI1
#define SPI1_PRIORITY				1
#define SPI1_TX_DMA_STREAM			ENUM_DMA2_STREAM_3
#define SPI1_TX_DMA_CHANNEL			DMA_CHANNEL_3
#define SPI1_TX_DMA_IRQn			DMA2_Stream3_IRQn
#define SPI1_TX_DMA_PRIORITY		1
#define SPI1_RX_DMA_STREAM			ENUM_DMA2_STREAM_0
#define SPI1_RX_DMA_CHANNEL			DMA_CHANNEL_3
#define SPI1_RX_DMA_IRQn			DMA2_Stream0_IRQn
#define SPI1_RX_DMA_PRIORITY		1
#define SPI1_SCK_PULL				GPIO_PULLDOWN

#define SPI2_ENABLE					0
#define SPI2_NSS_PIN				GPIO_PIN_12
#define SPI2_SCK_PIN				GPIO_PIN_13
#define SPI2_MISO_PIN				GPIO_PIN_14
#define SPI2_MOSI_PIN				GPIO_PIN_15
#define SPI2_NSS_GPIO_PORT			_PORTB
#define SPI2_SCK_GPIO_PORT			_PORTB
#define SPI2_MISO_GPIO_PORT			_PORTB
#define SPI2_MOSI_GPIO_PORT			_PORTB
#define SPI2_SCK_AF					GPIO_AF5_SPI2
#define SPI2_MISO_AF				GPIO_AF5_SPI2
#define SPI2_MOSI_AF				GPIO_AF5_SPI2
#define SPI2_SCK_AF					GPIO_AF5_SPI2
#define SPI2_PRIORITY				8
#define SPI2_TX_DMA_STREAM			ENUM_DMA1_STREAM_4
#define SPI2_TX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI2_TX_DMA_IRQn			DMA1_Stream4_IRQn
#define SPI2_TX_DMA_PRIORITY		8
#define SPI2_RX_DMA_STREAM			ENUM_DMA1_STREAM_3
#define SPI2_RX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI2_RX_DMA_IRQn			DMA1_Stream3_IRQn
#define SPI2_RX_DMA_PRIORITY		8
#define SPI2_SCK_PULL				GPIO_PULLDOWN

#define SPI3_ENABLE					1
#define SPI3_NSS_PIN				GPIO_PIN_3
#define SPI3_SCK_PIN				GPIO_PIN_10
#define SPI3_MISO_PIN				GPIO_PIN_11
#define SPI3_MOSI_PIN				GPIO_PIN_12
#define SPI3_NSS_GPIO_PORT			_PORTB
#define SPI3_SCK_GPIO_PORT			_PORTC
#define SPI3_MISO_GPIO_PORT			_PORTC
#define SPI3_MOSI_GPIO_PORT			_PORTC
#define SPI3_SCK_AF					GPIO_AF6_SPI3
#define SPI3_MISO_AF				GPIO_AF6_SPI3
#define SPI3_MOSI_AF				GPIO_AF6_SPI3
#define SPI3_SCK_AF					GPIO_AF6_SPI3
#define SPI3_PRIORITY				4
#define SPI3_TX_DMA_STREAM			ENUM_DMA1_STREAM_5
#define SPI3_TX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI3_TX_DMA_IRQn			DMA1_Stream5_IRQn
#define SPI3_TX_DMA_PRIORITY		4
#define SPI3_RX_DMA_STREAM			ENUM_DMA1_STREAM_2
#define SPI3_RX_DMA_CHANNEL			DMA_CHANNEL_0
#define SPI3_RX_DMA_IRQn			DMA1_Stream2_IRQn
#define SPI3_RX_DMA_PRIORITY		4
#define SPI3_SCK_PULL				GPIO_PULLUP
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
/*
 	#define USE_USART1
	#define USARTx					   ENUM_USART1
	#define USARTx_TX_PIN              GPIO_PIN_9
	#define USARTx_TX_GPIO_PORT        ENUM_PORTA
	#define USARTx_TX_AF               GPIO_AF7_USART1
	#define USARTx_RX_PIN              GPIO_PIN_9
	#define USARTx_RX_GPIO_PORT        ENUM_PORTA
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
 */
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
#define ACTUATOR1_TYPE					ENUM_ACTUATOR_TYPE_MOTOR
#define ACTUATOR1_TIM					ENUMTIM3
#define ACTUATOR1_PIN					GPIO_PIN_0
#define ACTUATOR1_GPIO					_PORTB
#define ACTUATOR1_ALTERNATE				GPIO_AF2_TIM3
#define ACTUATOR1_TIM_CH				TIM_CHANNEL_3
#define ACTUATOR1_TIM_CCR				TIM3CCR3
#define ACTUATOR1_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR1_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_3
#define ACTUATOR1_DMA_ENABLED			1
#define ACTUATOR1_DMA					ENUM_DMA1_STREAM_7
#define ACTUATOR1_DMA_IRQN				DMA1_Stream7_IRQn
#define ACTUATOR1_DMA_CHANNEL			DMA_CHANNEL_5
#define ACTUATOR1_DMA_HANDLE			TIM_DMA_ID_CC3
#define ACTUATOR1_DMACALLBACK			FP_DMA1_S7
#define ACTUATOR1_IRQN					TIM3_IRQn
#define ACTUATOR1_EXTIN					EXTI0_IRQn
#define ACTUATOR1_EXTICALLBACK			FP_EXTI0

#define ACTUATOR2_TYPE					ENUM_ACTUATOR_TYPE_MOTOR
#define ACTUATOR2_TIM					ENUMTIM3
#define ACTUATOR2_PIN					GPIO_PIN_1
#define ACTUATOR2_GPIO					_PORTB
#define ACTUATOR2_ALTERNATE				GPIO_AF2_TIM3
#define ACTUATOR2_TIM_CH				TIM_CHANNEL_4
#define ACTUATOR2_TIM_CCR				TIM3CCR4
#define ACTUATOR2_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR2_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_4
#define ACTUATOR2_DMA_ENABLED			1
#define ACTUATOR2_DMA					ENUM_DMA1_STREAM_2
#define ACTUATOR2_DMA_IRQN				DMA1_Stream2_IRQn
#define ACTUATOR2_DMA_CHANNEL			DMA_CHANNEL_5
#define ACTUATOR2_DMA_HANDLE			TIM_DMA_ID_CC4
#define ACTUATOR2_DMACALLBACK			FP_DMA1_S2
#define ACTUATOR2_IRQN					TIM3_IRQn
#define ACTUATOR2_EXTIN					EXTI1_IRQn
#define ACTUATOR2_EXTICALLBACK			FP_EXTI1

#define ACTUATOR3_TYPE					ENUM_ACTUATOR_TYPE_MOTOR
#define ACTUATOR3_TIM					ENUMTIM5
#define ACTUATOR3_PIN					GPIO_PIN_3
#define ACTUATOR3_GPIO					_PORTA
#define ACTUATOR3_ALTERNATE				GPIO_AF2_TIM5
#define ACTUATOR3_TIM_CH				TIM_CHANNEL_4
#define ACTUATOR3_TIM_CCR				TIM5CCR4
#define ACTUATOR3_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR3_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_4
#define ACTUATOR3_DMA_ENABLED			1
#define ACTUATOR3_DMA					ENUM_DMA1_STREAM_1
#define ACTUATOR3_DMA_IRQN				DMA1_Stream1_IRQn
#define ACTUATOR3_DMA_CHANNEL			DMA_CHANNEL_6
#define ACTUATOR3_DMA_HANDLE			TIM_DMA_ID_CC4
#define ACTUATOR3_DMACALLBACK			FP_DMA1_S1
#define ACTUATOR3_IRQN					TIM5_IRQn
#define ACTUATOR3_EXTIN					EXTI3_IRQn
#define ACTUATOR3_EXTICALLBACK			FP_EXTI3

#define ACTUATOR4_TYPE					ENUM_ACTUATOR_TYPE_MOTOR
#define ACTUATOR4_TIM					ENUMTIM5
#define ACTUATOR4_PIN					GPIO_PIN_2
#define ACTUATOR4_GPIO					_PORTA
#define ACTUATOR4_ALTERNATE				GPIO_AF2_TIM5
#define ACTUATOR4_TIM_CH				TIM_CHANNEL_3
#define ACTUATOR4_TIM_CCR				TIM5CCR3
#define ACTUATOR4_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR4_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_3
#define ACTUATOR4_DMA_ENABLED			0
#define ACTUATOR4_DMA					ENUM_DMA1_STREAM_0
#define ACTUATOR4_DMA_IRQN				DMA1_Stream0_IRQn
#define ACTUATOR4_DMA_CHANNEL			DMA_CHANNEL_6
#define ACTUATOR4_DMA_HANDLE			TIM_DMA_ID_CC3
#define ACTUATOR4_DMACALLBACK			FP_DMA1_S0
#define ACTUATOR4_IRQN					TIM5_IRQn
#define ACTUATOR4_EXTIN					EXTI2_IRQn
#define ACTUATOR4_EXTICALLBACK			FP_EXTI2

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

#define ACTUATOR6_TYPE					0
#define ACTUATOR6_TIM					0
#define ACTUATOR6_PIN					0
#define ACTUATOR6_GPIO					0
#define ACTUATOR6_ALTERNATE				0
#define ACTUATOR6_TIM_CH				0
#define ACTUATOR6_TIM_CCR				0
#define ACTUATOR6_POLARITY				0
#define ACTUATOR6_ACTIVE_TIM			0
#define ACTUATOR6_DMA_ENABLED			0
#define ACTUATOR6_DMA					0
#define ACTUATOR6_DMA_IRQN				0
#define ACTUATOR6_DMA_CHANNEL			0
#define ACTUATOR6_DMA_HANDLE			0
#define ACTUATOR6_DMACALLBACK			0
#define ACTUATOR6_IRQN					0
#define ACTUATOR6_EXTIN					0
#define ACTUATOR6_EXTICALLBACK			0

//TIM4 is LEDs only.
#define ACTUATOR7_TYPE					ENUM_ACTUATOR_TYPE_WS2812
#define ACTUATOR7_TIM					ENUMTIM4
#define ACTUATOR7_PIN					GPIO_PIN_6
#define ACTUATOR7_GPIO					_PORTB
#define ACTUATOR7_ALTERNATE				GPIO_AF2_TIM4
#define ACTUATOR7_TIM_CH				TIM_CHANNEL_1
#define ACTUATOR7_TIM_CCR				TIM4CCR1
#define ACTUATOR7_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR7_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_1
#define ACTUATOR7_DMA_ENABLED			1
#define ACTUATOR7_DMA					ENUM_DMA1_STREAM_0
#define ACTUATOR7_DMA_IRQN				DMA1_Stream0_IRQn
#define ACTUATOR7_DMA_CHANNEL			DMA_CHANNEL_2
#define ACTUATOR7_DMA_HANDLE			TIM_DMA_ID_CC1
#define ACTUATOR7_DMACALLBACK			0
#define ACTUATOR7_IRQN					0
#define ACTUATOR7_EXTIN					0
#define ACTUATOR7_EXTICALLBACK			0

#define ACTUATOR8_TYPE					ENUM_ACTUATOR_TYPE_SPORT
#define ACTUATOR8_TIM					ENUMTIM2
#define ACTUATOR8_PIN					GPIO_PIN_11
#define ACTUATOR8_GPIO					_PORTB
#define ACTUATOR8_ALTERNATE				GPIO_AF1_TIM2
#define ACTUATOR8_TIM_CH				TIM_CHANNEL_4
#define ACTUATOR8_TIM_CCR				TIM2CCR4
#define ACTUATOR8_POLARITY				TIM_OCPOLARITY_LOW
#define ACTUATOR8_ACTIVE_TIM			HAL_TIM_ACTIVE_CHANNEL_4
#define ACTUATOR8_DMA_ENABLED			1
#define ACTUATOR8_DMA					ENUM_DMA1_STREAM_6
#define ACTUATOR8_DMA_IRQN				DMA1_Stream6_IRQn
#define ACTUATOR8_DMA_CHANNEL			DMA_CHANNEL_3
#define ACTUATOR8_DMA_HANDLE			TIM_DMA_ID_CC4
#define ACTUATOR8_DMACALLBACK			FP_DMA1_S6
#define ACTUATOR8_IRQN					TIM2_IRQn
#define ACTUATOR8_EXTIN					EXTI15_10_IRQn
#define ACTUATOR8_EXTICALLBACK			FP_EXTI15_10

//Flash Config
#define FLASH_SPI_NUMBER			ENUM_SPI3
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
#define FLASH_RX_DMA_FP				FP_DMA1_S2

#define FLASH_SPI_FAST_BAUD			SPI_BAUDRATEPRESCALER_4
#define FLASH_SPI_SLOW_BAUD			SPI_BAUDRATEPRESCALER_4

#define FLASH_DMA_TX_IRQn			SPI3_TX_DMA_IRQn
#define FLASH_DMA_TX_IRQHandler		SPI3_TX_DMA_IRQHandler
#define FLASH_DMA_RX_IRQn			SPI3_RX_DMA_IRQn
#define FLASH_DMA_RX_IRQHandler		SPI3_RX_DMA_IRQHandler





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
