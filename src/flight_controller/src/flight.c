#include "includes.h"

static pid_output flightPids;
static float filteredGyroData[3];
static paf_state yawPafState;
static paf_state rollPafState;
static paf_state pitchPafState;

void InitFlightCode(void) {

	bzero(filteredGyroData,sizeof(filteredGyroData));
	bzero(flightPids,sizeof(flightPids));

	yawPafState   = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);
	rollPafState  = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);
	pitchPafState = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);

}


inline void InlineFlightCode(float dpsGyroArray[]) {

	static float actuatorRange = 0;

	//gyro interrupts
	//gyro read using DMA
	//updateGyro is called after the read is complete.
	//updateGyro will call InlineFlightCode.
	//gyro filter applied
	//pid controller is run
	//mixer is applied
	//output to motors

	PafUpdate(yawPafState, filteredGyroData[YAW]);
	PafUpdate(rollPafState, filteredGyroData[ROLL]);
	PafUpdate(pitchPafState, filteredGyroData[PITCH]);

	flightPids = InlinePidController(filteredGyroData, curvedRcCommandF, flightPids, actuatorRange);
	actuatorRange = InlineApplyMotorMixer(flightPids, curvedRcCommandF);
	OutputActuators(motorOutput, servoOutput);

}
