#include "includes.h"

float trueRcCommandF[MAXCHANNELS];     //4 sticks. range is -1 to 1, directly related to stick position
float curvedRcCommandF[MAXCHANNELS];   //4 sticks. range is -1 to 1, this is the rcCommand after the curve is applied
float smoothedRcCommandF[MAXCHANNELS]; //4 sticks. range is -1 to 1, this is the smoothed rcCommand
volatile unsigned char isRxDataNew;
volatile uint32_t disarmCount = 0, latchFirstArm = 0;

static uint32_t packetTime = 11;

uint32_t PreArmFilterCheck = 0;
uint32_t activeFailsafe = 0;
uint32_t failsafeHappend = 0;

uint32_t rxDataRaw[MAXCHANNELS];
uint32_t rxData[MAXCHANNELS];
volatile float maxFlopRate[3];
volatile float maxKissRate[3];

uint32_t skipRxMap = 0;
uint32_t progMode  = 0;
uint32_t progTimer = 0;
uint32_t ppmPin    = 99;
volatile uint32_t armCheckLatch = 0;

#define PPM_SYNC_MINIMUM_US 4000
#define PPM_BUFFER_SIZE 25
#define PPM_CHANNELS 8
uint32_t ppmBufferIdx = 0;
uint32_t ppmBuffer[PPM_BUFFER_SIZE];
uint32_t ppmData[PPM_CHANNELS];

// 2048 resolution
#define SPEKTRUM_FRAME_SIZE 16
uint32_t spektrumChannelShift = 3;
uint32_t spektrumChannelMask  = 0x07;

static rx_calibration_records rxCalibrationRecords[3];
static void ProcessPpmPacket(uint32_t ppmBuffer2[], uint32_t *ppmBufferIdx);

static void checkRxPreArmCalibration(void);
static float GetKissMaxRates(float rcCommand, uint32_t axis);
static float GetFlopMaxRates(float rcCommand, uint32_t axis);

//check
static void checkRxPreArmCalibration(void)
{
	uint32_t axis;
	uint32_t y;
	uint32_t z;
	uint32_t highestCount;

	for (axis = 0;axis<3;axis++)
	{
		highestCount=0;
		//if pitch is near center, we add it to array
		//mainConfig.rcControlsConfig.midRc[ChannelMap(axis)]
		//cruiser
		if ( ABS((int32_t)rxData[axis] - (int32_t)mainConfig.rcControlsConfig.midRc[axis]) < 50 )
		{
			z = 0;
			//check each value for
			for (y=0;y<RX_CHECK_AMOUNT;y++)
			{
				//
				if (rxCalibrationRecords[axis].rxCalibrationRecord[y].dataValue == rxData[axis])
				{
					highestCount = rxCalibrationRecords[axis].rxCalibrationRecord[y].timesOccurred;
					rxCalibrationRecords[axis].rxCalibrationRecord[y].timesOccurred++;
					z++;
				}

				//find and record the highest value
				if (highestCount < rxCalibrationRecords[axis].rxCalibrationRecord[y].timesOccurred)
				{
					//set the dataValue with the
					highestCount = rxCalibrationRecords[axis].rxCalibrationRecord[y].timesOccurred;
					rxCalibrationRecords[axis].highestDataValue = rxCalibrationRecords[axis].rxCalibrationRecord[y].dataValue;
				}
			}
			if (!z)
			{
				for (y=0;y<RX_CHECK_AMOUNT;y++)
				{
					if (!rxCalibrationRecords[axis].rxCalibrationRecord[y].dataValue)
					{
						rxCalibrationRecords[axis].rxCalibrationRecord[y].dataValue     = rxData[axis];
						rxCalibrationRecords[axis].rxCalibrationRecord[y].timesOccurred = 1;
						break;
					}
				}
			}
		}
	}
}

SPM_VTX_DATA vtxData;

#define SBUS_FRAME_SIZE 25
#define SBUS_FRAME_LOSS_FLAG (1 << 2)
#define SBUS_FAILSAFE_FLAG (1 << 3)
#define SBUS_STARTBYTE         0x0f
#define SBUS_ENDBYTE           0x00

typedef struct {
	uint8_t syncByte;
	unsigned int chan0  : 11;
	unsigned int chan1  : 11;
	unsigned int chan2  : 11;
	unsigned int chan3  : 11;
	unsigned int chan4  : 11;
	unsigned int chan5  : 11;
	unsigned int chan6  : 11;
	unsigned int chan7  : 11;
	unsigned int chan8  : 11;
	unsigned int chan9  : 11;
	unsigned int chan10 : 11;
	unsigned int chan11 : 11;
	unsigned int chan12 : 11;
	unsigned int chan13 : 11;
	unsigned int chan14 : 11;
	unsigned int chan15 : 11;
	uint8_t flags;
	uint8_t endByte;
} __attribute__ ((__packed__)) sbusFrame_t;


//uint32_t tempData[MAXCHANNELS];

unsigned char copiedBufferData[RXBUFFERSIZE];

volatile uint32_t rx_timeout=0;
uint32_t spekPhase=1;
uint32_t ignoreEcho = 0;

static uint16_t CRC16(uint16_t crc, uint8_t value);




#define CRC_POLYNOME 0x1021
/*******************************************************************************
* Function Name : CRC16
* Description : crc calculation, adds a 8 bit unsigned to 16 bit crc
*******************************************************************************/
static uint16_t CRC16(uint16_t crc, uint8_t value)
{
	uint8_t i;
	crc = crc ^ (int16_t)value<<8;

	for(i=0; i<8; i++)
	{
		if (crc & 0x8000)
			crc = (crc << 1) ^ CRC_POLYNOME;
		else
			crc = (crc << 1);
	}

	return crc;
}




inline void CheckFailsafe(void)
{
	rx_timeout++;

	FeedTheDog(); //resets IWDG time to 0. This tells the timer the board is running.

	if ((boardArmed) && ( (rx_timeout > 500) || (ModeActive(M_FAILSAFE)) ) )
	{
		failsafeHappend = 1;
		buzzerStatus.status = STATE_BUZZER_FAILSAFE;
		DisarmBoard();
		ZeroActuators(32000); //immediately set actuators to disarmed position.
	}

	//make sure buzzer mode doesn't overwrite failsafe buzzer
	if ( ModeActive(M_BUZZER) && (buzzerStatus.status != STATE_BUZZER_FAILSAFE) )
	{
		buzzerStatus.status = STATE_BUZZER_ON;
	}
	else if ( ModeSet(M_BUZZER) && (buzzerStatus.status != STATE_BUZZER_FAILSAFE) )
	{
		buzzerStatus.status = STATE_BUZZER_OFF;
	}

}

 void RxUpdate(void) // hook for when rx updates
{

	uint32_t throttleIsSafe = 0;

	 //get current flight modes
	CheckRxToModes();


	if ( (!threeDeeMode) && (trueRcCommandF[THROTTLE] < -0.85) )
		throttleIsSafe = 1;
	else if ( (threeDeeMode) && (trueRcCommandF[THROTTLE] > -0.10) && (trueRcCommandF[THROTTLE] < 0.10) )
		throttleIsSafe = 1;

	//throttle must be low and board must be set to not armed before we allow an arming
	if (!ModeActive(M_ARMED) &&  throttleIsSafe)
		armCheckLatch = 1;

	if (!latchFirstArm)
		checkRxPreArmCalibration(); //collect rx data if not armed yet

	if (armCheckLatch)
	{
		if ( (latchFirstArm == 0) && (!boardArmed) && (ModeActive(M_ARMED)) )
		{
			latchFirstArm = 1;
			PreArmFilterCheck = 1;
			buzzerStatus.status = STATE_BUZZER_ARMING;
			ResetGyroCalibration();
		}
		else if ( (mainConfig.rcControlsConfig.rcCalibrated) && (latchFirstArm == 2) && (!calibrateMotors) && (!boardArmed) && (ModeActive(M_ARMED)) && (mainConfig.gyroConfig.boardCalibrated) && throttleIsSafe && !progMode)
		{ //TODO: make uncalibrated board buzz

			latchFirstArm = 1; //1 is double single single single, 0 is double double double double
			disarmCount   = 0;

			if ( !(RtcReadBackupRegister(FC_STATUS_REG) == FC_STATUS_INFLIGHT) ) {
				//fc crashed during flight
				RtcWriteBackupRegister(FC_STATUS_REG,FC_STATUS_INFLIGHT);
				buzzerStatus.status = STATE_BUZZER_ARMING;
			}

			ArmBoard();

			//todo: make sure stick movement on these three axis are next to zero before setting centers.

			//ChannelMap(PITCH)

			if ( ABS((int32_t)rxCalibrationRecords[PITCH].highestDataValue - (int32_t)mainConfig.rcControlsConfig.midRc[PITCH]) < 30 )
				mainConfig.rcControlsConfig.midRc[PITCH] = rxCalibrationRecords[PITCH].highestDataValue;
			if ( ABS((int32_t)rxCalibrationRecords[ROLL].highestDataValue - (int32_t)mainConfig.rcControlsConfig.midRc[ROLL]) < 30 )
				mainConfig.rcControlsConfig.midRc[ROLL] = rxCalibrationRecords[ROLL].highestDataValue;
			if ( ABS((int32_t)rxCalibrationRecords[YAW].highestDataValue - (int32_t)mainConfig.rcControlsConfig.midRc[YAW]) < 30 )
				mainConfig.rcControlsConfig.midRc[YAW] = rxCalibrationRecords[YAW].highestDataValue;
			/*
			if ( ABS((int32_t)rxData[PITCH] - (int32_t)mainConfig.rcControlsConfig.midRc[PITCH]) < 30 )
				mainConfig.rcControlsConfig.midRc[PITCH] = rxData[PITCH];
			if ( ABS((int32_t)rxData[ROLL] - (int32_t)mainConfig.rcControlsConfig.midRc[ROLL]) < 30 )
				mainConfig.rcControlsConfig.midRc[ROLL]  = rxData[ROLL];
			if ( ABS((int32_t)rxData[YAW] - (int32_t)mainConfig.rcControlsConfig.midRc[YAW]) < 30 )
				mainConfig.rcControlsConfig.midRc[YAW]   = rxData[YAW];
			*/

		}
		else if ( !ModeActive(M_ARMED) )
		{
			if (disarmCount++ > 3)
			{
				if (latchFirstArm==1)
				{
					latchFirstArm = 2;
				}
				DisarmBoard();
				RtcWriteBackupRegister(FC_STATUS_REG,FC_STATUS_IDLE);
			}
		}
	}

	if (!boardArmed && rxData[0] > 1800 && rxData[1] < 200 && rxData[2] < 200 && rxData[3] < 200)
	{
		if (InlineMillis() - progTimer > 2000)
			progMode = 1;
	}
	else
	{
		progTimer = InlineMillis();
	}


}

void SpektrumBind (uint32_t bindNumber)
{

	if (!bindNumber)
		return;

	uint32_t i;

	//todo: init all RX ports and ping each one as a spektrum port, maybe check each one to see if it allows spektrum binding
	InitializeGpio(GPIOA, GPIO_PIN_9, 1);
	InitializeGpio(GPIOA, GPIO_PIN_10, 1);

	InitializeGpio(GPIOB, GPIO_PIN_10, 1);
	InitializeGpio(GPIOB, GPIO_PIN_11, 1);


	DelayMs(2);

	if (!bindNumber)
		bindNumber = 9;

	for (i=0; i < bindNumber; i++) {

		inlineDigitalLo(GPIOA, GPIO_PIN_9);
		inlineDigitalLo(GPIOA, GPIO_PIN_10);
		inlineDigitalLo(GPIOB, GPIO_PIN_10);
		inlineDigitalLo(GPIOB, GPIO_PIN_11);
		DelayMs(2);

		inlineDigitalHi(GPIOA, GPIO_PIN_9);
		inlineDigitalHi(GPIOA, GPIO_PIN_10);
		inlineDigitalHi(GPIOB, GPIO_PIN_10);
		inlineDigitalHi(GPIOB, GPIO_PIN_11);
		DelayMs(2);

	}

    if (mainConfig.rcControlsConfig.bind)
    {
    	mainConfig.rcControlsConfig.bind = 0;
    	SaveConfig(ADDRESS_CONFIG_START);
    }

}

inline uint32_t ChannelMap(uint32_t inChannel)
{
	volatile uint32_t outChannel;

	if ( (!skipRxMap) && (mainConfig.rcControlsConfig.channelMap[inChannel] <= MAXCHANNELS) )
	{
		outChannel =mainConfig.rcControlsConfig.channelMap[inChannel];
	}
	else if ( mainConfig.rcControlsConfig.rcCalibrated != 1)
	{
		outChannel = inChannel;//not calibrated and no need to skip, send default
	}
	else
	{
		outChannel = 15; //else dump to junk channel
	}

	return(outChannel);
}

void ProcessSpektrumPacket(uint32_t serialNumber)
{
	static uint32_t timeSinceLastPacket = 0;
	volatile uint32_t spektrumChannel;
	uint32_t x;
	int32_t y;
	uint32_t value;
	uint16_t channelIdMask;
	uint16_t servoPosMask;
	uint32_t bitShift;

	if ( (InlineMillis() - timeSinceLastPacket)  < 9)
		return;

	channelIdMask = 0x7800;
	servoPosMask  = 0x07FF;
	bitShift      = 11;
	//DSM2 is 10 bits and the others are 11 bits of data for the RX
	if ((board.serials[serialNumber].Protocol == USING_DSM2_T) || (board.serials[serialNumber].Protocol == USING_DSM2_R))
	{
		channelIdMask = 0xFC00;
		servoPosMask  = 0x03FF;
		bitShift      = 10;
	}
																													// Make sure this is very first thing done in function, and its called first on interrupt
	memcpy(copiedBufferData, serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1], SPEKTRUM_FRAME_SIZE);    // we do this to make sure we don't have a race condition, we copy before it has a chance to be written by dma
															   	   	   	   	   	   	   	   	   	   	   	   	   	   	// We know since we are highest priority interrupt, nothing can interrupt us, and copy happens so quick, we will alwyas be guaranteed to get it

	for (x = 2; x < 16; x += 2)
	{
		value = (copiedBufferData[x] << 8) + (copiedBufferData[x+1]);
		spektrumChannel = (value & channelIdMask) >> bitShift;
		if (spektrumChannel < MAXCHANNELS)
		{
			rxDataRaw[spektrumChannel] = (value & servoPosMask);
			rx_timeout = 0;
			if (buzzerStatus.status == STATE_BUZZER_FAILSAFE)
				buzzerStatus.status = STATE_BUZZER_OFF;
		}
		else
		{
			rx_timeout++;
		}
	}

	for (y=MAXCHANNELS-1;y>-1;y--)
	{
		rxData[y] = rxDataRaw[ChannelMap(y)];
	}

	timeSinceLastPacket = InlineMillis();
	if ( !(board.serials[serialNumber].Protocol == USING_DSM2_T) && !(board.serials[serialNumber].Protocol == USING_DSM2_R) )
	{
		spekPhase = copiedBufferData[2] & 0x80;

		//Check for vtx data
		if (copiedBufferData[12] == 0xE0)
		{
			vtxData.vtxChannel = (copiedBufferData[13] & 0x0F) + 1;
			vtxData.vtxBand    = (copiedBufferData[13] >> 5) & 0x07;
		}

			  //Check channel slot 7 for vtx power, pit, and region data
		if (copiedBufferData[14] == 0xE0)
		{
			vtxData.vtxPower  = copiedBufferData[15] & 0x03;
			vtxData.vtxRegion = (copiedBufferData[15] >> 3) & 0x01;
			vtxData.vtxPit    = (copiedBufferData[15] >> 4) & 0x01;
		}

		if (!spekPhase && mainConfig.telemConfig.telemSpek)
		{
			sendSpektrumTelemtryAt = Micros() + 1000;
		}
		else
		{
			sendSpektrumTelemtryAt = 0;
		}

		packetTime = 11;
	}
	else
	{
		packetTime = 22;
	}

	InlineCollectRcCommand();
	RxUpdate();
}

void PowerInveter(uint32_t port, uint32_t pin, uint32_t direction)
{
	if (direction)
		InitializeGpio(ports[port], pin, direction-1);
}

void ProcessSbusPacket(uint32_t serialNumber)
{
	static uint32_t outOfSync = 0, inSync = 0;
	int32_t y;

	sbusFrame_t *frame = (sbusFrame_t*)copiedBufferData;

	memcpy(copiedBufferData, serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1], SBUS_FRAME_SIZE);

	// do we need to hook these into rxData[ChannelMap(i)] ?
	if ( (frame->syncByte == SBUS_STARTBYTE) && (frame->endByte == SBUS_ENDBYTE) )
	{
		if ( !(frame->flags & (SBUS_FAILSAFE_FLAG) ) )
		{

			rx_timeout = 0;
			if (buzzerStatus.status == STATE_BUZZER_FAILSAFE)
				buzzerStatus.status = STATE_BUZZER_OFF;

			rxDataRaw[0] = frame->chan0;
			rxDataRaw[1] = frame->chan1;
			rxDataRaw[2] = frame->chan2;
			rxDataRaw[3] = frame->chan3;
			rxDataRaw[4] = frame->chan4;
			rxDataRaw[5] = frame->chan5;
			rxDataRaw[6] = frame->chan6;
			rxDataRaw[7] = frame->chan7;
			rxDataRaw[8] = frame->chan8;
			rxDataRaw[9] = frame->chan9;
			rxDataRaw[10] = frame->chan10;
			rxDataRaw[11] = frame->chan11;
			rxDataRaw[12] = frame->chan12;
			rxDataRaw[13] = frame->chan13;
			rxDataRaw[14] = frame->chan14;
			rxDataRaw[15] = frame->chan15;

			for (y=MAXCHANNELS-1;y>-1;y--)
			{
				rxData[y] = rxDataRaw[ChannelMap(y)];
			}

			inSync++;
			// TODO: is this best way to deal with failsafe stuff?
			//if (!(frame->flags & (SBUS_FRAME_LOSS_FLAG | SBUS_FAILSAFE_FLAG))) {
			//	rx_timeout = 0;
			//}
			// TODO: No, we should only look at SBUS_FAILSAFE_FLAG for failsafe.

			packetTime = 9;
			InlineCollectRcCommand();
			RxUpdate();
		}
	} else {
		outOfSync++;
	}

}

void ProcessSumdPacket(uint8_t serialRxBuffer[], uint32_t frameSize)
{

	uint32_t x;
	int32_t y;
	uint16_t value;
	uint16_t numOfChannels;
	//uint16_t receivedCrc;
	uint16_t calculatedCrc;
																													// Make sure this is very first thing done in function, and its called first on interrupt
	memcpy(copiedBufferData, serialRxBuffer, frameSize);    // we do this to make sure we don't have a race condition, we copy before it has a chance to be written by dma
															   	   	   	   	   	   	   	   	   	   	   	   	   	   	// We know since we are highest priority interrupt, nothing can interrupt us, and copy happens so quick, we will alwyas be guaranteed to get it
	calculatedCrc = 0;

	if ( (copiedBufferData[0] == 0xA8) && (copiedBufferData[1] == 0x01) ) { //0 is graupner, //valid and live header 1 is 0x01, failsafe is 0x81, any other value is invalid

		numOfChannels = copiedBufferData[2];

		//check CRC
		calculatedCrc = CRC16(calculatedCrc, copiedBufferData[0]);
		calculatedCrc = CRC16(calculatedCrc, copiedBufferData[1]);
		calculatedCrc = CRC16(calculatedCrc, copiedBufferData[2]);

		if ( (numOfChannels < 0x20) && (numOfChannels > 0x01) )
		{

			//receivedCrc = (uint32_t)((copiedBufferData[(numOfChannels + 1) * 2 + 1] << 8) & 0x0000FF00); //crc high byte
			//receivedCrc = (uint32_t)(copiedBufferData[(numOfChannels + 1) * 2 + 2] & 0x000000FF);        //crc low byte
			for (x=0;x<(numOfChannels);x++)
			{
				calculatedCrc = CRC16(calculatedCrc, copiedBufferData[x * 2 + 1]);
				calculatedCrc = CRC16(calculatedCrc, copiedBufferData[x * 2 + 2]);
			}

		}

		//if (receivedCrc == calculatedCrc)
		if (1 == 1)
		{
			if ( (numOfChannels < 0x20) && (numOfChannels > 0x01) )
			{

				for (x=0;x<numOfChannels;x++)
				{

					value = (uint32_t)( (uint16_t)((copiedBufferData[3 + x * 2 + 0] << 8) & 0x0000FF00) | (uint8_t)(copiedBufferData[3 + x * 2 + 1] & 0x000000FF) );
					rxDataRaw[x] = value; //high byte

				}

				packetTime = 10;
				rx_timeout = 0;
				if (buzzerStatus.status == STATE_BUZZER_FAILSAFE)
					buzzerStatus.status = STATE_BUZZER_OFF;
				InlineCollectRcCommand();
				RxUpdate();

			}

			for (y=MAXCHANNELS-1;y>-1;y--)
			{
				rxData[y] = rxDataRaw[ChannelMap(y)];
			}
		}

	}

}

void ProcessIbusPacket(uint8_t serialRxBuffer[], uint32_t frameSize)
{

	uint32_t i;
	int32_t y;
	uint16_t chkSum, rxSum;

															// Make sure this is very first thing done in function, and its called first on interrupt
	memcpy(copiedBufferData, serialRxBuffer, frameSize);    // we do this to make sure we don't have a race condition, we copy before it has a chance to be written by dma
															// We know since we are highest priority interrupt, nothing can interrupt us, and copy happens so quick, we will alwyas be guaranteed to get it
	chkSum = 0xFFFF;
	for (i = 0; i < 30; i++)
		chkSum -= copiedBufferData[i];

	rxSum = copiedBufferData[30] + (copiedBufferData[31] << 8);

	 if (chkSum == rxSum)
	 {
		rxDataRaw[0] = (copiedBufferData[ 3] << 8) + copiedBufferData[ 2];
		rxDataRaw[1] = (copiedBufferData[ 5] << 8) + copiedBufferData[ 4];
		rxDataRaw[2] = (copiedBufferData[ 7] << 8) + copiedBufferData[ 6];
		rxDataRaw[3] = (copiedBufferData[ 9] << 8) + copiedBufferData[ 8];
		rxDataRaw[4] = (copiedBufferData[11] << 8) + copiedBufferData[10];
		rxDataRaw[5] = (copiedBufferData[13] << 8) + copiedBufferData[12];
		rxDataRaw[6] = (copiedBufferData[15] << 8) + copiedBufferData[14];
		rxDataRaw[7] = (copiedBufferData[17] << 8) + copiedBufferData[16];
		rxDataRaw[8] = (copiedBufferData[19] << 8) + copiedBufferData[18];
		rxDataRaw[9] = (copiedBufferData[21] << 8) + copiedBufferData[20];

		for (y=MAXCHANNELS-1;y>-1;y--)
		{
			rxData[y] = rxDataRaw[ChannelMap(y)];
		}

		packetTime = 10;
		rx_timeout = 0;

		if (buzzerStatus.status == STATE_BUZZER_FAILSAFE)
			buzzerStatus.status = STATE_BUZZER_OFF;

		InlineCollectRcCommand();
		RxUpdate();
	}

}

void ProcessPpmPacket(uint32_t ppmBuffer2[], uint32_t *ppmBufferIdx)
{
//	ppmBuffer[*ppmBufferIdx]

	uint32_t x;
	//make sure sync is correct:
	//we have at least two times, make sure they are at least the PPM_SYNC_MINIMUM_US apart
	if ( (*ppmBufferIdx > 17) && ( (ppmBuffer2[1] - ppmBuffer[0]) >  PPM_SYNC_MINIMUM_US ) )
	{
		//sync looks good, where 0 is the end of the last pulse and 1 is the beginning of the new pulse
		//we have at least 8 channels which is 18 interrupts

		ppmData[0] = (ppmBuffer[ 3] - ppmBuffer[ 2]);
		ppmData[1] = (ppmBuffer[ 5] - ppmBuffer[ 4]);
		ppmData[2] = (ppmBuffer[ 7] - ppmBuffer[ 6]);
		ppmData[3] = (ppmBuffer[ 9] - ppmBuffer[ 8]);
		ppmData[4] = (ppmBuffer[11] - ppmBuffer[10]);
		ppmData[5] = (ppmBuffer[13] - ppmBuffer[12]);
		ppmData[6] = (ppmBuffer[15] - ppmBuffer[14]);
		ppmData[7] = (ppmBuffer[17] - ppmBuffer[16]);

		memcpy(rxDataRaw, ppmData, 8);

		for (x=0;x<8;x++)
		{
			if ( (rxDataRaw[x] < 2200) && (rxDataRaw[x] > 500) )
			{
				rxData[x] = rxDataRaw[ChannelMap(x)];
			}
		}

		packetTime = (ppmBuffer[17] - ppmBuffer[0]);
		rx_timeout = 0;

		if (buzzerStatus.status == STATE_BUZZER_FAILSAFE)
			buzzerStatus.status = STATE_BUZZER_OFF;

		InlineCollectRcCommand();
		RxUpdate();

		ppmBuffer[0] = ppmBuffer[*ppmBufferIdx-1];
		*ppmBufferIdx = 1;
	}
	else if ( (*ppmBufferIdx > 1) && (ppmBuffer[1] - ppmBuffer[0]) <  PPM_SYNC_MINIMUM_US ) //we have at least two times and the first two aren't apart enough, so we reset the sync
	{
		//reset sync until we see a sync pulse
		ppmBuffer[0] = ppmBuffer[*ppmBufferIdx-1];
		*ppmBufferIdx = 1;
	}

}

void InitRcData(void)
{
	uint32_t axis;

	//pitch yaw and roll must all use the same curve. We make sure that's set here.
	mainConfig.rcControlsConfig.useCurve[YAW]  = mainConfig.rcControlsConfig.useCurve[PITCH];
	mainConfig.rcControlsConfig.useCurve[ROLL] = mainConfig.rcControlsConfig.useCurve[PITCH];

	//precalculate max angles for RC Curves for the three stick axis
	for (axis = 0; axis < 3; axis++)
	{
		maxKissRate[axis] = GetKissMaxRates(1.0f, axis);
		maxFlopRate[axis] = GetFlopMaxRates(1.0f, axis);
	}

	bzero(trueRcCommandF, MAXCHANNELS);
	bzero(curvedRcCommandF, MAXCHANNELS);
	bzero(smoothedRcCommandF, MAXCHANNELS);
	bzero(ppmBuffer, sizeof(ppmBuffer));
	ppmBufferIdx = 0;

	isRxDataNew = 0;

}


void PpmExtiCallback(uint32_t callbackNumber)
{
	(void)(callbackNumber);
	// EXTI line interrupt detected
	if(__HAL_GPIO_EXTI_GET_IT(ppmPin) != RESET)
	{
		//record time of IRQ in microseconds
		ppmBuffer[ppmBufferIdx++] = Micros();
		ProcessPpmPacket(ppmBuffer, &ppmBufferIdx);
		if (ppmBufferIdx == PPM_BUFFER_SIZE)
		{
			ppmBufferIdx = 0;
		}
		__HAL_GPIO_EXTI_CLEAR_IT(ppmPin);
	}
}


inline void InlineCollectRcCommand (void)
{

	uint32_t axis;
	float rangedRx;

	isRxDataNew = 1; //this function is to be called by reception of vali RX data, so we know we have new RX data now

	//TAER
	//YAET
	//scale
    //////masterConfig.rxConfig.midrc = 1500;
    //////masterConfig.rxConfig.mincheck = 1005;
    //////masterConfig.rxConfig.maxcheck = 1990;
    //into a -1 to 1 float;
	//because of how midrc works we must do this separately or negative and positive values.
	//this method won't require a __disable_irq
	//////rcData is 1000 to 2000. It can never be negative.


	//calculate main controls.
	//rc data is taken from RX and using the map is put into the correct "axis"
	for (axis = 0; axis < MAXCHANNELS; axis++)
	{

		if (rxData[axis] < mainConfig.rcControlsConfig.midRc[axis])  //negative  range
			rangedRx = InlineChangeRangef(rxData[axis], mainConfig.rcControlsConfig.midRc[(axis)], mainConfig.rcControlsConfig.minRc[(axis)], 0.0f + mainConfig.rcControlsConfig.deadBand[axis], -1.0f); //-1 to 0
		else
			rangedRx = InlineChangeRangef(rxData[axis], mainConfig.rcControlsConfig.maxRc[(axis)], mainConfig.rcControlsConfig.midRc[(axis)], 1.0f, 0.0f - mainConfig.rcControlsConfig.deadBand[axis]); //0 to +1


		//do we want to apply deadband to trueRcCommandF? right now I think yes
		if (ABS(rangedRx) > mainConfig.rcControlsConfig.deadBand[axis])
		{
			trueRcCommandF[axis]   = InlineConstrainf( rangedRx, -1.0f, 1.0f);
			curvedRcCommandF[axis] = InlineApplyRcCommandCurve(trueRcCommandF[axis], mainConfig.rcControlsConfig.useCurve[axis], mainConfig.rcControlsConfig.curveExpo[axis], axis);
		}
		else
		{
			// no need to calculate if movement is below deadband
			trueRcCommandF[axis]   = 0.0f;
			curvedRcCommandF[axis] = 0.0f;
		}

	}

}

static float GetKissMaxRates(float rcCommand, uint32_t axis)
{
	float kissSetpoint, kissRate, kissGRate, kissUseCurve, kissTempCurve, kissRpyUseRates, kissRxRaw, kissAngle;

	kissUseCurve = (mainConfig.rcControlsConfig.curveExpo[axis]);
	kissRate     = (mainConfig.rcControlsConfig.acroPlus[axis]);
	kissGRate    = (mainConfig.rcControlsConfig.rates[axis]);
	kissSetpoint = rcCommand;

	kissRpyUseRates = 1.0f - abs(rcCommand) * kissGRate;
	kissRxRaw       = rcCommand * 1000.0f;
	kissTempCurve   = (kissRxRaw * kissRxRaw / 1000000.0f);
	kissSetpoint    = ((kissSetpoint * kissTempCurve) * kissUseCurve + kissSetpoint * (1.0f - kissUseCurve)) * (kissRate / 10.0f);
	kissAngle       = ((2000.0f * (1.0f / kissRpyUseRates)) * kissSetpoint); //setpoint is calculated directly here
	return(kissAngle);
}

static float GetFlopMaxRates(float rcCommand, uint32_t axis)
{
	float flopSuperRate, flopRcRate, flopExpo, flopFactor, flopAngle;

	flopExpo      = (mainConfig.rcControlsConfig.curveExpo[axis]);
	flopRcRate    = (mainConfig.rcControlsConfig.acroPlus[axis]);
	flopSuperRate = (mainConfig.rcControlsConfig.rates[axis]);

	if (flopRcRate > 2.0f)
		flopRcRate = flopRcRate + (14.55f * (flopRcRate - 1997.0f));

	if (flopExpo != 0.0f)
		rcCommand = rcCommand * Powerf(ABS(rcCommand), 3) * flopExpo + rcCommand * (1.0f-flopExpo);

	flopAngle = 200.0f * flopRcRate * rcCommand;

	if (flopSuperRate != 0.0f)
	{
		flopFactor = 1.0f / (InlineConstrainf(1.0f - (ABS(rcCommand) * (flopSuperRate)), 0.01f, 1.00f));
		flopAngle *= flopFactor; ; //setpoint is calculated directly here
	}
	return(flopAngle);
}

//return curved RC command as a percentage of max rate (-1.0 to 1.0)
inline float InlineApplyRcCommandCurve(float rcCommand, uint32_t curveToUse, float expo, uint32_t axis)
{

	float maxOutput, maxOutputMod, returnValue;
	float flopSuperRate, flopRcRate, flopExpo, flopFactor, flopAngle;
	float kissSetpoint, kissRate, kissGRate, kissUseCurve, kissTempCurve, kissRpyUseRates, kissRxRaw, kissAngle;

	maxOutput    = 1.0f;
	maxOutputMod = 0.01f;

	switch (curveToUse)
	{

		case SKITZO_EXPO:
			returnValue = ((maxOutput + maxOutputMod * expo * (rcCommand * rcCommand - 1.0f)) * rcCommand * rcCommand);
			if (rcCommand < 0.0f) {
				returnValue = -returnValue;
			}
			return (returnValue);
			break;
		case BETAFLOP_EXPO:
			flopExpo      = (mainConfig.rcControlsConfig.curveExpo[axis]);
			flopRcRate    = (mainConfig.rcControlsConfig.acroPlus[axis]);
			flopSuperRate = (mainConfig.rcControlsConfig.rates[axis]);

			if (flopRcRate > 2.0f)
				flopRcRate = flopRcRate + (14.55f * (flopRcRate - 1997.0f));

			if (flopExpo != 0.0f)
				rcCommand = rcCommand * Powerf(ABS(rcCommand), 3) * flopExpo + rcCommand * (1.0f-flopExpo);

			flopAngle = 200.0f * flopRcRate * rcCommand;

			if (flopSuperRate != 0.0f)
			{
				flopFactor = 1.0f / (InlineConstrainf(1.0f - (ABS(rcCommand) * (flopSuperRate)), 0.01f, 1.00f));
				flopAngle *= flopFactor; ; //setpoint is calculated directly here
			}
			returnValue = (flopAngle / maxFlopRate[axis]); //get curved stick position based on percentage of setpoint
			return(returnValue);
			break;
		case TARANIS_EXPO:
		case ACRO_PLUS:
		case FAST_EXPO:
			return ((maxOutput + maxOutputMod * expo * (rcCommand * rcCommand - 1.0f)) * rcCommand);
			break;
		case KISS_EXPO:
		case KISS_EXPO2:
			kissUseCurve = (mainConfig.rcControlsConfig.curveExpo[axis]);
			kissRate     = (mainConfig.rcControlsConfig.acroPlus[axis]);
			kissGRate    = (mainConfig.rcControlsConfig.rates[axis]);
			kissSetpoint = rcCommand;

			kissRpyUseRates = 1.0f - abs(rcCommand) * kissGRate;
			kissRxRaw       = rcCommand * 1000.0f;
			kissTempCurve   = (kissRxRaw * kissRxRaw / 1000000.0f);
			kissSetpoint    = ((kissSetpoint * kissTempCurve) * kissUseCurve + kissSetpoint * (1.0f - kissUseCurve)) * (kissRate / 10.0f);
			kissAngle       = ((2000.0f * (1.0f / kissRpyUseRates)) * kissSetpoint); //setpoint is calculated directly here
			returnValue = (kissAngle / maxKissRate[axis]); //get curved stick position based on percentage of setpoint
			return(returnValue);
			break;
		case NO_EXPO:
		default:
			return(rcCommand); //same as default for now.
			break;

	}
}


inline void InlineRcSmoothing(float curvedRcCommandF[], float smoothedRcCommandF[])
{
    static float lastCommand[4] = { 0, 0, 0, 0 };
    static float deltaRC[4] = { 0, 0, 0, 0 };
    static int32_t factor = 0;
    int32_t channel;

    int32_t smoothingInterval = (loopSpeed.khzDivider * packetTime); //todo: calculate this number to be number of loops between PID loops
	//88  for spektrum at  8 KHz loop time
	//264 for spektrum at 24 KHz loop time
	//352 for spektrum at 32 KHz loop time

    if (isRxDataNew)
    {
        for (channel=3; channel >= 0; channel--)
        {
            deltaRC[channel] = curvedRcCommandF[channel] -  (lastCommand[channel] - ((deltaRC[channel] * (float)factor) / (float)smoothingInterval));
            lastCommand[channel] = curvedRcCommandF[channel];
        }
        factor = smoothingInterval - 1;
        isRxDataNew = false;
    }
    else
    {
    	factor--;
    }

    if (factor > 0)
    {
    	for (channel=3; channel >= 0; channel--)
    	{
    		smoothedRcCommandF[channel] = (lastCommand[channel] - ( (deltaRC[channel] * (float)factor) / (float)smoothingInterval));
    	}
    }
    else
    {
    	factor = 0;
    }

}


void SetRxDefaults(uint32_t rxProtocol, uint32_t usart)
{

	mainConfig.rcControlsConfig.rxUsart   = usart;
	mainConfig.rcControlsConfig.rxProtcol = rxProtocol;

	//set some sane RC defaults for the protocol chosen
	switch(rxProtocol)
	{
		case USING_CPPM_R:
		case USING_CPPM_T:
			mainConfig.rcControlsConfig.midRc[PITCH]         = 1500;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 1500;
			mainConfig.rcControlsConfig.midRc[YAW]           = 1500;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 1500;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 1500;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 1000;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 1000;
			mainConfig.rcControlsConfig.minRc[YAW]           = 1000;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 1000;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 1000;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 1000;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 1000;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 1000;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 2000;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 2000;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 2000;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 2000;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 6;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 7;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 8;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 1000;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 1000;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 1000;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 1000;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 1000;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 1000;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 1000; //junk channel
			break;
		case USING_IBUS_R:
		case USING_IBUS_T:
			mainConfig.rcControlsConfig.midRc[PITCH]         = 1500;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 1500;
			mainConfig.rcControlsConfig.midRc[YAW]           = 1500;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 1500;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 1500;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 1000;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 1000;
			mainConfig.rcControlsConfig.minRc[YAW]           = 1000;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 1000;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 1000;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 1000;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 1000;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 1000;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 2000;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 2000;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 2000;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 2000;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 6;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 7;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 8;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 9;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 10;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 11;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 12;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 13;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 14;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 15; //junk channel
			break;
		case USING_SUMD_R:
		case USING_SUMD_T:
			mainConfig.rcControlsConfig.midRc[PITCH]         = 12000;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 12000;
			mainConfig.rcControlsConfig.midRc[YAW]           = 12000;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 12000;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 12000;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 12000;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 12000;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 12000;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 8000;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 16000;
			mainConfig.rcControlsConfig.minRc[YAW]           = 16000;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 16000;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 8000;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 8000;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 8000;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 8000;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 16000;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 8000;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 8000;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 8000;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 16000;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 16000;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 16000;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 16000;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 6;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 7;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 8;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 9;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 10;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 11;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 12;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 13;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 14;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 15; //junk channel
			break;
		case USING_DSM2_R:
		case USING_DSM2_T:
			mainConfig.rcControlsConfig.midRc[PITCH]         = 512;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 512;
			mainConfig.rcControlsConfig.midRc[YAW]           = 512;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 512;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 512;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 512;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 512;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 512;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 12;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 12;
			mainConfig.rcControlsConfig.minRc[YAW]           = 12;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 12;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 12;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 12;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 12;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 12;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 1024;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 1024;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 1024;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 1024;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 1024;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 1024;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 1024;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 1024;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 6;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 7;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 8;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 9;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 10;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 11;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 12;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 13;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 14;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 15; //junk channel
			break;
		case USING_SPEK_R:
		case USING_SPEK_T:
			mainConfig.rcControlsConfig.midRc[PITCH]         = 1024;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 1024;
			mainConfig.rcControlsConfig.midRc[YAW]           = 1024;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 1024;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 1024;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 1024;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 1024;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 1024;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 22;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 22;
			mainConfig.rcControlsConfig.minRc[YAW]           = 22;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 22;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 342;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 342;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 342;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 342;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 2025;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 2025;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 2025;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 2025;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 1706;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 6;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 7;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 8;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 9;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 10;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 11;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 12;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 13;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 14;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 15; //junk channel
			break;
		case USING_SBUS_R:
		case USING_SBUS_T:
			mainConfig.rcControlsConfig.midRc[PITCH]         = 990;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 990;
			mainConfig.rcControlsConfig.midRc[YAW]           = 990;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 990;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 990;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 990;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 990;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 990;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 170;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 170;
			mainConfig.rcControlsConfig.minRc[YAW]           = 170;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 170;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 170;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 170;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 170;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 170;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 1810;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 1810;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 1810;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 1810;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 1810;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 1810;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 1810;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 1810;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 6;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 7;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 8;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 9;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 10;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 11;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 12;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 13;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 14;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 15; //junk channel
			break;
		default:
			return; //fail
	}


}
