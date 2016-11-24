#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../flight_controller/inc/rf_math.h"
#include "includes.h"

GPIO_TypeDef       *ports[11];
serial_type         usarts[6];
spi_type            spis[5];
TIM_TypeDef        *timers[14];
volatile uint32_t  *ccr[56];
board_type          board;
DMA_Stream_TypeDef *dmaStream[16];
UART_HandleTypeDef  uartHandles[6];
DMA_HandleTypeDef   dmaHandles[16];

unsigned char serialRxBuffer[3][RXBUFFERSIZE];
unsigned char serialTxBuffer[3][TXBUFFERSIZE];

int InitializeMCUSettings() {
	//target_pinout pins;

	bzero(dmaStream, sizeof(dmaStream));
	dmaStream[0] = DMA1_Stream0;
	dmaStream[1] = DMA1_Stream1;
	dmaStream[2] = DMA1_Stream2;
	dmaStream[3] = DMA1_Stream3;
	dmaStream[4] = DMA1_Stream4;
	dmaStream[5] = DMA1_Stream5;
	dmaStream[6] = DMA1_Stream6;
	dmaStream[7] = DMA1_Stream7;
	dmaStream[8] = DMA2_Stream0;
	dmaStream[9] = DMA2_Stream1;
	dmaStream[10] = DMA2_Stream2;
	dmaStream[11] = DMA2_Stream3;
	dmaStream[12] = DMA2_Stream4;
	dmaStream[13] = DMA2_Stream5;
	dmaStream[14] = DMA2_Stream6;
	dmaStream[15] = DMA2_Stream7;

	bzero(ports, sizeof(ports));
	ports[0]=_GPIOA;
	ports[1]=_GPIOB;
	ports[2]=_GPIOC;
	ports[3]=_GPIOD;
	ports[4]=_GPIOE;
	ports[5]=_GPIOF;
	ports[6]=_GPIOG;
	ports[7]=_GPIOH;
	ports[8]=_GPIOI;
	ports[9]=_GPIOJ;
	ports[10]=_GPIOK;

	bzero(usarts, sizeof(usarts));
	usarts[0].port=_USART1;
	usarts[0].async=_USART1s;

	usarts[1].port=_USART2;
	usarts[1].async=_USART2s;

	usarts[2].port=_USART3;
	usarts[2].async=_USART3s;

	usarts[3].port=_USART4;
	usarts[3].async=_USART4s;

	usarts[4].port=_USART5;
	usarts[4].async=_USART5s;

	usarts[5].port=_USART6;
	usarts[5].async=_USART6s;

	bzero(timers, sizeof(timers));
	timers[0]=_TIM1;
	timers[1]=_TIM2;
	timers[2]=_TIM3;
	timers[3]=_TIM4;
	timers[4]=_TIM5;
	timers[5]=_TIM6;
	timers[6]=_TIM7;
	timers[7]=_TIM8;
	timers[8]=_TIM9;
	timers[9]=_TIM10;
	timers[10]=_TIM11;
	timers[11]=_TIM12;
	timers[12]=_TIM13;
	timers[13]=_TIM14;

	bzero(ccr, sizeof(ccr));
	ccr[0] = &_TIM1->CCR1;
	ccr[1] = &_TIM1->CCR2;
	ccr[2] = &_TIM1->CCR3;
	ccr[3] = &_TIM1->CCR4;
	ccr[4] = &_TIM2->CCR1;
	ccr[5] = &_TIM2->CCR2;
	ccr[6] = &_TIM2->CCR3;
	ccr[7] = &_TIM2->CCR4;
	ccr[8] = &_TIM3->CCR1;
	ccr[9] = &_TIM3->CCR2;
	ccr[10] = &_TIM3->CCR3;
	ccr[11] = &_TIM3->CCR4;
	ccr[12] = &_TIM4->CCR1;
	ccr[13] = &_TIM4->CCR2;
	ccr[14] = &_TIM4->CCR3;
	ccr[15] = &_TIM4->CCR4;
	ccr[16] = &_TIM5->CCR1;
	ccr[17] = &_TIM5->CCR2;
	ccr[18] = &_TIM5->CCR3;
	ccr[19] = &_TIM5->CCR4;
	ccr[20] = &_TIM6->CCR1;
	ccr[21] = &_TIM6->CCR2;
	ccr[22] = &_TIM6->CCR3;
	ccr[23] = &_TIM6->CCR4;
	ccr[24] = &_TIM7->CCR1;
	ccr[25] = &_TIM7->CCR2;
	ccr[26] = &_TIM7->CCR3;
	ccr[27] = &_TIM7->CCR4;
	ccr[28] = &_TIM8->CCR1;
	ccr[29] = &_TIM8->CCR2;
	ccr[30] = &_TIM8->CCR3;
	ccr[31] = &_TIM8->CCR4;
	ccr[32] = &_TIM9->CCR1;
	ccr[33] = &_TIM9->CCR2;
	ccr[34] = &_TIM9->CCR3;
	ccr[35] = &_TIM9->CCR4;
	ccr[36] = &_TIM10->CCR1;
	ccr[37] = &_TIM10->CCR2;
	ccr[38] = &_TIM10->CCR3;
	ccr[39] = &_TIM10->CCR4;
	ccr[40] = &_TIM11->CCR1;
	ccr[41] = &_TIM11->CCR2;
	ccr[42] = &_TIM11->CCR3;
	ccr[43] = &_TIM11->CCR4;
	ccr[44] = &_TIM12->CCR1;
	ccr[45] = &_TIM12->CCR2;
	ccr[46] = &_TIM12->CCR3;
	ccr[47] = &_TIM12->CCR4;
	ccr[48] = &_TIM13->CCR1;
	ccr[49] = &_TIM13->CCR2;
	ccr[50] = &_TIM13->CCR3;
	ccr[51] = &_TIM13->CCR4;
	ccr[52] = &_TIM14->CCR1;
	ccr[53] = &_TIM14->CCR2;
	ccr[54] = &_TIM14->CCR3;
	ccr[55] = &_TIM14->CCR4;

	return(1);
}

uint32_t boardSize;
void getBoardHardwareDefs(void)
{
	boardSize = sizeof(board);
	bzero(&board, sizeof(board));

	//PLL settings
	board.fc_pllm = FC_PLLM;
	board.fc_plln = FC_PLLN;
	board.fc_pllp = FC_PLLP;
	board.fc_pllq = FC_PLLQ;


	//LED Settings
	board.internalLeds[0].enabled  = LED1_ENABLED;
	board.internalLeds[0].pin      = LED1_GPIO_Pin;
	board.internalLeds[0].port     = LED1_GPIO_Port;
	board.internalLeds[0].inverted = LED1_INVERTED;

	board.internalLeds[1].enabled  = LED2_ENABLED;
	board.internalLeds[1].pin      = LED2_GPIO_Pin;
	board.internalLeds[1].port     = LED2_GPIO_Port;
	board.internalLeds[1].inverted = LED2_INVERTED;

	board.internalLeds[2].enabled  = LED3_ENABLED;
	board.internalLeds[2].pin      = LED3_GPIO_Pin;
	board.internalLeds[2].port     = LED3_GPIO_Port;
	board.internalLeds[2].inverted = LED3_INVERTED;


	//Buzzer Settings
	board.buzzerPort = BUZZER_GPIO_Port;	//Port C
	board.buzzerPin  = BUZZER_GPIO_Pin;

	
	//gyro settings

	//Motor output assignments
	board.motors[0].enabled    = 1;
	board.motors[0].timer      = MOTOR1_TIM;
	board.motors[0].pin        = MOTOR1_PIN;
	board.motors[0].port       = MOTOR1_GPIO;
	board.motors[0].AF         = MOTOR1_ALTERNATE;
	board.motors[0].timChannel = MOTOR1_TIM_CH;
	board.motors[0].timCCR     = MOTOR1_TIM_CCR;
	board.motors[0].polarity   = MOTOR2_POLARITY;

	board.motors[1].enabled    = 1;
	board.motors[1].timer      = MOTOR2_TIM;
	board.motors[1].pin        = MOTOR2_PIN;
	board.motors[1].port       = MOTOR2_GPIO;
	board.motors[1].AF         = MOTOR2_ALTERNATE;
	board.motors[1].timChannel = MOTOR2_TIM_CH;
	board.motors[1].timCCR     = MOTOR2_TIM_CCR;
	board.motors[1].polarity   = MOTOR2_POLARITY;

	board.motors[2].enabled    = 1;
	board.motors[2].timer      = MOTOR3_TIM;
	board.motors[2].pin        = MOTOR3_PIN;
	board.motors[2].port       = MOTOR3_GPIO;
	board.motors[2].AF         = MOTOR3_ALTERNATE;
	board.motors[2].timChannel = MOTOR3_TIM_CH;
	board.motors[2].timCCR     = MOTOR3_TIM_CCR;
	board.motors[2].polarity   = MOTOR3_POLARITY;

	board.motors[3].enabled    = 1;
	board.motors[3].timer      = MOTOR4_TIM;
	board.motors[3].pin        = MOTOR4_PIN;
	board.motors[3].port       = MOTOR4_GPIO;
	board.motors[3].AF         = MOTOR4_ALTERNATE;
	board.motors[3].timChannel = MOTOR4_TIM_CH;
	board.motors[3].timCCR     = MOTOR4_TIM_CCR;
	board.motors[3].polarity   = MOTOR4_POLARITY;

	//GYRO connection settings	------------------------------------------------------------------------------------------------------------------------------------------------------------
	board.gyro_pins.SPIInstance = GYRO_SPI;	//use this to determine spi irqn and irq handlers to use. No need to re define them here
	board.gyro_pins.SPINumber   = GYRO_SPI_NUMBER;
	board.gyro_pins.csPin       = GYRO_SPI_CS_GPIO_Pin;
	board.gyro_pins.csPort      = GYRO_SPI_CS_GPIO_Port;
	board.gyro_pins.extiPin     = GYRO_EXTI_GPIO_Pin;
	board.gyro_pins.extiPort    = GYRO_EXTI_GPIO_Port;
	board.gyro_pins.extiIRQn    = GYRO_EXTI_IRQn;
	board.gyro_pins.spiFastBaud = GYRO_SPI_FAST_BAUD;
	board.gyro_pins.spiSlowBaud = GYRO_SPI_SLOW_BAUD;


	//SPI settings ------------------------------------------------------------------------------------------------------------------------------------------------------------
	board.spis[0].enabled  = SPI1_ENABLE;

	board.spis[0].NSSPin   = SPI1_NSS_PIN;
	board.spis[0].SCKPin   = SPI1_SCK_PIN;
	board.spis[0].MISOPin  = SPI1_MISO_PIN;
	board.spis[0].MOSIPin  = SPI1_MOSI_PIN;

	board.spis[0].NSSPort  = SPI1_NSS_GPIO_PORT;
	board.spis[0].SCKPort  = SPI1_SCK_GPIO_PORT;
	board.spis[0].MISOPort = SPI1_MISO_GPIO_PORT;
	board.spis[0].MOSIPort = SPI1_MOSI_GPIO_PORT;
	
	//board.spis[0].NSSAlternate =;
	board.spis[0].SCKAlternate =  SPI1_SCK_AF;
	board.spis[0].MISOAlternate = SPI1_MISO_AF;
	board.spis[0].MOSIAlternate = SPI1_MOSI_AF;
	
	//board.spis[0].GPIOSpeed =;
	board.spis[0].SPI_IRQn = SPI1_IRQn;

	board.spis[0].TXDMAStream    = SPI1_TX_DMA_STREAM; // looked up from array
	board.spis[0].TXDMAChannel   = SPI1_TX_DMA_CHANNEL;
	board.spis[0].TXDMADirection = DMA_MEMORY_TO_PERIPH;
	board.spis[0].TXDMAPeriphInc = DMA_PINC_DISABLE;
	board.spis[0].TXDMAMemInc    = DMA_MINC_ENABLE;
	board.spis[0].TXDMAPeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	board.spis[0].TXDMAMemDataAlignment    = DMA_MDATAALIGN_BYTE;
	board.spis[0].TXDMAMode                = DMA_NORMAL;
	board.spis[0].TXDMAPriority            = DMA_PRIORITY_HIGH;
	board.spis[0].TXDMAFIFOMode            = DMA_FIFOMODE_DISABLE;

	board.spis[0].RXDMAStream = SPI1_RX_DMA_STREAM; // looked up from array
	board.spis[0].RXDMAChannel = SPI1_RX_DMA_CHANNEL;
	board.spis[0].RXDMADirection = DMA_PERIPH_TO_MEMORY;
	board.spis[0].RXDMAPeriphInc = DMA_PINC_DISABLE;
	board.spis[0].RXDMAMemInc = DMA_MINC_ENABLE;
	board.spis[0].RXDMAPeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	board.spis[0].RXDMAMemDataAlignment = DMA_MDATAALIGN_BYTE;
	board.spis[0].RXDMAMode = DMA_NORMAL;
	board.spis[0].RXDMAPriority = DMA_PRIORITY_HIGH;
	board.spis[0].RXDMAFIFOMode = DMA_FIFOMODE_DISABLE;

	board.spis[0].TXDMA_IRQn = SPI1_TX_DMA_IRQn;
	board.spis[0].RXDMA_IRQn = SPI1_RX_DMA_IRQn;

	board.spis[0].TXDma 		    = ENUM_DMA2_STREAM_3;
	board.spis[0].RXDma 		    = ENUM_DMA2_STREAM_0;

	board.dmas[board.spis[0].TXDma].enabled            = 1;
	board.dmas[board.spis[0].TXDma].dmaStream          = ENUM_DMA2_STREAM_3;
	board.dmas[board.spis[0].TXDma].dmaChannel         = DMA_CHANNEL_3;
	board.dmas[board.spis[0].TXDma].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmas[board.spis[0].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmas[board.spis[0].TXDma].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmas[board.spis[0].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmas[board.spis[0].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmas[board.spis[0].TXDma].dmaMode            = DMA_NORMAL;
	board.dmas[board.spis[0].TXDma].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmas[board.spis[0].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmas[board.spis[0].TXDma].dmaIRQn            = DMA2_Stream3_IRQn;
	board.dmas[board.spis[0].TXDma].dmaHandle          = ENUM_DMA2_STREAM_3;

	board.dmas[board.spis[0].RXDma].enabled            = 1;
	board.dmas[board.spis[0].RXDma].dmaStream          = ENUM_DMA2_STREAM_0;
	board.dmas[board.spis[0].RXDma].dmaChannel         = DMA_CHANNEL_3;
	board.dmas[board.spis[0].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmas[board.spis[0].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmas[board.spis[0].RXDma].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmas[board.spis[0].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmas[board.spis[0].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmas[board.spis[0].RXDma].dmaMode            = DMA_NORMAL;
	board.dmas[board.spis[0].RXDma].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmas[board.spis[0].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmas[board.spis[0].RXDma].dmaIRQn            = DMA2_Stream0_IRQn;
	board.dmas[board.spis[0].RXDma].dmaHandle          = ENUM_DMA2_STREAM_0;


	board.spis[1].enabled = SPI2_ENABLE;

	board.spis[1].NSSPin = SPI2_NSS_PIN;
	board.spis[1].SCKPin = SPI2_SCK_PIN;
	board.spis[1].MISOPin = SPI2_MISO_PIN;
	board.spis[1].MOSIPin = SPI2_MOSI_PIN;

	board.spis[1].NSSPort = SPI2_NSS_GPIO_PORT;
	board.spis[1].SCKPort = SPI2_SCK_GPIO_PORT;
	board.spis[1].MISOPort = SPI2_MISO_GPIO_PORT;
	board.spis[1].MOSIPort = SPI2_MOSI_GPIO_PORT;
	
	//board.spis[1].NSSAlternate =;
	board.spis[1].SCKAlternate = SPI2_SCK_AF;
	board.spis[1].MISOAlternate = SPI2_MISO_AF;
	board.spis[1].MOSIAlternate = SPI2_MOSI_AF;
	
	//board.spis[1].GPIOSpeed =;
	board.spis[1].SPI_IRQn = SPI2_IRQn;

	board.spis[1].TXDMAStream = SPI2_TX_DMA_STREAM; // looked up from array
	board.spis[1].TXDMAChannel = SPI2_TX_DMA_CHANNEL;
	board.spis[1].TXDMADirection = DMA_MEMORY_TO_PERIPH;
	board.spis[1].TXDMAPeriphInc = DMA_PINC_DISABLE;
	board.spis[1].TXDMAMemInc = DMA_MINC_ENABLE;
	board.spis[1].TXDMAPeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	board.spis[1].TXDMAMemDataAlignment = DMA_MDATAALIGN_BYTE;
	board.spis[1].TXDMAMode = DMA_NORMAL;
	board.spis[1].TXDMAPriority = DMA_PRIORITY_HIGH;
	board.spis[1].TXDMAFIFOMode = DMA_FIFOMODE_DISABLE;

	board.spis[1].RXDMAStream = SPI2_RX_DMA_STREAM; // looked up from array
	board.spis[1].RXDMAChannel = SPI2_RX_DMA_CHANNEL;
	board.spis[1].RXDMADirection = DMA_PERIPH_TO_MEMORY;
	board.spis[1].RXDMAPeriphInc = DMA_PINC_DISABLE;
	board.spis[1].RXDMAMemInc = DMA_MINC_ENABLE;
	board.spis[1].RXDMAPeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	board.spis[1].RXDMAMemDataAlignment = DMA_MDATAALIGN_BYTE;
	board.spis[1].RXDMAMode = DMA_NORMAL;
	board.spis[1].RXDMAPriority = DMA_PRIORITY_HIGH;
	board.spis[1].RXDMAFIFOMode = DMA_FIFOMODE_DISABLE;

	board.spis[1].TXDMA_IRQn = SPI2_TX_DMA_IRQn;
	board.spis[1].RXDMA_IRQn = SPI2_RX_DMA_IRQn;

	//board.spis[1].TXDMA_IRQ_Handler = SPI2_TX_DMA_IRQHandler;
	//board.spis[1].RXDMA_IRQ_Handler = SPI2_RX_DMA_IRQHandler;

	board.spis[2].enabled = SPI3_ENABLE;

	board.spis[2].NSSPin = SPI2_NSS_PIN;
	board.spis[2].SCKPin = SPI2_SCK_PIN;
	board.spis[2].MISOPin = SPI2_MISO_PIN;
	board.spis[2].MOSIPin = SPI2_MOSI_PIN;

	board.spis[2].NSSPort = SPI2_NSS_GPIO_PORT;
	board.spis[2].SCKPort = SPI2_SCK_GPIO_PORT;
	board.spis[2].MISOPort = SPI2_MISO_GPIO_PORT;
	board.spis[2].MOSIPort = SPI2_MOSI_GPIO_PORT;
	
	//board.spis[2].NSSAlternate =;
	board.spis[2].SCKAlternate = SPI2_SCK_AF;
	board.spis[2].MISOAlternate = SPI2_MISO_AF;
	board.spis[2].MOSIAlternate = SPI2_MOSI_AF;
	
	//board.spis[2].GPIOSpeed =;
	board.spis[2].SPI_IRQn = SPI3_IRQn;

	board.spis[2].TXDMAStream = SPI2_TX_DMA_STREAM; // looked up from array
	board.spis[2].TXDMAChannel = SPI2_TX_DMA_CHANNEL;
	board.spis[2].TXDMADirection = DMA_MEMORY_TO_PERIPH;
	board.spis[2].TXDMAPeriphInc = DMA_PINC_DISABLE;
	board.spis[2].TXDMAMemInc = DMA_MINC_ENABLE;
	board.spis[2].TXDMAPeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	board.spis[2].TXDMAMemDataAlignment = DMA_MDATAALIGN_BYTE;
	board.spis[2].TXDMAMode = DMA_NORMAL;
	board.spis[2].TXDMAPriority = DMA_PRIORITY_HIGH;
	board.spis[2].TXDMAFIFOMode = DMA_FIFOMODE_DISABLE;

	board.spis[2].RXDMAStream = SPI2_RX_DMA_STREAM; // looked up from array
	board.spis[2].RXDMAChannel = SPI2_RX_DMA_CHANNEL;
	board.spis[2].RXDMADirection = DMA_PERIPH_TO_MEMORY;
	board.spis[2].RXDMAPeriphInc = DMA_PINC_DISABLE;
	board.spis[2].RXDMAMemInc = DMA_MINC_ENABLE;
	board.spis[2].RXDMAPeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	board.spis[2].RXDMAMemDataAlignment = DMA_MDATAALIGN_BYTE;
	board.spis[2].RXDMAMode = DMA_NORMAL;
	board.spis[2].RXDMAPriority = DMA_PRIORITY_HIGH;
	board.spis[2].RXDMAFIFOMode = DMA_FIFOMODE_DISABLE;

	board.spis[2].TXDMA_IRQn = SPI3_TX_DMA_IRQn;
	board.spis[2].RXDMA_IRQn = SPI3_RX_DMA_IRQn;


	//UART settings ------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	board.serials[0].enabled         = 1;
	board.serials[0].PinMode         = GPIO_MODE_AF_PP;
	board.serials[0].Pull            = GPIO_PULLUP;
	board.serials[0].Speed           = GPIO_SPEED_HIGH;
	board.serials[0].TXAlternate     = GPIO_AF7_USART1;
	board.serials[0].TXPin           = GPIO_PIN_9;
	board.serials[0].TXPort          = ENUM_PORTA;
	board.serials[0].RXAlternate     = GPIO_AF7_USART1;
	board.serials[0].RXPin           = GPIO_PIN_9;
	board.serials[0].RXPort          = ENUM_PORTA;
	
	board.serials[0].SerialInstance = ENUM_USART1;  // loaded from port array

	board.serials[0].BaudRate       = 115200;
	board.serials[0].WordLength     = UART_WORDLENGTH_8B;
	board.serials[0].StopBits       = UART_STOPBITS_1;
	board.serials[0].Parity         = UART_PARITY_NONE;
	board.serials[0].HwFlowCtl      = UART_HWCONTROL_NONE;
	board.serials[0].Mode           = UART_MODE_TX_RX;

	board.serials[0].usartHandle    = ENUM_USART1;  //JUST A Define for the function name

	board.serials[0].serialTxBuffer = 1; //three buffers, give this one the first buffer
	board.serials[0].serialRxBuffer = 1; //three buffers, give this one the first buffer

	board.serials[0].TXDma 		    = ENUM_DMA2_STREAM_7;
	board.serials[0].RXDma 		    = ENUM_DMA2_STREAM_2;

	board.serials[0].Protocol	    = USING_SPEKTRUM;
	board.serials[0].FrameSize	    = 16;

	board.dmas[board.serials[0].TXDma].enabled            = 1;
	board.dmas[board.serials[0].TXDma].dmaStream          = ENUM_DMA2_STREAM_7;
	board.dmas[board.serials[0].TXDma].dmaChannel         = DMA_CHANNEL_4;
	board.dmas[board.serials[0].TXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmas[board.serials[0].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmas[board.serials[0].TXDma].dmaMemInc          = DMA_MINC_DISABLE;
	board.dmas[board.serials[0].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmas[board.serials[0].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmas[board.serials[0].TXDma].dmaMode            = DMA_CIRCULAR;
	board.dmas[board.serials[0].TXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmas[board.serials[0].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmas[board.serials[0].TXDma].dmaIRQn            = DMA2_Stream7_IRQn;
	board.dmas[board.serials[0].TXDma].dmaHandle          = ENUM_DMA2_STREAM_7;

	board.dmas[board.serials[0].RXDma].enabled            = 1;
	board.dmas[board.serials[0].RXDma].dmaStream          = ENUM_DMA2_STREAM_2;
	board.dmas[board.serials[0].RXDma].dmaChannel         = DMA_CHANNEL_4;
	board.dmas[board.serials[0].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmas[board.serials[0].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmas[board.serials[0].RXDma].dmaMemInc          = DMA_MINC_DISABLE;
	board.dmas[board.serials[0].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmas[board.serials[0].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmas[board.serials[0].RXDma].dmaMode            = DMA_CIRCULAR;
	board.dmas[board.serials[0].RXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmas[board.serials[0].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmas[board.serials[0].RXDma].dmaIRQn            = DMA2_Stream2_IRQn;
	board.dmas[board.serials[0].RXDma].dmaHandle          = ENUM_DMA2_STREAM_2;

}
