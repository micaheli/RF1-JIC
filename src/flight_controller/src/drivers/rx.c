#include "includes.h"

float trueRcCommandF[MAXCHANNELS];     //4 sticks. range is -1 to 1, directly related to stick position
float curvedRcCommandF[MAXCHANNELS];   //4 sticks. range is -1 to 1, this is the rcCommand after the curve is applied
float smoothedRcCommandF[MAXCHANNELS]; //4 sticks. range is -1 to 1, this is the smoothed rcCommand
unsigned char isRxDataNew;


uint32_t rxData[MAXCHANNELS];



// 2048 resolution
uint32_t spektrumChannelShift = 3;
uint32_t spektrumChannelMask = 0x07;

/*
//1024 resolution
spektrumChannelShift = 2;
spektrumChannelMask = 0x03;
*/


uint32_t tempData[MAXCHANNELS];


void ProcessSpektrumPacket(void)
{
	uint32_t spektrumChannel;
	uint32_t x;
	uint32_t value;
	static uint32_t disarmCount = 0;

	bzero(&tempData, sizeof(tempData));

	lastRXPacket = InlineMillis();

	for (x = 2; x < 16; x += 2) {

		value = (aRxBuffer[x] << 8) + (aRxBuffer[x+1]);
		spektrumChannel = (value & 0x7800) >> 11;
		if (spektrumChannel < MAXCHANNELS) {
			tempData[spektrumChannel] = value & 0x7FF;
		}
	}


	if ((tempData[0] != tempData[3]) && (tempData[0] != 0))
		rxData[3] = tempData[0];

	rxData[0] = tempData[3];
	rxData[1] = tempData[1];
	rxData[2] = tempData[2];


	for (x=4;x<MAXCHANNELS;x++)
	{
		if (tempData[x] != 0) {
			rxData[x] = tempData[x];
		}


	}


	/*
	if (rxData[0] > 200) {
		volatile uint32_t cat = 88;
	} else {
		volatile uint32_t dog = 88;

	}
*/
	/*
	x=rxData[0];
	rxData[0] = rxData[3];
	rxData[3] = x;

*/

	//todo: MOVE!!! - uglied up Preston's code.
	if ( (!boardArmed) && (rxData[4] > 1500) ) {
		disarmCount = 0;
		if (rtc_read_backup_reg(FC_STATUS_REG) == FC_STATUS_INFLIGHT) {
			//fc crashed during flight
			debugU32[6]=666;
		} else {
			ResetGyroCalibration();
			rtc_write_backup_reg(FC_STATUS_REG,FC_STATUS_INFLIGHT);
			debugU32[6]=777;
		}
		boardArmed = 1;
		mainConfig.rcControlsConfig.midRc[PITCH] = rxData[PITCH];
		mainConfig.rcControlsConfig.midRc[ROLL]  = rxData[ROLL];
		mainConfig.rcControlsConfig.midRc[YAW]   = rxData[YAW];
	} else if (rxData[4] < 400) {
		disarmCount++;
		if (disarmCount > 10) {
			boardArmed = 0;
		}
	}

	InlineCollectRcCommand();
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
			return (1 + 0.01 * expo * (rcCommand * rcCommand - 1)) * rcCommand;
			break;

		case TARANIS_EXPO:
			return ( expo * (rcCommand * rcCommand * rcCommand) + rcCommand * (1-expo) );
			break;

		case NO_EXPO:
		default:
			return rcCommand; //same as default for now.
			break;

	}
}


inline void InlineRcSmoothing(float curvedRcCommandF[], float smoothedRcCommandF[]) {
    static float lastCommand[4] = { 0, 0, 0, 0 };
    static float deltaRC[4] = { 0, 0, 0, 0 };
    static int32_t factor = 0;
    int32_t channel;

    int32_t smoothingInterval = 88; //todo: calculate this number to be number of loops between PID loops
	//88  for spektrum at  8 KHz loop time
	//264 for spektrum at 32 KHz loop time
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
