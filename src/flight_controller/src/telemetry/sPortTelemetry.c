#include "includes.h"

#define SPORT_PACKET_SIZE 8
#define SPORT_LUA_BUFFER_LIMIT 32
#define SPORT_LUA_BUFFER_SIZE  36
#define SPORT_SOFT_SERIAL_BUFFER_SIZE 10
#define SPORT_SOFT_SERIAL_TIME_BUFFER_SIZE 100

char              charMatrix[6][24];
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
int32_t           smartPortExti = -1; //dirty hack, smart port really needs to start using the soft serial driver
static uint32_t   sPortTelemCount;
uint32_t          lastTimeSent = 0;
volatile uint32_t luaPacketPendingTime = 0;
volatile uint32_t noiseCounter = 0;

static uint8_t  SmartPortGetByte(uint8_t inByte, uint16_t *crcp);
static void     SmartPortCreatePacket(uint32_t header, uint32_t id, int32_t val, uint8_t sPortPacket[]);
static void     PutSportIntoReceiveState(motor_type actuator, uint32_t inverted);
static void     PutSportIntoSendState(motor_type actuator, uint32_t inverted);
static uint32_t IsSoftSerialLineIdle();

enum {
	PROG_STAT_MENU_VTX       = 0,
	PROG_STAT_MENU_ROLL_PID  = 1,
	PROG_STAT_MENU_PITCH_PID = 2,
	PROG_STAT_MENU_YAW_PID   = 3,
	PROG_STAT_MENU_EXIT      = 4,
	PROG_STAT_MENU_MAX       = 5,
};

enum {
	PROG_STAT_LINE1 = 0,
	PROG_STAT_LINE2 = 1,
	PROG_STAT_LINE3 = 2,
	PROG_STAT_LINE4 = 3,
	PROG_STAT_LINE5 = 4,
};

enum {
	PROG_STAT_LINE_INACTIVE = 0,
	PROG_STAT_LINE_ACTIVE   = 1,
};

typedef struct {
	int32_t  menu;
	int32_t  lastMenu;
	int32_t  menuCountdown;
	int32_t  menuChgTime;
	uint32_t line;
	uint32_t lineActive;
	uint32_t updateTime;
	uint32_t saveAt;
	float *value[3];
} program_status;

program_status programStatus;

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

void FillLuaPacket(void)
{
	//int32_t fastTrack = -1;
	uint32_t cursor = 0;
	static uint32_t counter=0;
	volatile char outString[6] = {};
	volatile uint32_t line, column;
	//x,y,id,data,data,data

	//default is to send a print command
	outString[0] = ID_CMD_PRINT;
	bzero(charMatrix, sizeof(charMatrix));
	if (!progMode)
	{
		strcpy(charMatrix[0], "RaceFlight One Menu");
		strcpy(charMatrix[1], "Place both sticks in");
		strcpy(charMatrix[2], "the bottom center to");
		strcpy(charMatrix[3], "enable program mode. :)");
	}
	else
	{

		if (programStatus.updateTime == 0)
		{
			//init
			vtxRequested.vtxBandChannel = vtxRecord.vtxBandChannel;
			vtxRequested.vtxBand        = vtxRecord.vtxBand;
			vtxRequested.vtxChannel     = vtxRecord.vtxChannel;
			vtxRequested.vtxPit         = vtxRecord.vtxPit;
			vtxRequested.vtxPower       = vtxRecord.vtxPower;
			programStatus.updateTime    = InlineMillis();
			programStatus.menu          = PROG_STAT_MENU_VTX;
			programStatus.lastMenu      = PROG_STAT_MENU_VTX;
			programStatus.menuCountdown = 5;
			programStatus.menuChgTime   = InlineMillis();
		}

		if (programStatus.menu != programStatus.lastMenu)
		{
			programStatus.lastMenu = programStatus.menu;
			programStatus.menuCountdown = 5;
			programStatus.menuChgTime   = InlineMillis();
		}

		if (programStatus.menuCountdown > 0)
		{
			programStatus.menuCountdown--;
			outString[0] = ID_CMD_ERASE; //send a clear command several times to make sure the radio gets it
		}
		else if (InlineMillis() - programStatus.updateTime > 250) //500 ms wait time between RX checks
		{
			if ( (programStatus.lineActive == PROG_STAT_LINE_INACTIVE) && (trueRcCommandF[PITCH] > 0.95) )
			{
				programStatus.line = InlineConstrainui(programStatus.line-1, 0, 4); //cursor up
				programStatus.updateTime = InlineMillis();
				counter = 24;
			}
			else if ( (programStatus.lineActive == PROG_STAT_LINE_INACTIVE) && (trueRcCommandF[PITCH] < -0.95) )
			{
				programStatus.line = InlineConstrainui(programStatus.line+1, 0, 4); //cursor down
				programStatus.updateTime = InlineMillis();
				counter = 24;
			}
			else if ( (programStatus.lineActive == PROG_STAT_LINE_ACTIVE) && (trueRcCommandF[PITCH] > 0.95) )
			{
				if (programStatus.menu == PROG_STAT_MENU_VTX)
				{
					switch(programStatus.line)
					{
						case 1: //mode
							vtxRequested.vtxBand    += 1;
							if (vtxRequested.vtxBand >= VTX_BAND_END)
							{
								vtxRequested.vtxBand = 0;
							}
							vtxRequested.vtxBandChannel = VtxBandAndChannelToBandChannel(vtxRequested.vtxBand, vtxRequested.vtxChannel);
							vtxRequested.vtxFrequency = VtxBandChannelToFrequency(vtxRequested.vtxBandChannel);
							break;
						case 2: //band
							vtxRequested.vtxChannel += 1;
							if (vtxRequested.vtxChannel >= VTX_CHANNEL_END)
							{
								vtxRequested.vtxChannel = 0;
							}
							vtxRequested.vtxBandChannel = VtxBandAndChannelToBandChannel(vtxRequested.vtxBand, vtxRequested.vtxChannel);
							vtxRequested.vtxFrequency = VtxBandChannelToFrequency(vtxRequested.vtxBandChannel);
							break;
						case 3: //channel
							vtxRequested.vtxPower   += 1;
							if (vtxRequested.vtxPower >= VTX_POWER_END)
							{
								vtxRequested.vtxPower = 0;
							}
							break;
						case 4: //set
							break;
					}
					programStatus.updateTime = InlineMillis();
				}
				else
				{
					//value up
					if ( (*programStatus.value[programStatus.line-1]) < 400 )
						(*programStatus.value[programStatus.line-1]) = (*programStatus.value[programStatus.line-1]) + 5.0f;
					else
						(*programStatus.value[programStatus.line-1]) = (*programStatus.value[programStatus.line-1]) + 50.0f;
					programStatus.updateTime = InlineMillis();
				}
			}
			else if ( (programStatus.lineActive == PROG_STAT_LINE_ACTIVE) && (trueRcCommandF[PITCH] < -0.95) )
			{
				if (programStatus.menu == PROG_STAT_MENU_VTX)
				{
					switch(programStatus.line)
					{
						case 1: //mode
							if (vtxRequested.vtxBand == 0)
							{
								vtxRequested.vtxBand = VTX_BAND_END;
							}
							vtxRequested.vtxBand    -= 1;
							vtxRequested.vtxBandChannel = VtxBandAndChannelToBandChannel(vtxRequested.vtxBand, vtxRequested.vtxChannel);
							vtxRequested.vtxFrequency = VtxBandChannelToFrequency(vtxRequested.vtxBandChannel);
							break;
						case 2: //band
							if (vtxRequested.vtxChannel == 0)
							{ //roll around
								vtxRequested.vtxChannel = VTX_CHANNEL_END;
							}
							vtxRequested.vtxChannel -= 1;
							vtxRequested.vtxBandChannel = VtxBandAndChannelToBandChannel(vtxRequested.vtxBand, vtxRequested.vtxChannel);
							vtxRequested.vtxFrequency = VtxBandChannelToFrequency(vtxRequested.vtxBandChannel);
							break;
						case 3: //channel
							vtxRequested.vtxPower   -= 1;
							if (vtxRequested.vtxPower >= VTX_POWER_END)
							{
								vtxRequested.vtxPower = 0;
							}
							break;
						case 4: //set
							break;
					}
					programStatus.updateTime = InlineMillis();
				}
				else
				{
					//value down
					if ( (*programStatus.value[programStatus.line-1]) < 400 )
						(*programStatus.value[programStatus.line-1]) = (*programStatus.value[programStatus.line-1]) - 10.0f;
					else
						(*programStatus.value[programStatus.line-1]) = (*programStatus.value[programStatus.line-1]) - 50.0f;
					programStatus.updateTime = InlineMillis();
				}
			}
			else if (trueRcCommandF[ROLL] > 0.95)
			{


				if (programStatus.line == 0) //we're on the menu bar
				{
					programStatus.menu++; //blindly increment and deincrement. Menu is clamped (rolls over) below
					programStatus.updateTime = InlineMillis();
				}
				else if (programStatus.menu == PROG_STAT_MENU_EXIT)
				{
					vtxRequested.vtxBandChannel = vtxRecord.vtxBandChannel;
					vtxRequested.vtxBand        = vtxRecord.vtxBand;
					vtxRequested.vtxChannel     = vtxRecord.vtxChannel;
					vtxRequested.vtxPit         = vtxRecord.vtxPit;
					vtxRequested.vtxPower       = vtxRecord.vtxPower;
					progMode=0;
					bzero(&programStatus,sizeof(program_status));
					return;
				}
				else if ( (programStatus.menu == PROG_STAT_MENU_VTX) && (programStatus.line == 4) )
				{
					progMode=0;
					bzero(&programStatus,sizeof(program_status));
					return;
				}
				else if (programStatus.line == 4) //save
				{
					//
					programStatus.saveAt = InlineMillis() + 4500;
					programStatus.lastMenu = programStatus.menu+1; //force a screen clear
				}
				else
				{
					programStatus.lineActive = PROG_STAT_LINE_ACTIVE; //cursor active
				}
				programStatus.updateTime = InlineMillis();
			}
			else if (trueRcCommandF[ROLL] < -0.95)
			{
				//we're on the menu bar
				if (programStatus.line == 0)
				{
					programStatus.menu--; //blindly increment and deincrement. Menu is clamped (rolls over) below
				}
				else if (programStatus.line == 4) //save
				{
					//Does Nothing
				}
				else
				{
					programStatus.lineActive = PROG_STAT_LINE_INACTIVE; //cursor inactive
				}
				programStatus.updateTime = InlineMillis();
			}

		}

		//clamp/rollover menu
		if (programStatus.menu >= PROG_STAT_MENU_MAX)
			programStatus.menu = 0;

		//clamp/rollover menu
		if (programStatus.menu < 0)
			programStatus.menu = PROG_STAT_MENU_MAX-1;

		//set buffer
		//set cursor, either editing or not editing a line
		switch (programStatus.lineActive)
		{
			case PROG_STAT_LINE_INACTIVE:
				cursor = '>';
				break;
			case PROG_STAT_LINE_ACTIVE:
				cursor = '*';
				break;
		}

		if (programStatus.saveAt)
		{
			if (InlineMillis() > programStatus.saveAt)
			{
				SaveConfig(ADDRESS_CONFIG_START);
				programStatus.saveAt = 0;
				programStatus.lastMenu = programStatus.menu+1; //force a screen clear
			}
			else
			{
				strcpy(charMatrix[5], "Saving");
				programStatus.lineActive = PROG_STAT_LINE_INACTIVE;
				programStatus.line = 0;
			}
		}
		else
		{
			//which menu are we on?
			switch (programStatus.menu)
			{
				case PROG_STAT_MENU_ROLL_PID:
					//fill roll pids
					strcpy(charMatrix[0], "RaceFlight One PIDs");
					strcpy(charMatrix[1], " Roll");
					strcpy(charMatrix[2], " Kp");
					strcpy(charMatrix[3], " Ki");
					strcpy(charMatrix[4], " Kd");
					strcpy(charMatrix[5], " Save");
					charMatrix[programStatus.line+1][0] = cursor;
					itoa(lrint(mainConfig.pidConfig[ROLL].kp), &charMatrix[2][4], 10);
					itoa(lrint(mainConfig.pidConfig[ROLL].ki), &charMatrix[3][4], 10);
					itoa(lrint(mainConfig.pidConfig[ROLL].kd), &charMatrix[4][4], 10);
					programStatus.value[0] = &mainConfig.pidConfig[ROLL].kp;
					programStatus.value[1] = &mainConfig.pidConfig[ROLL].ki;
					programStatus.value[2] = &mainConfig.pidConfig[ROLL].kd;
					break;
				case PROG_STAT_MENU_PITCH_PID:
					//fill pitch pids
					strcpy(charMatrix[0], "RaceFlight One PIDs");
					strcpy(charMatrix[1], " Pitch");
					strcpy(charMatrix[2], " Kp");
					strcpy(charMatrix[3], " Ki");
					strcpy(charMatrix[4], " Kd");
					strcpy(charMatrix[5], " Save");
					charMatrix[programStatus.line+1][0] = cursor;
					itoa(lrint(mainConfig.pidConfig[PITCH].kp), &charMatrix[2][4], 10);
					itoa(lrint(mainConfig.pidConfig[PITCH].ki), &charMatrix[3][4], 10);
					itoa(lrint(mainConfig.pidConfig[PITCH].kd), &charMatrix[4][4], 10);
					programStatus.value[0] = &mainConfig.pidConfig[PITCH].kp;
					programStatus.value[1] = &mainConfig.pidConfig[PITCH].ki;
					programStatus.value[2] = &mainConfig.pidConfig[PITCH].kd;
					break;
				case PROG_STAT_MENU_YAW_PID:
					//fill yaw pids
					strcpy(charMatrix[0], "RaceFlight One PIDs");
					strcpy(charMatrix[1], " Yaw");
					strcpy(charMatrix[2], " Kp");
					strcpy(charMatrix[3], " Ki");
					strcpy(charMatrix[4], " Kd");
					strcpy(charMatrix[5], " Save");
					charMatrix[programStatus.line+1][0] = cursor;
					itoa(lrint(mainConfig.pidConfig[YAW].kp), &charMatrix[2][4], 10);
					itoa(lrint(mainConfig.pidConfig[YAW].ki), &charMatrix[3][4], 10);
					itoa(lrint(mainConfig.pidConfig[YAW].kd), &charMatrix[4][4], 10);
					programStatus.value[0] = &mainConfig.pidConfig[YAW].kp;
					programStatus.value[1] = &mainConfig.pidConfig[YAW].ki;
					programStatus.value[2] = &mainConfig.pidConfig[YAW].kd;
					break;
				case PROG_STAT_MENU_VTX:
					if (vtxRecord.vtxDevice)
					{
						strcpy(charMatrix[0], "RaceFlight One VTX");
						strcpy(charMatrix[1], " VTX");
						strcpy(charMatrix[2], " Band");
						strcpy(charMatrix[3], " Channel ");
						strcpy(charMatrix[4], " Power");
						strcpy(charMatrix[5], " Set and Exit");
						switch(vtxRequested.vtxBand)
						{
							case 0:
								strcpy(charMatrix[2], " Band A");
								break;
							case 1:
								strcpy(charMatrix[2], " Band B");
								break;
							case 2:
								strcpy(charMatrix[2], " Band E");
								break;
							case 3:
								strcpy(charMatrix[2], " Band FatShark");
								break;
							case 4:
								strcpy(charMatrix[2], " Band Race");
								break;
							default:
								strcpy(charMatrix[2], " Band Unknown");
								break;
						}
						itoa(vtxRequested.vtxChannel+1, &charMatrix[3][9], 10);
						switch(vtxRequested.vtxPower)
						{
							case 0:
								strcpy(charMatrix[4], " Power 25mw");
								break;
							case 1:
								strcpy(charMatrix[4], " Power 200mw");
								break;
							case 2:
								strcpy(charMatrix[4], " Power 500mw");
								break;
							case 3:
								strcpy(charMatrix[4], " Power 800mw");
								break;
							default:
								strcpy(charMatrix[4], " Power Unknown");
								break;
						}
						charMatrix[programStatus.line+1][0] = cursor;
					}
					else
					{
						strcpy(charMatrix[0], "RaceFlight One VTX");
						strcpy(charMatrix[1], " VTX");
						strcpy(charMatrix[2], " No VTX detected :(");
						programStatus.line = InlineConstrainui(programStatus.line, 0, 0);
						charMatrix[programStatus.line+1][0] = cursor;
					}
					break;
				case PROG_STAT_MENU_EXIT:
					strcpy(charMatrix[0], "RaceFlight One Menu");
					strcpy(charMatrix[1], " Mode");
					strcpy(charMatrix[2], " Exit Prog Mode :D");
					strcpy(charMatrix[3], "");
					strcpy(charMatrix[4], "");
					programStatus.line = InlineConstrainui(programStatus.line, 0, 1);
					charMatrix[programStatus.line+1][0] = cursor;
					break;
			}
		}
	}


	for (uint32_t x=0;x<10;x++)
	{
		if (!progMode)
			break;

		//skip title at 4 seconds
		if ( (InlineMillis() - programStatus.menuChgTime) > 4000)
		{
			if (counter < 24)
			{
				counter = 24;
			}
		}

		line   = (counter / 24);
		column = (counter - (line * 24));
		//skip blank strings
		if ( strlen(charMatrix[line]+column) < 1)
		{
			counter += 4;
		}
		line   = (counter / 24);
		column = (counter - (line * 24));
		if ( !strcmp("    ", charMatrix[line]+column) )
		{
			counter += 4;
		}
		if (counter >= (24*6))
			counter = 0;//first line is the title bar
	}

	line   = (counter / 24);
	column = (counter - (line * 24));

	//outstring 0 is set above
	outString[1] = counter;
	outString[2] = charMatrix[line][column+0];
	outString[3] = charMatrix[line][column+1];
	outString[4] = charMatrix[line][column+2];
	outString[5] = charMatrix[line][column+3];


	counter += 4;
	if (counter >= (24*6))
		counter = 0;//first line is the title bar

	luaOutPacketOne = ( (outString[0] & 0xFF) | ((outString[1] & 0xFF) << 8) );
	luaOutPacketTwo = ( (outString[2] & 0xFF) | ((outString[3] & 0xFF) << 8) | ((outString[4] & 0xFF) << 16)  | ((outString[5] & 0xFF) << 24) );

}

void CheckIfSportReadyToSend(void)
{

	static uint32_t timeLastSent;
	uint32_t bytesReceived;
	if (IsSoftSerialLineIdle()) //soft serial?
	{
		//process timer buffer, s.port bit width is 17.35us and there are 10 bits in the byte including the start and stop frame
		bytesReceived = NewProcessSoftSerialBits(telemtryRxTimerBuffer, &telemtryRxTimerBufferIdx, telemtryRxBuffer, &telemtryRxBufferIdx, 17.36, 10, TBS_HANDLING_OFF);
		if(bytesReceived)
		{
			if ( (telemtryRxBuffer[0] == 0x7E) && (telemtryRxBuffer[1] == 0x1B) && (!progMode))
			{
				sendSmartPortAt = Micros() + 1000;
				PutSportIntoSendState(sbusActuator, 1);

			}
			else if ( (telemtryRxBuffer[0] == 0x7E) && (telemtryRxBuffer[1] == 0x0D) )
			{
				//25 ms limit to sending
				if (InlineMillis() - timeLastSent > 1)
				{
					timeLastSent = InlineMillis();
					FillLuaPacket();
					sendSmartPortLuaAt = Micros() + 1000;
					PutSportIntoSendState(sbusActuator, 1);
				}
			}
		}
	}
	else if ( (telemtryRxBuffer[0] == 0x7E) && (telemtryRxBuffer[1] == 0x1B) ) //normal serial?
	{
		sendSmartPortAt = Micros() + 1000; //send telemetry reply in 1.5 ms
	}
	else if ( (telemtryRxBuffer[0] == 0x7E) && (telemtryRxBuffer[1] == 0x0D) )
	{
		sendSmartPortLuaAt = Micros() + 1000; //send telemetry reply in 1.5 ms
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
			break;
		case 7:
			SmartPortCreatePacket(SPORT_FRAME_HEADER, CURR_FIRST_ID, (int32_t)(adcCurrent * 100), sPortPacket );
			break;
		case 8:
			SmartPortCreatePacket(SPORT_FRAME_HEADER, FUEL_FIRST_ID, (int32_t)(adcMAh), sPortPacket );
			sPortTelemCount = 0;
			break;
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

	bzero(charMatrix,sizeof(charMatrix));
	bzero(&programStatus,sizeof(program_status));

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

void DeInitSoftSport(void)
{
	switch(mainConfig.telemConfig.telemSport)
	{
		case TELEM_ACTUATOR1:
		case TELEM_ACTUATOR2:
		case TELEM_ACTUATOR3:
		case TELEM_ACTUATOR4:
		case TELEM_ACTUATOR5:
		case TELEM_ACTUATOR6:
		case TELEM_ACTUATOR7:
		case TELEM_ACTUATOR8:
			EXTI_Deinit(ports[sbusActuator.port], sbusActuator.pin, sbusActuator.EXTIn);
			break;
	}
}

void InitAllSport(void)
{
	switch(mainConfig.telemConfig.telemSport)
	{
		case TELEM_ACTUATOR1:
		case TELEM_ACTUATOR2:
		case TELEM_ACTUATOR3:
		case TELEM_ACTUATOR4:
		case TELEM_ACTUATOR5:
		case TELEM_ACTUATOR6:
		case TELEM_ACTUATOR7:
		case TELEM_ACTUATOR8:
			InitSoftSport();
			break;
		case TELEM_USART1:
			InitSport(ENUM_USART1);
			break;
		case TELEM_USART2:
			InitSport(ENUM_USART2);
			break;
		case TELEM_USART3:
			InitSport(ENUM_USART3);
			break;
		case TELEM_USART4:
			InitSport(ENUM_USART4);
			break;
		case TELEM_USART5:
			InitSport(ENUM_USART5);
			break;
		case TELEM_USART6:
			InitSport(ENUM_USART6);
			break;
		default:
			break;
	}
	telemtryRxTimerBufferIdx = 0;
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
				///todonow changesmartPortExti =
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
		else if ( (!progMode && (timeNow - (float)telemtryRxTimerBuffer[telemtryRxTimerBufferIdx-1]) > (200)) )
		{
			PutSportIntoReceiveState(sbusActuator, 1);
			return(0);
		}
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
			EXTI_Deinit(ports[sbusActuator.port], sbusActuator.pin, sbusActuator.EXTIn);
			if (boardArmed)
				noiseCounter++;
		}
		__HAL_GPIO_EXTI_CLEAR_IT(sbusActuator.pin);
	}
	else
	{
		EXTI_Deinit(ports[sbusActuator.port], sbusActuator.pin, sbusActuator.EXTIn);
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

	if (noiseCounter>10)
		return;

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