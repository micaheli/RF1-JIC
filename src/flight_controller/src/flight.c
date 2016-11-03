#include "includes.h"

pid_output flightPids[AXIS_NUMBER];
float filteredGyroData[AXIS_NUMBER];
float filteredAccData[VECTOR_NUMBER];
paf_state pafGyroStates[AXIS_NUMBER];
paf_state pafAccStates[AXIS_NUMBER];
float actuatorRange;
float flightSetPoints[AXIS_NUMBER];
uint32_t boardArmed, calibrateMotors, fullKiLatched;
float currentGyroFilterConfig[AXIS_NUMBER];
float currentAccFilterConfig[AXIS_NUMBER];
uint32_t flightcodeTimeStart;
float flightcodeTime;
float pitchAttitude = 0, rollAttitude = 0, yawAttitude = 0;
uint32_t boardOrientation1 = 0;
uint32_t RfblDisasterPreventionCheck = 1;

uint32_t counterFish = 0;
uint32_t loopCounter = 429496729U;
float accCompAccTrust, accCompGyroTrust;

int SetCalibrate1(void) {

	if ( ABS(filteredAccData[ACCX]) > (ABS(filteredAccData[ACCY]) + ABS(filteredAccData[ACCZ])) )
	{
		// is king
		if (filteredAccData[ACCX] < 0) { //ACCX negative
			//board inverted
			return (boardOrientation1 = CALIBRATE_BOARD_INVERTED);
		} else { //ACCX positive
			//board upright
			return (boardOrientation1 = CALIBRATE_BOARD_UPRIGHT);
		}

	}
	else if ( ABS(filteredAccData[ACCY]) > (ABS(filteredAccData[ACCX]) + ABS(filteredAccData[ACCZ])) )
	{
		//ACCY is king
		if (filteredAccData[ACCY] < 0) { //ACCY negative
			return (CALIBRATE_BOARD_FAILED); //board sideways or on nose, return 0
		} else { //ACCY positive
			return (CALIBRATE_BOARD_FAILED); //board sideways or on nose, return 0
		}
	}
	else if ( ABS(filteredAccData[ACCZ]) > (ABS(filteredAccData[ACCX]) + ABS(filteredAccData[ACCY])) )
	{
		//ACCZ is king
		if (filteredAccData[ACCZ] < 0) { //ACCZ negative
			return (CALIBRATE_BOARD_FAILED); //board sideways or on nose, return 0
		} else { //ACCZ positive
			return (CALIBRATE_BOARD_FAILED); //board sideways or on nose, return 0
		}
	}

	return (CALIBRATE_BOARD_FAILED); //check sanity and return 0 if result not sane.

}

int SetCalibrate2(void) {

	if (!boardOrientation1) { //make sure step one completed successfully.
		return (0);
	}


	if ( ABS(filteredAccData[ACCX]) > (ABS(filteredAccData[ACCY]) + ABS(filteredAccData[ACCZ])) )
	{
		//ACCX is king
		if (filteredAccData[ACCX] < 0) { //ACCX negative
			boardOrientation1 = 0; //calibration done, reset check
			return (CALIBRATE_BOARD_FAILED); //board is not on its side
		} else { //ACCX positive
			boardOrientation1 = 0; //calibration done, reset check
			return (CALIBRATE_BOARD_FAILED); //board is not on its side
		}

	}
	else if ( ABS(filteredAccData[ACCY]) > (ABS(filteredAccData[ACCX]) + ABS(filteredAccData[ACCZ])) )
	{
		//ACCY is king
		if (filteredAccData[ACCY] < 0) { //ACCY negative
			if (boardOrientation1 == CALIBRATE_BOARD_UPRIGHT) {
				mainConfig.gyroConfig.boardCalibrated = 1;
				mainConfig.gyroConfig.gyroRotation = CW0; //
				boardOrientation1 = 0; //calibration done, reset check
				return (1); //calibration looks good
			} else {
				mainConfig.gyroConfig.boardCalibrated = 1;
				mainConfig.gyroConfig.gyroRotation = CW0_INV; //
				boardOrientation1 = 0; //calibration done, reset check
				return (1); //calibration looks good
			}
		} else { //ACCY positive
			if (boardOrientation1 == CALIBRATE_BOARD_UPRIGHT) {
				mainConfig.gyroConfig.boardCalibrated = 1;
				mainConfig.gyroConfig.gyroRotation = CW180;
				boardOrientation1 = 0; //calibration done, reset check
				return (1); //calibration looks good
			} else {
				mainConfig.gyroConfig.boardCalibrated = 1;
				mainConfig.gyroConfig.gyroRotation = CW180_INV;
				boardOrientation1 = 0; //calibration done, reset check
				return (1); //calibration looks good
			}
		}
	}
	else if ( ABS(filteredAccData[ACCZ]) > (ABS(filteredAccData[ACCX]) + ABS(filteredAccData[ACCY])) )
	{
		//ACCZ is king
		if (filteredAccData[ACCZ] < 0) { //ACCZ negative
			if (boardOrientation1 == CALIBRATE_BOARD_UPRIGHT) {
				mainConfig.gyroConfig.boardCalibrated = 1;
				mainConfig.gyroConfig.gyroRotation = CW90;
				boardOrientation1 = 0; //calibration done, reset check
				return (1); //calibration looks good
			} else {
				mainConfig.gyroConfig.boardCalibrated = 1;
				mainConfig.gyroConfig.gyroRotation = CW90_INV;
				boardOrientation1 = 0; //calibration done, reset check
				return (1); //calibration looks good
			}
		} else { //ACCZ positive
			if (boardOrientation1 == CALIBRATE_BOARD_UPRIGHT) {
				mainConfig.gyroConfig.boardCalibrated = 1;
				mainConfig.gyroConfig.gyroRotation = CW270;
				boardOrientation1 = 0; //calibration done, reset check
				return (1); //calibration looks good
			} else {
				mainConfig.gyroConfig.boardCalibrated = 1;
				mainConfig.gyroConfig.gyroRotation = CW270_INV;
				boardOrientation1 = 0; //calibration done, reset check
				return (1); //calibration looks good
			}
		}
	}

	boardOrientation1 = 0; //once we get this far we need to repeat step 1 in the event of a failure.

	return (CALIBRATE_BOARD_FAILED); //check sanity and return 0 if result not sane.

}

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
	InlineInitAccFilters();

}

inline void InlineInitGyroFilters(void)  {

	int32_t axis;

	for (axis = 2; axis >= 0; --axis) {

		pafGyroStates[axis]   = InitPaf( mainConfig.filterConfig[axis].acc.q, mainConfig.filterConfig[axis].acc.r, mainConfig.filterConfig[axis].gyro.p, filteredGyroData[axis]);

		currentGyroFilterConfig[axis] = mainConfig.filterConfig[axis].acc.r;

	}

}

inline void InlineInitAccFilters(void)  {

	int32_t vector;

	for (vector = 2; vector >= 0; --vector) {

		pafAccStates[vector]   = InitPaf( mainConfig.filterConfig[vector].acc.q, mainConfig.filterConfig[vector].acc.r, mainConfig.filterConfig[vector].acc.p, filteredAccData[vector]);

		currentAccFilterConfig[vector] = mainConfig.filterConfig[vector].acc.r;

	}

	accCompAccTrust = 0.1;
	accCompGyroTrust = 0.9;
}


inline void InlineUpdateAttitude(float geeForceAccArray[]) {
	//update gyro filter
	PafUpdate(&pafAccStates[ACCX], geeForceAccArray[ACCX]);
	PafUpdate(&pafAccStates[ACCY], geeForceAccArray[ACCY]);
	PafUpdate(&pafAccStates[ACCZ], geeForceAccArray[ACCZ]);

	filteredAccData[ACCX] = pafAccStates[ACCX].x;
	filteredAccData[ACCY] = pafAccStates[ACCY].x;
	filteredAccData[ACCZ] = pafAccStates[ACCZ].x;

}


void ComplementaryFilterUpdateAttitude(void)
{
	static float pitchAcc = 0, rollAcc = 0;

    // Integrate the gyroscope data
	pitchAttitude += (filteredGyroData[PITCH] * dT);
	rollAttitude += (filteredGyroData[ROLL] * dT);
	yawAttitude += (filteredGyroData[YAW] * dT);

    // Compensate for drift with accelerometer data is valid
    float forceMagnitudeApprox = ABS(filteredAccData[ACCX]) + ABS(filteredAccData[ACCY]) + ABS(filteredAccData[ACCZ]);
    if (forceMagnitudeApprox > .45 && forceMagnitudeApprox < 2.1) //only look at ACC data if it's within .45 and 2.1 Gees
    {
	// Turning around the X axis results in a vector on the Y-axis
    	pitchAcc = atan2f( (float)filteredAccData[ACCY], (float)filteredAccData[ACCZ]) * 180 * IPIf; //multiplying by the inverse of Pi is faster than dividing by Pi
    	pitchAttitude = pitchAttitude * accCompGyroTrust + pitchAcc * accCompAccTrust;

	// Turning around the Y axis results in a vector on the X-axis
        rollAcc = atan2f((float)filteredAccData[ACCX], (float)filteredAccData[ACCZ]) * 180 * IPIf;
        rollAttitude = rollAttitude * accCompGyroTrust + rollAcc * accCompAccTrust;
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

		if (
				(currentAccFilterConfig[ACCX] != mainConfig.filterConfig[ACCX].acc.r) ||
				(currentAccFilterConfig[ACCY] != mainConfig.filterConfig[ACCY].acc.r) ||
				(currentAccFilterConfig[ACCZ] != mainConfig.filterConfig[ACCZ].acc.r)
		) {
			InlineInitAccFilters();
		}

		//update blackbox here   //void UpdateBlackbox(pid_output *flightPids)
		UpdateBlackbox(flightPids);

		//check for fullKiLatched here
		if ( (boardArmed) && (smoothedRcCommandF[THROTTLE] > -0.65) ) {
			fullKiLatched = 1;
		}

		//cycle time averaged
		flightcodeTime = ( (float)Micros() - (float)flightcodeTimeStart ) * 0.03125; // 1/32 as a multiplier
		flightcodeTimeStart = Micros();
		debugU32[0] = flightcodeTime;

		if (RfblDisasterPreventionCheck) {
			if (InlineMillis() > 5000) {
				RfblDisasterPreventionCheck = 0;
				HandleRfblDisasterPrevention();
			}
		}

	}



	//update gyro filter
	PafUpdate(&pafGyroStates[YAW], dpsGyroArray[YAW]);
	PafUpdate(&pafGyroStates[ROLL], dpsGyroArray[ROLL]);
	PafUpdate(&pafGyroStates[PITCH], dpsGyroArray[PITCH]);

	filteredGyroData[YAW]   = pafGyroStates[YAW].x;
	filteredGyroData[ROLL]  = pafGyroStates[ROLL].x;
	filteredGyroData[PITCH] = pafGyroStates[PITCH].x;


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

	if (!boardArmed)
		ComplementaryFilterUpdateAttitude(); //stabilization above all else. This update happens after gyro stabilization


}

inline float InlineGetSetPoint(float curvedRcCommandF, float rates, float acroPlus) {
	return ( (curvedRcCommandF * (rates + (rates*acroPlus) ) ) ); //setpoint in DPS
}
