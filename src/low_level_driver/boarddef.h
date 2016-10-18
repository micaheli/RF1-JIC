#pragma once

#include "mcu_include.h"


#define uid0_1 (*(uint8_t*)0x1fff7a10)
#define uid0_2 (*(uint8_t*)0x1fff7a11)
#define uid0_3 (*(uint8_t*)0x1fff7a12)
#define uid0_4 (*(uint8_t*)0x1fff7a13)
#define uid1_1 (*(uint8_t*)0x1fff7a14)
#define uid1_2 (*(uint8_t*)0x1fff7a15)
#define uid1_3 (*(uint8_t*)0x1fff7a16)
#define uid1_4 (*(uint8_t*)0x1fff7a17)
#define uid2_1 (*(uint8_t*)0x1fff7a18)
#define uid2_2 (*(uint8_t*)0x1fff7a19)
#define uid2_3 (*(uint8_t*)0x1fff7a1a)
#define uid2_4 (*(uint8_t*)0x1fff7a1b)


#define	REVO

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
/*
#define LEDn                    2

#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_4

#define LED2_GPIO_Port          GPIOB
#define LED2_GPIO_Pin           GPIO_PIN_5


//buzzer setup
#define BUZZER_GPIO_Port        GPIOA
#define BUZZER_GPIO_Pin         GPIO_PIN_0
*/


//Flash Config
#define FLASH_SPI					SPI3
#define FLASH_SPI_CS_GPIO_Pin		GPIO_PIN_3
#define FLASH_SPI_CS_GPIO_Port		GPIOB
#define FLASH_SPI_SCK_PIN			GPIO_PIN_10
#define FLASH_SPI_SCK_GPIO_PORT		GPIOC
#define FLASH_SPI_SCK_AF			GPIO_AF6_SPI3
#define FLASH_SPI_MISO_PIN			GPIO_PIN_11
#define FLASH_SPI_MISO_GPIO_PORT	GPIOC
#define FLASH_SPI_MISO_AF			GPIO_AF6_SPI3
#define FLASH_SPI_MOSI_PIN			GPIO_PIN_12
#define FLASH_SPI_MOSI_GPIO_PORT	GPIOC
#define FLASH_SPI_MOSI_AF			GPIO_AF6_SPI3

#define FLASH_SPI_BAUD				SPI_BAUDRATEPRESCALER_2

#define FLASH_DMA_TX_IRQn			DMA1_Stream2_IRQn
#define FLASH_DMA_TX_IRQHandler		DMA1_Stream2_IRQHandler
#define FLASH_DMA_RX_IRQn			DMA1_Stream5_IRQn
#define FLASH_DMA_RX_IRQHandler		DMA1_Stream5_IRQHandler


//Gyro Config
#define GYRO_SPI                SPI2
#define GYRO_SPI_CS_GPIO_Port   GPIOB
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_12

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_2
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_DMA_TX_IRQn        DMA1_Stream4_IRQn
#define GYRO_DMA_TX_IRQHandler  DMA1_Stream4_IRQHandler
#define GYRO_DMA_RX_IRQn        DMA1_Stream3_IRQn
#define GYRO_DMA_RX_IRQHandler  DMA1_Stream3_IRQHandler

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     GPIOC
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_13
#define GYRO_EXTI_IRQn          EXTI15_10_IRQn
#define GYRO_EXTI_IRQHandler    EXTI15_10_IRQHandler


//Sbus inverter config
//#define USE_SBUS_HARDWARE_INVERTER
#define SBUS_HARDWARE_GPIO			GPIOC
#define SBUS_HARDWARE_PIN			GPIO_PIN_0
#define SBUS_INVERTER_USART			USART1


//Serial Config
#define USE_VCP
#define VBUS_SENSING_GPIO			GPIOA
#define VBUS_SENSING_PIN			GPIO_PIN_9

/*
#define USE_USART1
#define USART1_RX_GPIO				GPIOA
#define USART1_RX_PIN				GPIO_PIN_10
#define USART1_TX_GPIO				GPIOA
#define USART1_TX_PIN				GPIO_PIN_9
*/
#define USE_USART2
#define USART2_RX_GPIO				GPIOA
#define USART2_RX_PIN				GPIO_PIN_3
#define USART2_TX_GPIO				GPIOA
#define USART2_TX_PIN				GPIO_PIN_2

#define USE_USART3
#define USART3_RX_GPIO				GPIOC
#define USART3_RX_PIN				GPIO_PIN_11
#define USART3_TX_GPIO				GPIOC
#define USART3_TX_PIN				GPIO_PIN_10

#define USE_USART4
#define USART4_RX_GPIO				GPIOA
#define USART4_RX_PIN				GPIO_PIN_1
#define USART4_TX_GPIO				GPIOA
#define USART4_TX_PIN				GPIO_PIN_0

#define USE_USART5
#define USART5_RX_GPIO				GPIOC
#define USART5_RX_PIN				GPIO_PIN_12
#define USART5_TX_GPIO				GPIOC
#define USART5_TX_PIN				GPIO_PIN_12





/*
#define USE_USART6
#define USART6_RX_GPIO				GPIOC
#define USART6_RX_PIN				GPIO_PIN_7
#define USART6_TX_GPIO				GPIOC
#define USART6_TX_PIN				GPIO_PIN_6
*/

#define _PORTA 0
#define _PORTB 1
#define _PORTC 2
#define _PORTD 3
#define _PORTE 4
#define _PORTF 5
#define _PORTG 6
#define _PORTH 7
#define _PORTI 8

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
	uint32_t async;
} serial_type;


typedef struct {
	SPI_TypeDef *port;
} spi_type;

typedef struct {
	uint32_t	sckPort;
	uint32_t	sckPin;
	uint32_t	misoPort;
	uint32_t	misoPin;
	uint32_t	mosiPort;
	uint32_t	mosiPin;
	uint32_t	enabled;
} spi_pin_defs;

typedef struct {
	uint32_t	PinMode;
	uint32_t	Pull;
	uint32_t	Speed;
	uint32_t	TXAlternate;
	uint32_t	TXPin;
	uint32_t	RXAlternate;
	uint32_t	RXPin;

	GPIO_TypeDef *	RXPort;
	GPIO_TypeDef *	TXPort;

	USART_TypeDef *	SerialInstance; // loaded from port array

	uint32_t	BaudRate;
	uint32_t	WordLength;
	uint32_t	StopBits;
	uint32_t	Parity;
	uint32_t	HwFlowCtl;
	uint32_t	Mode;

	uint32_t	USART_IRQn;

	DMA_Stream_TypeDef *	TXDMAStream; // looked up from array
	uint32_t	TXDMAChannel;
	uint32_t	TXDMADirection;
	uint32_t	TXDMAPeriphInc;
	uint32_t	TXDMAMemInc;
	uint32_t	TXDMAPeriphDataAlignment;
	uint32_t	TXDMAMemDataAlignment;
	uint32_t	TXDMAMode;
	uint32_t	TXDMAPriority;
	uint32_t	TXDMAFIFOMode;

	DMA_Stream_TypeDef *	RXDMAStream; // looked up from array
	uint32_t	RXDMAChannel;
	uint32_t	RXDMADirection;
	uint32_t	RXDMAPeriphInc;
	uint32_t	RXDMAMemInc;
	uint32_t	RXDMAPeriphDataAlignment;
	uint32_t	RXDMAMemDataAlignment;
	uint32_t	RXDMAMode;
	uint32_t	RXDMAPriority;
	uint32_t	RXDMAFIFOMode;

	uint32_t	TXDMA_IRQn;
	uint32_t	RXDMA_IRQn;

} board_serial;


typedef struct {
	uint32_t	port;
	uint32_t	pin;
	uint32_t	inverted;
	uint32_t	enabled;
} internal_led_type;

typedef struct {
	uint32_t	nssPort;	//for spi only
	uint32_t	nssPin;		//for spi only
	uint32_t	intPort;	//for spi only
	uint32_t	intPin;		//for spi only
	uint32_t	enabled;
} gyro_type;

typedef struct {
	TIM_TypeDef *		timer;
	uint32_t			pin;
	GPIO_TypeDef *		port;
	uint32_t			AF;	
	uint32_t			timChannel;
	uint32_t			timCCR;
} motor_type;



typedef struct {
	uint32_t	fc_pllm;
	uint32_t	fc_plln;
	uint32_t	fc_pllp;
	uint32_t	fc_pllq;
	
	internal_led_type internalLeds[3];

	uint32_t	buzzerPort;
	uint32_t	buzzerPin;

	spi_pin_defs	spi_pins[3];

	gyro_type	gyros[3];
	
	motor_type motors[4];

	board_serial serials[6];

	
} board_type;



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
#define USARTx_DMA_TX_IRQn         DMA1_Stream7_IRQn
#define USARTx_DMA_RX_IRQn         DMA1_Stream0_IRQn
#define USARTx_DMA_TX_IRQHandler   DMA1_Stream7_IRQHandler
#define USARTx_DMA_RX_IRQHandler   DMA1_Stream0_IRQHandler


//UART 5 defines
#define USART5                     UART5
#define USART5_TX_PIN              GPIO_PIN_12
#define USART5_TX_GPIO_PORT        GPIOC
#define USART5_TX_AF               GPIO_AF8_UART5
#define USART5_RX_PIN              GPIO_PIN_12
#define USART5_RX_GPIO_PORT        GPIOC
#define USART5_RX_AF               GPIO_AF8_UART5
#define USART5_IRQHandler          UART5_IRQHandler
#define USART5_IRQn                UART5_IRQn
#define USART5_TX_DMA_STREAM       DMA1_Stream7
#define USART5_RX_DMA_STREAM       DMA1_Stream0
#define USART5_TX_DMA_CHANNEL      DMA_CHANNEL_4
#define USART5_RX_DMA_CHANNEL      DMA_CHANNEL_4
#define USART5_DMA_TX_IRQn         DMA1_Stream7_IRQn
#define USART5_DMA_RX_IRQn         DMA1_Stream0_IRQn
#define USART5_DMA_TX_IRQHandler   DMA1_Stream7_IRQHandler
#define USART5_DMA_RX_IRQHandler   DMA1_Stream0_IRQHandler


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

/* Definition for USARTx's DMA */


//#define USARTx_DMA_TX_IRQHandler   DMA1_Stream3_IRQHandler
//#define USARTx_DMA_RX_IRQHandler   DMA1_Stream1_IRQHandler
//#define USARTx_IRQHandler          USART3_IRQHandler

typedef enum {INVERTED_RX, RX, GPS, OSD} usart_usage ;





//uint16_t pins;

/*
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

*/

extern board_type board;
extern GPIO_TypeDef *ports[];

extern int InitializeMCUSettings();
void getBoardHardwareDefs();
