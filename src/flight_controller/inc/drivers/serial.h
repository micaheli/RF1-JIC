#pragma once
#include "includes.h"

extern UART_HandleTypeDef uartHandle[];
extern DMA_HandleTypeDef dmaUartRx[];
extern DMA_HandleTypeDef dmaUartTx[];
extern uint32_t lastRXPacket;

//#define RXBUFFERSIZE 64
//#define TXBUFFERSIZE 64

//extern unsigned char serialTxBuffer[TXBUFFERSIZE];
//extern unsigned char serialRxBuffer[TXBUFFERSIZE];

unsigned char txTransimissionReady;

void UsartInit(uint32_t serialNumber);
void UsartDeinit(uint32_t serialNumber);
void UsartDmaInit(uint32_t serialNumber);
void BoardUsartInit(void);
void BoardUsartDeinit(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void USARTx_DMA_RX_IRQHandler(void);
void USARTx_DMA_TX_IRQHandler(void);
void USARTx_IRQHandler(void);
