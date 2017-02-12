#include "includes.h"

#define SPORT_PACKET_SIZE 8
#define SPORT_SOFT_SERIAL_BUFFER_SIZE 10
#define SPORT_SOFT_SERIAL_TIME_BUFFER_SIZE 100

static const uint16_t bitLookup[] = {0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF};

volatile uint8_t  telemtryRxBuffer[SPORT_SOFT_SERIAL_BUFFER_SIZE];
volatile uint32_t telemtryRxTimerBuffer[SPORT_SOFT_SERIAL_TIME_BUFFER_SIZE];
volatile uint32_t telemtryRxBufferIdx;
volatile uint32_t telemtryRxTimerBufferIdx;

motor_type sbusActuator;


static uint8_t  SmartPortGetByte(uint8_t inByte, uint16_t *crcp);
static void     SmartPortCreatePacket(uint32_t id, int32_t val, uint8_t sPortPacket[]);

static void     PutSportIntoReceiveState(motor_type actuator, uint32_t inverted);
static void     PutSportIntoSendState(motor_type actuator, uint32_t inverted);
static void     NumberOfBits(uint32_t time2, uint32_t time1, uint32_t bitsInByte, float bitWidth, uint16_t *numberOfBits, uint32_t workingOnByte);
static uint32_t ProcessSoftSerialBits(void);
static float    FindSoftSerialBitWidth(uint32_t baudRate);
static float    FindSoftSerialByteWidth(float bitWidth, uint32_t bitsPerByte);
static float    FindSoftSerialLineIdleTime(float byteWidth);
static uint32_t IsSoftSerialLineIdle();
static uint32_t sPortTelemCount = 0;

uint32_t sendSmartPortAt = 0;




static uint8_t SmartPortGetByte(uint8_t inByte, uint16_t *crcp) {

	uint8_t outByte;
	uint16_t crc;

	outByte = inByte;

    // smart port escape sequence
    if (inByte == 0x7D || inByte == 0x7E) {
    	outByte = BYTESTUFF;
    	inByte ^= 0x20;
    }

    if (crcp == NULL)
    	return (outByte);

    crc = *crcp;
    crc += inByte;
    crc += crc >> 8;
    crc &= 0x00FF;
    *crcp = crc;

    return (outByte);
}

static void SmartPortCreatePacket(uint32_t id, int32_t val, uint8_t sPortPacket[]) {

	uint16_t crc = 0;

	//Frame Header
	sPortPacket[0] = SmartPortGetByte(SPORT_FRAME_HEADER, &crc);

	//Data ID
	sPortPacket[1] = SmartPortGetByte( (uint8_t)( (id >> 0) & 0xff), &crc);
	sPortPacket[2] = SmartPortGetByte( (uint8_t)( (id >> 8) & 0xff), &crc);

	//Data
	sPortPacket[3] = SmartPortGetByte( (uint8_t)( (val >> 0)  & 0xff), &crc);
	sPortPacket[4] = SmartPortGetByte( (uint8_t)( (val >> 8)  & 0xff), &crc);
	sPortPacket[5] = SmartPortGetByte( (uint8_t)( (val >> 16) & 0xff), &crc);
	sPortPacket[6] = SmartPortGetByte( (uint8_t)( (val >> 24) & 0xff), &crc);

	//CRC
	sPortPacket[7] = SmartPortGetByte( (uint8_t)( (0xFF - (uint8_t)(crc & 0xff) ) ), NULL);

//	OutputSerialDmaByte(sPortPacket, 8, board.motors[7], 0, 1); //send sPortPacket, which is 8 bytes, onto actuator 7, lsb, with serial frame, lo pulse is a one

//	OutputSerialDmaByte(uint8_t *serialOutBuffer, uint32_t outputLength, motor_type actuator, uint32_t msb)

}

void CheckIfSportReadyToSend(void)
{
	if (IsSoftSerialLineIdle()) //soft serial?
	{
		telemtryRxTimerBuffer[telemtryRxTimerBufferIdx] = telemtryRxTimerBuffer[telemtryRxTimerBufferIdx - 1] + lrintf(17.36*2); //put in last time so we can get the last byte. We need the last byte to calculate the frame.
		telemtryRxTimerBufferIdx++;
		if(ProcessSoftSerialBits())
		{
			if ( (telemtryRxBuffer[0] == 0x7E) && (telemtryRxBuffer[1] == 0x1B) )
			{
				//put actuator into rx state and set data to be sent 1.5 ms from now
				PutSportIntoSendState(sbusActuator, 1);
				sendSmartPortAt = Micros() + 1500;
			}
		}
		telemtryRxTimerBufferIdx = 0; //set time buffer index to zero
		telemtryRxBufferIdx = 0; //set time buffer index to zero
	}
	else if ( (telemtryRxBuffer[0] == 0x7E) && (telemtryRxBuffer[1] == 0x1B) ) //normal serial?
	{
		telemtryRxBuffer[1] = 0;
		sendSmartPortAt = Micros() + 1500;
	}
}


void SendSmartPort(void)
{
	uint32_t sentSerial = 0;
	uint8_t sPortPacket[SPORT_PACKET_SIZE];

	//create the s.port packet using the sensor id, sensor data ranged to what it needs to be and a buffer to store the packet.
	switch(sPortTelemCount++)
	{
		case 0:
			SmartPortCreatePacket(0x0700, (int32_t)(filteredAccData[ACCX] * 100), sPortPacket );
			break;
		case 1:
			SmartPortCreatePacket(0x0710, (int32_t)(filteredAccData[ACCY] * 100), sPortPacket );
			break;
		case 2:
			SmartPortCreatePacket(0x0720, (int32_t)(filteredAccData[ACCZ] * 100), sPortPacket );
			break;
		case 3:
			SmartPortCreatePacket(0x0701, (int32_t)(filteredGyroData[PITCH]), sPortPacket );
			break;
		case 4:
			SmartPortCreatePacket(0x0711, (int32_t)(filteredGyroData[ROLL]), sPortPacket );
			break;
		case 5:
			SmartPortCreatePacket(0x0721, (int32_t)(filteredGyroData[YAW]), sPortPacket );
			break;
		case 6:
			SmartPortCreatePacket(0xf104, (int32_t)(adcVoltage), sPortPacket );
			sPortTelemCount = 0;
			break;
		case 7:
		default:
			sPortTelemCount = 0;
			return;
			break;
	}


	//send via hard serial if it's configured
	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( (board.serials[serialNumber].enabled) && (mainConfig.telemConfig.telemSport) )
		{
			if (board.serials[serialNumber].Protocol == USING_SPORT)
			{
				sentSerial = 1;
				HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)sPortPacket, SPORT_PACKET_SIZE);
			}
		}
	}

	//otherwise send via soft serial
	if(!sentSerial)
	{
		OutputSerialDmaByte(sPortPacket, SPORT_PACKET_SIZE, sbusActuator, 0, 1, 0);
	}

}


void InitSport(uint32_t usartNumber)
{
	//use manual protocol to setup s.port.
	board.serials[usartNumber].enabled   = 1;
	board.serials[usartNumber].Protocol  = USING_SPORT;

	board.serials[usartNumber].BaudRate   = 57600;
	board.serials[usartNumber].WordLength = UART_WORDLENGTH_8B;
	board.serials[usartNumber].StopBits   = UART_STOPBITS_1;
	board.serials[usartNumber].Parity     = UART_PARITY_NONE;
	board.serials[usartNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
	board.serials[usartNumber].Mode       = UART_MODE_TX_RX;

	board.serials[usartNumber].RXPin  = board.serials[usartNumber].TXPin;
	board.serials[usartNumber].RXPort = board.serials[usartNumber].TXPort;

	board.serials[usartNumber].serialTxInverted = 1;
	board.serials[usartNumber].serialRxInverted = 1;


	board.dmasSerial[board.serials[usartNumber].TXDma].enabled  = 1;
	board.dmasSerial[board.serials[usartNumber].RXDma].enabled  = 1;

	board.dmasSerial[board.serials[usartNumber].TXDma].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaMode            = DMA_NORMAL;
	board.dmasSerial[board.serials[usartNumber].TXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmasSerial[board.serials[usartNumber].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	memcpy( &board.dmasActive[board.serials[usartNumber].TXDma], &board.dmasSerial[board.serials[usartNumber].TXDma], sizeof(board_dma) ); //TODO: Add dmasUsart

	board.dmasSerial[board.serials[usartNumber].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmasSerial[board.serials[usartNumber].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSerial[board.serials[usartNumber].RXDma].dmaMemInc          = DMA_MINC_DISABLE;
	board.dmasSerial[board.serials[usartNumber].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSerial[board.serials[usartNumber].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSerial[board.serials[usartNumber].RXDma].dmaMode            = DMA_CIRCULAR;
	board.dmasSerial[board.serials[usartNumber].RXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmasSerial[board.serials[usartNumber].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	memcpy( &board.dmasActive[board.serials[usartNumber].RXDma], &board.dmasSerial[board.serials[usartNumber].RXDma], sizeof(board_dma) );

	UsartDeInit(usartNumber); //deinits serial and associated pins and DMAs
	UsartInit(usartNumber); //inits serial and associated pins and DMAs
}


///////////////////////////// SOFT SPORT

void InitSoftSport(void)
{

	//set RX callback to Send sbus data if
	uint32_t actuatorNumOutput;
	uint32_t outputNumber;

	for (actuatorNumOutput = 0; actuatorNumOutput < MAX_MOTOR_NUMBER; actuatorNumOutput++)
	{
		outputNumber = mainConfig.mixerConfig.motorOutput[actuatorNumOutput];

		if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_SPORT)
		{

			if (!DoesDmaConflictWithActiveDmas(board.motors[outputNumber]))
			{
				sbusActuator = board.motors[outputNumber];
				PutSportIntoReceiveState(sbusActuator, 1);
			}

		}

	}

}

static uint32_t IsSoftSerialLineIdle() {
	volatile float timeNow;
	timeNow = (float)Micros();
	if ( (timeNow - (float)telemtryRxTimerBuffer[telemtryRxTimerBufferIdx-1]) > (230)) {
		if (telemtryRxTimerBufferIdx > 3)
			return(1);
	}
	return(0);
}

void SportSoftSerialExtiCallback(uint32_t callbackNumber)
{
	(void)(callbackNumber);
	// EXTI line interrupt detected
	if(__HAL_GPIO_EXTI_GET_IT(sbusActuator.pin) != RESET)
	{
		//record time of IRQ in microseconds
		telemtryRxTimerBuffer[telemtryRxTimerBufferIdx++] = Micros();
		if (telemtryRxTimerBufferIdx == SPORT_SOFT_SERIAL_TIME_BUFFER_SIZE) {
			telemtryRxTimerBufferIdx = 0;
		}
		__HAL_GPIO_EXTI_CLEAR_IT(sbusActuator.pin);
	}
}

void SportSoftSerialDmaCallback(uint32_t callbackNumber)
{
	(void)(callbackNumber);
	//after done sending we put soft serial back into rx state
	 if (dmaHandles[sbusActuator.Dma].State == HAL_DMA_STATE_READY)
	 {
			//DMA is done sending, let's switch GPIO to EXTI mode
		 PutSportIntoReceiveState(sbusActuator, 1);
	 }
}

static void PutSportIntoReceiveState(motor_type actuator, uint32_t inverted)
{
	//Set the IRQ callback functions
	callbackFunctionArray[actuator.EXTICallback] = SportSoftSerialExtiCallback;
	callbackFunctionArray[actuator.DmaCallback]  = SportSoftSerialDmaCallback;

	//Init the EXTI
    EXTI_Init(ports[actuator.port], actuator.pin, actuator.EXTIn, 0, 1, GPIO_MODE_IT_RISING_FALLING, inverted ? GPIO_PULLDOWN : GPIO_PULLUP); //pulldown if inverted, pullup if normal serial

    //reset reception buffer index.
    telemtryRxTimerBufferIdx = 0;

}

static void PutSportIntoSendState(motor_type actuator, uint32_t inverted)
{
	(void)(inverted);

	//Set callback functions for send and receive
	callbackFunctionArray[actuator.EXTICallback] = SportSoftSerialExtiCallback;
	callbackFunctionArray[actuator.DmaCallback]  = SportSoftSerialDmaCallback;

	//activate DMA output on actuator
	SetActiveDmaToActuatorDma(actuator);
	//Put actuator into Output state.
	InitDmaOutputForSoftSerial(DMA_OUTPUT_SPORT, actuator);

}

static void NumberOfBits(uint32_t time2, uint32_t time1, uint32_t bitsInByte, float bitWidth, uint16_t *numberOfBits, uint32_t workingOnByte)
{
	float timeD;
	float maxWidthPossible;
	*numberOfBits = 0;
	if (time2 <= time1) //no bits
	{
		return;
	}
	timeD = (float)(time2 - time1);
	maxWidthPossible = (bitWidth * (float)bitsInByte);
	if ( workingOnByte || (timeD < maxWidthPossible) ) //working on byte, so return line idle, or bits exist and this is a new byte
	{
		*numberOfBits = lrintf(round(timeD/(float)bitWidth));
	}
	return;
}

static uint32_t ProcessSoftSerialBits(void)
{

	uint32_t x;
	uint32_t fails;
	uint16_t bits;
	uint32_t currentBit;
	uint32_t bitsInByte;
	volatile uint32_t byte;
	uint16_t totalBitsFound;
	float    bitWidth;
	uint32_t byteFound;
	volatile uint32_t timeNow = Micros();

	byteFound  = 0;
	bitWidth   = 17.36;
	bitsInByte = 10;

	fails      = 0;
	byte       = 0;
	currentBit = 1;

	for (x = 0; x < telemtryRxTimerBufferIdx; x++)
	{

		NumberOfBits(telemtryRxTimerBuffer[x+1], telemtryRxTimerBuffer[x], bitsInByte, bitWidth, &bits, totalBitsFound);
		totalBitsFound = 0;

		if (totalBitsFound == 0) //starting new byte string from line idle.
		{

			currentBit = 1;

			while (totalBitsFound < bitsInByte)
			{
				if (currentBit)
				{
					if (!bits)
					{
						x++;
						NumberOfBits(telemtryRxTimerBuffer[x+1], telemtryRxTimerBuffer[x], bitsInByte, bitWidth, &bits, totalBitsFound);
					}
					if ( (bits > 0) && (bits <= bitsInByte) )
					{
						bits = CONSTRAIN(bits,1,(bitsInByte-totalBitsFound));
						byte &= ~(bitLookup[ bits ] << totalBitsFound);
						totalBitsFound += bits;
						currentBit=0;
						bits = 0;
					}
					else
					{
						//ignore this time as a corruption
						fails++;
					}
				}
				else
				{
					if (!bits)
					{
						x++;
						NumberOfBits(telemtryRxTimerBuffer[x+1], telemtryRxTimerBuffer[x], bitsInByte, bitWidth, &bits, totalBitsFound);
					}
					if ( (bits > 0) && (bits <= bitsInByte) )
					{
						bits = CONSTRAIN(bits,1,(bitsInByte-totalBitsFound));
						byte |= (bitLookup[ bits ] << totalBitsFound);
						totalBitsFound += bits;
						currentBit=1;
						bits = 0;
					}
					else
					{
						//ignore this time as a corruption
						fails++;
					}
				}
				if(fails > 10)
				{
					totalBitsFound = 0;
					break;
				}
				if (totalBitsFound >= 10)
				{
					//trim off frames
					telemtryRxBuffer[telemtryRxBufferIdx++] = (uint8_t)( (byte >> 1) & 0xFF );
					if (timeNow > 1)
					{
						totalBitsFound = 0;
					}
					byteFound = 1;
					break;
				}
			}
		}
	}
	return(byteFound);
}

inline static float FindSoftSerialBitWidth(uint32_t baudRate)
{
	return ( (1.0 / (float)baudRate) * 1000.0 * 1000.0 );
}

inline static float FindSoftSerialByteWidth(float bitWidth, uint32_t bitsPerByte)
{
	return (bitWidth * (float)bitsPerByte);
}

inline static float FindSoftSerialLineIdleTime(float byteWidth)
{
	return (byteWidth * 1.50);
}
