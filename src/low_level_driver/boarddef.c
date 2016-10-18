#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../flight_controller/inc/rf_math.h"
#include "includes.h"

GPIO_TypeDef *ports[11];
serial_type usarts[6];
spi_type spis[5];
TIM_TypeDef *timers[14];
board_type board;


int InitializeMCUSettings() {
	//target_pinout pins;



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

	return(1);
}

void getBoardHardwareDefs(void)
{
	bzero(&board, sizeof(board));

	//PLL settings
	board.fc_pllm = FC_PLLM;
	board.fc_plln = FC_PLLN;
	board.fc_pllp = FC_PLLP;
	board.fc_pllq = FC_PLLQ;


	//LED Settings
	board.internalLeds[0].enabled = 1;
	board.internalLeds[0].pin = GPIO_PIN_15;
	board.internalLeds[0].port = 0;
	board.internalLeds[0].inverted = 0;

	board.internalLeds[1].enabled = 1;
	board.internalLeds[1].pin = GPIO_PIN_8;
	board.internalLeds[1].port = 2;
	board.internalLeds[1].inverted = 0;

	board.internalLeds[2].enabled = 0;
	//board.internalLeds[2].pin = GPIO_PIN_1;
	//board.internalLeds[2].port = 0;
	board.internalLeds[2].inverted = 0;


	//Buzzer Settings
	board.buzzerPort = 2;	//Port C
	board.buzzerPin = GPIO_PIN_2;

	
	//gyro settings

	//Motor output assignments
	board.motors[0].timer = MOTOR1_TIM;
	board.motors[0].pin = MOTOR1_PIN;
	board.motors[0].port = MOTOR1_GPIO;
	board.motors[0].AF = MOTOR1_ALTERNATE;
	board.motors[0].timChannel = MOTOR1_TIM_CH;
	board.motors[0].timCCR = MOTOR1_TIM_CCR;

	board.motors[1].timer = MOTOR2_TIM;
	board.motors[1].pin = MOTOR2_PIN;
	board.motors[1].port = MOTOR2_GPIO;
	board.motors[1].AF = MOTOR2_ALTERNATE;
	board.motors[1].timChannel = MOTOR2_TIM_CH;
	board.motors[1].timCCR = MOTOR2_TIM_CCR;

	board.motors[2].timer = MOTOR3_TIM;
	board.motors[2].pin = MOTOR3_PIN;
	board.motors[2].port = MOTOR3_GPIO;
	board.motors[2].AF = MOTOR3_ALTERNATE;
	board.motors[2].timChannel = MOTOR3_TIM_CH;
	board.motors[2].timCCR = MOTOR3_TIM_CCR;

	board.motors[3].timer = MOTOR4_TIM;
	board.motors[3].pin = MOTOR4_PIN;
	board.motors[3].port = MOTOR4_GPIO;
	board.motors[3].AF = MOTOR4_ALTERNATE;
	board.motors[3].timChannel = MOTOR4_TIM_CH;
	board.motors[3].timCCR = MOTOR4_TIM_CCR;

	//Uart settings
	board.serials[4].PinMode = GPIO_MODE_AF_PP;
	board.serials[4].Pull = GPIO_PULLUP;
	board.serials[4].Speed = GPIO_SPEED_HIGH;
	board.serials[4].TXAlternate = USART5_TX_AF;
	board.serials[4].TXPin = USART5_TX_PIN;
	board.serials[4].TXPort = USART5_TX_GPIO_PORT;
	board.serials[4].RXAlternate = USART5_RX_AF;
	board.serials[4].RXPin = USART5_RX_PIN;
	board.serials[4].RXPort = USART5_RX_GPIO_PORT;
	

	board.serials[4].SerialInstance = UART5;  // loaded from port array

	board.serials[4].BaudRate = 115200;
	board.serials[4].WordLength = UART_WORDLENGTH_8B;
	board.serials[4].StopBits = UART_STOPBITS_1;
	board.serials[4].Parity = UART_PARITY_NONE;
	board.serials[4].HwFlowCtl = UART_HWCONTROL_NONE;
	board.serials[4].Mode = UART_MODE_TX_RX;

	board.serials[4].USART_IRQn = USART5_IRQn;

	board.serials[4].TXDMAStream = USART5_TX_DMA_STREAM; // looked up from array
	board.serials[4].TXDMAChannel = USART5_TX_DMA_CHANNEL;
	board.serials[4].TXDMADirection = DMA_MEMORY_TO_PERIPH;
	board.serials[4].TXDMAPeriphInc = DMA_PINC_DISABLE;
	board.serials[4].TXDMAMemInc = DMA_MINC_ENABLE;
	board.serials[4].TXDMAPeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	board.serials[4].TXDMAMemDataAlignment = DMA_MDATAALIGN_BYTE;
	board.serials[4].TXDMAMode = DMA_NORMAL;
	board.serials[4].TXDMAPriority = DMA_PRIORITY_LOW;
	board.serials[4].TXDMAFIFOMode = DMA_FIFOMODE_DISABLE;

	board.serials[4].RXDMAStream = USART5_RX_DMA_STREAM; // looked up from array
	board.serials[4].RXDMAChannel = USART5_RX_DMA_CHANNEL;
	board.serials[4].RXDMADirection = DMA_PERIPH_TO_MEMORY;
	board.serials[4].RXDMAPeriphInc = DMA_PINC_DISABLE;
	board.serials[4].RXDMAMemInc = DMA_MINC_ENABLE;
	board.serials[4].RXDMAPeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	board.serials[4].RXDMAMemDataAlignment = DMA_MDATAALIGN_BYTE;
	board.serials[4].RXDMAMode = DMA_CIRCULAR;
	board.serials[4].RXDMAPriority = DMA_PRIORITY_HIGH;
	board.serials[4].RXDMAFIFOMode = DMA_FIFOMODE_DISABLE;

	board.serials[4].TXDMA_IRQn = USART5_DMA_TX_IRQn;
	board.serials[4].RXDMA_IRQn = USART5_DMA_RX_IRQn;
}