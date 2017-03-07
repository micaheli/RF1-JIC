#include "includes.h"



smart_audio_vtx_structure smartAudioVtxStructure;
uint8_t                   smartAudioRxBuffer[12];
uint8_t                   smartAudioTxBuffer[12];
uint32_t                  smartAudioUsartSerialNumber = 999;
uint32_t                  vtxEnabled = 0;
motor_type                smartAudioActuator;


static uint32_t SmartAudioReceiveBlocking(uint32_t timeoutMs, uint32_t rxAmount);
static uint8_t  ShiftSmartAudioCommand(uint8_t data);
static void     FillVtxRecord(void);


static void FillVtxRecord(void)
{

	if (smartAudioVtxStructure.version == SM_VERSION_1)
	{
		vtxRecord.vtxDevice = VTX_DEVICE_SMARTV1;
	}
	else if (smartAudioVtxStructure.version == SM_VERSION_2)
	{
		vtxRecord.vtxDevice = VTX_DEVICE_SMARTV2;
	}
	else
	{
		vtxRecord.vtxDevice = VTX_DEVICE_NONE;
	}

	vtxRecord.vtxBandChannel = smartAudioVtxStructure.channel;
	ChannelToBandAndChannel(vtxRecord.vtxBandChannel, &vtxRecord.vtxBand, &vtxRecord.vtxChannel)

	vtxRecord.vtxPower = smartAudioVtxStructure.powerLevel;
	if ( BITMASK_CHECK(smartAudioVtxStructure.opMode, SM_OPMODE_PM) )
	{
		vtxRecord.vtxPower = VTX_MODE_ACTIVE;
	}
	else
	{
		vtxRecord.vtxPit = VTX_MODE_PIT;
	}

}

uint32_t InitSoftSmartAudio(void)
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

uint32_t InitSmartAudio(uint32_t usartNumber)
{

	uint32_t x;

	if (!mainConfig.telemConfig.telemSmartAudio)
		return(0);

	if (boardArmed)
		return(0);

	for (x=0;x<4;x++)
		rfVtxRxBuffer[x]=0;

	//use manual protocol to setup s.port.
	board.serials[usartNumber].enabled    = 1;
	board.serials[usartNumber].Protocol   = USING_SMARTAUDIO;

	board.serials[usartNumber].BaudRate   = 4900;//should be 4800bps 1 Start bit and 2 Stop bit, but the VTX drifts a lot
	board.serials[usartNumber].WordLength = UART_WORDLENGTH_8B;
	board.serials[usartNumber].StopBits   = UART_STOPBITS_2;
	board.serials[usartNumber].Parity     = UART_PARITY_NONE;
	board.serials[usartNumber].HwFlowCtl  = UART_HWCONTROL_NONE;
	board.serials[usartNumber].Mode       = UART_MODE_TX_RX;

	board.serials[usartNumber].serialTxInverted = 0;
	board.serials[usartNumber].serialRxInverted = 0;
	board.serials[usartNumber].FrameSize = 4;
	board.serials[usartNumber].Pull = GPIO_PULLDOWN;

	board.dmasSerial[board.serials[usartNumber].TXDma].enabled  = 0;
	board.dmasSerial[board.serials[usartNumber].RXDma].enabled  = 0;

	board.serials[usartNumber].RXPin  = board.serials[usartNumber].TXPin;
	board.serials[usartNumber].RXPort = board.serials[usartNumber].TXPort;

	UsartDeInit(usartNumber); //deinits serial and associated pins and DMAs
	UsartInit(usartNumber);   //inits serial and associated pins and DMAs

	smartAudioUsartSerialNumber = usartNumber;

	SmartAudioGetSettingsBlocking(150); //always get settings when initializing
	DelayMs(20);
	bzero(&smartAudioVtxStructure, sizeof(smart_audio_vtx_structure));
	return(SmartAudioGetSettingsBlocking(150)); //always get settings when initializing, do it twice since the first time the VTX talks isn't usually good data.

}

uint32_t TurnOnVtx(void)
{
	if (!mainConfig.telemConfig.telemSmartAudio)
		return(0);

	if (smartAudioUsartSerialNumber == 999)
	{
		InitSmartAudio(ENUM_USART3);
		DelayMs(20);
		if (SmartAudioSetOpModeBlocking( SM_SET_OPMODE_DIS_PMOR ))
		{
			DeInitSmartAudio();
			return(1);
		}
		else
		{
			DeInitSmartAudio();
			return(0);
		}
	}
	else
	{
		if (SmartAudioSetOpModeBlocking( SM_SET_OPMODE_DIS_PMOR ))
		{
			DeInitSmartAudio();
			return(1);
		}
		else
		{
			DeInitSmartAudio();
			return(0);
		}
	}
}

/*
  	VTX_CH_A1 = 0,
	VTX_CH_A2 = 1,
	VTX_CH_A3 = 2,
	VTX_CH_A4 = 3,
	VTX_CH_A5 = 4,
	VTX_CH_A6 = 5,
	VTX_CH_A7 = 6,
	VTX_CH_A8 = 7,
	VTX_CH_B1 = 8,
	VTX_CH_B2 = 9,
	VTX_CH_B3 = 10,
	VTX_CH_B4 = 11,
	VTX_CH_B5 = 12,
	VTX_CH_B6 = 13,
	VTX_CH_B7 = 14,
	VTX_CH_B8 = 15,
	VTX_CH_E1 = 16,
	VTX_CH_E2 = 17,
	VTX_CH_E3 = 18,
	VTX_CH_E4 = 19,
	VTX_CH_E5 = 20,
	VTX_CH_E6 = 21,
	VTX_CH_E7 = 22,
	VTX_CH_E8 = 23,
	VTX_CH_F1 = 24,
	VTX_CH_F2 = 25,
	VTX_CH_F3 = 26,
	VTX_CH_F4 = 27,
	VTX_CH_F5 = 28,
	VTX_CH_F6 = 29,
	VTX_CH_F7 = 30,
	VTX_CH_F8 = 31,
	VTX_CH_R1 = 32,
	VTX_CH_R2 = 33,
	VTX_CH_R3 = 34,
	VTX_CH_R4 = 35,
	VTX_CH_R5 = 36,
	VTX_CH_R6 = 37,
	VTX_CH_R7 = 38,
	VTX_CH_R8 = 39,
 */
uint32_t SpektrumBandAndChannelToChannel(VTX_BAND vtxBand, uint8_t channel)
{
	switch(vtxBand)
	{
		case VTX_BAND_A:
			return((uint32_t)channel);
			break;
		case VTX_BAND_B:
			return((uint32_t)channel+(uint32_t)7);
			break;
		case VTX_BAND_E:
			return((uint32_t)channel+(uint32_t)15);
			break;
		case VTX_BAND_FATSHARK:
			return((uint32_t)channel+(uint32_t)23);
			break;
		case VTX_BAND_RACEBAND:
			return((uint32_t)channel+(uint32_t)31);
			break;
	}
	return(0);
}

void ChannelToBandAndChannel(uint32_t inChannel, uint32_t *vtxBand, uint32_t *channel)
{

	uint32_t bandMultiplier;

	bandMultiplier = channel / 8;

	(*vtxBand) = bandMultiplier;
	(*channel) = inChannel - (bandMultiplier * 8) + 1;

}

void DeInitSmartAudio(void)
{
	if (smartAudioUsartSerialNumber != 999)
	{
		UsartDeInit(smartAudioUsartSerialNumber);
		smartAudioUsartSerialNumber = 999;
	}
}

static uint8_t ShiftSmartAudioCommand(uint8_t data)
{
	return( ((data) << 1) | 1 );
}

static uint32_t SmartAudioReceiveBlocking(uint32_t timeoutMs, uint32_t rxAmount)
{
	uint8_t crcCheck;
	uint8_t crcReturned;
	uint8_t returnLength;

	if (boardArmed)
		return(0);

	bzero(smartAudioRxBuffer, sizeof(smartAudioRxBuffer));

	if(HAL_UART_Receive(&uartHandles[board.serials[smartAudioUsartSerialNumber].usartHandle], (uint8_t *)smartAudioRxBuffer, rxAmount, timeoutMs) != HAL_OK)
	{
		returnLength = smartAudioRxBuffer[4];

		if ( (returnLength > 0) && (returnLength < 7) )
		{
			crcReturned = smartAudioRxBuffer[4+returnLength];
		}
		else
		{
			return(0);
		}

	}
	else
	{
		returnLength = smartAudioRxBuffer[4];

		if ( (returnLength > 0) && (returnLength < 7) )
		{
			crcReturned = smartAudioRxBuffer[4+returnLength];
		}
	}

	crcCheck = SmCrc8(&smartAudioRxBuffer[3], 1+returnLength);

	if (crcCheck == crcReturned)
	{
		return(1);
	}

	return(0);
}

uint32_t SmartAudioSetOpModeBlocking(uint32_t mask)
{

	uint32_t timeoutMs = 150;

	if (boardArmed)
		return(0);

	if (smartAudioUsartSerialNumber == 999)
	{
		if (InitSmartAudio(mainConfig.telemConfig.telemSmartAudio))
			DelayMs(20);
		else
			return(0);
	}

	smartAudioTxBuffer[0] = 0x00;
	smartAudioTxBuffer[1] = SM_START_CODE1;
	smartAudioTxBuffer[2] = SM_START_CODE2;
	smartAudioTxBuffer[3] = ShiftSmartAudioCommand(SM_SET_OPERATION_MODE);
	smartAudioTxBuffer[4] = 0x01;
	smartAudioTxBuffer[5] = (uint8_t)mask;
	smartAudioTxBuffer[6] = SmCrc8(&smartAudioTxBuffer[1], 5);

	if(HAL_UART_Transmit(&uartHandles[board.serials[smartAudioUsartSerialNumber].usartHandle], (uint8_t*)smartAudioTxBuffer, 7, timeoutMs)!= HAL_OK)
	{
		return(0);
	}

	DelayMs(20);

	if ( SmartAudioReceiveBlocking(timeoutMs, 8) )
	{
		smartAudioVtxStructure.opMode = (uint8_t)mask;
		return(1);
	}

	return(0);
}

uint32_t SmartAudioGetSettingsBlocking(uint32_t timeoutMs)
{

	if (boardArmed)
		return(0);

	if (smartAudioUsartSerialNumber == 999)
	{
		if (InitSmartAudio(mainConfig.telemConfig.telemSmartAudio))
			DelayMs(20);
		else
			return(0);
	}

	smartAudioTxBuffer[0] = 0x00;
	smartAudioTxBuffer[1] = SM_START_CODE1;
	smartAudioTxBuffer[2] = SM_START_CODE2;
	smartAudioTxBuffer[3] = ShiftSmartAudioCommand(SM_GET_SETTINGS);
	smartAudioTxBuffer[4] = 0x00;
	smartAudioTxBuffer[5] = SmCrc8(&smartAudioTxBuffer[1], 4);

	if(HAL_UART_Transmit(&uartHandles[board.serials[smartAudioUsartSerialNumber].usartHandle], (uint8_t*)smartAudioTxBuffer, 6, timeoutMs)!= HAL_OK)
	{
		return(0);
	}

	DelayMs(20);

	if ( SmartAudioReceiveBlocking(timeoutMs, 11) )
	{
		smartAudioVtxStructure.version    = smartAudioRxBuffer[3];
		smartAudioVtxStructure.channel    = smartAudioRxBuffer[5];
		smartAudioVtxStructure.powerLevel = smartAudioRxBuffer[6];
		smartAudioVtxStructure.opMode     = smartAudioRxBuffer[7];
		smartAudioVtxStructure.frequency  = ((smartAudioRxBuffer[8] << 8) | smartAudioRxBuffer[9]);
		return(1);
	}

	return(0);

}

uint32_t SmartAudioSetPowerBlocking(uint32_t powerLevel)
{

	uint32_t timeoutMs = 150;
	uint8_t powerNumber;


	if (boardArmed)
		return(0);

	if (smartAudioUsartSerialNumber == 999)
	{
		if (InitSmartAudio(mainConfig.telemConfig.telemSmartAudio))
			DelayMs(20);
		else
			return(0);
	}

	powerNumber = 99;

	switch(smartAudioVtxStructure.version)
	{
		case SM_VERSION_1:
			switch(powerLevel)
			{
				case VTX_POWER_025MW:
					powerNumber = 7;
					break;
				case VTX_POWER_200MW:
					powerNumber = 16;
					break;
				case VTX_POWER_500MW:
					powerNumber = 25;
					break;
				case VTX_POWER_800MW:
					powerNumber = 40;
					break;
			}
			break;
		case SM_VERSION_2:
			switch(powerLevel)
			{
				case VTX_POWER_025MW:
					powerNumber = 0;
					break;
				case VTX_POWER_200MW:
					powerNumber = 1;
					break;
				case VTX_POWER_500MW:
					powerNumber = 2;
					break;
				case VTX_POWER_800MW:
					powerNumber = 3;
					break;
			}
			break;
	}

	if (powerNumber != 99)
	{
		smartAudioTxBuffer[0] = 0x00;
		smartAudioTxBuffer[1] = SM_START_CODE1;
		smartAudioTxBuffer[2] = SM_START_CODE2;
		smartAudioTxBuffer[3] = ShiftSmartAudioCommand(SM_SET_POWER);
		smartAudioTxBuffer[4] = 0x01;
		smartAudioTxBuffer[5] = (uint8_t)powerNumber;
		smartAudioTxBuffer[6] = SmCrc8(&smartAudioTxBuffer[1], 5);

		if(HAL_UART_Transmit(&uartHandles[board.serials[smartAudioUsartSerialNumber].usartHandle], (uint8_t*)smartAudioTxBuffer, 7, timeoutMs)!= HAL_OK)
		{
			return(0);
		}

		DelayMs(20);

		if ( SmartAudioReceiveBlocking(timeoutMs, 8) )
		{
			smartAudioVtxStructure.powerLevel = (uint8_t)powerNumber;
			return(1);
		}

	}

	return(0);
}

uint32_t SmartAudioSetChannelBlocking(uint32_t channel)
{

	uint32_t timeoutMs = 150;

	if (boardArmed)
		return(0);

	if (smartAudioUsartSerialNumber == 999)
	{
		if (InitSmartAudio(mainConfig.telemConfig.telemSmartAudio))
			DelayMs(20);
		else
			return(0);
	}

	smartAudioTxBuffer[0] = 0x00;
	smartAudioTxBuffer[1] = SM_START_CODE1;
	smartAudioTxBuffer[2] = SM_START_CODE2;
	smartAudioTxBuffer[3] = ShiftSmartAudioCommand(SM_SET_CHANNEL);
	smartAudioTxBuffer[4] = 0x01;
	smartAudioTxBuffer[5] = (uint8_t)channel;
	smartAudioTxBuffer[6] = SmCrc8(&smartAudioTxBuffer[1], 5);

	if(HAL_UART_Transmit(&uartHandles[board.serials[smartAudioUsartSerialNumber].usartHandle], (uint8_t*)smartAudioTxBuffer, 7, timeoutMs)!= HAL_OK)
	{
		return(0);
	}

	DelayMs(20);

	if ( SmartAudioReceiveBlocking(timeoutMs, 8) )
	{
		smartAudioVtxStructure.channel = (uint8_t)channel;
		return(1);
	}

	return(0);
}

/*
uint32_t RfVtxOff(void)
{
	vtxOutPacket[0] = 0x55;
	vtxOutPacket[1] = 0xAA;
	vtxOutPacket[2] = 0x03;
	vtxOutPacket[3] = 0x00;
	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( board.serials[serialNumber].enabled )
		{
			if (board.serials[serialNumber].Protocol == USING_RFVTX)
			{
				HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)vtxOutPacket, 4);
				return(1);
			}
		}
	}
	return(0);
}

uint32_t RfVtxBaud(void)
{
	vtxOutPacket[0] = 0x55;
	vtxOutPacket[1] = 0xAA;
	vtxOutPacket[2] = 0x04;
	vtxOutPacket[3] = 0x01;
	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( board.serials[serialNumber].enabled )
		{
			if (board.serials[serialNumber].Protocol == USING_RFVTX)
			{
				HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)vtxOutPacket, 4);
				return(1);
			}
		}
	}
	return(0);
}


uint32_t RfVtxOn25(void)
{
	vtxOutPacket[0] = 0x55;
	vtxOutPacket[1] = 0xAA;
	vtxOutPacket[2] = 0x03;
	vtxOutPacket[3] = 0x01;
	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( board.serials[serialNumber].enabled )
		{
			if (board.serials[serialNumber].Protocol == USING_RFVTX)
			{
				HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)vtxOutPacket, 4);
				return(1);
			}
		}
	}
	return(0);
}

uint32_t RfVtxOn200(void)
{
	vtxOutPacket[0] = 0x55;
	vtxOutPacket[1] = 0xAA;
	vtxOutPacket[2] = 0x03;
	vtxOutPacket[3] = 0x02;
	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( board.serials[serialNumber].enabled )
		{
			if (board.serials[serialNumber].Protocol == USING_RFVTX)
			{
				HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)vtxOutPacket, 4);
				return(1);
			}
		}
	}
	return(0);
}


uint32_t RfVtxBand(uint32_t band)
{
	vtxOutPacket[0] = 0x55;
	vtxOutPacket[1] = 0xAA;
	vtxOutPacket[2] = 0x01;
	vtxOutPacket[3] = (uint8_t)band;
	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( board.serials[serialNumber].enabled )
		{
			if (board.serials[serialNumber].Protocol == USING_RFVTX)
			{
				HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)vtxOutPacket, 4);
				return(1);
			}
		}
	}
	return(0);
}

uint32_t RfVtxChannel(uint32_t channel)
{
	vtxOutPacket[0] = 0x55;
	vtxOutPacket[1] = 0xAA;
	vtxOutPacket[2] = 0x02;
	vtxOutPacket[3] = (uint8_t)channel;
	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( board.serials[serialNumber].enabled )
		{
			if (board.serials[serialNumber].Protocol == USING_RFVTX)
			{
				HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)vtxOutPacket, 4);
				return(1);
			}
		}
	}
	return(0);
}
*/
