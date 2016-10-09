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



void ProcessSpektrumPacket(void)
{
	uint32_t spektrumChannel;
	uint32_t x;

	for (x = 3; x < 16; x += 2) {
		spektrumChannel = 0x0F & (aRxBuffer[x - 1] >> spektrumChannelShift);
		if (spektrumChannel < MAXCHANNELS) {
			rxData[spektrumChannel] = ((uint32_t)(aRxBuffer[x - 1] & spektrumChannelMask) << 8) + aRxBuffer[x];
		}
	}
}


void InitRcData (void) {
	bzero(trueRcCommandF, MAXCHANNELS);
	bzero(curvedRcCommandF, MAXCHANNELS);
	bzero(smoothedRcCommandF, MAXCHANNELS);
	isRxDataNew = 0;
}

inline void InlineCollectRcCommand (uint32_t rcData[], float trueRcCommandF[], float curvedRcCommandF[], rc_control_config rcControlsConfig) {

	uint32_t axis;
	float rangedRx;

	isRxDataNew = 1; //this function is to be called by reception of vali RX data, so we know we have new RX data now

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

		if (rcData[axis] < rcControlsConfig.midRc[axis])  //negative  range
			rangedRx = InlineChangeRangef(rcData[axis], rcControlsConfig.midRc[axis], rcControlsConfig.minRc[axis], 0.0, -1.0); //-1 to 0
		else
			rangedRx = InlineChangeRangef(rcData[axis], rcControlsConfig.maxRc[axis], rcControlsConfig.midRc[axis], 1.0, 0.0); //0 to +1



		//do we want to apply deadband to trueRcCommandF? right now I think yes
		if (ABS(rangedRx) > rcControlsConfig.deadBand[axis]) {
			trueRcCommandF[axis]   = InlineConstrainf ( rangedRx, -1, 1);
			curvedRcCommandF[axis] = InlineApplyRcCommandCurve (trueRcCommandF[axis], rcControlsConfig.useCurve[axis], rcControlsConfig.curveExpo[axis]);
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
    int32_t factor;
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
