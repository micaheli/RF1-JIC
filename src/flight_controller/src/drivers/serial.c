#include "includes.h"

UART_HandleTypeDef uartHandle;
DMA_HandleTypeDef dmaUartRx;
DMA_HandleTypeDef dmaUartTx;

__IO ITStatus UartReady = RESET;

#define USING_SPEKTRUM 0
#define USING_SBUS     1
uint8_t dmaRxBuffer = '\000';
uint32_t dmaIndex = 0;

uint32_t lastRXPacket;


#if defined(USE_SBUS_SERIAL)
  //sbus
  #define FRAME_SIZE 25
  uint32_t currentProtocol = USING_SBUS;
#else
  //spektrum
  #define FRAME_SIZE 16
  uint32_t currentProtocol = USING_SPEKTRUM;
#endif


/* Buffer used for transmission */
unsigned char serialTxBuffer[TXBUFFERSIZE];

/* Buffer used for reception */
unsigned char serialRxBuffer[RXBUFFERSIZE];


void UsartInit(unsigned int baudRate, USART_TypeDef* Usart, UART_HandleTypeDef *huart) {

	GPIO_InitTypeDef  GPIO_InitStruct;

	(void)(baudRate);
	/*##-2- Configure peripheral GPIO ##########################################*/
	HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
	HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

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
	uartHandle.Instance        = Usart;

	uartHandle.Init.BaudRate     = USARTx_BAUDRATE;
	uartHandle.Init.WordLength   = USARTx_WORDLENGTH;
	uartHandle.Init.StopBits     = USARTx_STOPBITS;
	uartHandle.Init.Parity       = USARTx_PARITY;
	uartHandle.Init.HwFlowCtl    = USARTx_HWFLOWCTRL;
	uartHandle.Init.Mode         = USARTx_MODE;
	//uartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
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

	UsartDmaInit(huart);
	__HAL_UART_FLUSH_DRREGISTER(&uartHandle);
	HAL_UART_Receive_DMA(&uartHandle, &dmaRxBuffer, 1);
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
	dmaUartRx.Init.Channel             = USARTx_RX_DMA_CHANNEL;
	dmaUartRx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	dmaUartRx.Init.PeriphInc           = DMA_PINC_DISABLE;
	dmaUartRx.Init.MemInc              = DMA_MINC_DISABLE;
	dmaUartRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	dmaUartRx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	dmaUartRx.Init.Mode                = DMA_CIRCULAR;
	dmaUartRx.Init.Priority            = DMA_PRIORITY_LOW;
	dmaUartRx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

	HAL_DMA_Init(&dmaUartRx);

	/* Associate the initialized DMA handle to the the UART handle */
	__HAL_LINKDMA(huart, hdmarx, dmaUartRx);

//	if (HAL_UART_Receive_DMA(huart, (uint8_t *)serialRxBuffer, FRAME_SIZE) == HAL_OK)
//	{
//		__HAL_UART_FLUSH_DRREGISTER(huart);
//	}

    /* DMA interrupt init */
	HAL_NVIC_SetPriority(USARTx_TX_DMA_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(USARTx_TX_DMA_IRQn);
	HAL_NVIC_SetPriority(USARTx_RX_DMA_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(USARTx_RX_DMA_IRQn);

	return;
	//HAL_USART_Receive_DMA(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size);

	/*##-4- Configure the NVIC for DMA #########################################*/
	/* NVIC configuration for DMA transfer complete interrupt (USART6_TX) */
	//HAL_NVIC_SetPriority(board.serials[RECEIVER_UART].TXDMA_IRQn, 0, 1);
	//HAL_NVIC_EnableIRQ(board.serials[RECEIVER_UART].TXDMA_IRQn);

	/* NVIC configuration for DMA transfer complete interrupt (USART6_RX) */
	//HAL_NVIC_SetPriority(board.serials[RECEIVER_UART].RXDMA_IRQn, 0, 0);
	//HAL_NVIC_EnableIRQ(board.serials[RECEIVER_UART].RXDMA_IRQn);

	/* NVIC for USART, to catch the TX complete */
//	HAL_NVIC_SetPriority(USARTx_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(USARTx_IRQn);



	__HAL_UART_FLUSH_DRREGISTER(huart);

	if (HAL_UART_Receive_DMA(huart, (uint8_t *)serialRxBuffer, FRAME_SIZE) == HAL_OK)
	{

	}
/*
    for (x=0;x<100;x++)
    {
    	//if (HAL_UART_Receive_DMA(huart, (uint8_t *)serialRxBuffer, 16) == HAL_OK)
    		break;
    }

    if (x==100)
    {
    	// SHOW SOME CRAZY ERRORS
    }
*/
}

void BoardUsartInit () {

    bzero(serialRxBuffer, sizeof(serialRxBuffer));
    bzero(serialTxBuffer, sizeof(serialTxBuffer));

	lastRXPacket = InlineMillis();

	HAL_NVIC_DisableIRQ(USARTx_TX_DMA_IRQn);
	HAL_NVIC_DisableIRQ(USARTx_RX_DMA_IRQn);

	// read and write settings at slow speed
	UsartInit(USARTx_BAUDRATE, USARTx, &uartHandle);

}

//Interrupt callback routine
void HAL_USART_RxCpltCallback(USART_HandleTypeDef *huart)
{
	volatile uint32_t cat =1;
	return;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    //__HAL_UART_FLUSH_DRREGISTER(&uartHandle); // Clear the buffer to prevent overrun

    volatile uint32_t timeSinceLastPacket = 0;
    uint32_t currentTime = 0;
    static uint32_t timeOfLastPacket = 0;

    currentTime = InlineMillis();
    timeSinceLastPacket = (currentTime - timeOfLastPacket);
    timeOfLastPacket = currentTime;

    if (timeSinceLastPacket > 3) {
    	if (dmaIndex < FRAME_SIZE) {
    		__HAL_UART_FLUSH_DRREGISTER(&uartHandle); // Clear the buffer to prevent overrun
    	}
    	dmaIndex = 0;
    }

    serialRxBuffer[dmaIndex++] = dmaRxBuffer; // Add that character to the string


	if (dmaIndex >= FRAME_SIZE) // User typing too much, we can't have commands that big
	{
		dmaIndex = 0;
    	if (currentProtocol == USING_SPEKTRUM)
    		ProcessSpektrumPacket();
    	else if (currentProtocol == USING_SBUS)
    		ProcessSbusPacket();
		//bzero(serialRxBuffer, sizeof(serialRxBuffer));
	}

}

void USARTx_RX_DMA_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(USARTx_RX_DMA_IRQn);
    HAL_DMA_IRQHandler(&dmaUartRx);
}

void USARTx_TX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dmaUartTx);

}

uint32_t rxDMA;
uint32_t txDMA;
void USARTx_IRQHandler(void)
{
	return;
}
