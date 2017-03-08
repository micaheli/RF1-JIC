#include "includes.h"

//this doesn't really belong here
volatile uint8_t tInBuffer[HID_EPIN_SIZE], tOutBuffer[HID_EPOUT_SIZE-1];

uint32_t skipTaskHandlePcComm   = 0;
volatile uint32_t errorMask              = 0;

//soft serial buffer handling. TODO: make a structure
volatile uint32_t softSerialEnabled = 0;
volatile uint32_t softSerialBuf[2][SOFT_SERIAL_BIT_TIME_ARRAY_SIZE];
volatile uint32_t softSerialInd[2];
volatile uint32_t softSerialCurBuf;
volatile uint32_t softSerialLastByteProcessedLocation;
volatile uint32_t softSerialSwitchBuffer;

uint8_t    proccesedSoftSerial[25]; //25 byte buffer enough?
uint32_t   proccesedSoftSerialIdx = 0;
uint32_t   softSerialLineIdleSensed = 0;
uint32_t   lastBitFound = 0;

static void TaskProcessSoftSerial(void);
static void TaskTelemtry(void);
static void TaskWizard(void);
static void TaskHandlePcComm(void);
static void TaskLed(void);
static void TaskBuzzer(void);
static void TaskAdc(void);
static void TaskCheckVtx(void);
static void TaskCheckDelayedArming(void);
static void TaskProcessArmingStructure(void);

inline void Scheduler(int32_t count)
{

	switch (count) {

		case 0:
			TaskHandlePcComm();
			break;
		case 1:
			TaskLed();
			break;
		case 2:
			TaskBuzzer();
			break;
		case 3:
			TaskAdc();
			break;
		case 4:
			TaskProcessSoftSerial();
			break;
		case 5:
			TaskTelemtry();
			break;
		case 6:
			TaskWizard();
			break;
		case 7:
			TaskCheckVtx();
			break;
		case 8:
			TaskCheckDelayedArming();
			break;
		case 9:
			TaskProcessArmingStructure();
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		default:
			break;

	}

}

inline void TaskProcessArmingStructure(void)
{
	ProcessArmingStructure();
}

inline void TaskCheckDelayedArming(void)
{
	//TODO: Hack to make this lua crap work
	if ( ( luaPacketPendingTime ) || ( (transmitDataBufferIdx > 0) && (transmitDataBufferSent <= (transmitDataBufferIdx - 1)) ) )
	{
		return;
	}

	//handles VTX enabling as well
	if(armBoardAt)
	{
		//delayed arming window of 150 ms.
		if ( (InlineMillis() > armBoardAt) && ((InlineMillis() - armBoardAt) < 150) )
		{
			if( mainConfig.telemConfig.telemSmartAudio && !ModeSet(M_VTXON) && !vtxEnabled )
			{
				//only try turning on the VTX once per arming
				vtxEnabled = 1;
				TurnOnVtx();
			}
			ArmBoard();
			armBoardAt = 0;
		}
	}
	if (!armBoardAt && !boardArmed && mainConfig.telemConfig.telemSmartAudio && !ModeSet(M_VTXON) && vtxEnabled)
		vtxEnabled = 0;
}

inline void TaskCheckVtx(void)
{
	static uint8_t vtxChannel   = 0;
	static VTX_BAND vtxBand     = 0;
	static VTX_POWER vtxPower   = 0;
	//static VTX_REGION vtxRegion = 0;
	static VTX_PIT vtxPit       = 0;

	//TODO: Hack to make this lua crap work
	if ( ( luaPacketPendingTime ) || ( (transmitDataBufferIdx > 0) && (transmitDataBufferSent <= (transmitDataBufferIdx - 1)) ) )
	{
		return;
	}

	//don't do this task unless board is disarmed
	if (boardArmed)
		return;

	if (!mainConfig.telemConfig.telemSmartAudio)
		return;

	if (ModeSet(M_VTXON) && ModeActive(M_VTXON) && !vtxEnabled)
	{
		//only try turning on the VTX once per mode enabling
		vtxEnabled = 1;
		TurnOnVtx();
	}
	else if (ModeSet(M_VTXON) && !ModeActive(M_VTXON) && vtxEnabled)
	{
		vtxEnabled = 0; //we can't turn off the VTX using smart audio, but we can reset the flag
	}

	if ( (vtxData.vtxBand != vtxBand) || (vtxData.vtxChannel != vtxChannel) )
	{ //did vtxChannel change? If so, send change to SA VTX
		SmartAudioSetChannelBlocking( SpektrumBandAndChannelToChannel(vtxData.vtxBand, vtxData.vtxChannel) );
		if (vtxData.vtxPit == ACTIVE)
		{
			TurnOnVtx();
			vtxPit = vtxData.vtxPit;
		}
		vtxBand    = vtxData.vtxBand;
		vtxChannel = vtxData.vtxChannel;
	}

	if ( (vtxData.vtxPower != vtxPower) || (vtxData.vtxPit != vtxPit) )
	{
		SmartAudioSetPowerBlocking( vtxData.vtxPower );
		if (vtxData.vtxPit == ACTIVE)
		{
			vtxPit = vtxData.vtxPit;
			TurnOnVtx();
		}
		vtxPower = vtxData.vtxPower;
		vtxPit   = vtxData.vtxPit;
	}

}

inline void TaskAdc(void)
{
	//TODO: Hack to make this lua crap work
	if ( ( luaPacketPendingTime ) || ( (transmitDataBufferIdx > 0) && (transmitDataBufferSent <= (transmitDataBufferIdx - 1)) ) )
	{
		return;
	}
	PollAdc();
	CheckBatteryCellCount();
}

inline void TaskProcessSoftSerial(void)
{

	 if (oneWireActive)
		 FeedTheDog();
}

inline void TaskWizard(void) {

	switch(wizardStatus.currentWizard)
	{

		case WIZ_RC:
			if (wizardStatus.currentStep == 1) //step three needs to be polled by user/gui
				HandleWizRc();
				break;
		case 0:
		default:
			return;
	}
}

inline void TaskTelemtry(void)
{
	ProcessTelemtry();
}

inline void TaskHandlePcComm(void)
{
	uint32_t x;

	if (skipTaskHandlePcComm)
		return;

	if (tOutBuffer[0]==2)
	{ //we have a usb report
		ProcessCommand((char *)tOutBuffer);
		for (x=0;x<(HID_EPOUT_SIZE-1);x++)
			tOutBuffer[x] = 0;
	}

}

inline void TaskLed(void)
{
	UpdateLeds(); //update status LEDs

	//TODO: Hack to make this lua crap work
	if ( ( luaPacketPendingTime ) || ( (transmitDataBufferIdx > 0) && (transmitDataBufferSent <= (transmitDataBufferIdx - 1)) ) )
	{
		return;
	}
	UpdateWs2812Leds();
}

inline void TaskBuzzer(void)
{
	UpdateBuzzer();
}

void ErrorHandler(uint32_t error)
{
	errorMask |= (error);

	switch (error)
	{
		case TIMER_INPUT_INIT_FAILIURE:
		case ADC_INIT_FAILIURE:
		case ADC_DMA_INIT_FAILIURE:
		case MSP_DMA_GYRO_RX_INIT_FAILIURE:
		case MSP_DMA_GYRO_TX_INIT_FAILIURE:
		case MSP_DMA_SPI_RX_INIT_FAILIURE:
		case MSP_DMA_SPI_TX_INIT_FAILIURE:
			//ping warning to user here, may not a valid reason to crash the board though
			return;
			break;
		case SERIAL_HALF_DUPLEX_INIT_FAILURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case SERIAL_INIT_FAILURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case FLASH_SPI_INIT_FAILIURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case WS2812_LED_INIT_FAILIURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case GYRO_SPI_INIT_FAILIURE: //gyro failed to init. Can't fly like this.
		case GYRO_INIT_FAILIURE: //gyro failed to init. Can't fly like this.
		case GYRO_SETUP_COMMUNICATION_FAILIURE: //gyro init success, but setting up register failed. Can't fly like this.
			return;
			break;
		case HARD_FAULT:  //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		case MEM_FAULT:   //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		case BUS_FAULT:   //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		case USAGE_FAULT: //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		default:
			break;
	}

	//bad errors will fall through here
	ZeroActuators(32000);

    while (1)
    {
		DoLed(0, 1);
		DoLed(1, 0);
		simpleDelay_ASM(50000);
		DoLed(0, 0);
		DoLed(1, 1);
		simpleDelay_ASM(75000);
    	ZeroActuators(10);
    }

}
