#include "includes.h"

//paf_state kdFilterState[AXIS_NUMBER];
//biquad_state kdBqFilterState[AXIS_NUMBER];

//float currentKdFilterConfig[AXIS_NUMBER];
float kdRingBuffer[AXIS_NUMBER][KD_RING_BUFFER_SIZE];
float kdRingBufferSum[AXIS_NUMBER];
uint32_t kdRingBufferPoint[AXIS_NUMBER];
float kdDelta[AXIS_NUMBER];
float kiError[AXIS_NUMBER];
float kiErrorLimit[AXIS_NUMBER];


pid_terms  pidsUsed[AXIS_NUMBER];


uint32_t uhOhRecover = 0;


void InitPid (void) {

	bzero(kiError,sizeof(kiError));
	bzero(kiErrorLimit,sizeof(kiErrorLimit));
	bzero(kdDelta, sizeof(kdDelta));
	bzero(kdRingBuffer, sizeof(kdRingBuffer));
	bzero(kdRingBufferSum, sizeof(kdRingBufferSum));
	bzero(kdRingBufferPoint, sizeof(kdRingBufferPoint));

	uhOhRecover = 0; //unset recover mode

	pidsUsed[0].kp = mainConfig.pidConfig[0].kp / 100000;
	pidsUsed[0].ki = (mainConfig.pidConfig[0].ki / 50000) * loopSpeed.dT;
	pidsUsed[0].kd = (mainConfig.pidConfig[0].kd / 200000000 )  / loopSpeed.dT;
	pidsUsed[0].wc = mainConfig.pidConfig[0].wc;

	pidsUsed[1].kp = mainConfig.pidConfig[1].kp / 100000;
	pidsUsed[1].ki = (mainConfig.pidConfig[1].ki / 50000) * loopSpeed.dT;
	pidsUsed[1].kd = (mainConfig.pidConfig[1].kd / 200000000 )  / loopSpeed.dT;
	pidsUsed[1].wc = mainConfig.pidConfig[1].wc;

	pidsUsed[2].kp = mainConfig.pidConfig[2].kp / 100000;
	pidsUsed[2].ki = (mainConfig.pidConfig[2].ki / 50000) * loopSpeed.dT;
	pidsUsed[2].kd = (mainConfig.pidConfig[2].kd / 200000000 )  / loopSpeed.dT;
	pidsUsed[2].wc = mainConfig.pidConfig[2].wc;

}


inline void InlinePidController (float filteredGyroData[], float filteredGyroDataKd[], float flightSetPoints[], pid_output flightPids[], float actuatorRange, pid_terms pidConfig[]) {

	int32_t axis;
	float pidError;
	static float lastfilteredGyroDataKd[AXIS_NUMBER];
	static float usedFlightSetPoints[AXIS_NUMBER];

	(void)(pidConfig);

	//set point limiter.
	if ( actuatorRange >= 0.90 ) {
		//we don't change the setpoint when actuators are maxed, unless setpoint is shrinking
		for (axis = 2; axis >= 0; --axis) {
			if ( abs(usedFlightSetPoints[axis]) > abs(flightSetPoints[axis]) ) {
				usedFlightSetPoints[axis] = flightSetPoints[axis];
			}
		}
	} else if ( actuatorRange >= 0.80 ) {
		//if actuator is near max, we limit the change of the setpoint unless the setpoint is shrinking
		for (axis = 2; axis >= 0; --axis) {
			if ( abs(usedFlightSetPoints[axis]) > abs(flightSetPoints[axis]) ) {
				usedFlightSetPoints[axis] = flightSetPoints[axis];
			} else {
				usedFlightSetPoints[axis] += ( (flightSetPoints[axis]-usedFlightSetPoints[axis]) * 0.5 );
			}
		}
	} else {
		//else we set the full setpoint
		usedFlightSetPoints[0] = flightSetPoints[0];
		usedFlightSetPoints[1] = flightSetPoints[1];
		usedFlightSetPoints[2] = flightSetPoints[2];
	}

	usedFlightSetPoints[0] = flightSetPoints[0];
	usedFlightSetPoints[1] = flightSetPoints[1];
	usedFlightSetPoints[2] = flightSetPoints[2];

	for (axis = 2; axis >= 0; --axis) {

		pidError = usedFlightSetPoints[axis] - filteredGyroData[axis];

	    if ( SpinStopper(axis, pidError) ) {
		//if ( 0 ) {

	    	flightPids[axis].kp = 0;
	    	flightPids[axis].ki = 0;
	    	flightPids[axis].kd = 0;

	    } else {


			// calculate Kp
			flightPids[axis].kp = (pidError * pidsUsed[axis].kp);


			// calculate Ki ////////////////////////// V
			if ( fullKiLatched ) {

				flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki + pidsUsed[axis].ki * pidError, -MAX_KD, MAX_KD); //prevent insane windup

				if ( actuatorRange > .9999 ) { //actuator maxed out, don't allow Ki to increase to prevent windup from maxed actuators
					flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki, -kiErrorLimit[axis], kiErrorLimit[axis]);
				} else {
					kiErrorLimit[axis] = ABS(flightPids[axis].ki);
				}

			} else {

				flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki + pidsUsed[axis].ki * pidError, -0.0521f, 0.0521f); //limit Ki when fullKiLatched is false

			}
			// calculate Ki ////////////////////////// ^


			// calculate Kd ////////////////////////// V
			kdDelta[axis] = -(filteredGyroDataKd[axis] - lastfilteredGyroDataKd[axis]);
			lastfilteredGyroDataKd[axis] = filteredGyroDataKd[axis];

			InlineUpdateWitchcraft(pidsUsed);

			//updated Kd filter(s)
			//if (mainConfig.filterConfig[axis].kd.r > 40.0) {
			//	kdDelta[axis] = BiquadUpdate(kdDelta[axis], &kdBqFilterState[axis]);
			//}
			//if (mainConfig.filterConfig[axis].kd.r > 0) {
			//	PafUpdate(&kdFilterState[axis], kdDelta[axis]);
			//	kdDelta[axis] = kdFilterState[axis].x;
			//}

			flightPids[axis].kd = InlineConstrainf(kdDelta[axis] * pidsUsed[axis].kd, -0.312121f, 0.312121f);
			// calculate Kd ////////////////////////// ^

	    }

	}



}

inline uint32_t SpinStopper(int32_t axis, float pidError) {

    static uint32_t countErrorUhoh[AXIS_NUMBER]  = {0, 0, 0};
    static uint32_t uhOhRecoverCounter = 0;

	if (!uhOhRecover) {
		uhOhRecoverCounter = 0;
		if (ABS(pidError) > 1000) {
			countErrorUhoh[axis]++;
		} else {
			countErrorUhoh[axis] = 0;
		}
		if (countErrorUhoh[axis] > loopSpeed.uhohNumber ) {
			uhOhRecover = 1;
		}
	} else {
		uhOhRecoverCounter++;
	}
	if (uhOhRecoverCounter > loopSpeed.uhohNumber) {
		uhOhRecover = 0;
		uhOhRecoverCounter = 0;
	}
	if (uhOhRecoverCounter) {
		return (1);
	}
	return (0);
}

inline void InlineUpdateWitchcraft(pid_terms pidConfig[]) {

	int32_t axis;

	for (axis = 2; axis >= 0; --axis) {
		if (pidConfig[axis].wc) {
			kdRingBuffer[axis][kdRingBufferPoint[axis]++] = kdDelta[axis];
			kdRingBufferSum[axis] += kdDelta[axis];

			if (kdRingBufferPoint[axis] == pidConfig[axis].wc)
				kdRingBufferPoint[axis] = 0;

			kdRingBufferSum[axis] -= kdRingBuffer[axis][kdRingBufferPoint[axis]];
			kdDelta[axis] = (float)(kdRingBufferSum[axis] / (float) (pidConfig[axis].wc));
		}
	}

}
