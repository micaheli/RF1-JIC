#pragma once


#include "mcu_include.h"


#define MAX_MOTOR_NUMBER 8
#define MAX_SERVO_NUMBER 8
#define MAX_USARTS       6
#define RXBUFFERSIZE     64
#define TXBUFFERSIZE     64


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

#define ENUM_SPI1 0
#define ENUM_SPI2 1
#define ENUM_SPI3 2
#define ENUM_SPI4 3
#define ENUM_SPI5 4
#define ENUM_SPI6 5

#define FP_EXTI0     0
#define FP_EXTI1     1
#define FP_EXTI2     2
#define FP_EXTI3     3
#define FP_EXTI4     4
#define FP_EXTI9_5   5
#define FP_EXTI15_10 6
#define FP_DMA1_S0   7
#define FP_DMA1_S1   8
#define FP_DMA1_S2   9
#define FP_DMA1_S3   10
#define FP_DMA1_S4   11
#define FP_DMA1_S5   12
#define FP_DMA1_S6   13
#define FP_DMA1_S7   14
#define FP_DMA2_S0   15
#define FP_DMA2_S1   16
#define FP_DMA2_S2   17
#define FP_DMA2_S3   18
#define FP_DMA2_S4   19
#define FP_DMA2_S5   20
#define FP_DMA2_S6   21
#define FP_DMA2_S7   22
#define IRQH_FP_TOT  23


#define _PORTA 0
#define _PORTB 1
#define _PORTC 2
#define _PORTD 3
#define _PORTE 4
#define _PORTF 5
#define _PORTG 6
#define _PORTH 7
#define _PORTI 8


#define ENUM_PORTA _PORTA
#define ENUM_PORTB _PORTB
#define ENUM_PORTC _PORTC
#define ENUM_PORTD _PORTD
#define ENUM_PORTE _PORTE
#define ENUM_PORTF _PORTF
#define ENUM_PORTG _PORTG
#define ENUM_PORTH _PORTH
#define ENUM_PORTI _PORTI


#define ENUM_USART1_IRQn 0
#define ENUM_USART2_IRQn 1
#define ENUM_USART3_IRQn 2
#define ENUM_USART4_IRQn 3
#define ENUM_USART5_IRQn 4
#define ENUM_USART6_IRQn 5


#define ENUMTIM1	0
#define ENUMTIM2	1
#define ENUMTIM3	2
#define ENUMTIM4	3
#define ENUMTIM5	4
#define ENUMTIM6	5
#define ENUMTIM7	6
#define ENUMTIM8	7
#define ENUMTIM9	8
#define ENUMTIM10	9
#define ENUMTIM11	10
#define ENUMTIM12	11
#define ENUMTIM13	12
#define ENUMTIM14	13


#define	TIM1CCR1 0
#define	TIM1CCR2 1
#define	TIM1CCR3 2
#define	TIM1CCR4 3
#define	TIM2CCR1 4
#define	TIM2CCR2 5
#define	TIM2CCR3 6
#define	TIM2CCR4 7
#define	TIM3CCR1 8
#define	TIM3CCR2 9
#define	TIM3CCR3 10
#define	TIM3CCR4 11
#define	TIM4CCR1 12
#define	TIM4CCR2 13
#define	TIM4CCR3 14
#define	TIM4CCR4 15
#define	TIM5CCR1 16
#define	TIM5CCR2 17
#define	TIM5CCR3 18
#define	TIM5CCR4 19
#define	TIM6CCR1 20
#define	TIM6CCR2 21
#define	TIM6CCR3 22
#define	TIM6CCR4 23
#define	TIM7CCR1 24
#define	TIM7CCR2 25
#define	TIM7CCR3 26
#define	TIM7CCR4 27
#define	TIM8CCR1 28
#define	TIM8CCR2 29
#define	TIM8CCR3 30
#define	TIM8CCR4 31
#define	TIM9CCR1 32
#define	TIM9CCR2 33
#define	TIM9CCR3 34
#define	TIM9CCR4 35
#define	TIM10CCR1 36
#define	TIM10CCR2 37
#define	TIM10CCR3 38
#define	TIM10CCR4 39
#define	TIM11CCR1 40
#define	TIM11CCR2 41
#define	TIM11CCR3 42
#define	TIM11CCR4 43
#define	TIM12CCR1 44
#define	TIM12CCR2 45
#define	TIM12CCR3 46
#define	TIM12CCR4 47
#define	TIM13CCR1 48
#define	TIM13CCR2 49
#define	TIM13CCR3 50
#define	TIM13CCR4 51
#define	TIM14CCR1 52
#define	TIM14CCR2 53
#define	TIM14CCR3 54
#define	TIM14CCR4 55


#define ENUM_USART1 0
#define ENUM_USART2 1
#define ENUM_USART3 2
#define ENUM_USART4 3
#define ENUM_USART5 4
#define ENUM_USART6 5


#define ENUM_DMA1_STREAM_0 0
#define ENUM_DMA1_STREAM_1 1
#define ENUM_DMA1_STREAM_2 2
#define ENUM_DMA1_STREAM_3 3
#define ENUM_DMA1_STREAM_4 4
#define ENUM_DMA1_STREAM_5 5
#define ENUM_DMA1_STREAM_6 6
#define ENUM_DMA1_STREAM_7 7
#define ENUM_DMA2_STREAM_0 8
#define ENUM_DMA2_STREAM_1 9
#define ENUM_DMA2_STREAM_2 10
#define ENUM_DMA2_STREAM_3 11
#define ENUM_DMA2_STREAM_4 12
#define ENUM_DMA2_STREAM_5 13
#define ENUM_DMA2_STREAM_6 14
#define ENUM_DMA2_STREAM_7 15


//USB config
#define RFFW_HID_PRODUCT_STRING "RaceFlight FC"
#define RFBL_HID_PRODUCT_STRING "RaceFlight Boot Loader"
#define RFRC_HID_PRODUCT_STRING "RaceFlight Recovery"


#ifdef STM32F405xx
	#include "REVOLT.h"
#elif defined(STM32F446xx)
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
	uint32_t				instance;

	uint32_t				NSSPin;
	uint32_t				SCKPin;
	uint32_t				MISOPin;
	uint32_t				MOSIPin;

	uint32_t				NSSPort;
	uint32_t				SCKPort;
	uint32_t				MISOPort;
	uint32_t				MOSIPort;
	
	//uint32_t				NSSAlternate;
	uint32_t				SCKAlternate;
	uint32_t				MISOAlternate;
	uint32_t				MOSIAlternate;
	
	//uint32_t				GPIOSpeed;
	
	uint32_t				SPI_IRQn;
	uint32_t 				spiHandle;

	uint32_t				TXDma;
	uint32_t				RXDma;

	uint32_t				TXDMA_IRQn;
	uint32_t				RXDMA_IRQn;

	//uint32_t				TXDMA_IRQ_Handler;
	//uint32_t				RXDMA_IRQ_Handler;
} board_spi;

typedef struct {
	uint32_t				enabled;
	uint32_t				PinMode;
	uint32_t				Pull;
	uint32_t				Speed;
	uint32_t				TXAlternate;
	uint32_t				TXPin;
	uint32_t				RXAlternate;
	uint32_t				RXPin;

	uint32_t				RXPort; // loaded from port array
	uint32_t				TXPort; // loaded from port array

	uint32_t				SerialInstance; // loaded from usart array

	uint32_t				BaudRate;
	uint32_t				WordLength;
	uint32_t				StopBits;
	uint32_t				Parity;
	uint32_t				HwFlowCtl;
	uint32_t				Mode;

	uint32_t				USART_IRQn;
	uint32_t				usartHandle;

	uint32_t				TXDma;
	uint32_t				RXDma;

	uint32_t				Protocol;
	uint32_t				FrameSize;

	int32_t					serialTxBuffer;
	int32_t					serialRxBuffer;

} board_serial;

typedef struct {
	uint32_t				enabled;
	uint32_t				dmaStream;
	uint32_t				dmaChannel;
	uint32_t				dmaDirection;
	uint32_t				dmaPeriphInc;
	uint32_t				dmaMemInc;
	uint32_t				dmaPeriphAlignment;
	uint32_t				dmaMemAlignment;
	uint32_t				dmaMode;
	uint32_t				dmaPriority;
	uint32_t				fifoMode;
	uint32_t				fifoThreshold;
	uint32_t				MemBurst;
	uint32_t				PeriphBurst;
	uint32_t				dmaIRQn;
	uint32_t				dmaHandle;
} board_dma;

typedef struct {
	uint32_t				port;
	uint32_t				pin;
	uint32_t				inverted;
	uint32_t				enabled;
} internal_led_type;

typedef struct {
	uint32_t				enabled;
	uint32_t				spiNumber;
	uint32_t				csPin;
	uint32_t				csPort;
	uint32_t				extiPin;
	uint32_t				extiPort;
	uint32_t				extiIRQn;
	uint32_t				spiFastBaud;
	uint32_t				spiSlowBaud;
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
	uint32_t				enabled;
	uint32_t				timer;
	uint32_t				pin;
	uint32_t				port;
	uint32_t				AF;	
	uint32_t				timChannel;
	uint32_t				timChannelC;
	uint32_t				activeTim;
	uint32_t				timCCR;
	uint32_t				polarity;
	uint32_t				dmaHandle;
	uint32_t				timerHandle;
	uint32_t				Dma;
	uint32_t				CcDmaHandle;
	uint32_t				timerIRQn;
	uint32_t				motorOutputBuffer;
 	uint32_t				motorOutputLength;
 	uint32_t				sConfigOCHandle;
 	uint32_t				EXTIn;
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
	
	motor_type				motors[MAX_MOTOR_NUMBER];
	motor_type				servos[MAX_SERVO_NUMBER];

	board_gyro				gyro_pins;

	board_spi				spis[6];

	board_serial			serials[6];

	board_dma				dmas[16];

	board_dma				dmasMotor[16];

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

typedef void (*function_pointer)(void);

extern function_pointer callbackFunctionArray[];

extern board_type          board;
extern GPIO_TypeDef       *ports[];
extern volatile uint32_t  *ccr[];
extern TIM_TypeDef        *timers[];
extern serial_type         usarts[];
extern DMA_Stream_TypeDef *dmaStream[];
extern UART_HandleTypeDef  uartHandles[];
extern DMA_HandleTypeDef   dmaHandles[];
extern TIM_HandleTypeDef   pwmTimers[];
extern TIM_OC_InitTypeDef  sConfigOCHandles[];
extern SPI_HandleTypeDef   spiHandles[];
extern SPI_TypeDef        *spiInstance[];

extern unsigned char serialTxBuffer[][TXBUFFERSIZE];
extern unsigned char serialRxBuffer[][RXBUFFERSIZE];
extern uint32_t motorOutputBuffer[][128];

extern int InitializeMCUSettings();
void getBoardHardwareDefs();
