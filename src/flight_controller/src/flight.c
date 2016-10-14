#include "includes.h"

pid_output flightPids[AXIS_NUMBER];
float filteredGyroData[AXIS_NUMBER];
paf_state pafStates[AXIS_NUMBER];
paf_state rollPafState;
paf_state pitchPafState;
float actuatorRange;
float flightSetPoints[AXIS_NUMBER];
uint32_t boardArmed, calibrateMotors, fullKiLatched;
float currentGyroFilterConfig[AXIS_NUMBER];
uint32_t flightcodeTimeStart;
float flightcodeTime;

uint32_t counterFish = 0;
uint32_t loopCounter = 429496729U;

void InitFlightCode(void) {

	bzero(filteredGyroData,sizeof(filteredGyroData));
	bzero(&flightPids,sizeof(flightPids));
	actuatorRange = 0;
	boardArmed = 0;
	calibrateMotors = 0;
	fullKiLatched = 0;
	flightcodeTime = 0.0f;
	flightcodeTimeStart = 0;

	InlineInitGyroFilters();

}

inline void InlineInitGyroFilters(void)  {

	int32_t axis;

	for (axis = 2; axis >= 0; --axis) {

		pafStates[axis]   = InitPaf( mainConfig.filterConfig[axis].gyro.q, mainConfig.filterConfig[axis].gyro.r, mainConfig.filterConfig[axis].gyro.p, filteredGyroData[axis]);

		currentGyroFilterConfig[axis] = mainConfig.filterConfig[axis].gyro.r;

	}

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

	DoLed(0, 0);
	DoLed(1, 0);
	DoLed(2, 0);

	if (loopCounter-- & khzDivider ) { //this code runs at 1 KHz by checking the loop counter against the khzDivider bit set in InitPid();

		CheckFailsafe();

		//every 32 cycles we check if the filter needs an update.
		if (
				(currentGyroFilterConfig[YAW] != mainConfig.filterConfig[YAW].gyro.r) ||
				(currentGyroFilterConfig[ROLL] != mainConfig.filterConfig[ROLL].gyro.r) ||
				(currentGyroFilterConfig[PITCH] != mainConfig.filterConfig[PITCH].gyro.r)
		) {
			InlineInitGyroFilters();
		}

		if (
				(currentKdFilterConfig[YAW] != mainConfig.filterConfig[YAW].kd.r) ||
				(currentKdFilterConfig[ROLL] != mainConfig.filterConfig[ROLL].kd.r) ||
				(currentKdFilterConfig[PITCH] != mainConfig.filterConfig[PITCH].kd.r)
		) {
			InlineInitPidFilters();
		}

		//update blackbox here


		//check for fullKiLatched here
		if ( (boardArmed) && (smoothedRcCommandF[THROTTLE] > 0.15) ) {
			fullKiLatched = 1;
		}

		//cycle time averaged
		flightcodeTime = ( (float)Micros() - (float)flightcodeTimeStart ) * 0.03125; // 1/32 as a multiplier
		flightcodeTimeStart = Micros();
		debugU32[0] = flightcodeTime;


	}



	//update gyro filter
	PafUpdate(&pafStates[YAW], dpsGyroArray[YAW]);
	PafUpdate(&pafStates[ROLL], dpsGyroArray[ROLL]);
	PafUpdate(&pafStates[PITCH], dpsGyroArray[PITCH]);

	filteredGyroData[YAW]   = pafStates[YAW].x;
	filteredGyroData[ROLL]  = pafStates[ROLL].x;
	filteredGyroData[PITCH] = pafStates[PITCH].x;


	//smooth the rx data between rx signals
	InlineRcSmoothing(curvedRcCommandF, smoothedRcCommandF);

	//get setpoint for PIDC
	flightSetPoints[YAW]   = InlineGetSetPoint(smoothedRcCommandF[YAW], mainConfig.rcControlsConfig.rates[YAW], mainConfig.rcControlsConfig.acroPlus[YAW]);
	flightSetPoints[ROLL]  = InlineGetSetPoint(smoothedRcCommandF[ROLL], mainConfig.rcControlsConfig.rates[ROLL], mainConfig.rcControlsConfig.acroPlus[ROLL]);
	flightSetPoints[PITCH] = InlineGetSetPoint(smoothedRcCommandF[PITCH], mainConfig.rcControlsConfig.rates[PITCH], mainConfig.rcControlsConfig.acroPlus[PITCH]);

	//Run PIDC
	InlinePidController(filteredGyroData, flightSetPoints, flightPids, actuatorRange, mainConfig.pidConfig);

	if (boardArmed) {
	   if (gyroCalibrationCycles != 0) {
			return;
		}
		//only run mixer if armed
		actuatorRange = InlineApplyMotorMixer(flightPids, smoothedRcCommandF, motorOutput); //put in PIDs and Throttle or passthru
	} else {
		//otherwise we keep Ki zeroed.
		flightPids[YAW].ki = 0;
		flightPids[ROLL].ki = 0;
		flightPids[PITCH].ki = 0;
		fullKiLatched = 0;
	}

	//output to actuators
	OutputActuators(motorOutput, servoOutput);

	DoLed(0, 1);
	DoLed(1, 1);
	DoLed(2, 1);

}

inline float InlineGetSetPoint(float curvedRcCommandF, float rates, float acroPlus) {
	return ( (curvedRcCommandF * (rates + (rates*acroPlus) ) ) ); //setpoint in DPS
}
