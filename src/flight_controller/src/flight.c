#include "includes.h"

pid_output flightPids;
float filteredGyroData[3];
paf_state yawPafState;
paf_state rollPafState;
paf_state pitchPafState;
float actuatorRange;
float flightSetPoints[3];


void InitFlightCode(void) {

	bzero(filteredGyroData,sizeof(filteredGyroData));
	bzero(&flightPids,sizeof(flightPids));
	actuatorRange = 0;

	yawPafState   = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);
	rollPafState  = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);
	pitchPafState = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);

}


inline void InlineFlightCode(float dpsGyroArray[]) {

	//Gyro routine:
	//gyro interrupts
	//gyro read using DMA
	//updateGyro is called after the read is complete.
	//gyro and board rotation is applied to gyro data in updateGyro
	//updateGyro will call InlineFlightCode.

	//InlineFlightCode:
	//gyro filter applied
	//rc smoothing and acro+ is applied to arrive at setPoint. actuatorRange is used to limit setPoint.
	//pid controller is run using setpoint and smoothed gyro data
	//mixer is applied and outputs it's status as actuatorRange
	//output to motors

	uint32_t catfish = Micros();

	PafUpdate(&yawPafState, dpsGyroArray[YAW]);
	PafUpdate(&rollPafState, dpsGyroArray[ROLL]);
	PafUpdate(&pitchPafState, dpsGyroArray[PITCH]);

	filteredGyroData[YAW]   = yawPafState.x;
	filteredGyroData[ROLL]  = rollPafState.x;
	filteredGyroData[PITCH] = pitchPafState.x;

	InlineRcSmoothing(curvedRcCommandF, smoothedRcCommandF);

	flightSetPoints[YAW]   = InlineGetSetPoint(smoothedRcCommandF[YAW], rcControlsConfig.rates[YAW], rcControlsConfig.acroPlus[YAW]);
	flightSetPoints[ROLL]  = InlineGetSetPoint(smoothedRcCommandF[ROLL], rcControlsConfig.rates[ROLL], rcControlsConfig.acroPlus[ROLL]);
	flightSetPoints[PITCH] = InlineGetSetPoint(smoothedRcCommandF[PITCH], rcControlsConfig.rates[PITCH], rcControlsConfig.acroPlus[PITCH]);

	InlinePidController(filteredGyroData, flightSetPoints, &flightPids, actuatorRange);

	actuatorRange = InlineApplyMotorMixer(&flightPids, smoothedRcCommandF, motorOutput); //put in PIDs and Throttle or passthru

	OutputActuators(motorOutput, servoOutput);
	debugU32[0] = Micros() - catfish;



}

inline float InlineGetSetPoint(float curvedRcCommandF, float rates, float acroPlus) {
	return (curvedRcCommandF * (rates + (rates*acroPlus))); //setpoint in DPS
}
