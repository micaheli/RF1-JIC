#pragma once


void UsartInit(unsigned int baudRate);
static void DmaInit(void);
void BoardUsartInit (void);
void USARTx_DMA_RX_IRQHandler(void);
void USARTx_DMA_TX_IRQHandler(void);
void USARTx_IRQHandler(void);
