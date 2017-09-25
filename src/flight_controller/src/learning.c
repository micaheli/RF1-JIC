#include "includes.h"

//0.1 to -0.1 fits within uint8_t 
#define KI_LEARN_MULTIPLIER 0.00078740157186985015869140625f
#define KI_LEARN_MULTIPLIER_I 1270.0f
//for 20 value table:
#define X_LEARNING_AVERAGE 52.5f

volatile float xAverage;
volatile float yAverage[AXIS_NUMBER];
volatile learned_ki_model learnedKiModel[AXIS_NUMBER];

int motorTrimHasHappened;
int motorTrimCounter;
int kiTrimCounter;

int BuildLearnedKiModel(void)
{
    //apov
    //y=-58.8418891170431x+2.54989733059548
    //y=41.4291581108829x+0.721149897330598
    //y=9.38809034907598x+-20.9593429158111
    
    learnedKiModel[YAW].b = -58.8418891170431f;
    learnedKiModel[ROLL].b = 41.4291581108829f;
    learnedKiModel[PITCH].b = 9.38809034907598f;
    learnedKiModel[YAW].m = 2.54989733059548f;
    learnedKiModel[ROLL].m = 0.721149897330598f;
    learnedKiModel[PITCH].m = -20.9593429158111f;

    //for techsavy's quad:
    //yaw = y=74.8049281314168x+9.517659137577
    //roll = y=-2.50924024640657x+9.34188911704312
    //pitch = y=-80.4024640657084x+-6.7088295687885

    //learnedKiModel[YAW].b = 9.517659137577f;
    //learnedKiModel[ROLL].b = 9.34188911704312f;
    //learnedKiModel[PITCH].b = -6.7088295687885f;
    //learnedKiModel[YAW].m = 74.8049281314168f;
    //learnedKiModel[ROLL].m = -2.50924024640657f;
    //learnedKiModel[PITCH].m = -80.4024640657084f;

    return(0);
}

inline float ApplyLearningModelToKi(float throttle, uint32_t axis)
{
    return( ConvertInt8ToFloatForKi( (throttle * learnedKiModel[axis].m) + learnedKiModel[axis].b ) );
}

int LearningInit(void)
{
    for (int x=(AXIS_NUMBER-1); x>=0; x--)
    {
        learnedKiModel[x].b = 0;
        learnedKiModel[x].m = 0;
        yAverage[x] = 0.0f;
    }
    xAverage = X_LEARNING_AVERAGE;

    motorTrimHasHappened = 0;
	motorTrimCounter = 0;
    kiTrimCounter = 0;

    return( BuildLearnedKiModel() );
}

inline int8_t ConvertFloatToInt8ForKi(float kiNumber)
{
	//clamp the values to fit in an int8
	if(kiNumber > 0.1f)
		kiNumber = 0.1f;

	if(kiNumber < -0.1f)
		kiNumber = -0.1f;

	return((int8_t)(kiNumber * KI_LEARN_MULTIPLIER_I));
}

inline float ConvertInt8ToFloatForKi(int8_t kiNumber)
{
	return((float)((float)kiNumber * KI_LEARN_MULTIPLIER));
}

int TrimKi(pid_output flightPids[])
{

	if(!mainConfig.mixerConfig.foreAftMixerFixer)
		return(0);

	if(!boardArmed)
		return(0);

    if(
        //(motorTrimHasHappened) &&
        (smoothedRcCommandF[PITCH] == 0.0f) &&
        (smoothedRcCommandF[ROLL] == 0.0f) &&
        (smoothedRcCommandF[YAW] == 0.0f)
    )
    {
        //need at least 20ms to trim motors, so 19+ will work
        kiTrimCounter++;
    }
    else
    {
        kiTrimCounter = 0;
    }

	uint32_t position = lrintf(smoothCurvedThrottle0_1*19);

	if ( (kiTrimCounter >= 10) )
	{
		persistance.data.yawKiTrim8[position] = ConvertFloatToInt8ForKi(flightPids[YAW].ki + kiTrim[YAW]);
		persistance.data.rollKiTrim8[position] = ConvertFloatToInt8ForKi(flightPids[ROLL].ki + kiTrim[ROLL]);
		persistance.data.pitchKiTrim8[position] = ConvertFloatToInt8ForKi(flightPids[PITCH].ki + kiTrim[PITCH]);
		persistance.data.geeForce[position] = lrintf( (float)(persistance.data.geeForce[position] * 0.80f) + (float)((geeForceZ * 10.0f) * 0.20f));
		persistance.data.rememberence[position]++;
	}

	if(fullKiLatched)
	{
        kiTrim[YAW]   = ApplyLearningModelToKi(smoothCurvedThrottle0_1, YAW);
        kiTrim[ROLL]  = ApplyLearningModelToKi(smoothCurvedThrottle0_1, ROLL);
        kiTrim[PITCH] = ApplyLearningModelToKi(smoothCurvedThrottle0_1, PITCH);
	}
	else
	{
		kiTrim[YAW]   = 0;
		kiTrim[ROLL]  = 0;
		kiTrim[PITCH] = 0;
	}

	return(1);
}

int TrimMotors(void)
{
	if (!ModeSet(M_LEARN))
		return(0);

	if(!mainConfig.mixerConfig.foreAftMixerFixer)
		return(0);

	if(!boardArmed)
		return(0);

	if(smoothCurvedThrottle0_1 < 0.05)
	{
		//motorTrimHasHappened = 0;
	}

	//throttle is 100% and constrols in deadband range
	if(
		ModeActive(M_LEARN) &&
		//(motorTrimHasHappened == 0) &&
		(smoothCurvedThrottle0_1 > 0.99f) &&
		(smoothedRcCommandF[PITCH] == 0.0f) &&
		(smoothedRcCommandF[ROLL] == 0.0f) &&
		(smoothedRcCommandF[YAW] == 0.0f)
	)
	{
		//need at least 50ms to trim motors, so 49+ will work
		motorTrimCounter++;
	}
	else
	{
		motorTrimCounter = 0;
		if(motorTrimHasHappened == 1)
			motorTrimHasHappened = 2;
	}

	if(motorTrimCounter>49)
	{
		if(motorTrimCounter>=99)
			motorTrimCounter = 0;

		motorTrimHasHappened = 1;
		float tallestMotor = 0.0f;
		//gradually trim motors
		//only check active motors
		//activeMotorCounter in mixer.c
		//for now quad only
		for(int xxx = 0; xxx < 4; xxx++)
		{
			persistance.data.motorTrim[xxx] = CONSTRAIN( (persistance.data.motorTrim[xxx] * 0.95f) + (motorOutput[xxx] * 0.05f), 0.85f, 1.0f);
			if(tallestMotor < persistance.data.motorTrim[xxx])
				tallestMotor = persistance.data.motorTrim[xxx];
		}

		for(int xxx = 0; xxx < 4; xxx++)
		{
			persistance.data.motorTrim[xxx] += (1.0f - tallestMotor);
		}

	}

	return(0);
}
