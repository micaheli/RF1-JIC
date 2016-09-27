#pragma once

#define	REVO


//MCU config
#define FC_PLLM	8
#define FC_PLLN	384
#define FC_PLLP	2
#define FC_PLLQ	8


//LED config
#define LEDn                    2

#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_4

#define LED2_GPIO_Port          GPIOB
#define LED2_GPIO_Pin           GPIO_PIN_5


//buzzer setup
#define BUZZER_GPIO_Port        GPIOA
#define BUZZER_GPIO_Pin         GPIO_PIN_0


//Gyro Config
#define GYRO_SPI                SPI1
#define GYRO_SPI_CS_GPIO_Port   GPIOA
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_4

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_4
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_DMA_TX_IRQn        DMA2_Stream3_IRQn
#define GYRO_DMA_TX_IRQHandler  DMA2_Stream3_IRQHandler
#define GYRO_DMA_RX_IRQn        DMA2_Stream0_IRQn
#define GYRO_DMA_RX_IRQHandler  DMA2_Stream0_IRQHandler

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     GPIOC
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_4
#define GYRO_EXTI_IRQn          EXTI4_IRQn
#define GYRO_EXTI_IRQHandler    EXTI4_IRQHandler


//USART Config
#define USARTx                     USART3
#define USARTx_TX_PIN              GPIO_PIN_10
#define USARTx_TX_GPIO_PORT        GPIOB
#define USARTx_TX_AF               GPIO_AF7_USART3
#define USARTx_RX_PIN              GPIO_PIN_11
#define USARTx_RX_GPIO_PORT        GPIOB
#define USARTx_RX_AF               GPIO_AF7_USART3
#define USART_DMA_TX_IRQn          DMA1_Stream3_IRQn
#define USART_DMA_RX_IRQn          DMA1_Stream1_IRQn
#define USARTx_DMA_TX_IRQHandler   DMA1_Stream3_IRQHandler
#define USARTx_DMA_RX_IRQHandler   DMA1_Stream1_IRQHandler
#define USARTx_IRQn                USART3_IRQn
#define USARTx_IRQHandler          USART3_IRQHandler


//USB config
#define RFFW_HID_PRODUCT_STRING "Revolution"
#define RFBL_HID_PRODUCT_STRING "Revolution RFBL"


//STM32F4 UID address
#define DEVICE_ID1					0x1FFF7A10
#define DEVICE_ID2					0x1FFF7A14
#define DEVICE_ID3					0x1FFF7A18


//Sbus inverter config
#define USE_SBUS_HARDWARE_INVERTER
#define SBUS_HARDWARE_GPIO			GPIOC
#define SBUS_HARDWARE_PIN			GPIO_PIN_0
#define SBUS_INVERTER_USART			USART1


//Serial Config
#define USE_VCP
#define VBUS_SENSING_GPIO			GPIOC
#define VBUS_SENSING_PIN			GPIO_PIN_5

#define USE_USART1
#define USART1_RX_GPIO				GPIOA
#define USART1_RX_PIN				GPIO_PIN_10
#define USART1_TX_GPIO				GPIOA
#define USART1_TX_PIN				GPIO_PIN_9

#define USE_USART3
#define USART3_RX_GPIO				GPIOB
#define USART3_RX_PIN				GPIO_PIN_11
#define USART3_TX_GPIO				GPIOB
#define USART3_TX_PIN				GPIO_PIN_10

#define USE_USART6
#define USART6_RX_GPIO				GPIOC
#define USART6_RX_PIN				GPIO_PIN_7
#define USART6_TX_GPIO				GPIOC
#define USART6_TX_PIN				GPIO_PIN_6


//RFBL config
#define HARDWARE_RFBL_PLUG
#define RFBL_GPIO1   GPIOA //Servo5
#define RFBL_PIN1    GPIO_PIN_1
#define RFBL_GPIO2   GPIOA //Servo6
#define RFBL_PIN2    GPIO_PIN_0
#define SPEK_GPIO    GPIOB
#define SPEK_PIN     GPIO_PIN_11

#define USE_RFBL
#define ADDRESS_RFBL_START		(0x08000000)
#define ADDRESS_CONFIG_START	(0x0800C000)
#define ADDRESS_FLASH_START		(0x08020000)
#define ADDRESS_FLASH_END		(0x080FFFF0)


/*
UART_HandleTypeDef uartHandle1;
UART_HandleTypeDef uartHandle2;
UART_HandleTypeDef uartHandle3;
UART_HandleTypeDef uartHandle4;
UART_HandleTypeDef uartHandle5;
UART_HandleTypeDef uartHandle6;
*/

enum {
	SERIAL_NOT_USED=0,
	SERIAL_SBUS_NORMAL,
	SERIAL_SBUS_INVERTED,
	SERIAL_SPORT_NORMAL,
	SERIAL_SPORT_INVERTER,
	SERIAL_SPEKTRUM,
	SERIAL_SPKTRUM_TELEMETRY,
};

typedef struct {
	unsigned char used;
	UART_HandleTypeDef handle;
	USART_TypeDef *usart;
	GPIO_TypeDef *txGpio;
	uint16_t txPin;
	uint8_t txGpioAf;
	uint8_t txIrqn;
	GPIO_TypeDef *rxGpio;
	uint16_t rxPin;
	uint8_t rxGpioAf;
	uint8_t rxIrqn;
} board_serial;


typedef struct {
	board_serial serials[6];
} board_record;

/*
//USART Config
#define USARTx                     USART3
#define USARTx_TX_PIN              GPIO_PIN_10
#define USARTx_TX_GPIO_PORT        GPIOB
#define USARTx_TX_AF               GPIO_AF7_USART3
#define USARTx_RX_PIN              GPIO_PIN_11
#define USARTx_RX_GPIO_PORT        GPIOB
#define USARTx_RX_AF               GPIO_AF7_USART3
#define USART_DMA_TX_IRQn          DMA1_Stream3_IRQn
#define USART_DMA_RX_IRQn          DMA1_Stream1_IRQn
#define USARTx_IRQn                USART3_IRQn
*/

#define USARTx_DMA_TX_IRQHandler   DMA1_Stream3_IRQHandler
#define USARTx_DMA_RX_IRQHandler   DMA1_Stream1_IRQHandler
#define USARTx_IRQHandler          USART3_IRQHandler


