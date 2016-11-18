#include "includes.h"

UART_HandleTypeDef uartHandle;
DMA_HandleTypeDef dmaUartRx;
DMA_HandleTypeDef dmaUartTx;

__IO ITStatus UartReady = RESET;

uint8_t catfish;
uint32_t lastRXPacket;

#define SBUS_FRAME_SIZE 25


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

	// TODO: make this configurable, eg SBUS 2 stopbis, even parity
	uartHandle.Init.BaudRate   = baudRate;
	uartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	uartHandle.Init.StopBits   = UART_STOPBITS_2;
	uartHandle.Init.Parity     = UART_PARITY_EVEN;
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
		
	 UsartDmaInit(huart);
	 __HAL_UART_FLUSH_DRREGISTER(huart);
     __HAL_UART_CLEAR_IDLEFLAG(&uartHandle);
      __HAL_UART_ENABLE_IT(&uartHandle, UART_IT_IDLE);
      if (HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)serialRxBuffer, SBUS_FRAME_SIZE) != HAL_OK) {
        // error
      	return;
      }

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
	dmaUartRx.Init.MemInc              = DMA_MINC_ENABLE;
	dmaUartRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	dmaUartRx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	dmaUartRx.Init.Mode                = DMA_NORMAL;
	dmaUartRx.Init.Priority            = DMA_PRIORITY_HIGH;
	dmaUartRx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

	HAL_DMA_Init(&dmaUartRx);

	/* Associate the initialized DMA handle to the the UART handle */
	__HAL_LINKDMA(huart, hdmarx, dmaUartRx);

//	if (HAL_UART_Receive_DMA(huart, (uint8_t *)serialRxBuffer, SBUS_FRAME_SIZE) == HAL_OK)
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

	if (HAL_UART_Receive_DMA(huart, (uint8_t *)serialRxBuffer, SBUS_FRAME_SIZE) == HAL_OK)
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
	UsartInit(100000, USARTx, &uartHandle);
	//UsartInit(115200, USARTx, &uartHandle);

}

extern uint32_t ignoreEcho;
extern uint32_t spekPhase;
//Interrupt callback routine

//void HAL_UART_RxIdleCallback(UART_HandleTypeDef *huart)
//{
//  __HAL_UART_DISABLE_IT(huart, UART_IT_IDLE);
//}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	// ##-2- Put UART peripheral in reception process ###########################
	//__HAL_UART_FLUSH_DRREGISTER(&uartHandle);

    if ( huart == &uartHandle )
    {
       // HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)serialRxBuffer, 2);
    	__HAL_UART_FLUSH_DRREGISTER(&uartHandle); // Clear the buffer to prevent overrun
    	HAL_UART_DMAStop(&uartHandle);
    	ProcessSbusPacket();
		USARTx_RX_DMA_STREAM->NDTR = 0;
		__HAL_UART_CLEAR_IDLEFLAG(&uartHandle);
		__HAL_UART_ENABLE_IT(&uartHandle, UART_IT_IDLE);
		if (HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)serialRxBuffer, SBUS_FRAME_SIZE) != HAL_OK) {
		  // error
			return;
		}
        return;
    }
    return;

	//if(huart->Instance == USART1)
	//{
		lastRXPacket = InlineMillis();
		ProcessSbusPacket();
		HAL_UART_Receive_DMA(huart, (uint8_t *)serialRxBuffer, SBUS_FRAME_SIZE);
	//}
	return;
		ProcessSbusPacket();

		lastRXPacket = InlineMillis();
		if(HAL_UART_Init(&uartHandle) != HAL_OK)
		{
			ErrorHandler();
		}
		if (HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)serialRxBuffer, SBUS_FRAME_SIZE) == HAL_OK)
		{
			__HAL_UART_FLUSH_DRREGISTER(&uartHandle);
		}
/*
	if (!ignoreEcho)
	{
		if(HAL_UART_Receive_DMA(&dmaUartRx, (uint8_t *)serialRxBuffer, SBUS_FRAME_SIZE) != HAL_OK)
		{
			ErrorHandler();
		}
	}
*/
	//uartHandle.Instance->SR;
}
/*
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
  //UART_DMA_RX_ENABLE(&dmaUartRx);
  //HAL_UART_Receive_DMA(&dmaUartRx, (uint8_t *)serialRxBuffer, SBUS_FRAME_SIZE);
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
//	if(uartHandle.RxState != HAL_UART_STATE_BUSY_RX)
//	{
	while (__HAL_DMA_GET_COUNTER(&dmaUartTx) != 0)
	{
	}
	HAL_UART_IRQHandler(&uartHandle);
	HAL_DMA_IRQHandler(&dmaUartRx);
	HAL_DMA_IRQHandler(&dmaUartTx);
	
	// ##-2- Put UART peripheral in reception process ###########################
	__HAL_UART_FLUSH_DRREGISTER(&uartHandle);

	/*
	if (__HAL_UART_GET_FLAG(&uartHandle, UART_FLAG_PE))
	{
		__HAL_UART_CLEAR_PEFLAG(&uartHandle);
	}

	if (__HAL_UART_GET_FLAG(&uartHandle, UART_FLAG_FE))
	{
		__HAL_UART_CLEAR_FEFLAG(&uartHandle);
	}

	if (__HAL_UART_GET_FLAG(&uartHandle, UART_FLAG_NE))
	{
		__HAL_UART_CLEAR_NEFLAG(&uartHandle);
	}

	if (__HAL_UART_GET_FLAG(&uartHandle, UART_FLAG_ORE))
	{
		__HAL_UART_CLEAR_OREFLAG(&uartHandle);
	}
	  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg USART_IT_IDLE: Idle line detection interrupt
  *            @arg USART_IT_ORE: OverRun Error interrupt
  *            @arg USART_IT_NE: Noise Error interrupt
  *            @arg USART_IT_FE: Framing Error interrupt
  *            @arg USART_IT_PE: Parity Error interrupt
  *            #define USART_IT_PE                     ((uint32_t)(USART_CR1_REG_INDEX << 28U | USART_CR1_PEIE))
#define USART_IT_TXE                    ((uint32_t)(USART_CR1_REG_INDEX << 28U | USART_CR1_TXEIE))
#define USART_IT_TC                     ((uint32_t)(USART_CR1_REG_INDEX << 28U | USART_CR1_TCIE))
#define USART_IT_RXNE                   ((uint32_t)(USART_CR1_REG_INDEX << 28U | USART_CR1_RXNEIE))
#define USART_IT_IDLE                   ((uint32_t)(USART_CR1_REG_INDEX << 28U | USART_CR1_IDLEIE))

#define USART_IT_LBD                    ((uint32_t)(USART_CR2_REG_INDEX << 28U | USART_CR2_LBDIE))

#define USART_IT_CTS                    ((uint32_t)(USART_CR3_REG_INDEX << 28U | USART_CR3_CTSIE))
#define USART_IT_ERR                    ((uint32_t)(USART_CR3_REG_INDEX << 28U | USART_CR3_EIE))
	*/
	volatile uint32_t fishy = __HAL_DMA_GET_COUNTER(&dmaUartRx);
	volatile uint32_t doggy1 = __HAL_USART_GET_IT_SOURCE(&uartHandle, USART_IT_TXE);
	volatile uint32_t doggy2 = __HAL_USART_GET_IT_SOURCE(&uartHandle, USART_IT_TC);
	volatile uint32_t doggy3 = __HAL_USART_GET_IT_SOURCE(&uartHandle, USART_IT_RXNE);
	volatile uint32_t doggy4 = __HAL_USART_GET_IT_SOURCE(&uartHandle, USART_IT_IDLE);
	volatile uint32_t doggy5 = __HAL_USART_GET_IT_SOURCE(&uartHandle, USART_IT_LBD);
	volatile uint32_t doggy6 = __HAL_USART_GET_IT_SOURCE(&uartHandle, USART_IT_CTS);
	volatile uint32_t doggy7 = __HAL_USART_GET_IT_SOURCE(&uartHandle, USART_IT_ERR);
	volatile uint32_t doggy8 = USARTx_RX_DMA_STREAM->NDTR;
/*
	if (__HAL_USART_GET_IT_SOURCE(&uartHandle, USART_IT_IDLE))
	    {
	while (__HAL_DMA_GET_COUNTER(&dmaUartRx) != 0)
	    {
	    }
	        HAL_UART_DMAStop(&uartHandle);
	        if ((uint16_t)(USARTx_RX_DMA_STREAM->NDTR) == 0)
	        {
	            // TODO: dispatch configurable callback
	            //ProcessSpektrumPacket();
	            ProcessSbusPacket();
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
	        HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)serialRxBuffer, SBUS_FRAME_SIZE);
	    }
*/
	if (__HAL_USART_GET_IT_SOURCE(&uartHandle, USART_IT_IDLE))
	{
		HAL_UART_DMAStop(&uartHandle);	
		if ((uint16_t)(USARTx_RX_DMA_STREAM->NDTR) == 0)
		{

			// TODO: dispatch configurable callback
			//ProcessSpektrumPacket();
			ProcessSbusPacket();
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

		HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)serialRxBuffer, SBUS_FRAME_SIZE);
	}
return;


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