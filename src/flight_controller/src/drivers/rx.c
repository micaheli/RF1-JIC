#include "includes.h"

float trueRcCommandF[MAXCHANNELS];     //4 sticks. range is -1 to 1, directly related to stick position
float curvedRcCommandF[MAXCHANNELS];   //4 sticks. range is -1 to 1, this is the rcCommand after the curve is applied
float smoothedRcCommandF[MAXCHANNELS]; //4 sticks. range is -1 to 1, this is the smoothed rcCommand
volatile unsigned char isRxDataNew;
volatile uint32_t disarmCount = 0, latchFirstArm = 0;
uint32_t usingSpektrum = 0; //only works if we are ONLY using Spektrum. Not sure why we would ever use more than one type of RX at a time.

static uint32_t packetTime = 11;

uint32_t rxData[MAXCHANNELS];

uint32_t skipRxMap = 0;
uint32_t progMode = 0;
uint32_t progTimer = 0;

// 2048 resolution
#define SPEKTRUM_FRAME_SIZE 16
uint32_t spektrumChannelShift = 3;
uint32_t spektrumChannelMask = 0x07;


/*
//1024 resolution
spektrumChannelShift = 2;
spektrumChannelMask = 0x03;
*/

SPM_VTX_DATA vtxData;

#define SBUS_FRAME_SIZE 25
#define SBUS_FRAME_LOSS_FLAG (1 << 2)
#define SBUS_FAILSAFE_FLAG (1 << 3)
#define SBUS_STARTBYTE         0x0f
#define SBUS_ENDBYTE           0x00

typedef struct {
	uint8_t syncByte;
	unsigned int chan0 : 11;
	unsigned int chan1 : 11;
	unsigned int chan2 : 11;
	unsigned int chan3 : 11;
	unsigned int chan4 : 11;
	unsigned int chan5 : 11;
	unsigned int chan6 : 11;
	unsigned int chan7 : 11;
	unsigned int chan8 : 11;
	unsigned int chan9 : 11;
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


inline void CheckFailsafe(void) {

	rx_timeout++;
	FeedTheDog(); //resets IWDG time to 0. This tells the timer the board is running.

	if ((boardArmed) && (rx_timeout > 1000))
	{
		DisarmBoard();
		ZeroActuators(32000); //immediately set actuators to disarmed position.
	}
}

 void RxUpdate(void) // hook for when rx updates
{

	if ( (latchFirstArm == 0) && (!boardArmed) && (trueRcCommandF[AUX1] > 0.5) ) {
		latchFirstArm = 1;
	} else if ( (mainConfig.rcControlsConfig.rcCalibrated) && (latchFirstArm == 2) && (!calibrateMotors) && (!boardArmed) && (trueRcCommandF[AUX1] > 0.5) && (mainConfig.gyroConfig.boardCalibrated) && (trueRcCommandF[THROTTLE] < -0.8) && !progMode) { //TODO: make uncalibrated board buzz

		latchFirstArm = 0;
		disarmCount = 0;

		if ( !(rtc_read_backup_reg(FC_STATUS_REG) == FC_STATUS_INFLIGHT) ) {
			//fc crashed during flight
			ResetGyroCalibration();
			rtc_write_backup_reg(FC_STATUS_REG,FC_STATUS_INFLIGHT);
		}

		ArmBoard();

		//todo: make sure stick movement on these three axis are next to zero before setting centers.
		if ( ABS((int32_t)rxData[PITCH] - (int32_t)mainConfig.rcControlsConfig.midRc[PITCH]) < 30 )
			mainConfig.rcControlsConfig.midRc[PITCH] = rxData[PITCH];
		if ( ABS((int32_t)rxData[ROLL] - (int32_t)mainConfig.rcControlsConfig.midRc[ROLL]) < 30 )
			mainConfig.rcControlsConfig.midRc[ROLL]  = rxData[ROLL];
		if ( ABS((int32_t)rxData[YAW] - (int32_t)mainConfig.rcControlsConfig.midRc[YAW]) < 30 )
			mainConfig.rcControlsConfig.midRc[YAW]   = rxData[YAW];

	} else if (trueRcCommandF[AUX1] < 0.5) {

		if (disarmCount++ > 3) {
			if (latchFirstArm==1) {
				latchFirstArm = 2;
			}
			DisarmBoard();
			rtc_write_backup_reg(FC_STATUS_REG,FC_STATUS_IDLE);
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

void SpektrumBind (uint32_t bindNumber) {

	uint32_t i;

	//todo: init all RX ports and ping each one as a spektrum port, maybe check each one to see if it allows spektrum binding
	InitializeGpio(GPIOB, GPIO_PIN_11, 1);
	InitializeGpio(GPIOA, GPIO_PIN_9, 1);

	DelayMs(70);

	if (!bindNumber)
		bindNumber = 9;

	for (i=0; i < bindNumber; i++) {

		inlineDigitalLo(GPIOB, GPIO_PIN_11);
		inlineDigitalLo(GPIOA, GPIO_PIN_9);
		DelayMs(2);

		inlineDigitalHi(GPIOB, GPIO_PIN_11);
		inlineDigitalHi(GPIOA, GPIO_PIN_9);
		DelayMs(2);

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

	if (1) // here is where we check which rx we are using probably use case
	{

		if ( (outChannel == THROTTLE) && (usingSpektrum) )
			rx_timeout = 0;
	}

	return(outChannel);
}

void ProcessSpektrumPacket(uint32_t serialNumber)
{
	volatile uint32_t spektrumChannel;
	uint32_t x;
	uint32_t value;

																													// Make sure this is very first thing done in function, and its called first on interrupt
	memcpy(copiedBufferData, serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1], SPEKTRUM_FRAME_SIZE);    // we do this to make sure we don't have a race condition, we copy before it has a chance to be written by dma
															   	   	   	   	   	   	   	   	   	   	   	   	   	   	// We know since we are highest priority interrupt, nothing can interrupt us, and copy happens so quick, we will alwyas be guaranteed to get it

	for (x = 2; x < 16; x += 2) {
		value = (copiedBufferData[x] << 8) + (copiedBufferData[x+1]);
		spektrumChannel = (value & 0x7800) >> 11;
		if (spektrumChannel < MAXCHANNELS) {
			usingSpektrum=1;
			rxData[ChannelMap(spektrumChannel)] = value & 0x7FF;
		}
	}
	spekPhase = copiedBufferData[2] & 0x80;

	//Check for vtx data
	if (copiedBufferData[12] == 0xE0) { 
		vtxData.vtxChannel = (copiedBufferData[13] & 0x0F) + 1;
		vtxData.vtxBand    = (copiedBufferData[13] >> 5) & 0x07;
	}
      
	      //Check channel slot 7 for vtx power, pit, and region data
	if (copiedBufferData[14] == 0xE0) { 
		vtxData.vtxPower  = copiedBufferData[15] & 0x03;
		vtxData.vtxRegion = (copiedBufferData[15] >> 3) & 0x01;
		vtxData.vtxPit    = (copiedBufferData[15] >> 4) & 0x01;
	}

	packetTime = 11;
	InlineCollectRcCommand();
	RxUpdate();
}

void ProcessSbusPacket(uint32_t serialNumber)
{
	static uint32_t outOfSync = 0, inSync = 0;

	sbusFrame_t *frame = (sbusFrame_t*)copiedBufferData;

	memcpy(copiedBufferData, serialRxBuffer[board.serials[serialNumber].serialRxBuffer-1], SBUS_FRAME_SIZE);

	// do we need to hook these into rxData[ChannelMap(i)] ?
	if ( (frame->syncByte == SBUS_STARTBYTE) && (frame->endByte == SBUS_ENDBYTE) ) {
		usingSpektrum=0;
		rxData[ChannelMap(0)] = frame->chan0;
		rxData[ChannelMap(1)] = frame->chan1;
		rxData[ChannelMap(2)] = frame->chan2;
		rxData[ChannelMap(3)] = frame->chan3;
		rxData[ChannelMap(4)] = frame->chan4;
		rxData[ChannelMap(5)] = frame->chan5;
		rxData[ChannelMap(6)] = frame->chan6;
		rxData[ChannelMap(7)] = frame->chan7;
		rxData[ChannelMap(8)] = frame->chan8;
		rxData[ChannelMap(9)] = frame->chan9;
		rxData[ChannelMap(10)] = frame->chan10;
		rxData[ChannelMap(11)] = frame->chan11;
		rxData[ChannelMap(12)] = frame->chan12;
		rxData[ChannelMap(13)] = frame->chan13;
		rxData[ChannelMap(14)] = frame->chan14;
		rxData[ChannelMap(15)] = frame->chan15;
		inSync++;
		// TODO: is this best way to deal with failsafe stuff?
		//if (!(frame->flags & (SBUS_FRAME_LOSS_FLAG | SBUS_FAILSAFE_FLAG))) {
		//	rx_timeout = 0;
		//}
		// TODO: No, we should only look at SBUS_FAILSAFE_FLAG for failsafe.
		if ( !(frame->flags & (SBUS_FAILSAFE_FLAG) ) ) {
			rx_timeout = 0;
		}
		packetTime = 9;
		InlineCollectRcCommand();
		RxUpdate();
	} else {
		outOfSync++;
	}

}

void InitRcData (void) {
	bzero(trueRcCommandF, MAXCHANNELS);
	bzero(curvedRcCommandF, MAXCHANNELS);
	bzero(smoothedRcCommandF, MAXCHANNELS);
	isRxDataNew = 0;
}

inline void InlineCollectRcCommand (void) {

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
	for (axis = 0; axis < MAXCHANNELS; axis++) {

		if (axis == THROTTLE)
		{
			if (rxData[axis] < mainConfig.rcControlsConfig.midRc[axis])  //negative  range
				rangedRx = InlineChangeRangef(rxData[axis], mainConfig.rcControlsConfig.midRc[axis], mainConfig.rcControlsConfig.minRc[axis], 0 + mainConfig.rcControlsConfig.deadBand[axis], -1.0); //-1 to 0
			else
				rangedRx = InlineChangeRangef(rxData[axis], mainConfig.rcControlsConfig.maxRc[axis], mainConfig.rcControlsConfig.midRc[axis], 1.0, 0 - mainConfig.rcControlsConfig.deadBand[axis]); //0 to +1
		}
		else
		{
			if (rxData[axis] < mainConfig.rcControlsConfig.midRc[axis])  //negative  range
				rangedRx = InlineChangeRangef(rxData[axis], mainConfig.rcControlsConfig.midRc[axis], mainConfig.rcControlsConfig.minRc[axis], 0 + mainConfig.rcControlsConfig.deadBand[axis], -1.0); //-1 to 0
			else
				rangedRx = InlineChangeRangef(rxData[axis], mainConfig.rcControlsConfig.maxRc[axis], mainConfig.rcControlsConfig.midRc[axis], 0.9, 0 - mainConfig.rcControlsConfig.deadBand[axis]); //0 to +1

		}


		//do we want to apply deadband to trueRcCommandF? right now I think yes
		if (ABS(rangedRx) > mainConfig.rcControlsConfig.deadBand[axis]) {
			trueRcCommandF[axis]   = InlineConstrainf ( rangedRx, -1, 1);
			curvedRcCommandF[axis] = InlineApplyRcCommandCurve (trueRcCommandF[axis], mainConfig.rcControlsConfig.useCurve[axis], mainConfig.rcControlsConfig.curveExpo[axis]);
		} else {
			// no need to calculate if movement is below deadband
			trueRcCommandF[axis]   = 0;
			curvedRcCommandF[axis] = 0;
		}

	}


}


 float InlineApplyRcCommandCurve (float rcCommand, uint32_t curveToUse, float expo) {

	float maxOutput, maxOutputMod, returnValue;

	maxOutput    = 1;
	maxOutputMod = 0.01;

	switch (curveToUse) {

		case SKITZO_EXPO:
			returnValue = ((maxOutput + maxOutputMod * expo * (rcCommand * rcCommand - 1.0)) * rcCommand * rcCommand);
			if (rcCommand < 0) {
				returnValue = -returnValue;
			}
			return (returnValue);
			break;

		case TARANIS_EXPO:
			return ( expo * (rcCommand * rcCommand * rcCommand) + rcCommand * (1-expo) );
			break;

		case FAST_EXPO:
				return ((maxOutput + maxOutputMod * expo * (rcCommand * rcCommand - 1.0)) * rcCommand);
				break;

		case NO_EXPO:
		default:
			return(rcCommand); //same as default for now.
			break;

	}
}


inline void InlineRcSmoothing(float curvedRcCommandF[], float smoothedRcCommandF[]) {
    static float lastCommand[4] = { 0, 0, 0, 0 };
    static float deltaRC[4] = { 0, 0, 0, 0 };
    static int32_t factor = 0;
    int32_t channel;

    int32_t smoothingInterval = (32 * packetTime); //todo: calculate this number to be number of loops between PID loops
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
