#include "includes.h"

#define SPORT_PACKET_SIZE 8
#define SPORT_LUA_BUFFER_LIMIT 32
#define SPORT_LUA_BUFFER_SIZE  36
#define SPORT_SOFT_SERIAL_BUFFER_SIZE 10
#define SPORT_SOFT_SERIAL_TIME_BUFFER_SIZE 100

uint8_t           sPortPacket[SPORT_PACKET_SIZE];
uint8_t           receivedDataBuffer[SPORT_LUA_BUFFER_SIZE];
uint8_t           transmitDataBuffer[SPORT_LUA_BUFFER_SIZE];
uint32_t          receivedDataBufferIdx  = 0;
uint32_t          transmitDataBufferIdx  = 0;
uint32_t          transmitDataBufferSent = 0;
uint32_t          expectedPacketCount    = 0xAC;
uint32_t          expectedPacketOutCount = 0xAC;
volatile int32_t  luaOutPacketOne; //32 bit variable holds 2 8 bit variable. max we can send via s.port
volatile int32_t  luaOutPacketTwo; //32 bit variable holds 4 8 bit variable. max we can send via s.port
volatile uint8_t  telemtryRxBuffer[SPORT_SOFT_SERIAL_BUFFER_SIZE];
volatile uint32_t telemtryRxTimerBuffer[SPORT_SOFT_SERIAL_TIME_BUFFER_SIZE];
volatile uint32_t telemtryRxBufferIdx;
volatile uint32_t telemtryRxTimerBufferIdx;
motor_type        sbusActuator;
static uint32_t   sPortTelemCount;
uint32_t          lastTimeSent = 0;
volatile uint32_t luaPacketPendingTime = 0;

static uint8_t  SmartPortGetByte(uint8_t inByte, uint16_t *crcp);
static void     SmartPortCreatePacket(uint32_t header, uint32_t id, int32_t val, uint8_t sPortPacket[]);
static void     ProcessSportLuaStuff(void);
static void     PutSportIntoReceiveState(motor_type actuator, uint32_t inverted);
static void     PutSportIntoSendState(motor_type actuator, uint32_t inverted);
static uint32_t IsSoftSerialLineIdle();


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

static void SmartPortCreatePacket(uint32_t header, uint32_t id, int32_t val, uint8_t sPortPacket[])
{

	uint16_t crc = 0;

	//Frame Header
	sPortPacket[0] = SmartPortGetByte( (uint8_t)header, &crc);

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

}

static void ResetLuaBuffer(void)
{
	expectedPacketCount   = 0xAC;
	receivedDataBufferIdx = 0;
	bzero(receivedDataBuffer, sizeof(receivedDataBuffer));
}

static void ProcessSportLuaStuff(void)
{
	static uint32_t packetCount;
	static uint32_t dataExpected;

	packetCount = telemtryRxBuffer[3];

	if (packetCount == 0xAC)
	{	//start of data packets
		dataExpected          = telemtryRxBuffer[4];
		receivedDataBufferIdx = 0;
		receivedDataBuffer[receivedDataBufferIdx++] = telemtryRxBuffer[5];
		receivedDataBuffer[receivedDataBufferIdx++] = telemtryRxBuffer[6];
		receivedDataBuffer[receivedDataBufferIdx++] = telemtryRxBuffer[7];
		expectedPacketCount = 0x01; //next expected packet will be 0x01
	}
	else if (packetCount == expectedPacketCount)
	{
		expectedPacketCount++; //next expected packet will be the current packet number plus 1
		receivedDataBuffer[receivedDataBufferIdx++] = telemtryRxBuffer[4];
		receivedDataBuffer[receivedDataBufferIdx++] = telemtryRxBuffer[5];
		receivedDataBuffer[receivedDataBufferIdx++] = telemtryRxBuffer[6];
		receivedDataBuffer[receivedDataBufferIdx++] = telemtryRxBuffer[7];
	}
	else
	{
		//return error, we missed a packet
		luaOutPacketOne = 0x000001AC;
		luaOutPacketTwo = 0x0000001A;
		ResetLuaBuffer();
		luaPacketPendingTime = InlineMillis();
	}

	if (receivedDataBufferIdx > 32)
	{
		//return error, bad data, we received more data than we were allowed to received
		luaOutPacketOne = 0x000001AC;
		luaOutPacketTwo = 0x0000001B;
		ResetLuaBuffer();
		luaPacketPendingTime = InlineMillis();
	}

	if (receivedDataBufferIdx >= dataExpected)
	{
		//All data received, process data received and send back proper reply
/*
#define REQUEST_ID      0x01
#define REQUEST_PIDS    0x02
#define REQUEST_RATES   0x03
#define REQUEST_VRX     0x04

#define ID_ROLL_KP     0x01
#define ID_ROLL_KI     0x02
#define ID_ROLL_KD     0x03
#define ID_PITCH_KP    0x04
#define ID_PITCH_KI    0x05
#define ID_PITCH_KD    0x06
#define ID_YAW_KP      0x07
#define ID_YAW_KI      0x08
#define ID_YAW_KD      0x09
*/

		if (receivedDataBuffer[0] == REQUEST_PIDS)
		{
			bzero(transmitDataBuffer, sizeof(transmitDataBuffer)); //new data request wanted, fill buffer and reset indexes

			transmitDataBuffer[0]  = 0xAC;
			transmitDataBuffer[1]  = ID_ROLL_KP;
			transmitDataBuffer[2]  = lrint(mainConfig.pidConfig[ROLL].kp) & 0xFF;
			transmitDataBuffer[3]  = (lrint(mainConfig.pidConfig[ROLL].kp) >> 8) & 0xFF;

			transmitDataBuffer[4]  = 0xAC;
			transmitDataBuffer[5]  = ID_PITCH_KP;
			transmitDataBuffer[6]  = lrint(mainConfig.pidConfig[PITCH].kp) & 0xFF;
			transmitDataBuffer[7]  = (lrint(mainConfig.pidConfig[PITCH].kp) >> 8) & 0xFF;

			transmitDataBuffer[8]  = 0xAC;
			transmitDataBuffer[9]  = ID_YAW_KP;
			transmitDataBuffer[10]  = lrint(mainConfig.pidConfig[YAW].kp) & 0xFF;
			transmitDataBuffer[11]  = (lrint(mainConfig.pidConfig[YAW].kp) >> 8) & 0xFF;

			transmitDataBuffer[12]  = 0xAC;
			transmitDataBuffer[13]  = ID_ROLL_KI;
			transmitDataBuffer[14] = lrint(mainConfig.pidConfig[ROLL].ki) & 0xFF;
			transmitDataBuffer[15] = (lrint(mainConfig.pidConfig[ROLL].ki) >> 8) & 0xFF;

			transmitDataBuffer[16]  = 0xAC;
			transmitDataBuffer[17] = ID_PITCH_KI;
			transmitDataBuffer[18] = lrint(mainConfig.pidConfig[PITCH].ki) & 0xFF;
			transmitDataBuffer[19] = (lrint(mainConfig.pidConfig[PITCH].ki) >> 8) & 0xFF;

			transmitDataBuffer[20]  = 0xAC;
			transmitDataBuffer[21] = ID_YAW_KI;
			transmitDataBuffer[22] = lrint(mainConfig.pidConfig[YAW].ki) & 0xFF;
			transmitDataBuffer[23] = (lrint(mainConfig.pidConfig[YAW].ki) >> 8) & 0xFF;

			transmitDataBuffer[24]  = 0xAC;
			transmitDataBuffer[25] = ID_ROLL_KD;
			transmitDataBuffer[26] = lrint(mainConfig.pidConfig[ROLL].kd) & 0xFF;
			transmitDataBuffer[27] = (lrint(mainConfig.pidConfig[ROLL].kd) >> 8) & 0xFF;

			transmitDataBuffer[28]  = 0xAC;
			transmitDataBuffer[29]  = ID_PITCH_KD;
			transmitDataBuffer[30]  = lrint(mainConfig.pidConfig[PITCH].kd) & 0xFF;
			transmitDataBuffer[31]  = (lrint(mainConfig.pidConfig[PITCH].kd) >> 8) & 0xFF;

			transmitDataBuffer[32]  = 0xAC;
			transmitDataBuffer[33]  = ID_YAW_KD;
			transmitDataBuffer[34]  = lrint(mainConfig.pidConfig[YAW].kd) & 0xFF;
			transmitDataBuffer[35]  = (lrint(mainConfig.pidConfig[YAW].kd) >> 8) & 0xFF;

			transmitDataBufferIdx  = 36;

			luaOutPacketOne = ( (transmitDataBuffer[0] & 0xFF) | ((transmitDataBuffer[1] & 0xFF) << 8) );
			luaOutPacketTwo = ( (transmitDataBuffer[2]) | (transmitDataBuffer[3] << 8) );

			transmitDataBufferSent = 4;
			ResetLuaBuffer();
			luaPacketPendingTime = InlineMillis();
		}
		else if (receivedDataBuffer[0] == REQUEST_RATES)
		{
			bzero(transmitDataBuffer, sizeof(transmitDataBuffer)); //new data request wanted, fill buffer and reset indexes

			transmitDataBuffer[0]  = 0xAC;
			transmitDataBuffer[1]  = ID_ROLL_RATE;
			transmitDataBuffer[2]  = lrint(mainConfig.rcControlsConfig.rates[ROLL]) & 0xFF;
			transmitDataBuffer[3]  = (lrint(mainConfig.rcControlsConfig.rates[ROLL]) >> 8) & 0xFF;

			transmitDataBuffer[4]  = 0xAC;
			transmitDataBuffer[5]  = ID_ROLL_EXPO;
			transmitDataBuffer[6]  = lrint(mainConfig.rcControlsConfig.curveExpo[PITCH]) & 0xFF;
			transmitDataBuffer[7]  = (lrint(mainConfig.rcControlsConfig.curveExpo[PITCH]) >> 8) & 0xFF;

			transmitDataBuffer[8]  = 0xAC;
			transmitDataBuffer[9]  = ID_ROLL_ACROP;
			transmitDataBuffer[10]  = lrint(mainConfig.rcControlsConfig.acroPlus[YAW]) & 0xFF;
			transmitDataBuffer[11]  = (lrint(mainConfig.rcControlsConfig.acroPlus[YAW]) >> 8) & 0xFF;

			transmitDataBuffer[12]  = 0xAC;
			transmitDataBuffer[13]  = ID_PITCH_RATE;
			transmitDataBuffer[14] = lrint(mainConfig.rcControlsConfig.rates[ROLL]) & 0xFF;
			transmitDataBuffer[15] = (lrint(mainConfig.rcControlsConfig.rates[ROLL]) >> 8) & 0xFF;

			transmitDataBuffer[16]  = 0xAC;
			transmitDataBuffer[17] = ID_PITCH_EXPO;
			transmitDataBuffer[18] = lrint(mainConfig.rcControlsConfig.curveExpo[PITCH]) & 0xFF;
			transmitDataBuffer[19] = (lrint(mainConfig.rcControlsConfig.curveExpo[PITCH]) >> 8) & 0xFF;

			transmitDataBuffer[20]  = 0xAC;
			transmitDataBuffer[21] = ID_PITCH_ACROP;
			transmitDataBuffer[22] = lrint(mainConfig.rcControlsConfig.acroPlus[YAW]) & 0xFF;
			transmitDataBuffer[23] = (lrint(mainConfig.rcControlsConfig.acroPlus[YAW]) >> 8) & 0xFF;

			transmitDataBuffer[24]  = 0xAC;
			transmitDataBuffer[25] = ID_YAW_RATE;
			transmitDataBuffer[26] = lrint(mainConfig.rcControlsConfig.rates[ROLL]) & 0xFF;
			transmitDataBuffer[27] = (lrint(mainConfig.rcControlsConfig.rates[ROLL]) >> 8) & 0xFF;

			transmitDataBuffer[28]  = 0xAC;
			transmitDataBuffer[29]  = ID_YAW_EXPO;
			transmitDataBuffer[30]  = lrint(mainConfig.rcControlsConfig.curveExpo[PITCH]) & 0xFF;
			transmitDataBuffer[31]  = (lrint(mainConfig.rcControlsConfig.curveExpo[PITCH]) >> 8) & 0xFF;

			transmitDataBuffer[32]  = 0xAC;
			transmitDataBuffer[33]  = ID_YAW_ACROP;
			transmitDataBuffer[34]  = lrint(mainConfig.rcControlsConfig.acroPlus[YAW]) & 0xFF;
			transmitDataBuffer[35]  = (lrint(mainConfig.rcControlsConfig.acroPlus[YAW]) >> 8) & 0xFF;

			transmitDataBufferIdx  = 36;

			luaOutPacketOne = ( (transmitDataBuffer[0] & 0xFF) | ((transmitDataBuffer[1] & 0xFF) << 8) );
			luaOutPacketTwo = ( (transmitDataBuffer[2]) | (transmitDataBuffer[3] << 8) );

			transmitDataBufferSent = 4;
			ResetLuaBuffer();
			luaPacketPendingTime = InlineMillis();
		}
		else if (receivedDataBuffer[0] == REQUEST_VRX)
		{
			bzero(transmitDataBuffer, sizeof(transmitDataBuffer)); //new data request wanted, fill buffer and reset indexes

			if ( InitSmartAudio(ENUM_USART3) )
			{
				ChannelToBandAndChannel(smartAudioVtxStructure.channel, &vtxData.vtxChannel, &vtxData.vtxBand);
				vtxData.vtxPower   = smartAudioVtxStructure.powerLevel;
			}
			transmitDataBuffer[0]  = 0xAC;
			transmitDataBuffer[1]  = ID_DEVICE;
			transmitDataBuffer[2]  = 0;
			transmitDataBuffer[3]  = vtxData.vtxChannel;

			transmitDataBuffer[4]  = 0xAC;
			transmitDataBuffer[5]  = ID_BAND;
			transmitDataBuffer[6]  = 0;
			transmitDataBuffer[7]  = vtxData.vtxBand;

			transmitDataBuffer[8]  = 0xAC;
			transmitDataBuffer[9]  = ID_CHANNEL;
			transmitDataBuffer[10] = 0;
			transmitDataBuffer[11] = vtxData.vtxChannel;

			transmitDataBuffer[12] = 0xAC;
			transmitDataBuffer[13] = ID_POWER;
			transmitDataBuffer[14] = 0;
			transmitDataBuffer[15] = vtxData.vtxPower;

			transmitDataBuffer[16] = 0xAC;
			transmitDataBuffer[17] = ID_PIT;
			transmitDataBuffer[18] = 0;
			transmitDataBuffer[19] = vtxData.vtxPit;

			transmitDataBufferIdx  = 20;

			luaOutPacketOne = ( (transmitDataBuffer[0] & 0xFF) | ((transmitDataBuffer[1] & 0xFF) << 8) );
			luaOutPacketTwo = ( (transmitDataBuffer[2]) | (transmitDataBuffer[3] << 8) );

			transmitDataBufferSent = 4;
			ResetLuaBuffer();
			luaPacketPendingTime = InlineMillis();
		}

	}
	else
	{
		//data received was good and added to buffer, send reply to show data was received
		luaOutPacketOne = 0x000001AC;
		luaOutPacketTwo = 0x0000001E;
		luaPacketPendingTime = InlineMillis();
	}

}

uint32_t CheckSportCrc(volatile uint8_t telemtryRxBuffer[], volatile uint32_t bytesReceived)
{
	uint16_t crc = 0;
	uint32_t x;

	for (x = 2; x<bytesReceived; x++)
		SmartPortGetByte( telemtryRxBuffer[x], &crc);

	if (crc == 0xff)
		return(1);

	return(0);

}


void CheckIfSportReadyToSend(void)
{

	uint32_t bytesReceived;
	if (IsSoftSerialLineIdle()) //soft serial?
	{
		//process timer buffer, s.port bit width is 17.35us and there are 10 bits in the byte including the start and stop frame
		bytesReceived = NewProcessSoftSerialBits(telemtryRxTimerBuffer, &telemtryRxTimerBufferIdx, telemtryRxBuffer, &telemtryRxBufferIdx, 17.36, 10);
		if(bytesReceived)
		{
			if ( (telemtryRxBuffer[0] == 0x7E) && (telemtryRxBuffer[1] == 0x1B) )
			{
				//We're allowed to send data when we get the sensor ID 0x1B
				//We can either send telemetry or lua script data
				if (InlineMillis() - luaPacketPendingTime > 500)
				{
					//100 ms timeout
					luaOutPacketOne = 0;
					luaOutPacketTwo = 0;
					luaPacketPendingTime = 0;
				}

				if (luaPacketPendingTime)
				{   //we have a pending out packet
					sendSmartPortAt = 0;
					sendSmartPortLuaAt = Micros() + 100;
				}
				else if ( (transmitDataBufferIdx > 0) && (transmitDataBufferSent <= (transmitDataBufferIdx - 1)) )
				{   //data in buffer to send, put it into packet
					//fill packets
					ResetLuaBuffer();
					luaOutPacketOne = ( (transmitDataBuffer[transmitDataBufferSent] & 0xFF) | ((transmitDataBuffer[transmitDataBufferSent+1] & 0xFF) << 8) );
					luaOutPacketTwo = ( (transmitDataBuffer[transmitDataBufferSent+2]) | (transmitDataBuffer[transmitDataBufferSent+3] << 8) );
					luaPacketPendingTime = InlineMillis();
					transmitDataBufferSent += 4;
					sendSmartPortAt = 0;
					sendSmartPortLuaAt = Micros() + 100;
				}
				else
				{   //we send normal telemetry
					sendSmartPortAt = Micros() + 100;
					sendSmartPortLuaAt = 0;
				}
				PutSportIntoSendState(sbusActuator, 1);

			}
			else if ( (telemtryRxBuffer[0] == 0x7E) && (telemtryRxBuffer[1] == 0x0D) && (telemtryRxBuffer[2] == 0x30) )
			{
				//sensor ID 0x0D is used by the LUA script.
				if ( CheckSportCrc(telemtryRxBuffer, bytesReceived) )
				{
					//get data and decided how to set luaOutPacket
					ProcessSportLuaStuff();
				}
				else
				{
					//bad data, set luaOutPacket to 0xAC011D00 which means bad CRC
					//want to send AC 01 1D
					// packet 1 is 0x000001AC
					// packet 2 is 0x0000001D
					//luaOutPacketOne = 0x000001AC;
					//luaOutPacketTwo = 0x0000001D;
					//luaPacketPendingTime = InlineMillis();
					//ResetLuaBuffer(); //reset the buffer
				}
			}
		}
	}
	else if ( (telemtryRxBuffer[0] == 0x7E) && (telemtryRxBuffer[1] == 0x1B) ) //normal serial?
	{
		sendSmartPortAt = Micros() + 100; //send telemetry reply in 1.5 ms
	}
	else if ( (telemtryRxBuffer[0] == 0x7E) && (telemtryRxBuffer[1] == 0x0D) && (telemtryRxBuffer[2] == 0x30) )
	{
		ProcessSportLuaStuff(); //get reply ready
	}
	telemtryRxBuffer[0] = 0;
	telemtryRxBuffer[1] = 0;

}

void SendSmartPortLua(void)
{
	uint32_t sentSerial = 0;

	SmartPortCreatePacket(0x32, luaOutPacketOne, luaOutPacketTwo, sPortPacket );

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

	lastTimeSent = InlineMillis();
	luaOutPacketOne = 0;
	luaOutPacketTwo = 0;
	luaPacketPendingTime = 0;
}

void SendSmartPort(void)
{
	uint32_t sentSerial = 0;
	uint8_t sPortPacket[SPORT_PACKET_SIZE];

	//create the s.port packet using the sensor id, sensor data ranged to what it needs to be and a buffer to store the packet.
	switch(sPortTelemCount++)
	{
		case 0:
			SmartPortCreatePacket(SPORT_FRAME_HEADER, 0x0700, (int32_t)(filteredAccData[ACCX] * 100), sPortPacket );
			break;
		case 1:
			SmartPortCreatePacket(SPORT_FRAME_HEADER, 0x0710, (int32_t)(filteredAccData[ACCY] * 100), sPortPacket );
			break;
		case 2:
			SmartPortCreatePacket(SPORT_FRAME_HEADER, 0x0720, (int32_t)(filteredAccData[ACCZ] * 100), sPortPacket );
			break;
		case 3:
			SmartPortCreatePacket(SPORT_FRAME_HEADER, 0x0701, (int32_t)(filteredGyroData[PITCH]), sPortPacket );
			break;
		case 4:
			SmartPortCreatePacket(SPORT_FRAME_HEADER, 0x0711, (int32_t)(filteredGyroData[ROLL]), sPortPacket );
			break;
		case 5:
			SmartPortCreatePacket(SPORT_FRAME_HEADER, 0x0721, (int32_t)(filteredGyroData[YAW]), sPortPacket );
			break;
		case 6:
			SmartPortCreatePacket(SPORT_FRAME_HEADER, VFAS_FIRST_ID, (int32_t)(averageVoltage * 100), sPortPacket );
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

	lastTimeSent = InlineMillis();
}

///////////////////////////// HARD SPORT
void InitSport(uint32_t usartNumber)
{
	sPortTelemCount = 0;

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
	board.serials[usartNumber].FrameSize = 2;


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

	sPortTelemCount = 0;

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

static uint32_t IsSoftSerialLineIdle()
{
	volatile float timeNow;
	timeNow = (float)Micros();
	if ( (timeNow - (float)telemtryRxTimerBuffer[telemtryRxTimerBufferIdx-1]) > (200))
	{
		if (telemtryRxTimerBufferIdx > 1)
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
		if (telemtryRxTimerBufferIdx == SPORT_SOFT_SERIAL_TIME_BUFFER_SIZE)
		{
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
