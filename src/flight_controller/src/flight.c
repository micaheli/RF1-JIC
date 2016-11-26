#include "includes.h"

#define GYRO_AVERAGE_MAX_SUM 33

pid_output flightPids[AXIS_NUMBER];
biquad_state lpfFilterState[AXIS_NUMBER];
biquad_state lpfFilterStateKd[AXIS_NUMBER];

float averagedGyroData[AXIS_NUMBER][GYRO_AVERAGE_MAX_SUM];
uint32_t averagedGyroDataPointer[AXIS_NUMBER];
float averagedGyroDataPointerMultiplier[AXIS_NUMBER];
float filteredGyroData[AXIS_NUMBER];
float filteredGyroDataKd[AXIS_NUMBER];
float filteredAccData[VECTOR_NUMBER];
paf_state pafGyroStates[AXIS_NUMBER];
paf_state pafAccStates[AXIS_NUMBER];
float actuatorRange;
float flightSetPoints[AXIS_NUMBER];
volatile uint32_t boardArmed, calibrateMotors, fullKiLatched;
float currentGyroFilterConfig[AXIS_NUMBER];
float currentKdFilterConfig[AXIS_NUMBER];
float currentAccFilterConfig[AXIS_NUMBER];
volatile uint32_t flightcodeTimeStart;
volatile float flightcodeTime;
float pitchAttitude = 0, rollAttitude = 0, yawAttitude = 0;
uint32_t boardOrientation1 = 0;
uint32_t RfblDisasterPreventionCheck = 1;

uint32_t counterFish = 0;
uint32_t loopCounter = 0;
float accCompAccTrust, accCompGyroTrust;
volatile uint32_t SKIP_GYRO = 0;
float rollAttitudeError = 0;
float pitchAttitudeError = 0;
float rollAttitudeErrorKi = 0;
float pitchAttitudeErrorKi = 0;

void ArmBoard(void) {
	InitWatchdog(WATCHDOG_TIMEOUT_2S);
	boardArmed = 1;
}

void DisarmBoard(void) {
	boardArmed = 0;
	InitWatchdog(WATCHDOG_TIMEOUT_32S);
}

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

	bzero(lpfFilterState,sizeof(lpfFilterState));
	bzero(lpfFilterStateKd,sizeof(lpfFilterStateKd));
	bzero(averagedGyroData,sizeof(averagedGyroData));
	bzero(filteredGyroData,sizeof(filteredGyroData));
	bzero(filteredGyroDataKd,sizeof(filteredGyroDataKd));
	bzero(averagedGyroDataPointer,sizeof(averagedGyroDataPointer));
	bzero(&flightPids,sizeof(flightPids));
	averagedGyroDataPointerMultiplier[YAW] = (1.0 / (float)mainConfig.pidConfig[YAW].ga);
	averagedGyroDataPointerMultiplier[ROLL] = (1.0 / (float)mainConfig.pidConfig[ROLL].ga);
	averagedGyroDataPointerMultiplier[PITCH] = (1.0 / (float)mainConfig.pidConfig[PITCH].ga);
	actuatorRange = 0;
	boardArmed = 0;
	calibrateMotors = 0;
	fullKiLatched = 0;
	flightcodeTime = 0.0f;
	flightcodeTimeStart = 0;

	InlineInitGyroFilters();
	InlineInitKdFilters();
	InlineInitAccFilters();

}

inline void InlineInitGyroFilters(void)  {

	int32_t axis;

	for (axis = 2; axis >= 0; --axis) {

		InitBiquad(mainConfig.filterConfig[axis].gyro.r, &lpfFilterState[axis], 0.00003125, 0, &lpfFilterState[axis]);

		//pafGyroStates[axis]   = InitPaf( mainConfig.filterConfig[axis].gyro.q, mainConfig.filterConfig[axis].gyro.r, mainConfig.filterConfig[axis].gyro.p, filteredGyroData[axis]);

		currentGyroFilterConfig[axis] = mainConfig.filterConfig[axis].gyro.r;

	}

}

inline void InlineInitKdFilters(void)  {

	int32_t axis;

	for (axis = 2; axis >= 0; --axis) {

		InitBiquad(mainConfig.filterConfig[axis].kd.r, &lpfFilterStateKd[axis], 0.00003125, 0, &lpfFilterStateKd[axis]);

		//pafGyroStates[axis]   = InitPaf( mainConfig.filterConfig[axis].gyro.q, mainConfig.filterConfig[axis].gyro.r, mainConfig.filterConfig[axis].gyro.p, filteredGyroData[axis]);

		currentKdFilterConfig[axis] = mainConfig.filterConfig[axis].kd.r;

	}

}

inline void InlineInitAccFilters(void)  {

	int32_t vector;

	for (vector = 2; vector >= 0; --vector) {

		pafAccStates[vector]   = InitPaf( mainConfig.filterConfig[vector].acc.q, mainConfig.filterConfig[vector].acc.r, mainConfig.filterConfig[vector].acc.p, filteredAccData[vector]);

		currentAccFilterConfig[vector] = mainConfig.filterConfig[vector].acc.r;

	}

	accCompAccTrust  = 0.30;
	accCompGyroTrust = 0.70;

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

	if (yawAttitude > 180)
			yawAttitude = (yawAttitude - 360);
	else if (yawAttitude < -180)
			yawAttitude = (yawAttitude + 360);

	//ACCX is Z
	//ACCZ is Y
	//ACCY is X
    // Compensate for drift with accelerometer data is valid
    float forceMagnitudeApprox = ABS(filteredAccData[ACCX]) + ABS(filteredAccData[ACCY]) + ABS(filteredAccData[ACCZ]);
    if (forceMagnitudeApprox > .45 && forceMagnitudeApprox < 2.1) //only look at ACC data if it's within .45 and 2.1 Gees
    {
	// Turning around the X axis results in a vector on the Y-axis
    	pitchAcc = (atan2f( (float)filteredAccData[ACCY], (float)filteredAccData[ACCX]) + PIf) * (180.0 * IPIf) - 180.0; //multiplying by the inverse of Pi is faster than dividing by Pi
    	pitchAttitude = pitchAttitude * accCompGyroTrust + pitchAcc * accCompAccTrust;

	// Turning around the Y axis results in a vector on the X-axis
        rollAcc = (atan2f((float)filteredAccData[ACCZ], (float)filteredAccData[ACCX]) + PIf) * (180.0 * IPIf) - 180.0;
        rollAttitude = rollAttitude * accCompGyroTrust + rollAcc * accCompAccTrust;
    }
}

inline float AverageGyroADCbuffer(uint32_t axis, float currentData)
{

	if (mainConfig.pidConfig[axis].ga) {

		averagedGyroData[axis][averagedGyroDataPointer[axis]++] = currentData;
		averagedGyroData[axis][GYRO_AVERAGE_MAX_SUM-1] += currentData;

		if (averagedGyroDataPointer[axis] == mainConfig.pidConfig[axis].ga)
			averagedGyroDataPointer[axis] = 0;

		averagedGyroData[axis][GYRO_AVERAGE_MAX_SUM-1] -= averagedGyroData[axis][averagedGyroDataPointer[axis]];

		return (averagedGyroData[axis][GYRO_AVERAGE_MAX_SUM-1] * averagedGyroDataPointerMultiplier[axis]) ;

	}

	return currentData;

}

inline void InlineFlightCode(float dpsGyroArray[]) {

	static float kdAverage[3];
	static uint32_t kdAverageCounter = 0;
	int32_t axis;
	//cycle time
	//flightcodeTimeStart = Micros();

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

	if (SKIP_GYRO) {
		CheckFailsafe();
		ledStatus.status = LEDS_FAST_BLINK;
		return;
	}

	//update gyro filter
	for (axis = 2; axis >= 0; --axis) {
		filteredGyroData[axis] = BiquadUpdate(dpsGyroArray[axis], &lpfFilterState[axis]);
		filteredGyroDataKd[axis] = BiquadUpdate(dpsGyroArray[axis], &lpfFilterStateKd[axis]);
		//PafUpdate(&pafGyroStates[axis], AverageGyroADCbuffer(axis, dpsGyroArray[axis]) );
		//filteredGyroData[axis]    = pafGyroStates[axis].x;
	}

	//smooth the rx data between rx signals
	InlineRcSmoothing(curvedRcCommandF, smoothedRcCommandF);

	//get setpoint for PIDC
	if (1==1) { //if rateMode
		flightSetPoints[YAW]   = -InlineGetSetPoint(smoothedRcCommandF[YAW], mainConfig.rcControlsConfig.rates[YAW], mainConfig.rcControlsConfig.acroPlus[YAW]); //yaw is backwards for some reason
		flightSetPoints[ROLL]  = InlineGetSetPoint(smoothedRcCommandF[ROLL], mainConfig.rcControlsConfig.rates[ROLL], mainConfig.rcControlsConfig.acroPlus[ROLL]);
		flightSetPoints[PITCH] = InlineGetSetPoint(smoothedRcCommandF[PITCH], mainConfig.rcControlsConfig.rates[PITCH], mainConfig.rcControlsConfig.acroPlus[PITCH]);
	} else { //if rateMode
		flightSetPoints[YAW]   = -InlineGetSetPoint(smoothedRcCommandF[YAW], mainConfig.rcControlsConfig.rates[YAW], mainConfig.rcControlsConfig.acroPlus[YAW]); //yaw is backwards for some reason
		rollAttitudeError      = -( rollAttitude - (smoothedRcCommandF[ROLL] * 70) );
		pitchAttitudeError     = ( pitchAttitude - (smoothedRcCommandF[PITCH] * -70) );
		rollAttitudeErrorKi    = (rollAttitudeErrorKi+rollAttitudeErrorKi * 3 * dT);
		pitchAttitudeErrorKi   = (pitchAttitudeErrorKi+pitchAttitudeErrorKi * 3 * dT);
		flightSetPoints[ROLL]  = (rollAttitudeError * 5) + rollAttitudeErrorKi;
		flightSetPoints[PITCH] = (pitchAttitudeError * 5) + pitchAttitudeErrorKi;
	}

	//Run PIDC
	InlinePidController(filteredGyroData, filteredGyroDataKd, flightSetPoints, flightPids, actuatorRange, mainConfig.pidConfig);

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
		rollAttitudeErrorKi = 0;
		pitchAttitudeErrorKi = 0;
		fullKiLatched = 1;
	}

	//output to actuators. This is the end of the flight code for this iteration.
	OutputActuators(motorOutput, servoOutput);


	//this code is less important that stabilization, so it happens AFTER stabilization.
	//Anything changed here can happen after the next iteration without any drawbacks. Stabilization above all else!

	ComplementaryFilterUpdateAttitude(); //stabilization above all else. This update happens after gyro stabilization

	kdAverage[YAW]   += flightPids[YAW].kd;
	kdAverage[ROLL]  += flightPids[ROLL].kd;
	kdAverage[PITCH] += flightPids[PITCH].kd;
	//MAX_KD
	if (loopCounter-- == 0) {
		loopCounter=khzDivider;

		kdAverageCounter++;
		if (kdAverageCounter == 11) {
			kdAverageCounter=0;
			kdAverage[YAW]   /= (khzDivider * 10);
			kdAverage[ROLL]  /= (khzDivider * 10);
			kdAverage[PITCH] /= (khzDivider * 10);

			//experimental auto filter
			mainConfig.filterConfig[YAW].kd.r   = InlineChangeRangef(ABS(kdAverage[YAW]), MAX_KD, 0, 70, 40);
			mainConfig.filterConfig[ROLL].kd.r  = InlineChangeRangef(ABS(kdAverage[ROLL]), MAX_KD, 0, 70, 40);
			mainConfig.filterConfig[PITCH].kd.r = InlineChangeRangef(ABS(kdAverage[PITCH]), MAX_KD, 0, 70, 40);

			kdAverage[YAW]   = 0;
			kdAverage[ROLL]  = 0;
			kdAverage[PITCH] = 0;
		}

/*
		if (trueRcCommandF[AUX3] < -0.5)
		{
			mainConfig.filterConfig[YAW].kd.r = 45.0;
			mainConfig.filterConfig[ROLL].kd.r = 65.0;
			mainConfig.filterConfig[PITCH].kd.r = 65.0;
		}
		else if (trueRcCommandF[AUX3] > 0.5)
		{
			mainConfig.filterConfig[YAW].kd.r = 45.0;
			mainConfig.filterConfig[ROLL].kd.r = 50.0;
			mainConfig.filterConfig[PITCH].kd.r = 50.0;
		}
		else
		{
			mainConfig.filterConfig[YAW].kd.r = 45.0;
			mainConfig.filterConfig[ROLL].kd.r = 57.5;
			mainConfig.filterConfig[PITCH].kd.r = 57.5;
		}
*/
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
			InlineInitKdFilters();
		}

		if (
				(currentAccFilterConfig[ACCX] != mainConfig.filterConfig[ACCX].acc.r) ||
				(currentAccFilterConfig[ACCY] != mainConfig.filterConfig[ACCY].acc.r) ||
				(currentAccFilterConfig[ACCZ] != mainConfig.filterConfig[ACCZ].acc.r)
		) {
			InlineInitAccFilters();
		}

		//update blackbox here   //void UpdateBlackbox(pid_output *flightPids)
		UpdateBlackbox(flightPids, flightSetPoints, dpsGyroArray, filteredGyroData, filteredAccData);

		//check for fullKiLatched here
		if ( (boardArmed) && (smoothedRcCommandF[THROTTLE] > -0.65) ) {
			fullKiLatched = 1;
		}


		if (RfblDisasterPreventionCheck) {
			if (InlineMillis() > 5000) {
				RfblDisasterPreventionCheck = 0;
				HandleRfblDisasterPrevention();
			}
		}

	}


	//cycle time
	//flightcodeTime = ( (float)Micros() - (float)flightcodeTimeStart );
	//debugU32[0] = flightcodeTime;
}

inline float InlineGetSetPoint(float curvedRcCommandF, float rates, float acroPlus) {
	return ( (curvedRcCommandF * ( rates + (rates * acroPlus) ) ) ); //setpoint in DPS
}
