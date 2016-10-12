#pragma once
#include "includes.h"

UART_HandleTypeDef uartHandle;
DMA_HandleTypeDef dmaUartRx;
DMA_HandleTypeDef dmaUartTx;
extern uint32_t lastRXPacket;

#define RXBUFFERSIZE 64
#define TXBUFFERSIZE 64

extern unsigned char serialTxBuffer[TXBUFFERSIZE];
extern unsigned char serialRxBuffer[TXBUFFERSIZE];

unsigned char txTransimissionReady;

void UsartInit(unsigned int baudRate, USART_TypeDef* Usart, UART_HandleTypeDef *huart);
void UsartDeinit(UART_HandleTypeDef *huart, USART_TypeDef *Usart, GPIO_TypeDef *GPIOx_tx, uint16_t GPIO_Pin_tx, GPIO_TypeDef *GPIOx_rx, uint16_t GPIO_Pin_rx, uint8_t usartDmaTxIrqN, uint8_t usartDmaRxIrqN);
void UsartDmaInit(UART_HandleTypeDef *huart);
void BoardUsartInit ();
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void USARTx_DMA_RX_IRQHandler(void);
void USARTx_DMA_TX_IRQHandler(void);
void USARTx_IRQHandler(void);
