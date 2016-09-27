#include "includes.h"

UART_HandleTypeDef uartHandle;
DMA_HandleTypeDef dmaUartRx;
DMA_HandleTypeDef dmaUartTx;

__IO ITStatus UartReady = RESET;

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****UART_TwoBoards communication based on DMA****  ****UART_TwoBoards communication based on DMA****  ****UART_TwoBoards communication based on DMA**** ";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];


void UsartInit(unsigned int baudRate, USART_TypeDef* Usart) {

	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART configured as follows:
		- Word Length = 8 Bits
		- Stop Bit = One Stop bit
		- Parity = None
		- BaudRate = 9600 baud
		- Hardware flow control disabled (RTS and CTS signals) */
	uartHandle.Instance        = Usart;

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

void UsartDeinit(UART_HandleTypeDef *huart, USART_TypeDef *Usart, GPIO_TypeDef *GPIOx_tx, uint16_t GPIO_Pin_tx, GPIO_TypeDef *GPIOx_rx, uint16_t GPIO_Pin_rx, uint8_t usartDmaTxIrqN, uint8_t usartDmaRxIrqN) {

	/*##-1- Reset peripherals ##################################################*/
	switch (Usart) {
		case USART1:
					__USART1_FORCE_RESET()
					__USART1_RELEASE_RESET()
					break;
		case USART2:
					__USART2_FORCE_RESET()
					__USART2_RELEASE_RESET()
					break;
		case USART3:
					__USART3_FORCE_RESET()
					__USART3_RELEASE_RESET()
					break;
		case USART4:
					__USART4_FORCE_RESET()
					__USART4_RELEASE_RESET()
					break;
		case USART5:
					__USART5_FORCE_RESET()
					__USART5_RELEASE_RESET()
					break;
		case USART6:
					__USART6_FORCE_RESET()
					__USART6_RELEASE_RESET()
					break;
	}

	/*##-2- Disable peripherals and GPIO Clocks #################################*/
	/* Configure USARTx Tx as alternate function  */
	HAL_GPIO_DeInit(GPIOx_tx, GPIO_Pin_tx);
	/* Configure USARTx Rx as alternate function  */
	HAL_GPIO_DeInit(GPIOx_rx, GPIO_Pin_rx);

	/*##-3- Disable the DMA #####################################################*/
	/* De-Initialize the DMA channel associated to reception process */
	if (huart->hdmarx != 0)
	{
		HAL_DMA_DeInit(huart->hdmarx);
	}
	/* De-Initialize the DMA channel associated to transmission process */
	if (huart->hdmatx != 0)
	{
		HAL_DMA_DeInit(huart->hdmatx);
	}

	/*##-4- Disable the NVIC for DMA ###########################################*/
	HAL_NVIC_DisableIRQ(usartDmaTxIrqN);
	HAL_NVIC_DisableIRQ(usartDmaRxIrqN);

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
    UsartInit(9600, USART3);
    DMA_Init();

}


void USARTx_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dmaUartRx);

  if (HAL_DMA_GetState(&dmaUartRx) == HAL_DMA_STATE_READY) {
      // reset chip select line
      //HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_SET);

      // run callback for completed gyro read
      //accgyroDeviceReadComplete();
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
