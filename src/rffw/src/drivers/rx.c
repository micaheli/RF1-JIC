#include "includes.h"

inline void collectRcCommand (uint16_t rcData[4], float *trueRcCommandF, float *curvedRcCommandF) {

	uint8_t axis;
	float OldRange, OldValue, NewRange, NewValue, OldMax, OldMin, NewMax, NewMin;

	//scale
    //////masterConfig.rxConfig.midrc = 1500;
    //////masterConfig.rxConfig.mincheck = 1005;
    //////masterConfig.rxConfig.maxcheck = 1990;
    //into a -1 to 1 float;
	//because of how midrc works we must do this separately or negative and positive values.
	//this method won't require a __disable_irq
	//////rcData is 1000 to 2000. It can never be negative.

	//todo these need to be configurable
	uint16_t midRc    = 1500; // middle of rc input
	uint16_t minCheck = 1005; // rcData should be adjusted to 1000 to 2000, which makes this range work well
	uint16_t maxCheck = 1995; // rcData should be adjusted to 1000 to 2000, which makes this range work well

	for (axis = 0; axis < 3; axis++) {

		if (rcData[axis] < midRc) { //negative  range
			//-1 to 0
			OldMax = midRc;
			OldMin = minCheck;
			NewMax = 0;
			NewMin = -1;
		} else { //positive range
			//0 to +1
			OldMax = maxCheck;
			OldMin = midRc;
			NewMax = 1;
			NewMin = 0;
		}

		OldValue = rcData[axis];
		OldRange = (OldMax - OldMin);
		NewRange = (NewMax - NewMin);
		NewValue = (((OldValue - OldMin) * NewRange) / OldRange) + NewMin;

		trueRcCommandF[axis] = constrainf ( NewValue, -1, 1);

		curvedRcCommandF[axis] = applyRcCommandCurve (trueRcCommandF[axis]);

	}

}

inline float applyRcCommandCurve (float rcCommand) {

	float expo = 60; //todo: Replace with config variable

	return (1 + 0.01 * expo * (rcCommand * rcCommand - 1)) * rcCommand;

}
