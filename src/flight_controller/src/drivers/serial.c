#include "includes.h"

UART_HandleTypeDef uartHandle;
DMA_HandleTypeDef dmaUartRx;
DMA_HandleTypeDef dmaUartTx;

__IO ITStatus UartReady = RESET;

uint8_t catfish;
uint32_t lastRXPacket;


/* Buffer used for transmission */
unsigned char serialTxBuffer[TXBUFFERSIZE];

/* Buffer used for reception */
unsigned char serialRxBuffer[RXBUFFERSIZE];


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
		- Word LengNoneth = 8 Bits
		- Stop Bit = One Stop bit
		- Parity = 
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

	//Config uart as  half duplex if TX and RX pins are the same
	if (USARTx_TX_PIN == USARTx_RX_PIN && USARTx_RX_GPIO_PORT == USARTx_TX_GPIO_PORT)
	{
		if (HAL_HalfDuplex_Init(&uartHandle) != HAL_OK)
		{
			//ErrorHandler();
		}
	}
	else
	{
		if(HAL_UART_Init(&uartHandle) != HAL_OK)
		{
			//ErrorHandler();
		}
	}
		

	__HAL_UART_ENABLE_IT(&uartHandle, UART_IT_IDLE);

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
	int x;
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
	dmaUartTx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

	HAL_DMA_Init(&dmaUartTx);

	/* Associate the initialized DMA handle to the UART handle */
	__HAL_LINKDMA(huart, hdmatx, dmaUartTx);

	/* Configure the DMA handler for reception process */
	dmaUartRx.Instance                 = USARTx_RX_DMA_STREAM;
	dmaUartRx.Init.Channel             = USARTx_TX_DMA_CHANNEL;
	dmaUartRx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	dmaUartRx.Init.PeriphInc           = DMA_PINC_DISABLE;
	dmaUartRx.Init.MemInc              = DMA_MINC_ENABLE;
	dmaUartRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	dmaUartRx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	dmaUartRx.Init.Mode                = DMA_NORMAL;
	dmaUartRx.Init.Priority            = DMA_PRIORITY_HIGH;
	dmaUartRx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

	HAL_DMA_Init(&dmaUartRx);

	/* Associate the initialized DMA handle to the the UART handle */
	__HAL_LINKDMA(huart, hdmarx, dmaUartRx);
	
	/*##-4- Configure the NVIC for DMA #########################################*/
	/* NVIC configuration for DMA transfer complete interrupt (USART6_TX) */
	//HAL_NVIC_SetPriority(board.serials[RECEIVER_UART].TXDMA_IRQn, 0, 1);
	//HAL_NVIC_EnableIRQ(board.serials[RECEIVER_UART].TXDMA_IRQn);

	/* NVIC configuration for DMA transfer complete interrupt (USART6_RX) */
	//HAL_NVIC_SetPriority(board.serials[RECEIVER_UART].RXDMA_IRQn, 0, 0);
	//HAL_NVIC_EnableIRQ(board.serials[RECEIVER_UART].RXDMA_IRQn);

	/* NVIC for USART, to catch the TX complete */
	HAL_NVIC_SetPriority(USARTx_IRQn, 8, 8);
	HAL_NVIC_EnableIRQ(USARTx_IRQn);

    /* DMA interrupt init */
	//HAL_NVIC_SetPriority(board.serials[RECEIVER_UART].TXDMA_IRQn, 1, 0);
	//HAL_NVIC_EnableIRQ(board.serials[RECEIVER_UART].TXDMA_IRQn);
	//HAL_NVIC_SetPriority(board.serials[RECEIVER_UART].RXDMA_IRQn, 1, 0);
	//HAL_NVIC_EnableIRQ(board.serials[RECEIVER_UART].RXDMA_IRQn);
	
    __HAL_UART_FLUSH_DRREGISTER(huart);

	if (HAL_UART_Receive_DMA(huart, (uint8_t *)serialRxBuffer, 16) == HAL_OK)
	{
		
	}

    for (x=0;x<100;x++)
    {
    	//if (HAL_UART_Receive_DMA(huart, (uint8_t *)serialRxBuffer, 16) == HAL_OK)
    		break;
    }

    if (x==100)
    {
    	// SHOW SOME CRAZY ERRORS
    }
}

void BoardUsartInit () {

    bzero(serialRxBuffer, sizeof(serialRxBuffer));
    bzero(serialTxBuffer, sizeof(serialTxBuffer));

	lastRXPacket = InlineMillis();

	HAL_NVIC_DisableIRQ(USARTx_TX_DMA_IRQn);
	HAL_NVIC_DisableIRQ(USARTx_RX_DMA_IRQn);

    // read and write settings at slow speed
	UsartInit(115200, USARTx, &uartHandle);

}

extern uint32_t ignoreEcho;
extern uint32_t spekPhase;
//Interrupt callback routine
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	// ##-2- Put UART peripheral in reception process ###########################
	__HAL_UART_FLUSH_DRREGISTER(&uartHandle);
	
		ProcessSpektrumPacket();

		lastRXPacket = InlineMillis();

	if (!ignoreEcho)
	{
		if(HAL_UART_Receive_DMA(huart, (uint8_t *)serialRxBuffer, 16) != HAL_OK)
		{
			//ErrorHandler();
		}
	}
	uartHandle.Instance->SR;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{

	__HAL_UART_FLUSH_DRREGISTER(&uartHandle);
	if (HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)serialRxBuffer, 16) != HAL_OK)
	{
		
	}
	ignoreEcho = 0;

	uartHandle.Instance->SR;
}
	//Preston, look at this
	//http://electronics.stackexchange.com/questions/173025/stm32f0-uart-dma-interrupt-with-stm32cubemx-hal-1-2-1-problem
	*/

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file.
   */

void USARTx_RX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dmaUartRx);

}

void USARTx_TX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dmaUartTx);
	
}

uint32_t minDMA = 16;
uint32_t rxDMA;
uint32_t txDMA;
void USARTx_IRQHandler(void)
{

//	if(uartHandle.RxState != HAL_UART_STATE_BUSY_RX)
//	{
	while (__HAL_DMA_GET_COUNTER(&dmaUartTx) != 0)
	{
		minDMA = __HAL_DMA_GET_COUNTER(&dmaUartTx);
	}
	HAL_UART_IRQHandler(&uartHandle);
	HAL_DMA_IRQHandler(&dmaUartRx);
	HAL_DMA_IRQHandler(&dmaUartTx);
	
	// ##-2- Put UART peripheral in reception process ###########################
	__HAL_UART_FLUSH_DRREGISTER(&uartHandle);

	
		
	if (__HAL_USART_GET_IT_SOURCE(&uartHandle, USART_IT_IDLE))
	{
		HAL_UART_DMAStop(&uartHandle);	
		rxDMA = DMA1_Stream0->NDTR;
		txDMA = DMA1_Stream7->NDTR;
		if ((uint16_t)(USARTx_RX_DMA_STREAM->NDTR) == 0 && !ignoreEcho)
		{
			ProcessSpektrumPacket();
			lastRXPacket = InlineMillis();
#ifdef SPEKTRUM_TELEM
			if (!spekPhase)
			{
				ignoreEcho = 1;
				sendSpektrumTelem();
			}
#endif
		}
		else
		{
			ignoreEcho = 0;
		}

		if (HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)serialRxBuffer, 16) != HAL_OK)
		{
			minDMA = 0;
		}
	}

	//if ((uint16_t)(board.serials[RECEIVER_UART].RXDMAStream->NDTR) == 0)
	//{
	//	ProcessSpektrumPacket();
	//}
	//else
	//	ignoreEcho = 0;
		
		
		//lastRXPacket = InlineMillis();
		//if(HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)serialRxBuffer, 16) != HAL_OK)
		//{
			//ErrorHandler();
		//}

//	}
//	if(HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
//	{
//		ErrorHandler();
//	}
  /*
	if(uartHandle.RxState != HAL_UART_STATE_BUSY_RX)
	{

		// ##-2- Put UART peripheral in reception process ###########################
		if(HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
		{
			ErrorHandler();
		}
		for (unsigned char i=0;i<RF_BUFFER_SIZE;i++) {
			tInBuffer[i] = aRxBuffer[i];
		}
		tInBuffer[0] = 1;

		USBD_HID_SendReport (&hUsbDeviceFS, tInBuffer, HID_EPIN_SIZE);
	}
	*/
}
