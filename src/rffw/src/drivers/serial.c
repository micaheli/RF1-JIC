#include "includes.h"

UART_HandleTypeDef uartHandle;
DMA_HandleTypeDef dmaUartRx;
DMA_HandleTypeDef dmaUartTx;

__IO ITStatus UartReady = RESET;

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****UART_TwoBoards communication based on DMA****  ****UART_TwoBoards communication based on DMA****  ****UART_TwoBoards communication based on DMA**** ";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);


void UsartInit(unsigned int baudRate) {

	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART configured as follows:
		- Word Length = 8 Bits
		- Stop Bit = One Stop bit
		- Parity = None
		- BaudRate = 9600 baud
		- Hardware flow control disabled (RTS and CTS signals) */
	uartHandle.Instance        = USART3;

	uartHandle.Init.BaudRate   = baudRate;
	uartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	uartHandle.Init.StopBits   = UART_STOPBITS_1;
	uartHandle.Init.Parity     = UART_PARITY_NONE;
	uartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	uartHandle.Init.Mode       = UART_MODE_TX_RX;
	if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
	{
		ErrorHandler();
	}
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		ErrorHandler();
	}

}

static void DmaInit(void)
{
    /* DMA interrupt init */
    HAL_NVIC_SetPriority(USART_DMA_TX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART_DMA_TX_IRQn);
    HAL_NVIC_SetPriority(USART_DMA_RX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART_DMA_RX_IRQn);
}

void BoardUsartInit (void) {

    HAL_NVIC_DisableIRQ(USART_DMA_TX_IRQn);
    HAL_NVIC_DisableIRQ(USART_DMA_RX_IRQn);

    // read and write settings at slow speed
    UsartInit(9600);
    DMA_Init();

}


void USARTx_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dmaUartRx);

  if (HAL_DMA_GetState(&dmaUartRx) == HAL_DMA_STATE_READY) {
      // reset chip select line
      //HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_SET);

      // run callback for completed gyro read
      accgyroDeviceReadComplete();
  }

}

void USARTx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dmaUartTx);
}

void USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(&uartHandle);
}


/*
void GYRO_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&dma_gyro_tx);
}


void GYRO_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&dma_gyro_rx);

    if (HAL_DMA_GetState(&dma_gyro_rx) == HAL_DMA_STATE_READY) {
        // reset chip select line
        HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_SET);

        // run callback for completed gyro read
        accgyroDeviceReadComplete();
    }
}

*/
