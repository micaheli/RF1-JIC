#pragma once

#include "mcu_include.h"

#define	REVO

//USB config
#define RFFW_HID_PRODUCT_STRING "RaceFlightFC"
#define RFBL_HID_PRODUCT_STRING "RaceFlight Boot Loader"


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




/*
UART_HandleTypeDef uartHandle1;
UART_HandleTypeDef uartHandle2;
UART_HandleTypeDef uartHandle3;
UART_HandleTypeDef uartHandle4;
UART_HandleTypeDef uartHandle5;
UART_HandleTypeDef uartHandle6;
*/

/*
enum {
	SERIAL_NOT_USED=0,
	SERIAL_SBUS_NORMAL,
	SERIAL_SBUS_INVERTED,
	SERIAL_SPORT_NORMAL,
	SERIAL_SPORT_INVERTER,
	SERIAL_SPEKTRUM,
	SERIAL_ SPKTRUM_TELEMETRY,
};


*/


typedef struct {
	USART_TypeDef *port;
	uint8_t async;
} serial_type;


typedef struct {
	SPI_TypeDef *port;
} spi_type;

typedef struct {
	uint32_t PinMode;
	uint32_t Pull;
	uint32_t Speed;
	uint32_t TXAlternate;
	uint32_t TXPin;
	uint32_t RXAlternate;
	uint32_t RXPin;

	uint8_t RXPort;
	uint8_t TXPort;

	uint8_t SerialInstance; // loaded from port array

	uint32_t BaudRate;
	uint32_t WordLength;
	uint32_t StopBits;
	uint32_t Parity;
	uint32_t HwFlowCtl;
	uint32_t Mode;

	uint8_t  TXDMAStream; // looked up from array
	uint32_t TXDMAChannel;
	uint32_t TXDMADirection;
	uint32_t TXDMAPeriphInc;
	uint32_t TXDMAMemInc;
	uint32_t TXDMAPeriphDataAlignment;
	uint32_t TXDMAMemDataAlignment;
	uint32_t TXDMAMode;
	uint32_t TXDMAPriority;

	uint8_t  RXDMAStream; // looked up from array
	uint32_t RXDMAChannel;
	uint32_t RXDMADirection;
	uint32_t RXDMAPeriphInc;
	uint32_t RXDMAMemInc;
	uint32_t RXDMAPeriphDataAlignment;
	uint32_t RXDMAMemDataAlignment;
	uint32_t RXDMAMode;
	uint32_t RXDMAPriority;

} board_serial;


typedef struct {
	uint8_t port;
	uint32_t pin;
	uint8_t enabled;
} internal_led_type;

typedef struct {
	uint8_t port;
	uint32_t pin;
	uint8_t enabled;
} gyro_type;



typedef struct {
	uint32_t fc_pllm;
	uint32_t fc_plln;
	uint32_t fc_pllp;
	uint32_t fc_pllq;

	internal_led_type internalLeds[3];

	uint8_t buzzerPort;
	uint32_t buzzerPin;

	gyro_type gyros[3];

	board_serial serials[6];
} board_type;



//USART Config
#define USARTx                     USART3
#define USARTx_TX_PIN              GPIO_PIN_10
#define USARTx_TX_GPIO_PORT        GPIOB
#define USARTx_TX_AF               GPIO_AF7_USART3
#define USARTx_RX_PIN              GPIO_PIN_11
#define USARTx_RX_GPIO_PORT        GPIOB
#define USARTx_RX_AF               GPIO_AF7_USART3
#define USARTx_IRQHandler          USART3_IRQHandler
#define USARTx_IRQn                USART3_IRQn
#define USARTx_TX_DMA_STREAM       DMA1_Stream3
#define USARTx_RX_DMA_STREAM       DMA1_Stream1
#define USARTx_TX_DMA_CHANNEL      DMA_CHANNEL_4
#define USARTx_RX_DMA_CHANNEL      DMA_CHANNEL_4
#define USARTx_DMA_TX_IRQn         DMA1_Stream3_IRQn
#define USARTx_DMA_RX_IRQn         DMA1_Stream1_IRQn
#define USARTx_DMA_TX_IRQHandler   DMA1_Stream3_IRQHandler
#define USARTx_DMA_RX_IRQHandler   DMA1_Stream1_IRQHandler

/* Definition for USARTx's DMA */


//#define USARTx_DMA_TX_IRQHandler   DMA1_Stream3_IRQHandler
//#define USARTx_DMA_RX_IRQHandler   DMA1_Stream1_IRQHandler
//#define USARTx_IRQHandler          USART3_IRQHandler

typedef enum {INVERTED_RX, RX, GPS, OSD} usart_usage ;





//uint16_t pins;

typedef struct
{
	unsigned char usart;

	unsigned char rxAF;
	unsigned char rxIRQn;
	unsigned char rxGPIO;
	uint16_t rxPIN;

	unsigned char txAF;
	unsigned char txIRQn;
	unsigned char txGPIO;
	uint16_t txPIN;

	usart_usage usart_type; //verify this turns into a byte, and if not switch it to them


} usart_pinout;

typedef struct
{
	usart_pinout usart_ports[8];


} target_pinout;
