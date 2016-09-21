#include "includes.h"

inline void inlineCollectRcCommand (uint16_t rcData[4], float *trueRcCommandF, float *curvedRcCommandF, rcControlsConfig_t rcControlsConfig) {

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

		rangedRx = inlineChangeRangef(oldValue, oldMax, oldMin, newMax, newMin);

		//do we want to apply deadband to trueRcCommandF? right now I think yes
		if (ABS(rangedRx) > rcControlsConfig.deadBand[axis]) {
			trueRcCommandF[axis]   = inlineConstrainf ( rangedRx, -1, 1);
			curvedRcCommandF[axis] = inlineApplyRcCommandCurve (trueRcCommandF[axis], rcControlsConfig.useCurve[axis], rcControlsConfig.curveExpo[axis]);
		} else {
			// no need to calculate if movement is below deadband
			trueRcCommandF[axis]   = 0;
			curvedRcCommandF[axis] = 0;
		}

	}

}


inline float inlineApplyRcCommandCurve (float rcCommand, uint8_t curveToUse, float expo) {

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
