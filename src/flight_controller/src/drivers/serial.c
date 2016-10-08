#include "includes.h"

UART_HandleTypeDef uartHandle;
DMA_HandleTypeDef dmaUartRx;
DMA_HandleTypeDef dmaUartTx;

__IO ITStatus UartReady = RESET;



void UsartInit(unsigned int baudRate, USART_TypeDef* Usart, UART_HandleTypeDef *huart) {

	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = USARTx_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = USARTx_TX_AF;

	HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = USARTx_RX_PIN;
	GPIO_InitStruct.Alternate = USARTx_RX_AF;

	HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

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
	if(HAL_UART_DeInit(&uartHandle) != HAL_OK)
	{
//		ErrorHandler();
	}
	if(HAL_UART_Init(&uartHandle) != HAL_OK)
	{
//		ErrorHandler();
	}

	 UsartDmaInit(huart);
}

void UsartDeinit(UART_HandleTypeDef *huart, USART_TypeDef *Usart, GPIO_TypeDef *GPIOx_tx, uint16_t GPIO_Pin_tx, GPIO_TypeDef *GPIOx_rx, uint16_t GPIO_Pin_rx, uint8_t usartDmaTxIrqN, uint8_t usartDmaRxIrqN) {

	/*##-1- Reset peripherals ##################################################*/
	Usart = Usart;
/*
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
*/
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

void UsartDmaInit(UART_HandleTypeDef *huart)
{
	/*##-3- Configure the DMA ##################################################*/
	/* Configure the DMA handler for Transmission process */
	dmaUartTx.Instance                 = USARTx_TX_DMA_STREAM;
	dmaUartTx.Init.Channel             = USARTx_TX_DMA_CHANNEL;
	dmaUartTx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	dmaUartTx.Init.PeriphInc           = DMA_PINC_DISABLE;
	dmaUartTx.Init.MemInc              = DMA_MINC_ENABLE;
	dmaUartTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	dmaUartTx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	dmaUartTx.Init.Mode                = DMA_NORMAL;
	dmaUartTx.Init.Priority            = DMA_PRIORITY_LOW;

	HAL_DMA_Init(&dmaUartTx);

	/* Associate the initialized DMA handle to the UART handle */
	__HAL_LINKDMA(huart, hdmatx, dmaUartTx);

	/* Configure the DMA handler for reception process */
	dmaUartRx.Instance                 = USARTx_RX_DMA_STREAM;
	dmaUartRx.Init.Channel             = USARTx_RX_DMA_CHANNEL;
	dmaUartRx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	dmaUartRx.Init.PeriphInc           = DMA_PINC_DISABLE;
	dmaUartRx.Init.MemInc              = DMA_MINC_ENABLE;
	dmaUartRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	dmaUartRx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	dmaUartRx.Init.Mode                = DMA_CIRCULAR;
	dmaUartRx.Init.Priority            = DMA_PRIORITY_HIGH;

	HAL_DMA_Init(&dmaUartRx);

	/* Associate the initialized DMA handle to the the UART handle */
	__HAL_LINKDMA(huart, hdmarx, dmaUartRx);

	/*##-4- Configure the NVIC for DMA #########################################*/
	/* NVIC configuration for DMA transfer complete interrupt (USART6_TX) */
	HAL_NVIC_SetPriority(USARTx_DMA_TX_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USARTx_DMA_TX_IRQn);

	/* NVIC configuration for DMA transfer complete interrupt (USART6_RX) */
	HAL_NVIC_SetPriority(USARTx_DMA_RX_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USARTx_DMA_RX_IRQn);

	/* NVIC for USART, to catch the TX complete */
	HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USARTx_IRQn);

    /* DMA interrupt init */
    HAL_NVIC_SetPriority(USARTx_DMA_TX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USARTx_DMA_TX_IRQn);
    HAL_NVIC_SetPriority(USARTx_DMA_RX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USARTx_DMA_RX_IRQn);
}

void BoardUsartInit () {

    HAL_NVIC_DisableIRQ(USARTx_DMA_TX_IRQn);
    HAL_NVIC_DisableIRQ(USARTx_DMA_RX_IRQn);

    // read and write settings at slow speed
    UsartInit(115200, USART3, &uartHandle);

}

volatile char Rx_data;

//Interrupt callback routine
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Transmit_DMA(&huart, (uint8_t *)&Rx_data, 1);
	(void)(Rx_data);
	(void)(Rx_data);
}

	//Preston, look at this
	//http://electronics.stackexchange.com/questions/173025/stm32f0-uart-dma-interrupt-with-stm32cubemx-hal-1-2-1-problem
  /* Prevent unused argument(s) compilation warning */
//	__HAL_UART_FLUSH_DRREGISTER(huart);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t *)&rxBuffer, 1);
//  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file.
   */

void USARTx_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dmaUartRx);
//  typedef enum
//  {
//    HAL_DMA_STATE_RESET             = 0x00U,  /*!< DMA not yet initialized or disabled */
//    HAL_DMA_STATE_READY             = 0x01U,  /*!< DMA initialized and ready for use   */
//    HAL_DMA_STATE_BUSY              = 0x02U,  /*!< DMA process is ongoing              */
//    HAL_DMA_STATE_TIMEOUT           = 0x03U,  /*!< DMA timeout state                   */
//    HAL_DMA_STATE_ERROR             = 0x04U,  /*!< DMA error state                     */
//    HAL_DMA_STATE_ABORT             = 0x05U,  /*!< DMA Abort state                     */
//  }HAL_DMA_StateTypeDef;

//  if (HAL_DMA_GetState(&dmaUartRx) == HAL_DMA_STATE_READY) {
//      // RX is done. Call whatever function you want to happen here.
//
//  }

}

void USARTx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dmaUartTx);

}

void USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(&uartHandle);
}
