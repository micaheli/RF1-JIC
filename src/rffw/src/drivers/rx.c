#include "includes.h"

float trueRcCommandF[MAXCHANNELS];     //4 sticks. range is -1 to 1, directly related to stick position
float curvedRcCommandF[MAXCHANNELS];   //4 sticks. range is -1 to 1, this is the rcCommand after the curve is applied
float smoothedRcCommandF[MAXCHANNELS]; //4 sticks. range is -1 to 1, this is the smoothed rcCommand

void InitRcData (void) {
	bzero(trueRcCommandF, MAXCHANNELS);
	bzero(curvedRcCommandF, MAXCHANNELS);
	bzero(smoothedRcCommandF, MAXCHANNELS);
}

inline void InlineCollectRcCommand (uint16_t rcData[], float trueRcCommandF[], float curvedRcCommandF[], rc_control_config rcControlsConfig) {

	uint8_t axis;
	float rangedRx, oldValue, oldMax, oldMin, newMax, newMin;

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

		if (rcData[axis] < rcControlsConfig.midRc[axis]) { //negative  range
			//-1 to 0
			oldMax = rcControlsConfig.midRc[axis];
			oldMin = rcControlsConfig.minRc[axis];
			newMax = 0;
			newMin = -1;
		} else { //positive range
			//0 to +1
			oldMax = rcControlsConfig.maxRc[axis];
			oldMin = rcControlsConfig.midRc[axis];
			newMax = 1;
			newMin = 0;
		}

		oldValue = rcData[axis];

		rangedRx = InlineChangeRangef(oldValue, oldMax, oldMin, newMax, newMin);

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


inline float InlineApplyRcCommandCurve (float rcCommand, uint8_t curveToUse, float expo) {

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
