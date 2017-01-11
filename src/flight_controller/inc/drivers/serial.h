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

extern void UsartInit(uint32_t serialNumber);
extern void UsartDeInit(uint32_t serialNumber);
extern void UsartDmaInit(uint32_t serialNumber);
extern void InitBoardUsarts(void);
extern void DeInitBoardUsarts(void);
extern void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void USARTx_DMA_RX_IRQHandler(void);
extern void USARTx_DMA_TX_IRQHandler(void);
extern void USARTx_IRQHandler(void);
extern void SerialTxCallback(void);
