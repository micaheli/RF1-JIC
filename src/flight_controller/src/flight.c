#include "includes.h"

pid_output flightPids;
float filteredGyroData[3];
paf_state yawPafState;
paf_state rollPafState;
paf_state pitchPafState;
float actuatorRange;

void InitFlightCode(void) {

	bzero(filteredGyroData,sizeof(filteredGyroData));
	bzero(&flightPids,sizeof(flightPids));
	actuatorRange = 0;

	yawPafState   = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);
	rollPafState  = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);
	pitchPafState = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);

}


inline void InlineFlightCode(float dpsGyroArray[]) {

	//gyro interrupts
	//gyro read using DMA
	//updateGyro is called after the read is complete.
	//updateGyro will call InlineFlightCode.
	//gyro filter applied
	//pid controller is run
	//mixer is applied
	//output to motors

	PafUpdate(&yawPafState, filteredGyroData[YAW]);
	PafUpdate(&rollPafState, filteredGyroData[ROLL]);
	PafUpdate(&pitchPafState, filteredGyroData[PITCH]);

	InlinePidController(filteredGyroData, curvedRcCommandF, &flightPids, actuatorRange);
	actuatorRange = InlineApplyMotorMixer(&flightPids, curvedRcCommandF);
	OutputActuators(motorOutput, servoOutput);

}
