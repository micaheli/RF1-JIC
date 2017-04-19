#include "includes.h"

volatile uint8_t mspRxBuffer[25];
volatile uint8_t mspTxBuffer[25];


void SendMspAttitude(void)
{
	uint8_t crc = 0;
	uint32_t packetSize = 12;
	//<preamble>,<direction>,<size>,<command>,,<crc>
	mspTxBuffer[0] =  '$';
	mspTxBuffer[1] =  'M';
	mspTxBuffer[2] =  '>';
	mspTxBuffer[3] =  6;
	mspTxBuffer[4] =  MSP_ATTITUDE;
	mspTxBuffer[5] =  (uint8_t)((int16_t)(rollAttitude * 10.0f) & 0xFF);
	mspTxBuffer[6] =  (uint8_t)(((int16_t)(rollAttitude * 10.0f) >> 8) & 0xFF);;
	mspTxBuffer[7] =  (uint8_t)((int16_t)(pitchAttitude * 10.0f) & 0xFF);
	mspTxBuffer[8] =  (uint8_t)(((int16_t)(pitchAttitude * 10.0f) >> 8) & 0xFF);;
	mspTxBuffer[9] =  (uint8_t)((int16_t)(yawAttitude * 10.0f) & 0xFF);
	mspTxBuffer[10] =  (uint8_t)(((int16_t)(yawAttitude * 10.0f) >> 8) & 0xFF);;

	crc ^= mspTxBuffer[3];
	crc ^= mspTxBuffer[4];
	crc ^= mspTxBuffer[5];
	crc ^= mspTxBuffer[6];
	crc ^= mspTxBuffer[7];
	crc ^= mspTxBuffer[8];
	crc ^= mspTxBuffer[9];
	crc ^= mspTxBuffer[10];

	mspTxBuffer[11] = crc;

	if (!telemEnabled)
			return;

	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( (board.serials[serialNumber].enabled) && (mainConfig.telemConfig.telemMsp) )
		{
			if (board.serials[serialNumber].Protocol == USING_MSP)
				HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)mspTxBuffer, packetSize);
		}
	}

/*
	//this isn't calculated in glue mode'
		rollAttitude  =  InlineRadiansToDegrees( Atan2fast(attitudeFrameQuat.y * attitudeFrameQuat.z + attitudeFrameQuat.w * attitudeFrameQuat.x, 0.5f - (attitudeFrameQuat.x * attitudeFrameQuat.x + attitudeFrameQuat.y * attitudeFrameQuat.y)) );
		pitchAttitude =  InlineRadiansToDegrees( arm_sin_f32(2.0f * (attitudeFrameQuat.x * attitudeFrameQuat.z - attitudeFrameQuat.w * attitudeFrameQuat.y)) );
		yawAttitude   = -InlineRadiansToDegrees( Atan2fast(attitudeFrameQuat.x * attitudeFrameQuat.y + attitudeFrameQuat.w * attitudeFrameQuat.z, 0.5f - (attitudeFrameQuat.y * attitudeFrameQuat.y + attitudeFrameQuat.z * attitudeFrameQuat.z)) );
*/

/*
	angx	INT 16	Range [-1800;1800] (unit: 1/10 degree)
	angy	INT 16	Range [-900;900] (unit: 1/10 degree)
	heading	INT 16	Range [-180;180]
*/
}

void SendMspAnalog(void)
{
	uint8_t crc = 0;
	uint32_t packetSize = 13;
	//<preamble>,<direction>,<size>,<command>,,<crc>
	mspTxBuffer[0] =  '$';
	mspTxBuffer[1] =  'M';
	mspTxBuffer[2] =  '>';
	mspTxBuffer[3] =  7;
	mspTxBuffer[4] =  MSP_ANALOG;
	mspTxBuffer[5] =  55;
	mspTxBuffer[6] =  (uint8_t)((int16_t)(100.0f  * 10.0f) & 0xFF);
	mspTxBuffer[7] =  (uint8_t)(((int16_t)(100.0f * 10.0f) >> 8) & 0xFF);;
	mspTxBuffer[8] =  (uint8_t)((int16_t)(1023) & 0xFF);
	mspTxBuffer[9] =  (uint8_t)(((int16_t)(1023) >> 8) & 0xFF);;
	mspTxBuffer[10] = (uint8_t)((int16_t)(4) & 0xFF);
	mspTxBuffer[11] = (uint8_t)(((int16_t)(4) >> 8) & 0xFF);;

	crc ^= mspTxBuffer[3];
	crc ^= mspTxBuffer[4];
	crc ^= mspTxBuffer[5];
	crc ^= mspTxBuffer[6];
	crc ^= mspTxBuffer[7];
	crc ^= mspTxBuffer[8];
	crc ^= mspTxBuffer[9];
	crc ^= mspTxBuffer[10];
	crc ^= mspTxBuffer[11];

	mspTxBuffer[12] = crc;

	if (!telemEnabled)
			return;

	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( (board.serials[serialNumber].enabled) && (mainConfig.telemConfig.telemMsp) )
		{
			if (board.serials[serialNumber].Protocol == USING_MSP)
				HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)mspTxBuffer, packetSize);
		}
	}

/*
	//this isn't calculated in glue mode'
		rollAttitude  =  InlineRadiansToDegrees( Atan2fast(attitudeFrameQuat.y * attitudeFrameQuat.z + attitudeFrameQuat.w * attitudeFrameQuat.x, 0.5f - (attitudeFrameQuat.x * attitudeFrameQuat.x + attitudeFrameQuat.y * attitudeFrameQuat.y)) );
		pitchAttitude =  InlineRadiansToDegrees( arm_sin_f32(2.0f * (attitudeFrameQuat.x * attitudeFrameQuat.z - attitudeFrameQuat.w * attitudeFrameQuat.y)) );
		yawAttitude   = -InlineRadiansToDegrees( Atan2fast(attitudeFrameQuat.x * attitudeFrameQuat.y + attitudeFrameQuat.w * attitudeFrameQuat.z, 0.5f - (attitudeFrameQuat.y * attitudeFrameQuat.y + attitudeFrameQuat.z * attitudeFrameQuat.z)) );
*/

/*
	angx	INT 16	Range [-1800;1800] (unit: 1/10 degree)
	angy	INT 16	Range [-900;900] (unit: 1/10 degree)
	heading	INT 16	Range [-180;180]
*/
}

void InitMsp(uint32_t usartNumber)
{
	uint32_t x;

	for (x=0;x<4;x++)
		rfVtxRxBuffer[x]=0;

	//use manual protocol to setup s.port.
	board.serials[usartNumber].enabled   = 1;
	board.serials[usartNumber].Protocol  = USING_MSP;

	board.serials[usartNumber].BaudRate   = 115200;
	board.serials[usartNumber].WordLength = UART_WORDLENGTH_8B;
	board.serials[usartNumber].StopBits   = UART_STOPBITS_1;
	board.serials[usartNumber].Parity     = UART_PARITY_NONE;
	board.serials[usartNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
	//board.serials[usartNumber].Mode       = UART_MODE_TX_RX;
	//can do tx and rx, but for now we send blindly
	board.serials[usartNumber].Mode       = UART_MODE_TX;

	board.serials[usartNumber].serialTxInverted = 0;
	board.serials[usartNumber].serialRxInverted = 0;
	board.serials[usartNumber].FrameSize = 10; //variable

	board.dmasSerial[board.serials[usartNumber].TXDma].enabled  = 1;
	//board.dmasSerial[board.serials[usartNumber].RXDma].enabled  = 1;
	board.dmasSerial[board.serials[usartNumber].RXDma].enabled  = 0;

	board.dmasSerial[board.serials[usartNumber].TXDma].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaMode            = DMA_NORMAL;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmasSerial[board.serials[usartNumber].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	memcpy( &board.dmasActive[board.serials[usartNumber].TXDma], &board.dmasSerial[board.serials[usartNumber].TXDma], sizeof(board_dma) ); //TODO: Add dmasUsart

	//board.dmasSerial[board.serials[usartNumber].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	//board.dmasSerial[board.serials[usartNumber].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	//board.dmasSerial[board.serials[usartNumber].RXDma].dmaMemInc          = DMA_MINC_DISABLE;
	//board.dmasSerial[board.serials[usartNumber].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	//board.dmasSerial[board.serials[usartNumber].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	//board.dmasSerial[board.serials[usartNumber].RXDma].dmaMode            = DMA_CIRCULAR;
	//board.dmasSerial[board.serials[usartNumber].RXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	//board.dmasSerial[board.serials[usartNumber].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	//memcpy( &board.dmasActive[board.serials[usartNumber].RXDma], &board.dmasSerial[board.serials[usartNumber].RXDma], sizeof(board_dma) );

	UsartDeInit(usartNumber); //deinits serial and associated pins and DMAs
	UsartInit(usartNumber); //inits serial and associated pins and DMAs
}

void SendMsp(uint8_t charArray, uint32_t size)
{

	(void)(charArray);
	(void)(size);
	//uint8_t sPortPacket[4];

	//bzero(rfVtxRxBuffer, sizeof(rfVtxRxBuffer));
}
