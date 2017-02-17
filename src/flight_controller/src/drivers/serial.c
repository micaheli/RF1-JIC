#include "includes.h"

__IO ITStatus UartReady = RESET;

uint8_t dmaRxBuffer = '\000';
uint32_t dmaIndex[MAX_USARTS] = {0,0,0,0,0,0}; //todo: change assumption that we have 6 usarts
uint32_t dmaTxCallbackToUsartHandle[IRQH_FP_TOT] =  { 0 };


uint32_t lastRXPacket;

void UsartInit(uint32_t serialNumber)
{

	GPIO_InitTypeDef  GPIO_InitStruct;

	uint32_t txPin;
	uint32_t rxPin;
	GPIO_TypeDef *txPort;
	GPIO_TypeDef *rxPort;

	switch (board.serials[serialNumber].Protocol)
	{
		case USING_SPEK_R:
		case USING_DSM2_R:
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
		case USING_SPEK_T:
		case USING_DSM2_T:
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
		case USING_SBUS_R:
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
		case USING_SBUS_T:
			board.serials[serialNumber].FrameSize  = 25;
			board.serials[serialNumber].BaudRate   = 100000;
			board.serials[serialNumber].WordLength = UART_WORDLENGTH_8B;
			board.serials[serialNumber].StopBits   = UART_STOPBITS_2;
			board.serials[serialNumber].Parity     = UART_PARITY_EVEN;
			board.serials[serialNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
			board.serials[serialNumber].Mode       = UART_MODE_TX_RX;
			txPin  = board.serials[serialNumber].TXPin;
			rxPin  = board.serials[serialNumber].TXPin;
			txPort = ports[board.serials[serialNumber].TXPort];
			rxPort = ports[board.serials[serialNumber].TXPort];
			break;
		case USING_SUMD_R:
			board.serials[serialNumber].FrameSize  = 21; //variable packet size. Will be set based on data
			board.serials[serialNumber].BaudRate   = 115200;
			board.serials[serialNumber].WordLength = UART_WORDLENGTH_8B;
			board.serials[serialNumber].StopBits   = UART_STOPBITS_1;
			board.serials[serialNumber].Parity     = UART_PARITY_NONE;
			board.serials[serialNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
			board.serials[serialNumber].Mode       = UART_MODE_RX;
			txPin  = board.serials[serialNumber].TXPin;
			rxPin  = board.serials[serialNumber].RXPin;
			txPort = ports[board.serials[serialNumber].TXPort];
			rxPort = ports[board.serials[serialNumber].RXPort];
			break;
		case USING_SUMD_T:
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
		case USING_IBUS_T:
			board.serials[serialNumber].FrameSize  = 32;
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
		case USING_IBUS_R:
			board.serials[serialNumber].FrameSize  = 32;
			board.serials[serialNumber].BaudRate   = 115200;
			board.serials[serialNumber].WordLength = UART_WORDLENGTH_8B;
			board.serials[serialNumber].StopBits   = UART_STOPBITS_1;
			board.serials[serialNumber].Parity     = UART_PARITY_NONE;
			board.serials[serialNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
			board.serials[serialNumber].Mode       = UART_MODE_RX;
			txPin  = board.serials[serialNumber].TXPin;
			rxPin  = board.serials[serialNumber].RXPin;
			txPort = ports[board.serials[serialNumber].TXPort];
			rxPort = ports[board.serials[serialNumber].RXPort];
			break;
		case USING_MSP:
		case USING_SPORT:
		case USING_RFVTX:
		case USING_MANUAL:
		default:
			txPin  = board.serials[serialNumber].TXPin;
			rxPin  = board.serials[serialNumber].RXPin;
			txPort = ports[board.serials[serialNumber].TXPort];
			rxPort = ports[board.serials[serialNumber].RXPort];
			break;
	}

	if ( (serialNumber == 0) && (mainConfig.rcControlsConfig.rxInvertDirection) )
		PowerInveter(ENUM_PORTC, GPIO_PIN_0, mainConfig.rcControlsConfig.rxInvertDirection);

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

#ifdef USART_USED_ADVANCED
	//f0, f3, f7 usart advanced features.
	//setup inversion
	uartHandles[board.serials[serialNumber].usartHandle].AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (board.serials[usartNumber].serialTxInverted)
	{
		uartHandles[board.serials[serialNumber].usartHandle].AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_TXINVERT_INIT;
		uartHandles[board.serials[serialNumber].usartHandle].AdvancedInit.TxPinLevelInvert = UART_ADVFEATURE_TXINV_ENABLE;
	}
	if (board.serials[usartNumber].serialRxInverted)
	{
		uartHandles[board.serials[serialNumber].usartHandle].AdvancedInit.AdvFeatureInit = (uartHandles[board.serials[serialNumber].usartHandle].AdvancedInit.AdvFeatureInit | UART_ADVFEATURE_RXINVERT_INIT);
		uartHandles[board.serials[serialNumber].usartHandle].AdvancedInit.RxPinLevelInvert = UART_ADVFEATURE_RXINV_ENABLE;
	}
#endif

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

		dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle].Instance                = dmaStream[board.dmasActive[board.serials[serialNumber].TXDma].dmaStream];
		dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle].Init.Channel             = board.dmasActive[board.serials[serialNumber].TXDma].dmaChannel;
		dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle].Init.Direction           = board.dmasActive[board.serials[serialNumber].TXDma].dmaDirection;
		dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle].Init.PeriphInc           = board.dmasActive[board.serials[serialNumber].TXDma].dmaPeriphInc;
		dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle].Init.MemInc              = board.dmasActive[board.serials[serialNumber].TXDma].dmaMemInc;
		dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle].Init.PeriphDataAlignment = board.dmasActive[board.serials[serialNumber].TXDma].dmaPeriphAlignment;
		dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle].Init.MemDataAlignment    = board.dmasActive[board.serials[serialNumber].TXDma].dmaMemAlignment;
		dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle].Init.Mode                = board.dmasActive[board.serials[serialNumber].TXDma].dmaMode;
		dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle].Init.Priority            = board.dmasActive[board.serials[serialNumber].TXDma].dmaPriority;
		dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle].Init.FIFOMode            = board.dmasActive[board.serials[serialNumber].TXDma].fifoMode;

		HAL_DMA_Init(&dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle]);

		/* Associate the initialized DMA handle to the UART handle */
		__HAL_LINKDMA(&uartHandles[board.serials[serialNumber].usartHandle], hdmatx, dmaHandles[board.dmasActive[board.serials[serialNumber].TXDma].dmaHandle]);

	    /* DMA interrupt init */
		HAL_NVIC_SetPriority(board.dmasActive[board.serials[serialNumber].TXDma].dmaIRQn, 1, 1);
		HAL_NVIC_EnableIRQ(board.dmasActive[board.serials[serialNumber].TXDma].dmaIRQn);
		board.dmasActive[board.serials[serialNumber].TXDma].enabled = 1;

		//set this so we know which DMA callback is assigned to which usart handle when the callback function is callse
		dmaTxCallbackToUsartHandle[GetDmaCallbackFromDmaStream(board.dmasActive[board.serials[serialNumber].TXDma].dmaStream)] = board.serials[serialNumber].usartHandle;
		//set the callback function
		callbackFunctionArray[GetDmaCallbackFromDmaStream(board.dmasActive[board.serials[serialNumber].TXDma].dmaStream)] = SerialTxCallback;

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

void InitBoardUsarts (void)
{

	//turn off all usarts.
	for (uint32_t serialNumber = 0; serialNumber<MAX_USARTS;serialNumber++)
	{
		board.serials[serialNumber].enabled  = 0;
		board.dmasSerial[board.serials[serialNumber].TXDma].enabled = 0;
		board.dmasSerial[board.serials[serialNumber].RXDma].enabled = 0;
	}

	//set RX usarts based on rx setup
	if ((mainConfig.rcControlsConfig.rxProtcol == USING_SPEK_R) || (mainConfig.rcControlsConfig.rxProtcol == USING_DSM2_R))
    {
    	board.serials[mainConfig.rcControlsConfig.rxUsart].enabled   = 1;
    	board.serials[mainConfig.rcControlsConfig.rxUsart].Protocol  = mainConfig.rcControlsConfig.rxProtcol;
		board.dmasSerial[board.serials[mainConfig.rcControlsConfig.rxUsart].RXDma].enabled  = 1;
    }
    else if ((mainConfig.rcControlsConfig.rxProtcol == USING_SPEK_T) || (mainConfig.rcControlsConfig.rxProtcol == USING_DSM2_T))
    {
		board.serials[mainConfig.rcControlsConfig.rxUsart].enabled   = 1;
		board.serials[mainConfig.rcControlsConfig.rxUsart].Protocol  = mainConfig.rcControlsConfig.rxProtcol;
		if (mainConfig.telemConfig.telemSpek)
		{
			board.dmasSerial[board.serials[mainConfig.rcControlsConfig.rxUsart].TXDma].enabled  = 1;
		}
		board.dmasSerial[board.serials[mainConfig.rcControlsConfig.rxUsart].RXDma].enabled  = 1;
	}
    else if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS_R)
    {
		board.serials[mainConfig.rcControlsConfig.rxUsart].enabled   = 1;
		board.serials[mainConfig.rcControlsConfig.rxUsart].Protocol  = mainConfig.rcControlsConfig.rxProtcol;
		board.dmasSerial[board.serials[mainConfig.rcControlsConfig.rxUsart].RXDma].enabled  = 1;
	}
    else if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS_T)
    {
		board.serials[mainConfig.rcControlsConfig.rxUsart].enabled   = 1;
		board.serials[mainConfig.rcControlsConfig.rxUsart].Protocol  = mainConfig.rcControlsConfig.rxProtcol;
		board.dmasSerial[board.serials[mainConfig.rcControlsConfig.rxUsart].RXDma].enabled  = 1;
	}
	else if (mainConfig.rcControlsConfig.rxProtcol == USING_SUMD_R)
	{
		board.serials[mainConfig.rcControlsConfig.rxUsart].enabled  = 1;
		board.serials[mainConfig.rcControlsConfig.rxUsart].Protocol = mainConfig.rcControlsConfig.rxProtcol;
		board.dmasSerial[board.serials[mainConfig.rcControlsConfig.rxUsart].RXDma].enabled  = 1;
	}
	else if (mainConfig.rcControlsConfig.rxProtcol == USING_SUMD_T)
	{
		board.serials[mainConfig.rcControlsConfig.rxUsart].enabled  = 1;
		board.serials[mainConfig.rcControlsConfig.rxUsart].Protocol = mainConfig.rcControlsConfig.rxProtcol;
		board.dmasSerial[board.serials[mainConfig.rcControlsConfig.rxUsart].RXDma].enabled  = 1;
	}
	else if (mainConfig.rcControlsConfig.rxProtcol == USING_IBUS_R)
	{
		board.serials[mainConfig.rcControlsConfig.rxUsart].enabled  = 1;
		board.serials[mainConfig.rcControlsConfig.rxUsart].Protocol = mainConfig.rcControlsConfig.rxProtcol;
		board.dmasSerial[board.serials[mainConfig.rcControlsConfig.rxUsart].RXDma].enabled  = 1;
	}
	else if (mainConfig.rcControlsConfig.rxProtcol == USING_IBUS_T)
	{
		board.serials[mainConfig.rcControlsConfig.rxUsart].enabled  = 1;
		board.serials[mainConfig.rcControlsConfig.rxUsart].Protocol = mainConfig.rcControlsConfig.rxProtcol;
		board.dmasSerial[board.serials[mainConfig.rcControlsConfig.rxUsart].RXDma].enabled  = 1;
	}
	else if (mainConfig.rcControlsConfig.rxProtcol == USING_CPPM_R)
	{
		ppmPin = board.serials[mainConfig.rcControlsConfig.rxUsart].RXPin;

		if ( (board.gyros[0].extiIRQn) != (GetExtinFromPin(ppmPin)) )
		{
			board.serials[mainConfig.rcControlsConfig.rxUsart].Protocol = mainConfig.rcControlsConfig.rxProtcol;
			callbackFunctionArray[GetExtiCallbackFromPin(ppmPin)] = PpmExtiCallback;
			EXTI_Init(ports[board.serials[mainConfig.rcControlsConfig.rxUsart].RXPort], board.serials[mainConfig.rcControlsConfig.rxUsart].RXPin, GetExtinFromPin(ppmPin), 0, 5, GPIO_MODE_IT_RISING_FALLING, GPIO_PULLUP);
		}
		else
		{
			ppmPin = 99;
		}
	}
	else if (mainConfig.rcControlsConfig.rxProtcol == USING_CPPM_T)
	{
		ppmPin = board.serials[mainConfig.rcControlsConfig.rxUsart].TXPin;

		if ( (board.gyros[0].extiIRQn) != (GetExtinFromPin(ppmPin)) )
		{
			board.serials[mainConfig.rcControlsConfig.rxUsart].Protocol = mainConfig.rcControlsConfig.rxProtcol;
			callbackFunctionArray[GetExtiCallbackFromPin(ppmPin)] = PpmExtiCallback;
			EXTI_Init(ports[board.serials[mainConfig.rcControlsConfig.rxUsart].TXPort], board.serials[mainConfig.rcControlsConfig.rxUsart].TXPin, GetExtinFromPin(ppmPin), 0, 5, GPIO_MODE_IT_RISING_FALLING, GPIO_PULLUP);
		}
		else
		{
			ppmPin = 99;
		}
	}

	lastRXPacket = InlineMillis();

	// read and write settings at slow speed
	// starting serial ENUM_USART1 which is serial 0
	for (uint32_t serialNumber = 0; serialNumber<MAX_USARTS;serialNumber++)
	{
		if (board.serials[serialNumber].enabled)
		{

			if (board.dmasSerial[board.serials[serialNumber].TXDma].enabled) //only move the DMA into the Active DMA if the serial needs it
			{
				board.dmasSerial[board.serials[serialNumber].TXDma].dmaDirection       = DMA_MEMORY_TO_PERIPH;
				board.dmasSerial[board.serials[serialNumber].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
				board.dmasSerial[board.serials[serialNumber].TXDma].dmaMemInc          = DMA_MINC_ENABLE;
				board.dmasSerial[board.serials[serialNumber].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
				board.dmasSerial[board.serials[serialNumber].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
				board.dmasSerial[board.serials[serialNumber].TXDma].dmaMode            = DMA_NORMAL;
				board.dmasSerial[board.serials[serialNumber].TXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
				board.dmasSerial[board.serials[serialNumber].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
				memcpy( &board.dmasActive[board.serials[serialNumber].TXDma], &board.dmasSerial[board.serials[serialNumber].TXDma], sizeof(board_dma) ); //TODO: Add dmasUsart
			}

		    if (board.dmasSerial[board.serials[serialNumber].RXDma].enabled) //only move the DMA into the Active DMA if the serial needs it
		    {
		    	board.dmasSerial[board.serials[serialNumber].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
				board.dmasSerial[board.serials[serialNumber].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
				board.dmasSerial[board.serials[serialNumber].RXDma].dmaMemInc          = DMA_MINC_DISABLE;
				board.dmasSerial[board.serials[serialNumber].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
				board.dmasSerial[board.serials[serialNumber].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
				board.dmasSerial[board.serials[serialNumber].RXDma].dmaMode            = DMA_CIRCULAR;
				board.dmasSerial[board.serials[serialNumber].RXDma].dmaPriority        = DMA_PRIORITY_VERY_HIGH;
				board.dmasSerial[board.serials[serialNumber].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
		    	memcpy( &board.dmasActive[board.serials[serialNumber].RXDma], &board.dmasSerial[board.serials[serialNumber].RXDma], sizeof(board_dma) );
		    }

			UsartDeInit(serialNumber); //deinits serial and associated pins and DMAs
			UsartInit(serialNumber); //inits serial and associated pins and DMAs
		}
	}

}

void DeInitBoardUsarts (void) {

	uint32_t serialNumber;

	if (ppmPin != 99)
	{
		for (serialNumber = 0; serialNumber<MAX_USARTS;serialNumber++)
		{
			if (mainConfig.rcControlsConfig.rxProtcol == USING_CPPM_T)
			{
				ppmPin = 99;
				EXTI_Deinit(ports[board.serials[mainConfig.rcControlsConfig.rxUsart].TXPort], board.serials[mainConfig.rcControlsConfig.rxUsart].TXPin, GetExtinFromPin(board.serials[mainConfig.rcControlsConfig.rxUsart].TXPin));
			}
			if (mainConfig.rcControlsConfig.rxProtcol == USING_CPPM_R)
			{
				ppmPin = 99;
				EXTI_Deinit(ports[board.serials[mainConfig.rcControlsConfig.rxUsart].RXPort], board.serials[mainConfig.rcControlsConfig.rxUsart].RXPin, GetExtinFromPin(board.serials[mainConfig.rcControlsConfig.rxUsart].RXPin));
			}
		}
	}

	for (uint32_t serialNumber = 0; serialNumber<MAX_USARTS;serialNumber++)
	{
		if (board.serials[serialNumber].enabled)
		{
			UsartDeInit(serialNumber); //deinits serial and associated pins and DMAs
		}
	}

}

void SerialTxCallback(uint32_t callbackNumber)
{
	HAL_DMA_IRQHandler(uartHandles[dmaTxCallbackToUsartHandle[callbackNumber]].hdmatx);
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if (huart == &uartHandles[board.serials[serialNumber].usartHandle])
		{
			//do stuff here for this USART.
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

	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if (huart == &uartHandles[board.serials[serialNumber].usartHandle])
		{
			//todo: How do we handle multiple RXs with this?
			timeSinceLastPacket[serialNumber] = (currentTime - timeOfLastPacket[serialNumber]); //todo: How do we handle multiple RXs with this?
			timeOfLastPacket[serialNumber]    = currentTime; //todo: How do we handle multiple RXs with this?

			if ( ( (board.serials[serialNumber].Protocol == USING_SUMD_T) || (board.serials[serialNumber].Protocol == USING_SUMD_R)) && (dmaIndex[serialNumber] == 2) )
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
				if ((board.serials[serialNumber].Protocol == USING_SPEK_T) || (board.serials[serialNumber].Protocol == USING_SPEK_R) || (board.serials[serialNumber].Protocol == USING_DSM2_T) || (board.serials[serialNumber].Protocol == USING_DSM2_R))
					ProcessSpektrumPacket(serialNumber);
				else if ((board.serials[serialNumber].Protocol == USING_SBUS_T) || (board.serials[serialNumber].Protocol == USING_SBUS_R))
					ProcessSbusPacket(serialNumber);
				else if ( (board.serials[serialNumber].Protocol == USING_SUMD_T) || (board.serials[serialNumber].Protocol == USING_SUMD_R) )
					ProcessSumdPacket(serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1], board.serials[serialNumber].FrameSize);
				else if ( (board.serials[serialNumber].Protocol == USING_IBUS_T) || (board.serials[serialNumber].Protocol == USING_IBUS_R) )
					ProcessIbusPacket(serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1], board.serials[serialNumber].FrameSize);
				else if (board.serials[serialNumber].Protocol == USING_SPORT)
				{
					//simply fill the telemetry RX buffer. The scheduler and s.port driver will handle the rest.
					telemtryRxBuffer[0] = serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1][0];
					telemtryRxBuffer[1] = serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1][1];
				}
				else if (board.serials[serialNumber].Protocol == USING_MSP)
				{
					//simply fill the telemetry RX buffer. The scheduler and s.port driver will handle the rest.
					telemtryRxBuffer[0] = serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1][0];
					telemtryRxBuffer[1] = serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1][1];
				}
				else if (board.serials[serialNumber].Protocol == USING_RFVTX)
				{
					//simply fill the telemetry RX buffer. The scheduler and s.port driver will handle the rest.
					rfVtxRxBuffer[0] = serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1][0];
					rfVtxRxBuffer[1] = serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1][1];
					rfVtxRxBuffer[2] = serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1][2];
					rfVtxRxBuffer[3] = serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1][3];
				}


			}
			break;
		}
	}

}
