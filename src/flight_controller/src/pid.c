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
kd_filter kdFilter[AXIS_NUMBER];
pid_terms pidsUsed[AXIS_NUMBER];
int32_t axis;


uint32_t uhOhRecover = 0;


void InitPid (void)
{

	int axis; //set

	bzero(kiError,           sizeof(kiError));
	bzero(kiErrorLimit,      sizeof(kiErrorLimit));
	bzero(kdDelta,           sizeof(kdDelta));
	bzero(kdRingBuffer,      sizeof(kdRingBuffer));
	bzero(kdRingBufferSum,   sizeof(kdRingBufferSum));
	bzero(kdRingBufferPoint, sizeof(kdRingBufferPoint));
	bzero(kdBqFilterState,   sizeof(kdBqFilterState));

	uhOhRecover = 0; //unset recover mode

	pidsUsed[YAW].kp   = (DEFAULT_YAW_KP * mainConfig.tuneProfile[activeProfile].pidConfig[YAW].kp) / DEFAULT_PID_CONFIG_VALUE;
	pidsUsed[YAW].ki   = (DEFAULT_YAW_KI * mainConfig.tuneProfile[activeProfile].pidConfig[YAW].ki) / DEFAULT_PID_CONFIG_VALUE;
	pidsUsed[YAW].kd   = (DEFAULT_YAW_KD * mainConfig.tuneProfile[activeProfile].pidConfig[YAW].kd) / DEFAULT_PID_CONFIG_VALUE;

	pidsUsed[ROLL].kp  = (DEFAULT_ROLL_KP * mainConfig.tuneProfile[activeProfile].pidConfig[ROLL].kp) / DEFAULT_PID_CONFIG_VALUE;
	pidsUsed[ROLL].ki  = (DEFAULT_ROLL_KI * mainConfig.tuneProfile[activeProfile].pidConfig[ROLL].ki) / DEFAULT_PID_CONFIG_VALUE;
	pidsUsed[ROLL].kd  = (DEFAULT_ROLL_KD * mainConfig.tuneProfile[activeProfile].pidConfig[ROLL].kd) / DEFAULT_PID_CONFIG_VALUE;

	pidsUsed[PITCH].kp = (DEFAULT_PITCH_KP * mainConfig.tuneProfile[activeProfile].pidConfig[PITCH].kp) / DEFAULT_PID_CONFIG_VALUE;
	pidsUsed[PITCH].ki = (DEFAULT_PITCH_KI * mainConfig.tuneProfile[activeProfile].pidConfig[PITCH].ki) / DEFAULT_PID_CONFIG_VALUE;
	pidsUsed[PITCH].kd = (DEFAULT_PITCH_KD * mainConfig.tuneProfile[activeProfile].pidConfig[PITCH].kd) / DEFAULT_PID_CONFIG_VALUE;
	//cross multiply pids

	pidsUsed[YAW].wc   = mainConfig.tuneProfile[activeProfile].pidConfig[YAW].wc;
	pidsUsed[ROLL].wc  = mainConfig.tuneProfile[activeProfile].pidConfig[ROLL].wc;
	pidsUsed[PITCH].wc = mainConfig.tuneProfile[activeProfile].pidConfig[PITCH].wc;

	if (mainConfig.mixerConfig.mixerStyle == 1)
	{
		pidsUsed[YAW].kp = pidsUsed[YAW].kp  / 100000;
		pidsUsed[YAW].ki = (pidsUsed[YAW].ki / 50000) * loopSpeed.dT;
		pidsUsed[YAW].kd = (pidsUsed[YAW].kd / 200000000)  / loopSpeed.dT;
		if (pidsUsed[YAW].wc == 0)
			pidsUsed[YAW].wc = (mainConfig.tuneProfile[activeProfile].filterConfig[YAW].ga * 0.333);

		pidsUsed[ROLL].kp = pidsUsed[ROLL].kp  / 100000;
		pidsUsed[ROLL].ki = (pidsUsed[ROLL].ki / 50000) * loopSpeed.dT;
		pidsUsed[ROLL].kd = (pidsUsed[ROLL].kd / 200000000)  / loopSpeed.dT;
		if (pidsUsed[ROLL].wc == 0)
			pidsUsed[ROLL].wc = (mainConfig.tuneProfile[activeProfile].filterConfig[ROLL].ga * 0.333);

		pidsUsed[PITCH].kp = pidsUsed[PITCH].kp  / 100000;
		pidsUsed[PITCH].ki = (pidsUsed[PITCH].ki / 50000) * loopSpeed.dT;
		pidsUsed[PITCH].kd = (pidsUsed[PITCH].kd / 200000000)  / loopSpeed.dT;
		if (pidsUsed[PITCH].wc == 0)
			pidsUsed[PITCH].wc = (mainConfig.tuneProfile[activeProfile].filterConfig[PITCH].ga * 0.333);
	}
	else
	{
		pidsUsed[YAW].kp = pidsUsed[YAW].kp  / 50000;
		pidsUsed[YAW].ki = (pidsUsed[YAW].ki / 25000) * loopSpeed.dT;
		pidsUsed[YAW].kd = (pidsUsed[YAW].kd / 100000000)  / loopSpeed.dT;
		if (pidsUsed[YAW].wc == 0)
			pidsUsed[YAW].wc = (mainConfig.tuneProfile[activeProfile].filterConfig[YAW].ga * 0.333);

		pidsUsed[ROLL].kp = pidsUsed[ROLL].kp  / 50000;
		pidsUsed[ROLL].ki = (pidsUsed[ROLL].ki / 25000) * loopSpeed.dT;
		pidsUsed[ROLL].kd = (pidsUsed[ROLL].kd / 100000000)  / loopSpeed.dT;
		if (pidsUsed[ROLL].wc == 0)
			pidsUsed[ROLL].wc = (mainConfig.tuneProfile[activeProfile].filterConfig[ROLL].ga * 0.333);

		pidsUsed[PITCH].kp = pidsUsed[PITCH].kp  / 50000;
		pidsUsed[PITCH].ki = (pidsUsed[PITCH].ki / 25000) * loopSpeed.dT;
		pidsUsed[PITCH].kd = (pidsUsed[PITCH].kd / 100000000)  / loopSpeed.dT;
		if (pidsUsed[PITCH].wc == 0)
			pidsUsed[PITCH].wc = (mainConfig.tuneProfile[activeProfile].filterConfig[PITCH].ga * 0.333);
	}

	if (loopSpeed.khzDivider == 32)
	{
		pidsUsed[0].kd *= 1.0f;
		pidsUsed[1].kd *= 1.0f;
		pidsUsed[2].kd *= 1.0f;
	}
	else if (loopSpeed.khzDivider == 16)
	{
		pidsUsed[0].kd *= 2.0f;
		pidsUsed[1].kd *= 2.0f;
		pidsUsed[2].kd *= 2.0f;
	}
	else if (loopSpeed.khzDivider == 8)
	{
		pidsUsed[0].kd *= 4.0f;
		pidsUsed[1].kd *= 4.0f;
		pidsUsed[2].kd *= 4.0f;
	}
	else if (loopSpeed.khzDivider == 4)
	{
		pidsUsed[0].kd *= 8.0f;
		pidsUsed[1].kd *= 8.0f;
		pidsUsed[2].kd *= 8.0f;
	}

	for (axis = 2; axis >= 0; --axis)
	{
		InitKdFilter(&kdFilter[axis]);
		InitBiquad(mainConfig.tuneProfile[activeProfile].filterConfig[axis].kd.r, &kdBqFilterState[axis], loopSpeed.dT, FILTER_TYPE_LOWPASS, &kdBqFilterState[axis], 2.11f);
	}
}


inline uint32_t InlinePidController(float filteredGyroData[], float flightSetPoints[], pid_output flightPids[], float actuatorRange)
{

	int32_t axis;
	static int bounceStopper[3] = {0,};
	//uint32_t everyOther;
	float   pidError;
	static float lastfilteredGyroData[AXIS_NUMBER] = {0,};
	//static float setPointSmoother[AXIS_NUMBER] = {0,};
	
	(void)(actuatorRange);

	for (axis = 2; axis >= 0; --axis)
	{

		pidError = flightSetPoints[axis] - filteredGyroData[axis];

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

				if (axis == YAW)
					flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki + pidsUsed[axis].ki * pidError, -mainConfig.tuneProfile[activeProfile].pidConfig[0].kiLimit * 2.0f, mainConfig.tuneProfile[activeProfile].pidConfig[0].kiLimit * 1.0f); //prevent insane windup
				else
					flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki + pidsUsed[axis].ki * pidError, -mainConfig.tuneProfile[activeProfile].pidConfig[0].kiLimit, mainConfig.tuneProfile[activeProfile].pidConfig[0].kiLimit); //prevent insane windup

				float ag = 1.0f;

				if (ABS(flightSetPoints[axis]) > 50)
					bounceStopper[axis]++;
				else
					bounceStopper[axis]--;

				bounceStopper[axis] = CONSTRAIN(bounceStopper[axis],0,2000);
				if(bounceStopper[axis])
				{
					if (ABS(pidError) > 50)
						ag = InlineChangeRangef(ABS(pidError)-50, 2000, 0, 3, 1);
				}
				else
				{
					ag = 1.0f;
				}

				if ( actuatorRange > .9999 ) //actuator maxed out, don't allow Ki to increase to prevent windup from maxed actuators
				{
					flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki * ag, -kiErrorLimit[axis], kiErrorLimit[axis]);
				}
				else
				{
					kiErrorLimit[axis] = ABS(flightPids[axis].ki * ag);
				}


			}
			else
			{
				flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki + pidsUsed[axis].ki * pidError, -0.0521f, 0.0521f); //limit Ki when fullKiLatched is false
			}
			// calculate Ki ////////////////////////// ^


			// calculate Kd ////////////////////////// V
			//if (everyOther)
			//{
				if (ModeActive(M_BRAINDRAIN))
				{
					kdDelta[axis] = pidError - lastfilteredGyroData[axis];
					lastfilteredGyroData[axis] = pidError;
				}
				else
				{
					kdDelta[axis] = -(filteredGyroData[axis] - lastfilteredGyroData[axis]);
					lastfilteredGyroData[axis] = filteredGyroData[axis];
				}

				InlineUpdateWitchcraft(pidsUsed);

				//KdFilterUpdate(&kdFilter[axis], kdDelta[axis]);
				//kdDelta[axis] = kdFilter[axis].x;
				kdDelta[axis] = BiquadUpdate(kdDelta[axis], &kdBqFilterState[axis]);

				flightPids[axis].kd = InlineConstrainf(kdDelta[axis] * pidsUsed[axis].kd, -mainConfig.tuneProfile[activeProfile].pidConfig[0].kdLimit, mainConfig.tuneProfile[activeProfile].pidConfig[0].kdLimit);
			//}
			// calculate Kd ////////////////////////// ^

	    }

		//if (everyOther)
		//	everyOther = 0;
		//else
		//	everyOther = 1;
	}

	return (1);

}

inline uint32_t SpinStopper(int32_t axis, float pidError)
{

    static uint32_t countErrorUhoh[AXIS_NUMBER]  = {0, 0, 0};
    static uint32_t uhOhRecoverCounter = 0;
	int multiplier = 1;

	if (axis == YAW)
		multiplier = 2;

	if (!uhOhRecover)
	{
		uhOhRecoverCounter = 0;
		if (ABS(pidError) > mainConfig.mixerConfig.spinRecoveryStrength)
		{
			countErrorUhoh[axis]++;
		}
		else
		{
			countErrorUhoh[axis] = 0;
		}
		if (countErrorUhoh[axis] > (loopSpeed.uhohNumber * multiplier) )
		{
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
		return(1);
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
