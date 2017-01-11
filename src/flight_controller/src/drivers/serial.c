#include "includes.h"

UART_HandleTypeDef uartHandle[6];
DMA_HandleTypeDef dmaUartRx[6];
DMA_HandleTypeDef dmaUartTx[6];

__IO ITStatus UartReady = RESET;

uint8_t dmaRxBuffer = '\000';
uint32_t dmaIndex[MAX_USARTS] = {0,0,0,0,0,0}; //todo: change assumption that we have 5 usarts

uint32_t lastRXPacket;



/* Buffer used for transmission */
//unsigned char serialTxBuffer[TXBUFFERSIZE];

/* Buffer used for reception */
//unsigned char serialRxBuffer[RXBUFFERSIZE];


void UsartInit(uint32_t serialNumber) {

	GPIO_InitTypeDef  GPIO_InitStruct;

	uint32_t txPin;
	uint32_t rxPin;
	GPIO_TypeDef *txPort;
	GPIO_TypeDef *rxPort;

	switch (board.serials[serialNumber].Protocol) {
		case USING_SPEKTRUM_ONE_WAY:
			board.serials[serialNumber].FrameSize  = 16;
			board.serials[serialNumber].BaudRate   = 115200;
			board.serials[serialNumber].WordLength = UART_WORDLENGTH_8B;
			board.serials[serialNumber].StopBits   = UART_STOPBITS_1;
			board.serials[serialNumber].Parity     = UART_PARITY_NONE;
			board.serials[serialNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
			board.serials[serialNumber].Mode       = UART_MODE_TX_RX;
			txPin  = board.serials[serialNumber].TXPin;
			rxPin  = board.serials[serialNumber].RXPin;
			txPort = ports[board.serials[serialNumber].TXPort];
			rxPort = ports[board.serials[serialNumber].RXPort];
			break;
		case USING_SPEKTRUM_TWO_WAY:
			board.serials[serialNumber].FrameSize  = 16;
			board.serials[serialNumber].BaudRate   = 115200;
			board.serials[serialNumber].WordLength = UART_WORDLENGTH_8B;
			board.serials[serialNumber].StopBits   = UART_STOPBITS_1;
			board.serials[serialNumber].Parity     = UART_PARITY_NONE;
			board.serials[serialNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
			board.serials[serialNumber].Mode       = UART_MODE_TX_RX;
			txPin  = board.serials[serialNumber].TXPin;
			rxPin  = board.serials[serialNumber].TXPin;
			txPort = ports[board.serials[serialNumber].TXPort];
			rxPort = ports[board.serials[serialNumber].TXPort];
			break;
		case USING_SBUS:
		case USING_SBUS_SPORT:
			board.serials[serialNumber].FrameSize  = 25;
			board.serials[serialNumber].BaudRate   = 100000;
			board.serials[serialNumber].WordLength = UART_WORDLENGTH_8B;
			board.serials[serialNumber].StopBits   = UART_STOPBITS_2;
			board.serials[serialNumber].Parity     = UART_PARITY_EVEN;
			board.serials[serialNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
			board.serials[serialNumber].Mode       = UART_MODE_RX; //sbus only has input, TX is handled via soft serial
			txPin  = board.serials[serialNumber].TXPin;
			rxPin  = board.serials[serialNumber].RXPin;
			txPort = ports[board.serials[serialNumber].TXPort];
			rxPort = ports[board.serials[serialNumber].RXPort];
			break;
		case USING_SUMD:
			board.serials[serialNumber].FrameSize  = 21; //variable packet size. Will be set based on data
			board.serials[serialNumber].BaudRate   = 115200;
			board.serials[serialNumber].WordLength = UART_WORDLENGTH_8B;
			board.serials[serialNumber].StopBits   = UART_STOPBITS_1;
			board.serials[serialNumber].Parity     = UART_PARITY_NONE;
			board.serials[serialNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
			board.serials[serialNumber].Mode       = UART_MODE_TX_RX;
			txPin  = board.serials[serialNumber].TXPin;
			rxPin  = board.serials[serialNumber].RXPin;
			txPort = ports[board.serials[serialNumber].TXPort];
			rxPort = ports[board.serials[serialNumber].RXPort];
			break;
		case USING_SUMD_TWO_WAY:
			board.serials[serialNumber].FrameSize  = 21; //variable packet size. Will be set based on data
			board.serials[serialNumber].BaudRate   = 115200;
			board.serials[serialNumber].WordLength = UART_WORDLENGTH_8B;
			board.serials[serialNumber].StopBits   = UART_STOPBITS_1;
			board.serials[serialNumber].Parity     = UART_PARITY_NONE;
			board.serials[serialNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
			board.serials[serialNumber].Mode       = UART_MODE_TX_RX;
			txPin  = board.serials[serialNumber].TXPin;
			rxPin  = board.serials[serialNumber].TXPin;
			txPort = ports[board.serials[serialNumber].TXPort];
			rxPort = ports[board.serials[serialNumber].TXPort];
			break;
		case USING_MANUAL:
		default:
			txPin  = board.serials[serialNumber].TXPin;
			rxPin  = board.serials[serialNumber].RXPin;
			txPort = ports[board.serials[serialNumber].TXPort];
			rxPort = ports[board.serials[serialNumber].RXPort];
			break;
	}


	/*##-2- Configure peripheral GPIO ##########################################*/
	if (board.serials[serialNumber].Mode != UART_MODE_RX)
		HAL_GPIO_DeInit(txPort, txPin); //no need for TX pin is serial is in RX only mode.

	HAL_GPIO_DeInit(rxPort, rxPin);

	/* UART TX GPIO pin configuration  */
	if (board.serials[serialNumber].Mode != UART_MODE_RX)
		GPIO_InitStruct.Pin       = txPin; //no need for TX pin is serial is in RX only mode.

	GPIO_InitStruct.Mode      = board.serials[serialNumber].PinMode;
	GPIO_InitStruct.Pull      = board.serials[serialNumber].Pull;
	GPIO_InitStruct.Speed     = board.serials[serialNumber].Speed;
	GPIO_InitStruct.Alternate = board.serials[serialNumber].TXAlternate;

	if (board.serials[serialNumber].Mode != UART_MODE_RX)
		HAL_GPIO_Init(txPort, &GPIO_InitStruct);

	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = rxPin;
	GPIO_InitStruct.Alternate = board.serials[serialNumber].RXAlternate;

	HAL_GPIO_Init(rxPort, &GPIO_InitStruct);


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
	if ( (txPin == rxPin) && (txPort == rxPort) )
	{
		if (HAL_HalfDuplex_Init(&uartHandles[board.serials[serialNumber].usartHandle]) != HAL_OK)
		{
			ErrorHandler(SERIAL_HALF_DUPLEX_INIT_FAILURE);
		}
	}
	else
	{
		if(HAL_UART_Init(&uartHandles[board.serials[serialNumber].usartHandle]) != HAL_OK)
		{
			ErrorHandler(SERIAL_INIT_FAILURE);
		}
	}

	UsartDmaInit(serialNumber);
	__HAL_UART_FLUSH_DRREGISTER(&uartHandles[board.serials[serialNumber].usartHandle]);
	HAL_UART_Receive_DMA(&uartHandles[board.serials[serialNumber].usartHandle], &dmaRxBuffer, 1);

}

void UsartDeInit(uint32_t serialNumber) {


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
		HAL_NVIC_DisableIRQ(board.dmasActive[board.serials[serialNumber].RXDma].dmaIRQn);
		board.dmasActive[board.serials[serialNumber].RXDma].enabled = 0;
	}
	/* De-Initialize the DMA channel associated to transmission process */
	if (huart->hdmatx != 0)
	{
		HAL_DMA_DeInit(huart->hdmatx);
		HAL_NVIC_DisableIRQ(board.dmasActive[board.serials[serialNumber].TXDma].dmaIRQn);
		board.dmasActive[board.serials[serialNumber].TXDma].enabled = 0;
	}

	/*##-4- Disable the NVIC for Active DMA ###########################################*/
	//if (board.dmas[board.serials[serialNumber].TXDma].enabled) //only mess with the Active DMA is the current serial device was using it or needs it
	//	HAL_NVIC_DisableIRQ(board.dmasActive[board.serials[serialNumber].TXDma].dmaIRQn);

//	if (board.dmas[board.serials[serialNumber].RXDma].enabled) //only mess with the Active DMA is the current serial device was using it or needs it
//		HAL_NVIC_DisableIRQ(board.dmasActive[board.serials[serialNumber].RXDma].dmaIRQn);

}

void UsartDmaInit(uint32_t serialNumber)
{

	/*##-3- Configure the DMA ##################################################*/
	/* Configure the DMA handler for Transmission process */
	if (board.dmasSerial[board.serials[serialNumber].TXDma].enabled) { //only mess with the DMA is the current serial device needs it

		dmaUartTx[serialNumber].Instance                 = dmaStream[board.dmasActive[board.serials[serialNumber].TXDma].dmaStream];
		dmaUartTx[serialNumber].Init.Channel             = board.dmasActive[board.serials[serialNumber].TXDma].dmaChannel;
		dmaUartTx[serialNumber].Init.Direction           = board.dmasActive[board.serials[serialNumber].TXDma].dmaDirection;
		dmaUartTx[serialNumber].Init.PeriphInc           = board.dmasActive[board.serials[serialNumber].TXDma].dmaPeriphInc;
		dmaUartTx[serialNumber].Init.MemInc              = board.dmasActive[board.serials[serialNumber].TXDma].dmaMemInc;
		dmaUartTx[serialNumber].Init.PeriphDataAlignment = board.dmasActive[board.serials[serialNumber].TXDma].dmaPeriphAlignment;
		dmaUartTx[serialNumber].Init.MemDataAlignment    = board.dmasActive[board.serials[serialNumber].TXDma].dmaMemAlignment;
		dmaUartTx[serialNumber].Init.Mode                = board.dmasActive[board.serials[serialNumber].TXDma].dmaMode;
		dmaUartTx[serialNumber].Init.Priority            = board.dmasActive[board.serials[serialNumber].TXDma].dmaPriority;
		dmaUartTx[serialNumber].Init.FIFOMode            = board.dmasActive[board.serials[serialNumber].TXDma].fifoMode;

		HAL_DMA_Init(&dmaUartTx[serialNumber]);

		/* Associate the initialized DMA handle to the UART handle */
		__HAL_LINKDMA(&uartHandles[board.serials[serialNumber].usartHandle], hdmatx, dmaUartTx[serialNumber]);

	    /* DMA interrupt init */
		HAL_NVIC_SetPriority(board.dmasActive[board.serials[serialNumber].TXDma].dmaIRQn, 1, 1);
		HAL_NVIC_EnableIRQ(board.dmasActive[board.serials[serialNumber].TXDma].dmaIRQn);
		board.dmasActive[board.serials[serialNumber].TXDma].enabled = 1;
	}

	/* Configure the DMA handler for reception process */
	if (board.dmasSerial[board.serials[serialNumber].RXDma].enabled) { //only mess with the DMA is the current serial device needs it

		dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle].Instance                 = dmaStream[board.dmasActive[board.serials[serialNumber].RXDma].dmaStream];
		dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle].Init.Channel             = board.dmasActive[board.serials[serialNumber].RXDma].dmaChannel;
		dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle].Init.Direction           = board.dmasActive[board.serials[serialNumber].RXDma].dmaDirection;
		dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle].Init.PeriphInc           = board.dmasActive[board.serials[serialNumber].RXDma].dmaPeriphInc;
		dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle].Init.MemInc              = board.dmasActive[board.serials[serialNumber].RXDma].dmaMemInc;
		dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle].Init.PeriphDataAlignment = board.dmasActive[board.serials[serialNumber].RXDma].dmaPeriphAlignment;
		dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle].Init.MemDataAlignment    = board.dmasActive[board.serials[serialNumber].RXDma].dmaMemAlignment;
		dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle].Init.Mode                = board.dmasActive[board.serials[serialNumber].RXDma].dmaMode;
		dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle].Init.Priority            = board.dmasActive[board.serials[serialNumber].RXDma].dmaPriority;
		dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle].Init.FIFOMode            = board.dmasActive[board.serials[serialNumber].RXDma].fifoMode;

		HAL_DMA_Init(&dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle]);

		/* Associate the initialized DMA handle to the the UART handle */
		__HAL_LINKDMA(&uartHandles[board.serials[serialNumber].usartHandle], hdmarx, dmaHandles[board.dmasActive[board.serials[serialNumber].RXDma].dmaHandle]);

	    /* DMA interrupt init */
		HAL_NVIC_SetPriority(board.dmasActive[board.serials[serialNumber].RXDma].dmaIRQn, board.dmasActive[board.serials[serialNumber].RXDma].priority, 1);
		HAL_NVIC_EnableIRQ(board.dmasActive[board.serials[serialNumber].RXDma].dmaIRQn);
		board.dmasActive[board.serials[serialNumber].RXDma].enabled = 1;
	}

}

void InitBoardUsarts (void) {

	uint32_t usartOn;
	uint32_t usartOff;

	if (mainConfig.rcControlsConfig.rxUsart == ENUM_USART1) {
		usartOn  = ENUM_USART1;
		usartOff = ENUM_USART3;
	}
	else
	{
		usartOn  = ENUM_USART3;
		usartOff = ENUM_USART1;
	}

    //TODO: change this up, for ability to set usarts now on revolt
    if (mainConfig.rcControlsConfig.rxProtcol == USING_SPEKTRUM_ONE_WAY) {

    	board.serials[usartOff].enabled  = 0;
    	board.serials[usartOn].enabled   = 1;
    	board.serials[usartOn].Protocol  = USING_SPEKTRUM_ONE_WAY;
    	board.dmasSerial[board.serials[usartOn].TXDma].enabled  = 0;
		board.dmasSerial[board.serials[usartOn].RXDma].enabled  = 1;
		board.dmasSerial[board.serials[usartOff].TXDma].enabled = 0;
		board.dmasSerial[board.serials[usartOff].RXDma].enabled = 0;
    } else
    if (mainConfig.rcControlsConfig.rxProtcol == USING_SPEKTRUM_TWO_WAY) {
		board.serials[usartOff].enabled  = 0;
		board.serials[usartOn].enabled   = 1;
		board.serials[usartOn].Protocol  = USING_SPEKTRUM_TWO_WAY;
		board.dmasSerial[board.serials[usartOn].TXDma].enabled  = 1;
		board.dmasSerial[board.serials[usartOn].RXDma].enabled  = 1;
		board.dmasSerial[board.serials[usartOff].TXDma].enabled = 0;
		board.dmasSerial[board.serials[usartOff].RXDma].enabled = 0;
	} else
    if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS) {
		board.serials[usartOff].enabled  = 0;
		board.serials[usartOn].enabled   = 1;
		board.serials[usartOn].Protocol  = USING_SBUS;
		board.dmasSerial[board.serials[usartOn].TXDma].enabled  = 0;
		board.dmasSerial[board.serials[usartOn].RXDma].enabled  = 1;
		board.dmasSerial[board.serials[usartOff].TXDma].enabled = 0;
		board.dmasSerial[board.serials[usartOff].RXDma].enabled = 0;
	} else
	if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS_SPORT) {
		board.serials[usartOff].enabled  = 0;
		board.serials[usartOn].enabled   = 1;
		board.serials[usartOn].Protocol  = USING_SBUS_SPORT;
		board.dmasSerial[board.serials[usartOn].TXDma].enabled  = 0;
		board.dmasSerial[board.serials[usartOn].RXDma].enabled  = 1;
		board.dmasSerial[board.serials[usartOff].TXDma].enabled = 0;
		board.dmasSerial[board.serials[usartOff].RXDma].enabled = 0;
	} else
	if (mainConfig.rcControlsConfig.rxProtcol == USING_SUMD) {
		board.serials[usartOff].enabled = 0;
		board.serials[usartOn].enabled  = 1;
		board.serials[usartOn].Protocol = USING_SUMD;
		board.dmasSerial[board.serials[usartOn].TXDma].enabled  = 0;
		board.dmasSerial[board.serials[usartOn].RXDma].enabled  = 1;
		board.dmasSerial[board.serials[usartOff].TXDma].enabled = 0;
		board.dmasSerial[board.serials[usartOff].RXDma].enabled = 0;
	} else
	if (mainConfig.rcControlsConfig.rxProtcol == USING_SUMD_TWO_WAY) {
		board.serials[usartOff].enabled = 0;
		board.serials[usartOn].enabled  = 1;
		board.serials[usartOn].Protocol = USING_SUMD_TWO_WAY;
		board.dmasSerial[board.serials[usartOn].TXDma].enabled  = 1;
		board.dmasSerial[board.serials[usartOn].RXDma].enabled  = 1;
		board.dmasSerial[board.serials[usartOff].TXDma].enabled = 0;
		board.dmasSerial[board.serials[usartOff].RXDma].enabled = 0;
	}

	lastRXPacket = InlineMillis();

	// read and write settings at slow speed
	// starting serial ENUM_USART1 which is serial 0
	for (uint32_t serialNumber = 0; serialNumber<MAX_USARTS;serialNumber++)
	{
		if (board.serials[serialNumber].enabled)
		{

			if (board.dmasSerial[board.serials[serialNumber].TXDma].enabled) //only move the DMA into the Active DMA if the serial needs it
		    	memcpy( &board.dmasActive[board.serials[serialNumber].TXDma], &board.dmasSerial[board.serials[serialNumber].TXDma], sizeof(board_dma) ); //TODO: Add dmasUsart

		    if (board.dmasSerial[board.serials[serialNumber].RXDma].enabled) //only move the DMA into the Active DMA if the serial needs it
		    	memcpy( &board.dmasActive[board.serials[serialNumber].RXDma], &board.dmasSerial[board.serials[serialNumber].RXDma], sizeof(board_dma) );

			UsartDeInit(serialNumber); //deinits serial and associated pins and DMAs
			UsartInit(serialNumber); //inits serial and associated pins and DMAs
		}
	}

}

void DeInitBoardUsarts (void) {

	for (uint32_t serialNumber = 0; serialNumber<MAX_USARTS;serialNumber++)
	{
		if (board.serials[serialNumber].enabled)
		{
			UsartDeInit(serialNumber); //deinits serial and associated pins and DMAs
		}
	}

}

//extern char rf_custom_out_buffer[];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	//works for all serials, we check huart against each serial handle to decide which one has interrupted then deal with it.
    volatile uint32_t timeSinceLastPacket[MAX_USARTS] = {0,0,0,0,0,0}; //todo: change assumption that we have 5 usarts
    uint32_t currentTime;
    static uint32_t timeOfLastPacket[MAX_USARTS] = {0,0,0,0,0,0}; //todo: change assumption that we have 5 usarts

    currentTime = InlineMillis();

	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if (huart == &uartHandles[board.serials[serialNumber].usartHandle])
		{
			//todo: How do we handle multiple RXs with this?
			timeSinceLastPacket[serialNumber] = (currentTime - timeOfLastPacket[serialNumber]); //todo: How do we handle multiple RXs with this?
			timeOfLastPacket[serialNumber]    = currentTime; //todo: How do we handle multiple RXs with this?

			if ( ( (board.serials[serialNumber].Protocol == USING_SUMD) || (board.serials[serialNumber].Protocol == USING_SUMD_TWO_WAY)) && (dmaIndex[serialNumber] == 2) )
			{
				//Sumd packet 2 (third one) is number of channels.
				//total frame length is header (3) + number of channels * 2 (variable) + crc length (2).
				board.serials[serialNumber].FrameSize = CONSTRAIN( (5 + (dmaRxBuffer * 2) ), 9, 47); //sumd can be between 7 and 37 long
			}

			if (timeSinceLastPacket[serialNumber] > 3)
			{
				if (dmaIndex[serialNumber] < board.serials[serialNumber].FrameSize)
				{
					__HAL_UART_FLUSH_DRREGISTER(&uartHandles[board.serials[serialNumber].usartHandle]); // Clear the buffer to prevent overrun
				}
				dmaIndex[serialNumber] = 0;
			}

			serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1][dmaIndex[serialNumber]++] = dmaRxBuffer; // Add that character to the string

			if (dmaIndex[serialNumber] >= board.serials[serialNumber].FrameSize)
			{
				dmaIndex[serialNumber] = 0;
				if ((board.serials[serialNumber].Protocol == USING_SPEKTRUM_TWO_WAY) || (board.serials[serialNumber].Protocol == USING_SPEKTRUM_ONE_WAY))
					ProcessSpektrumPacket(serialNumber);
				else if ((board.serials[serialNumber].Protocol == USING_SBUS) || (board.serials[serialNumber].Protocol == USING_SBUS_SPORT))
					ProcessSbusPacket(serialNumber);
				else if ( (board.serials[serialNumber].Protocol == USING_SUMD) || (board.serials[serialNumber].Protocol == USING_SUMD_TWO_WAY) )
					ProcessSumdPacket(serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1], board.serials[serialNumber].FrameSize);
			}
			break;
		}
	}

}

void SerialTxCallback(void)
{
	//HAL_DMA_IRQHandler(&uartHandles[board.serials[2].usartHandle].hdmatx);
}
