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


//1000000 114
pid_terms  pidsUsed[AXIS_NUMBER];

float dT; //8KHz
float InversedT; //8KHz
uint32_t uhohNumber = 4000;
uint32_t uhOhRecover = 0;
volatile uint32_t khzDivider = 0;

/*
 *     LOOP_L1,
    LOOP_M1,
    LOOP_M2,
    LOOP_M4,
    LOOP_M8,
    LOOP_H1,
    LOOP_H2,
    LOOP_H4,
    LOOP_H8,
    LOOP_H16,
    LOOP_H32,
    LOOP_UH1,
    LOOP_UH2,
    LOOP_UH4,
    LOOP_UH8,
    LOOP_UH16,
    LOOP_UH32,
 */
void InitPid (void) {

	bzero(kiError,sizeof(kiError));
	bzero(kiErrorLimit,sizeof(kiErrorLimit));
	bzero(kdDelta, sizeof(kdDelta));
	bzero(kdRingBuffer, sizeof(kdRingBuffer));
	bzero(kdRingBufferSum, sizeof(kdRingBufferSum));
	bzero(kdRingBufferPoint, sizeof(kdRingBufferPoint));

	uhOhRecover = 0; //unset recover mode

	switch (mainConfig.gyroConfig.loopCtrl) {
		case LOOP_UH32:
		case LOOP_H32:
			dT = 0.00003125;
			uhohNumber = 16000;
			khzDivider = 32;
			break;
		case LOOP_UH16:
		case LOOP_H16:
			dT = 0.00006250;
			uhohNumber = 8000;
			khzDivider = 16;
			break;
		case LOOP_UH8:
		case LOOP_H8:
		case LOOP_M8:
			dT = 0.00012500;
			uhohNumber = 4000;
			khzDivider = 8;
			break;
		case LOOP_UH4:
		case LOOP_H4:
		case LOOP_M4:
			dT = 0.00025000;
			uhohNumber = 2000;
			khzDivider = 4;
			break;
		case LOOP_UH2:
		case LOOP_H2:
		case LOOP_M2:
			dT = 0.00050000;
			uhohNumber = 1000;
			khzDivider = 2;
			break;
		case LOOP_UH1:
		case LOOP_H1:
		case LOOP_M1:
		case LOOP_L1:
		default:
			dT = 0.00100000;
			uhohNumber = 500;
			khzDivider = 1;
			break;
	}

	InversedT = (1/dT);
	//InlineInitPidFilters();

	//0.00   (x / 10000) * 1 = .00140000
	//0.00   (x / 10000) * 0.00003125 = .00140000
	//0.00   (x * 100) * 0.00003125 = .00000280

	//140
	//140
	//200

	pidsUsed[0].kp = mainConfig.pidConfig[0].kp / 100000;
	pidsUsed[0].ki = (mainConfig.pidConfig[0].ki / 50000) * dT;
	pidsUsed[0].kd = (mainConfig.pidConfig[0].kd / 200000000 )  / dT;
	pidsUsed[0].wc = mainConfig.pidConfig[0].wc;

	pidsUsed[1].kp = mainConfig.pidConfig[1].kp / 100000;
	pidsUsed[1].ki = (mainConfig.pidConfig[1].ki / 50000) * dT;
	pidsUsed[1].kd = (mainConfig.pidConfig[1].kd / 200000000 )  / dT;
	pidsUsed[1].wc = mainConfig.pidConfig[1].wc;

	pidsUsed[2].kp = mainConfig.pidConfig[2].kp / 100000;
	pidsUsed[2].ki = (mainConfig.pidConfig[2].ki / 50000) * dT;
	pidsUsed[2].kd = (mainConfig.pidConfig[2].kd / 200000000 )  / dT;
	pidsUsed[2].wc = mainConfig.pidConfig[2].wc;

}

//inline void InlineInitPidFilters (void) {

	//static int onlyOnce = 1;
	//int32_t axis;

	//for (axis = 2; axis >= 0; --axis) {

		//kdFilterState[axis] = InitPaf( mainConfig.filterConfig[axis].kd.q, mainConfig.filterConfig[axis].kd.r, mainConfig.filterConfig[axis].kd.p, kdDelta[axis]);
		//currentKdFilterConfig[axis] = mainConfig.filterConfig[axis].kd.r;

		//if (onlyOnce) { //biquad can't be reinitialized.
		//	//mainConfig.filterConfig[axis].kdBq.lpfHz
		//	InitBiquad(mainConfig.filterConfig[axis].kd.r, &kdBqFilterState[axis], dT, 0);
		//}

	//}

	//onlyOnce = 0;

//}

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
		if (countErrorUhoh[axis] > uhohNumber ) {
			uhOhRecover = 1;
		}
	} else {
		uhOhRecoverCounter++;
	}
	if (uhOhRecoverCounter > uhohNumber) {
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
