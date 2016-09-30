#pragma once
#include "includes.h"

UART_HandleTypeDef uartHandle;
DMA_HandleTypeDef dmaUartRx;
DMA_HandleTypeDef dmaUartTx;


#define RXBUFFERSIZE 64
#define TXBUFFERSIZE 64

/* Buffer used for transmission */
unsigned char aTxBuffer[TXBUFFERSIZE];

/* Buffer used for reception */
unsigned char aRxBuffer[RXBUFFERSIZE];

unsigned char txTransimissionReady;

void UsartInit(unsigned int baudRate, USART_TypeDef* Usart, UART_HandleTypeDef *huart);
void UsartDmaInit(UART_HandleTypeDef *huart);
void BoardUsartInit ();
void USARTx_DMA_RX_IRQHandler(void);
void USARTx_DMA_TX_IRQHandler(void);
void USARTx_IRQHandler(void);
