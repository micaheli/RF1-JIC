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
biquad_state kdBqFilterState[AXIS_NUMBER];
lpf_state yawKpLpf;

pid_terms  pidsUsed[AXIS_NUMBER];


uint32_t uhOhRecover = 0;


void InitPid (void)
{

	bzero(kiError,           sizeof(kiError));
	bzero(kiErrorLimit,      sizeof(kiErrorLimit));
	bzero(kdDelta,           sizeof(kdDelta));
	bzero(kdRingBuffer,      sizeof(kdRingBuffer));
	bzero(kdRingBufferSum,   sizeof(kdRingBufferSum));
	bzero(kdRingBufferPoint, sizeof(kdRingBufferPoint));
	bzero(kdBqFilterState,   sizeof(kdBqFilterState));

	uhOhRecover = 0; //unset recover mode

	if (mainConfig.mixerConfig.mixerStyle == 1)
	{
		pidsUsed[0].kp = mainConfig.pidConfig[0].kp  / 100000;
		pidsUsed[0].ki = (mainConfig.pidConfig[0].ki / 50000) * loopSpeed.dT;
		pidsUsed[0].kd = (mainConfig.pidConfig[0].kd / 200000000)  / loopSpeed.dT;
		pidsUsed[0].wc = mainConfig.pidConfig[0].wc;

		pidsUsed[1].kp = mainConfig.pidConfig[1].kp  / 100000;
		pidsUsed[1].ki = (mainConfig.pidConfig[1].ki / 50000) * loopSpeed.dT;
		pidsUsed[1].kd = (mainConfig.pidConfig[1].kd / 200000000)  / loopSpeed.dT;
		pidsUsed[1].wc = mainConfig.pidConfig[1].wc;

		pidsUsed[2].kp = mainConfig.pidConfig[2].kp  / 100000;
		pidsUsed[2].ki = (mainConfig.pidConfig[2].ki / 50000) * loopSpeed.dT;
		pidsUsed[2].kd = (mainConfig.pidConfig[2].kd / 200000000)  / loopSpeed.dT;
		pidsUsed[2].wc = mainConfig.pidConfig[2].wc;
	}
	else
	{
		pidsUsed[0].kp = mainConfig.pidConfig[0].kp  / 50000;
		pidsUsed[0].ki = (mainConfig.pidConfig[0].ki / 25000) * loopSpeed.dT;
		pidsUsed[0].kd = (mainConfig.pidConfig[0].kd / 100000000)  / loopSpeed.dT;
		pidsUsed[0].wc = mainConfig.pidConfig[0].wc;

		pidsUsed[1].kp = mainConfig.pidConfig[1].kp  / 50000;
		pidsUsed[1].ki = (mainConfig.pidConfig[1].ki / 25000) * loopSpeed.dT;
		pidsUsed[1].kd = (mainConfig.pidConfig[1].kd / 100000000)  / loopSpeed.dT;
		pidsUsed[1].wc = mainConfig.pidConfig[1].wc;

		pidsUsed[2].kp = mainConfig.pidConfig[2].kp  / 50000;
		pidsUsed[2].ki = (mainConfig.pidConfig[2].ki / 25000) * loopSpeed.dT;
		pidsUsed[2].kd = (mainConfig.pidConfig[2].kd / 100000000)  / loopSpeed.dT;
		pidsUsed[2].wc = mainConfig.pidConfig[2].wc;
	}

	LpfInit(&yawKpLpf, 30.0f, loopSpeed.dT);

}


inline uint32_t InlinePidController (float filteredGyroData[], float flightSetPoints[], pid_output flightPids[], float actuatorRange, pid_terms pidConfig[])
{

	int32_t axis;
	float pidError;
	static float lastfilteredGyroData[AXIS_NUMBER];
	static float usedFlightSetPoints[AXIS_NUMBER];
	static uint32_t filterSetup[AXIS_NUMBER] = {0,0,0};
	static float lastSetPoint[AXIS_NUMBER] = {0,0,0};

	//test yaw reductor
	static uint32_t yawCounter = 0;
	static float yawKp = 0;
	static float yawKd = 0;
	static float yawKi = 0;
	static float yawKpStore = 0;
	static float yawKdStore = 0;
	static float yawKiStore = 0;
	//test yaw reductor

	(void)(pidConfig);
	(void)(actuatorRange);

	//set point limiter.
	if ( actuatorRange >= 0.90 )
	{
		//we don't change the setpoint when actuators are maxed, unless setpoint is shrinking
		for (axis = 2; axis >= 0; --axis)
		{
			if ( abs(usedFlightSetPoints[axis]) > abs(flightSetPoints[axis]) )
			{
				usedFlightSetPoints[axis] = flightSetPoints[axis];
			}
		}
	}
	else if ( actuatorRange >= 0.80 )
	{
		//if actuator is near max, we limit the change of the setpoint unless the setpoint is shrinking

		for (axis = 2; axis >= 0; --axis)
		{
			if ( abs(usedFlightSetPoints[axis]) > abs(flightSetPoints[axis]) )
			{
				usedFlightSetPoints[axis] = flightSetPoints[axis];
			}
			else
			{
				usedFlightSetPoints[axis] += ( (flightSetPoints[axis]-usedFlightSetPoints[axis]) * 0.5 );
			}
		}

	}
	else
	{

		//else we set the full setpoint
		usedFlightSetPoints[0] = flightSetPoints[0];
		usedFlightSetPoints[1] = flightSetPoints[1];
		usedFlightSetPoints[2] = flightSetPoints[2];

	}

	//bypass limiter
	usedFlightSetPoints[0] = flightSetPoints[0];
	usedFlightSetPoints[1] = flightSetPoints[1];
	usedFlightSetPoints[2] = flightSetPoints[2];


	for (axis = 2; axis >= 0; --axis)
	{

		if (ModeActive(M_TEST1))
		{
			//limit setpoint change to 0.05 degrees per iteration
			if (flightSetPoints[axis] > lastSetPoint[axis])
			{
				lastSetPoint[axis] +=mainConfig.filterConfig[0].gyro.p;
				InlineConstrainf(lastSetPoint[axis], lastSetPoint[axis], flightSetPoints[axis]);
			}
			else if (flightSetPoints[axis] < lastSetPoint[axis])
			{
				lastSetPoint[axis] -= mainConfig.filterConfig[0].gyro.p;
				InlineConstrainf(lastSetPoint[axis], flightSetPoints[axis], lastSetPoint[axis]);
			}
			usedFlightSetPoints[axis] = lastSetPoint[axis];
		}
		else if (ModeActive(M_TEST2))
		{
			//limit setpoint change to 0.05 degrees per iteration only when setpoint is being reduced and under 200 DPS (near center stick and reducing)
			if ( (flightSetPoints[axis] > lastSetPoint[axis]) && ( ABS(flightSetPoints[axis]) < mainConfig.filterConfig[1].gyro.p) && ( ABS(lastSetPoint[axis]) < mainConfig.filterConfig[1].gyro.p) )
			{
				lastSetPoint[axis] += mainConfig.filterConfig[0].gyro.p;
				InlineConstrainf(lastSetPoint[axis], lastSetPoint[axis], flightSetPoints[axis]);
			}
			else if ( (flightSetPoints[axis] < lastSetPoint[axis]) && ( ABS(flightSetPoints[axis]) < mainConfig.filterConfig[1].gyro.p) && ( ABS(lastSetPoint[axis]) < mainConfig.filterConfig[1].gyro.p) )
			{
				lastSetPoint[axis] -= mainConfig.filterConfig[0].gyro.p;
				InlineConstrainf(lastSetPoint[axis], flightSetPoints[axis], lastSetPoint[axis]);
			}
			else
			{
				lastSetPoint[axis] = flightSetPoints[axis];
			}
			usedFlightSetPoints[axis] = lastSetPoint[axis];
		}

		pidError = usedFlightSetPoints[axis] - filteredGyroData[axis];

	    if ( SpinStopper(axis, pidError) )
	    {

	    	for (uint32_t motorNum=0; motorNum < MAX_MOTOR_NUMBER; motorNum++)
	    	{
	    		motorOutput[motorNum] = 0.05;
	    	}

	    	flightPids[YAW].kp   = 0;
	    	flightPids[YAW].ki   = 0;
	    	flightPids[YAW].kd   = 0;
	    	flightPids[ROLL].kp  = 0;
			flightPids[ROLL].ki  = 0;
			flightPids[ROLL].kd  = 0;
			flightPids[PITCH].kp = 0;
			flightPids[PITCH].ki = 0;
			flightPids[PITCH].kd = 0;

	    	return (0);

	    }
	    else
	    {

			// calculate Kp
			//flightPids[axis].kp = InlineConstrainf((pidError * pidsUsed[axis].kp), -MAX_KP, MAX_KP);
			flightPids[axis].kp = (pidError * pidsUsed[axis].kp);

			// calculate Ki ////////////////////////// V
			if ( fullKiLatched )
			{
				if (ModeActive(M_TEST3)) //let Ki only act on erros above 100
				{
					flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki + pidsUsed[axis].ki * pidError, -MAX_KI, MAX_KI); //prevent insane windup
					if ( ( actuatorRange > .9999 ) || (pidError > 110) ) //actuator maxed out, don't allow Ki to increase to prevent windup from maxed actuators
					{
						flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki, -kiErrorLimit[axis], kiErrorLimit[axis]);
					}
					else
					{
						kiErrorLimit[axis] = ABS(flightPids[axis].ki);
					}
				}
				else
				{
					flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki + pidsUsed[axis].ki * pidError, -MAX_KI, MAX_KI); //prevent insane windup
					if ( actuatorRange > .9999 ) //actuator maxed out, don't allow Ki to increase to prevent windup from maxed actuators
					{
						flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki, -kiErrorLimit[axis], kiErrorLimit[axis]);
					}
					else
					{
						kiErrorLimit[axis] = ABS(flightPids[axis].ki);
					}
				}
			}
			else
			{
				flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki + pidsUsed[axis].ki * pidError, -0.0521f, 0.0521f); //limit Ki when fullKiLatched is false
			}
			// calculate Ki ////////////////////////// ^


			// calculate Kd ////////////////////////// V
			kdDelta[axis] = -(filteredGyroData[axis] - lastfilteredGyroData[axis]);
			lastfilteredGyroData[axis] = filteredGyroData[axis];

			InlineUpdateWitchcraft(pidsUsed);

			if (filterSetup[axis])
			{
				kdDelta[axis] = BiquadUpdate(kdDelta[axis], &kdBqFilterState[axis]);
			}
			else
			{
				filterSetup[axis] = 1;
				InitBiquad(mainConfig.filterConfig[axis].kd.r, &kdBqFilterState[axis], loopSpeed.dT, FILTER_TYPE_LOWPASS, &kdBqFilterState[axis], 1.66f);
			}

			flightPids[axis].kd = InlineConstrainf(kdDelta[axis] * pidsUsed[axis].kd, -MAX_KD, MAX_KD);
			// calculate Kd ////////////////////////// ^

			//yaw Kd 8 KHz
			//yaw reductor
			if ( (axis == YAW) && (mainConfig.filterConfig[0].filterMod == 1) )
			{
				yawCounter++;
				yawKp += flightPids[YAW].kp;
				yawKi += flightPids[YAW].ki;
				yawKd += flightPids[YAW].kd;

				if (yawCounter == 4)
				{
					yawKpStore = yawKp / yawCounter;
					yawKiStore = yawKi / yawCounter;
					yawKdStore = yawKd / yawCounter;
					yawKp = 0;
					yawKd = 0;
					yawKi = 0;
					yawCounter = 0;
				}

				flightPids[YAW].kp = yawKpStore;
				flightPids[YAW].ki = yawKiStore;
				flightPids[YAW].kd = yawKdStore;

			}

	    }

	}

	return (1);

}

inline uint32_t SpinStopper(int32_t axis, float pidError)
{

    static uint32_t countErrorUhoh[AXIS_NUMBER]  = {0, 0, 0};
    static uint32_t uhOhRecoverCounter = 0;

	if (!uhOhRecover)
	{
		uhOhRecoverCounter = 0;
		if (ABS(pidError) > 1400) {
			countErrorUhoh[axis]++;
		} else {
			countErrorUhoh[axis] = 0;
		}
		if (countErrorUhoh[axis] > loopSpeed.uhohNumber ) {
			uhOhRecover = 1;
		}
	}
	else
	{
		uhOhRecoverCounter++;
	}
	if (uhOhRecoverCounter > (loopSpeed.uhohNumber))
	{
		uhOhRecover = 0;
		uhOhRecoverCounter = 0;
	}
	if (uhOhRecoverCounter)
	{
		return (1);
	}
	return (0);
}

inline void InlineUpdateWitchcraft(pid_terms pidConfig[])
{

	int32_t axis;

	for (axis = 2; axis >= 0; --axis)
	{
		if (pidConfig[axis].wc > 1)
		{
			kdRingBuffer[axis][kdRingBufferPoint[axis]++] = kdDelta[axis];
			kdRingBufferSum[axis] += kdDelta[axis];

			if (kdRingBufferPoint[axis] == pidConfig[axis].wc)
				kdRingBufferPoint[axis] = 0;

			kdDelta[axis] = (float)(kdRingBufferSum[axis] / (float) (pidConfig[axis].wc));
			kdRingBufferSum[axis] -= kdRingBuffer[axis][kdRingBufferPoint[axis]];
		}
	}

}
