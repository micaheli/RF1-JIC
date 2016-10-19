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

#ifdef stm32f405xx
	#include "REVOLT"
#elif defined(stm32f446xx)
	#include "SPMFC400.h"
#endif

typedef struct {
	USART_TypeDef			*port;
	uint32_t				async;
} serial_type;


typedef struct {
	SPI_TypeDef				*port;
} spi_type;

typedef struct {
	uint32_t				enabled;

	uint32_t				NSSPin;
	uint32_t				SCKPin;
	uint32_t				MISOPin;
	uint32_t				MOSIPin;

	GPIO_TypeDef *			NSSPort;
	GPIO_TypeDef *			SCKPort;
	GPIO_TypeDef *			MISOPort;
	GPIO_TypeDef *			MOSIPort;
	
	//uint32_t				NSSAlternate;
	uint32_t				SCKAlternate;
	uint32_t				MISOAlternate;
	uint32_t				MOSIAlternate;
	
	//uint32_t				GPIOSpeed;
	
	uint32_t				SPI_IRQn;

	DMA_Stream_TypeDef *	TXDMAStream; // looked up from array
	uint32_t				TXDMAChannel;
	uint32_t				TXDMADirection;
	uint32_t				TXDMAPeriphInc;
	uint32_t				TXDMAMemInc;
	uint32_t				TXDMAPeriphDataAlignment;
	uint32_t				TXDMAMemDataAlignment;
	uint32_t				TXDMAMode;
	uint32_t				TXDMAPriority;
	uint32_t				TXDMAFIFOMode;

	DMA_Stream_TypeDef *	RXDMAStream; // looked up from array
	uint32_t				RXDMAChannel;
	uint32_t				RXDMADirection;
	uint32_t				RXDMAPeriphInc;
	uint32_t				RXDMAMemInc;
	uint32_t				RXDMAPeriphDataAlignment;
	uint32_t				RXDMAMemDataAlignment;
	uint32_t				RXDMAMode;
	uint32_t				RXDMAPriority;
	uint32_t				RXDMAFIFOMode;

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

	uint32_t				TXDMA_IRQn;
	uint32_t				RXDMA_IRQn;

	//uint32_t				TXDMA_IRQ_Handler;
	//uint32_t				RXDMA_IRQ_Handler;
} board_spi;

typedef struct {
	uint32_t				PinMode;
	uint32_t				Pull;
	uint32_t				Speed;
	uint32_t				TXAlternate;
	uint32_t				TXPin;
	uint32_t				RXAlternate;
	uint32_t				RXPin;

	GPIO_TypeDef *			RXPort;
	GPIO_TypeDef *			TXPort;

	USART_TypeDef *			SerialInstance; // loaded from port array

	uint32_t				BaudRate;
	uint32_t				WordLength;
	uint32_t				StopBits;
	uint32_t				Parity;
	uint32_t				HwFlowCtl;
	uint32_t				Mode;

	uint32_t				USART_IRQn;

	DMA_Stream_TypeDef *	TXDMAStream; // looked up from array
	uint32_t				TXDMAChannel;
	uint32_t				TXDMADirection;
	uint32_t				TXDMAPeriphInc;
	uint32_t				TXDMAMemInc;
	uint32_t				TXDMAPeriphDataAlignment;
	uint32_t				TXDMAMemDataAlignment;
	uint32_t				TXDMAMode;
	uint32_t				TXDMAPriority;
	uint32_t				TXDMAFIFOMode;

	DMA_Stream_TypeDef *	RXDMAStream; // looked up from array
	uint32_t				RXDMAChannel;
	uint32_t				RXDMADirection;
	uint32_t				RXDMAPeriphInc;
	uint32_t				RXDMAMemInc;
	uint32_t				RXDMAPeriphDataAlignment;
	uint32_t				RXDMAMemDataAlignment;
	uint32_t				RXDMAMode;
	uint32_t				RXDMAPriority;
	uint32_t				RXDMAFIFOMode;

	uint32_t				TXDMA_IRQn;
	uint32_t				RXDMA_IRQn;

} board_serial;


typedef struct {
	uint32_t				port;
	uint32_t				pin;
	uint32_t				inverted;
	uint32_t				enabled;
} internal_led_type;

typedef struct {
	uint32_t				port;
	uint32_t				pin;
	uint32_t				enabled;
} gyro_type;

typedef struct {
	SPI_TypeDef *			SPIInstance;	//use this to determine spi irqn and irq handlers to use. No need to re define them here
	uint32_t				SPINumber;		
	uint32_t				csPin;
	GPIO_TypeDef *			csPort;
	uint32_t				extiPin;
	GPIO_TypeDef *			extiPort;
	uint32_t				extiIRQn;
	uint32_t				spiFastBaud;
	uint32_t				spiSlowBaud;
	//i2c stuff here?
} board_gyro;

typedef struct {
	TIM_TypeDef *			timer;
	uint32_t				pin;
	GPIO_TypeDef *			port;
	uint32_t				AF;	
	uint32_t				timChannel;
	uint32_t				timCCR;
} motor_type;



typedef struct {
	uint32_t				fc_pllm;
	uint32_t				fc_plln;
	uint32_t				fc_pllp;
	uint32_t				fc_pllq;
	
	internal_led_type		internalLeds[3];

	uint32_t				buzzerPort;
	uint32_t				buzzerPin;

	

	gyro_type				gyros[3];
	
	motor_type				motors[4];

	board_gyro				gyro_pins;

	board_spi				spis[3];

	board_serial			serials[6];

	
} board_type;






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
