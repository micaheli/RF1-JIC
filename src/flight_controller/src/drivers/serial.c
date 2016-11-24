#include "includes.h"

UART_HandleTypeDef uartHandle[6];
DMA_HandleTypeDef dmaUartRx[6];
DMA_HandleTypeDef dmaUartTx[6];

__IO ITStatus UartReady = RESET;

uint8_t dmaRxBuffer = '\000';
uint32_t dmaIndex[MAX_USARTS] = {0,0,0,0,0,0}; //todo: change assumption that we have 5 usarts

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
//unsigned char serialTxBuffer[TXBUFFERSIZE];

/* Buffer used for reception */
//unsigned char serialRxBuffer[RXBUFFERSIZE];


void UsartInit(uint32_t serialNumber) {

	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-2- Configure peripheral GPIO ##########################################*/
	HAL_GPIO_DeInit(ports[board.serials[serialNumber].TXPort], board.serials[serialNumber].TXPin);
	HAL_GPIO_DeInit(ports[board.serials[serialNumber].RXPort], board.serials[serialNumber].RXPin);

	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = board.serials[serialNumber].TXPin;
	GPIO_InitStruct.Mode      = board.serials[serialNumber].PinMode;
	GPIO_InitStruct.Pull      = board.serials[serialNumber].Pull;
	GPIO_InitStruct.Speed     = board.serials[serialNumber].Speed;
	GPIO_InitStruct.Alternate = board.serials[serialNumber].TXAlternate;

	HAL_GPIO_Init(ports[board.serials[serialNumber].TXPort], &GPIO_InitStruct);

	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = board.serials[serialNumber].RXPin;
	GPIO_InitStruct.Alternate = board.serials[serialNumber].RXAlternate;

	HAL_GPIO_Init(ports[board.serials[serialNumber].RXPort], &GPIO_InitStruct);

	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	uartHandles[board.serials[serialNumber].usartHandle].Instance = usarts[board.serials[serialNumber].SerialInstance].port;

	uartHandles[board.serials[serialNumber].usartHandle].Init.BaudRate     = board.serials[serialNumber].BaudRate;
	uartHandles[board.serials[serialNumber].usartHandle].Init.WordLength   = board.serials[serialNumber].WordLength;
	uartHandles[board.serials[serialNumber].usartHandle].Init.StopBits     = board.serials[serialNumber].StopBits;
	uartHandles[board.serials[serialNumber].usartHandle].Init.Parity       = board.serials[serialNumber].Parity;
	uartHandles[board.serials[serialNumber].usartHandle].Init.HwFlowCtl    = board.serials[serialNumber].HwFlowCtl;
	uartHandles[board.serials[serialNumber].usartHandle].Init.Mode         = board.serials[serialNumber].Mode;
	//uartHandle.Init.OverSampling = UART_OVERSAMPLING_16;


	//Config uart as  half duplex if TX and RX pins are the same
	if (board.serials[serialNumber].TXPin == board.serials[serialNumber].RXPin && ports[board.serials[serialNumber].TXPort] == ports[board.serials[serialNumber].RXPort])
	{
		if (HAL_HalfDuplex_Init(&uartHandles[board.serials[serialNumber].usartHandle]) != HAL_OK)
		{
			//ErrorHandler();
		}
	}
	else
	{
		if(HAL_UART_Init(&uartHandles[board.serials[serialNumber].usartHandle]) != HAL_OK)
		{
			//ErrorHandler();
		}
	}

	UsartDmaInit(serialNumber);
	__HAL_UART_FLUSH_DRREGISTER(&uartHandles[board.serials[serialNumber].usartHandle]);
	HAL_UART_Receive_DMA(&uartHandles[board.serials[serialNumber].usartHandle], &dmaRxBuffer, 1);

}

void UsartDeinit(uint32_t serialNumber) {


	if (board.serials[serialNumber].serialTxBuffer)
		bzero(serialTxBuffer[board.serials[serialNumber].serialTxBuffer-1], sizeof(serialTxBuffer[board.serials[serialNumber].serialTxBuffer-1]));

	if (board.serials[serialNumber].serialRxBuffer)
		bzero(serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1], sizeof(serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1]));

	HAL_UART_DeInit(&uartHandles[board.serials[serialNumber].usartHandle]);

	/*##-1- Reset peripherals ##################################################*/

	switch (board.serials[serialNumber].SerialInstance) {
		case ENUM_USART1:
					__USART1_FORCE_RESET();
					__USART1_RELEASE_RESET();
					break;
		case ENUM_USART2:
					__USART2_FORCE_RESET();
					__USART2_RELEASE_RESET();
					break;
		case ENUM_USART3:
					__USART3_FORCE_RESET();
					__USART3_RELEASE_RESET();
					break;
		case ENUM_USART4:
					__UART4_FORCE_RESET();
					__UART4_RELEASE_RESET();
					break;
		case ENUM_USART5:
					__UART5_FORCE_RESET();
					__UART5_RELEASE_RESET();
					break;
		case ENUM_USART6:
					__USART6_FORCE_RESET();
					__USART6_RELEASE_RESET();
					break;
	}

	/*##-2- Disable peripherals and GPIO Clocks #################################*/
	HAL_GPIO_DeInit(ports[board.serials[serialNumber].TXPort], board.serials[serialNumber].TXPin);
	HAL_GPIO_DeInit(ports[board.serials[serialNumber].RXPort], board.serials[serialNumber].RXPin);

	UART_HandleTypeDef *huart = &uartHandles[board.serials[serialNumber].usartHandle];
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
	if (board.dmas[board.serials[serialNumber].TXDma].enabled)
		HAL_NVIC_DisableIRQ(board.dmas[board.serials[serialNumber].TXDma].dmaIRQn);

	if (board.dmas[board.serials[serialNumber].RXDma].enabled)
		HAL_NVIC_DisableIRQ(board.dmas[board.serials[serialNumber].RXDma].dmaIRQn);

}

void UsartDmaInit(uint32_t serialNumber)
{

	/*##-3- Configure the DMA ##################################################*/
	/* Configure the DMA handler for Transmission process */
	if (board.dmas[board.serials[serialNumber].TXDma].enabled) {
		dmaUartTx[serialNumber].Instance                 = dmaStream[board.dmas[board.serials[serialNumber].TXDma].dmaStream];
		dmaUartTx[serialNumber].Init.Channel             = board.dmas[board.serials[serialNumber].TXDma].dmaChannel;
		dmaUartTx[serialNumber].Init.Direction           = board.dmas[board.serials[serialNumber].TXDma].dmaDirection;
		dmaUartTx[serialNumber].Init.PeriphInc           = board.dmas[board.serials[serialNumber].TXDma].dmaPeriphInc;
		dmaUartTx[serialNumber].Init.MemInc              = board.dmas[board.serials[serialNumber].TXDma].dmaMemInc;
		dmaUartTx[serialNumber].Init.PeriphDataAlignment = board.dmas[board.serials[serialNumber].TXDma].dmaPeriphAlignment;
		dmaUartTx[serialNumber].Init.MemDataAlignment    = board.dmas[board.serials[serialNumber].TXDma].dmaMemAlignment;
		dmaUartTx[serialNumber].Init.Mode                = board.dmas[board.serials[serialNumber].TXDma].dmaMode;
		dmaUartTx[serialNumber].Init.Priority            = board.dmas[board.serials[serialNumber].TXDma].dmaPriority;
		dmaUartTx[serialNumber].Init.FIFOMode            = board.dmas[board.serials[serialNumber].TXDma].fifoMode;

		HAL_DMA_Init(&dmaUartTx[serialNumber]);

		/* Associate the initialized DMA handle to the UART handle */
		__HAL_LINKDMA(&uartHandles[board.serials[serialNumber].usartHandle], hdmatx, dmaUartTx[serialNumber]);

	    /* DMA interrupt init */
		HAL_NVIC_SetPriority(board.dmas[board.serials[serialNumber].TXDma].dmaIRQn, 1, 1);
		HAL_NVIC_EnableIRQ(board.dmas[board.serials[serialNumber].TXDma].dmaIRQn);
	}

	/* Configure the DMA handler for reception process */
	if (board.dmas[board.serials[serialNumber].RXDma].enabled) {

		dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle].Instance                 = dmaStream[board.dmas[board.serials[serialNumber].RXDma].dmaStream];
		dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle].Init.Channel             = board.dmas[board.serials[serialNumber].RXDma].dmaChannel;
		dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle].Init.Direction           = board.dmas[board.serials[serialNumber].RXDma].dmaDirection;
		dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle].Init.PeriphInc           = board.dmas[board.serials[serialNumber].RXDma].dmaPeriphInc;
		dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle].Init.MemInc              = board.dmas[board.serials[serialNumber].RXDma].dmaMemInc;
		dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle].Init.PeriphDataAlignment = board.dmas[board.serials[serialNumber].RXDma].dmaPeriphAlignment;
		dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle].Init.MemDataAlignment    = board.dmas[board.serials[serialNumber].RXDma].dmaMemAlignment;
		dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle].Init.Mode                = board.dmas[board.serials[serialNumber].RXDma].dmaMode;
		dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle].Init.Priority            = board.dmas[board.serials[serialNumber].RXDma].dmaPriority;
		dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle].Init.FIFOMode            = board.dmas[board.serials[serialNumber].RXDma].fifoMode;

		HAL_DMA_Init(&dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle]);

		/* Associate the initialized DMA handle to the the UART handle */
		__HAL_LINKDMA(&uartHandles[board.serials[serialNumber].usartHandle], hdmarx, dmaHandles[board.dmas[board.serials[serialNumber].RXDma].dmaHandle]);

	    /* DMA interrupt init */
		HAL_NVIC_SetPriority(board.dmas[board.serials[serialNumber].RXDma].dmaIRQn, 1, 1);
		HAL_NVIC_EnableIRQ(board.dmas[board.serials[serialNumber].RXDma].dmaIRQn);
	}

}

void BoardUsartInit () {



	lastRXPacket = InlineMillis();

	// read and write settings at slow speed
	// starting serial ENUM_USART1 which is serial 0
	for (uint32_t serialNumber = 0; serialNumber<MAX_USARTS;serialNumber++) {
		if (board.serials[serialNumber].enabled) {
			UsartDeinit(serialNumber); //deinits serial and associated pins and DMAs
			UsartInit(serialNumber); //inits serial and associated pins and DMAs
		}
	}

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	//works for all serials, we check huart against each serial handle to decide which one has interrupted then deal with it.
    volatile uint32_t timeSinceLastPacket[MAX_USARTS] = {0,0,0,0,0,0}; //todo: change assumption that we have 5 usarts
    uint32_t currentTime;
    static uint32_t timeOfLastPacket[MAX_USARTS] = {0,0,0,0,0,0}; //todo: change assumption that we have 5 usarts

    currentTime = InlineMillis();

	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++) {
		if (huart == &uartHandles[board.serials[serialNumber].usartHandle]) {
			//todo: How do we handle multiple RXs with this?
			timeSinceLastPacket[serialNumber] = (currentTime - timeOfLastPacket[serialNumber]); //todo: How do we handle multiple RXs with this?
			timeOfLastPacket[serialNumber]    = currentTime; //todo: How do we handle multiple RXs with this?

			if (timeSinceLastPacket[serialNumber] > 3) {
				if (dmaIndex[serialNumber] < board.serials[serialNumber].FrameSize) {
					__HAL_UART_FLUSH_DRREGISTER(&uartHandles[board.serials[serialNumber].usartHandle]); // Clear the buffer to prevent overrun
				}
				dmaIndex[serialNumber] = 0;
			}

			serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1][dmaIndex[serialNumber]++] = dmaRxBuffer; // Add that character to the string


			if (dmaIndex[serialNumber] >= board.serials[serialNumber].FrameSize) // User typing too much, we can't have commands that big
			{
				dmaIndex[serialNumber] = 0;
				if (board.serials[serialNumber].Protocol == USING_SPEKTRUM)
					ProcessSpektrumPacket(serialNumber);
				else if (board.serials[serialNumber].Protocol == USING_SBUS)
					ProcessSbusPacket(serialNumber);
			}
			break;
		}
	}

}
