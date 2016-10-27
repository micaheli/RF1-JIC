#include "includes.h"

float trueRcCommandF[MAXCHANNELS];     //4 sticks. range is -1 to 1, directly related to stick position
float curvedRcCommandF[MAXCHANNELS];   //4 sticks. range is -1 to 1, this is the rcCommand after the curve is applied
float smoothedRcCommandF[MAXCHANNELS]; //4 sticks. range is -1 to 1, this is the smoothed rcCommand
unsigned char isRxDataNew;
uint32_t disarmCount = 0, latchFirstArm = 0;


uint32_t rxData[MAXCHANNELS];



// 2048 resolution
uint32_t spektrumChannelShift = 3;
uint32_t spektrumChannelMask = 0x07;


/*
//1024 resolution
spektrumChannelShift = 2;
spektrumChannelMask = 0x03;
*/

SPM_VTX_DATA vtxData;

//uint32_t tempData[MAXCHANNELS];

unsigned char copiedBufferData[16];

volatile uint32_t rx_timeout=0;
uint32_t spekPhase=1;
uint32_t ignoreEcho = 0;

inline void CheckFailsafe(void) {

	rx_timeout++;
	FeedTheDog(); //resets IWDG time to 0. This tells the timer the board is running.

	if ((boardArmed) && (rx_timeout > 1000))
	{
		boardArmed = 0;
		ZeroActuators(); //imediately set actuators to disarmed position.
	}

}

inline void RxUpdate(void) // hook for when rx updates
{

	if ( (latchFirstArm == 0) && (!boardArmed) && (rxData[4] > 1500) ) {
		latchFirstArm = 1;
	} else if ( (latchFirstArm == 2) && (!boardArmed) && (rxData[4] > 1500) && (mainConfig.gyroConfig.boardCalibrated) && (trueRcCommandF[THROTTLE] < 0.1) ) { //TODO: make uncalibrated board buzz

		latchFirstArm = 0;
		disarmCount = 0;

		if ( !(rtc_read_backup_reg(FC_STATUS_REG) == FC_STATUS_INFLIGHT) ) {
			//fc crashed during flight
			ResetGyroCalibration();
			rtc_write_backup_reg(FC_STATUS_REG,FC_STATUS_INFLIGHT);
		}

		boardArmed = 1;

		//if ( ABS(rxData[PITCH] - mainConfig.rcControlsConfig.midRc[PITCH]) < 200 )
			mainConfig.rcControlsConfig.midRc[PITCH] = rxData[PITCH];
		//if ( ABS(rxData[ROLL] - mainConfig.rcControlsConfig.midRc[ROLL]) < 200 )
			mainConfig.rcControlsConfig.midRc[ROLL]  = rxData[ROLL];
		//if ( ABS(rxData[YAW] - mainConfig.rcControlsConfig.midRc[YAW]) < 200 )
			mainConfig.rcControlsConfig.midRc[YAW]   = rxData[YAW];

	} else if (rxData[4] < 400) {

		if (disarmCount++ > 3) {
			if (latchFirstArm==1) {
				latchFirstArm = 2;
			}
			boardArmed = 0;
			rtc_write_backup_reg(FC_STATUS_REG,FC_STATUS_IDLE);
		}

	}



}

inline uint32_t SpektrumChannelMap(uint32_t inChannel) {
	if (inChannel == 3)
		return(0);

	if (inChannel == 0)
		return(3);

	return(inChannel);
}

void SpektrumBind (uint32_t bindNumber) {

	uint32_t i;

	//todo: init all RX ports and ping each one as a spektrum port, maybe check each one to see if it allows spektrum binding
	InitializeGpio(GPIOB, GPIO_PIN_11, 1);

	DelayMs(70);

	if (!bindNumber)
		bindNumber = 9;

	for (i=0; i < bindNumber; i++) {

		inlineDigitalLo(GPIOB, GPIO_PIN_11);
		DelayMs(2);

		inlineDigitalHi(GPIOB, GPIO_PIN_11);
		DelayMs(2);

	}
}

inline uint32_t ChannelMap(uint32_t inChannel)
{
	uint32_t channel = 0;

	if (1) // here is where we check which rx we are using probably use case
	{
		channel = SpektrumChannelMap(inChannel);

		if (channel == 3)
			rx_timeout = 0;

	}

	return(channel);
}

void ProcessSpektrumPacket(void)
{
	uint32_t spektrumChannel;
	uint32_t x;
	uint32_t value;
															   // Make sure this is very first thing done in function, and its called first on interrupt
	memcpy(copiedBufferData, serialRxBuffer, sizeof(copiedBufferData));    // we do this to make sure we don't have a race condition, we copy before it has a chance to be written by dma
															   // We know since we are highest priority interrupt, nothing can interrupt us, and copy happens so quick, we will alwyas be guaranteed to get it

	lastRXPacket = InlineMillis();  // why are we doing this, for failsafe?   this would have caused issues, possibly if we didn't copy buffer first
									// kalyn why are we doing this, and not just basing it on packet count?

	for (x = 2; x < 16; x += 2) {
		value = (copiedBufferData[x] << 8) + (copiedBufferData[x+1]);
		spektrumChannel = (value & 0x7800) >> 11;
		if (spektrumChannel < MAXCHANNELS) {
			rxData[ChannelMap(spektrumChannel)] = value & 0x7FF;
		}
	}
	spekPhase = copiedBufferData[2] & 0x80;

	//Check for vtx data
	if (copiedBufferData[12] == 0xE0) { 
		vtxData.vtxChannel = (copiedBufferData[13] & 0x0F) + 1;
		vtxData.vtxBand = (copiedBufferData[13] >> 5) & 0x07;
	}
      
	      //Check channel slot 7 for vtx power, pit, and region data
	if (copiedBufferData[14] == 0xE0) { 
		vtxData.vtxPower = copiedBufferData[15] & 0x03;
		vtxData.vtxRegion = (copiedBufferData[15] >> 3) & 0x01;
		vtxData.vtxPit = (copiedBufferData[15] >> 4) & 0x01;
	}

	InlineCollectRcCommand();
	RxUpdate();
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

		if (rxData[axis] < mainConfig.rcControlsConfig.midRc[axis])  //negative  range
			rangedRx = InlineChangeRangef(rxData[axis], mainConfig.rcControlsConfig.midRc[axis], mainConfig.rcControlsConfig.minRc[axis], 0.0, -1.0); //-1 to 0
		else
			rangedRx = InlineChangeRangef(rxData[axis], mainConfig.rcControlsConfig.maxRc[axis], mainConfig.rcControlsConfig.midRc[axis], 1.0, 0.0); //0 to +1



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


inline float InlineApplyRcCommandCurve (float rcCommand, uint32_t curveToUse, float expo) {

	switch (curveToUse) {

		case SKITZO_EXPO:
			return ((1 + 0.01 * expo * (rcCommand * rcCommand - 1)) * rcCommand); // KALYN listen to your ide, IT SAID USE () :)  This isn't some lame language
			break;

		case TARANIS_EXPO:
			return ( expo * (rcCommand * rcCommand * rcCommand) + rcCommand * (1-expo) );
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

    int32_t smoothingInterval = 352; //todo: calculate this number to be number of loops between PID loops
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
