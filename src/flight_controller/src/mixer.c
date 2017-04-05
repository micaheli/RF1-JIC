#include "includes.h"

//default is quad xl 1234
//stinky yaw

const actuator_mixer CONST_MIXER_X1234[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{-1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1
	{ 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2
	{-1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 3
	{ 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 4
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 5
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 6
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 7
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 8
};

const actuator_mixer CONST_MIXER_X1234RY[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{ 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1
	{-1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2
	{ 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 3
	{-1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 4
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 5
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 6
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 7
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 8
};

const actuator_mixer CONST_MIXER_X4213[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{-1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 3
	{ 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2
	{ 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 4
	{-1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 5
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 6
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 7
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 8
};

const actuator_mixer CONST_MIXER_X4213RY[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{ 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 3
	{-1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2
	{-1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 4
	{ 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 5
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 6
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 7
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 8
};

const actuator_mixer CONST_MIXER_PLUS1234[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{ 1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1 counter clockwise
	{-1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2 clockwise
	{ 1.0f, 0.0f,  -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 3 counter clockwise
	{-1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 4 clockwise
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 5
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 6
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 7
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 8
};

const actuator_mixer CONST_MIXER_PLUS1234RY[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{-1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1
	{ 1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2
	{-1.0f, 0.0f,  -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 3
	{ 1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 4
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 5
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 6
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 7
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 8
};

actuator_mixer motorMixer[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{-1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1
	{ 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2
	{-1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 3
	{ 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 4
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 5
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 6
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 7
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 8
};

actuator_mixer servoMixer[MAX_SERVO_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //servo 1
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //servo 2
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //servo 3
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //servo 4
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //servo 5
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //servo 6
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //servo 7
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //servo 8
};

uint32_t threeDeeMode;
static   float stabilizerAttenuation;
volatile float motorOutput[MAX_MOTOR_NUMBER];
volatile float servoOutput[MAX_SERVO_NUMBER];

int32_t activeMotorCounter = -1; //number of active motors minus 1

static float kiAttenuationCurve[ATTENUATION_CURVE_SIZE] = {1.05, 1.05, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 0.95};
static float kpAttenuationCurve[ATTENUATION_CURVE_SIZE] = {1.30, 1.00, 0.85, 0.75, 0.65, 0.60, 0.55, 0.50, 0.55};
static float kdAttenuationCurve[ATTENUATION_CURVE_SIZE] = {1.35, 1.15, 1.10, 1.05, 0.95, 0.80, 0.75, 0.70, 0.65};

static void PrintTpaKp(void);
static void PrintTpaKi(void);
static void PrintTpaKd(void);

static float ApplyAttenuationCurve (float input, float curve[], uint32_t curveSize);

void ResetTpaCurves(void)
{
	memcpy(mainConfig.mixerConfig.tpaKpCurve, kpAttenuationCurve, sizeof(kpAttenuationCurve));
	memcpy(mainConfig.mixerConfig.tpaKiCurve, kiAttenuationCurve, sizeof(kiAttenuationCurve));
	memcpy(mainConfig.mixerConfig.tpaKdCurve, kdAttenuationCurve, sizeof(kdAttenuationCurve));
}

static void PrintTpaKp(void)
{
	uint32_t i;
	RfCustomReplyBuffer("tpakp ");
	for (i=0;i<ATTENUATION_CURVE_SIZE;i++)
	{
		if (i == ATTENUATION_CURVE_SIZE - 1)
			sprintf(rf_custom_out_buffer, "%lu", (uint32_t)(mainConfig.mixerConfig.tpaKpCurve[i]*100) );
		else
			sprintf(rf_custom_out_buffer, "%lu=", (uint32_t)(mainConfig.mixerConfig.tpaKpCurve[i]*100) );
		RfCustomReplyBuffer(rf_custom_out_buffer);
	}
	RfCustomReplyBuffer("\n");
}

static void PrintTpaKi(void)
{
	uint32_t i;
	RfCustomReplyBuffer("tpaki ");
	for (i=0;i<ATTENUATION_CURVE_SIZE;i++)
	{
		if (i == ATTENUATION_CURVE_SIZE - 1)
			sprintf(rf_custom_out_buffer, "%lu", (uint32_t)(mainConfig.mixerConfig.tpaKiCurve[i]*100) );
		else
			sprintf(rf_custom_out_buffer, "%lu=", (uint32_t)(mainConfig.mixerConfig.tpaKiCurve[i]*100) );
		RfCustomReplyBuffer(rf_custom_out_buffer);
	}
	RfCustomReplyBuffer("\n");
}

static void PrintTpaKd(void)
{
	uint32_t i;
	RfCustomReplyBuffer("tpakd ");
	for (i=0;i<ATTENUATION_CURVE_SIZE;i++)
	{
		if (i == ATTENUATION_CURVE_SIZE - 1)
			sprintf(rf_custom_out_buffer, "%lu", (uint32_t)(mainConfig.mixerConfig.tpaKdCurve[i]*100) );
		else
			sprintf(rf_custom_out_buffer, "%lu=", (uint32_t)(mainConfig.mixerConfig.tpaKdCurve[i]*100) );
		RfCustomReplyBuffer(rf_custom_out_buffer);
	}
	RfCustomReplyBuffer("\n");
}

void AdjustKpTpa(char *modString)
{
	uint32_t i;

	(void)(modString);
	PrintTpaKp();
}

void AdjustKiTpa(char *modString)
{
	uint32_t i;

	(void)(modString);
	PrintTpaKi();
}

void AdjustKdTpa(char *modString)
{
	uint32_t i;

	(void)(modString);
	PrintTpaKd();
}

void PrintTpaCurves(void)
{
	uint32_t i;

	PrintTpaKp();
	PrintTpaKi();
	PrintTpaKd();
}

void InitMixer(void) {
	int32_t i;

	threeDeeMode = 0;
	stabilizerAttenuation = 0;

	for (i=0;i<MAX_MOTOR_NUMBER;i++)
		motorOutput[i]=0.0f;

	for (i=0;i<MAX_SERVO_NUMBER;i++)
		servoOutput[i]=0.0f;

	switch (mainConfig.mixerConfig.mixerType) {
		case MIXER_X4213_3D:
			threeDeeMode = 1;
		case MIXER_X4213:
			memcpy(motorMixer, CONST_MIXER_X4213, sizeof(motorMixer));
			break;
		case MIXER_X4213RY_3D:
			threeDeeMode = 1;
		case MIXER_X4213RY:
			memcpy(motorMixer, CONST_MIXER_X4213RY, sizeof(motorMixer));
			break;
		case MIXER_PLUS1234:
			memcpy(motorMixer, CONST_MIXER_PLUS1234, sizeof(motorMixer));
			break;
		case MIXER_PLUS1234RY:
			memcpy(motorMixer, CONST_MIXER_PLUS1234RY, sizeof(motorMixer));
			break;
		case MIXER_CUSTOM:
			//fill mixer customer here, for now it defaults to MIXER_X1234
		case MIXER_X1234RY_3D:
			threeDeeMode = 1;
		case MIXER_X1234RY:
			memcpy(motorMixer, CONST_MIXER_X1234RY, sizeof(motorMixer));
			break;
		case MIXER_X1234_3D:
			threeDeeMode = 1;
		case MIXER_X1234:
		default:
			memcpy(motorMixer, CONST_MIXER_X1234, sizeof(motorMixer));
			break;
	}

	//number of active moters starting from 0.
	//quad is 3, hex is 5, octo is 7
	//todo: need to set this based on mixer
	for (i = activeMotorCounter; i >= 0; i--) {
		//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
		if ( (motorMixer[i].yaw != 0 ) || (motorMixer[i].roll != 0 ) || (motorMixer[i].pitch != 0 ) || (motorMixer[i].throttle != 0 ) )
		{
			activeMotorCounter++; //increment active motor counter
		}
	}

	activeMotorCounter=3;
}

static float ApplyAttenuationCurve (float inputAttn, float curve[], uint32_t curveSize)
{

    uint32_t indexAttn;
    float remainderAttn;

#ifdef STM32F446xx
    return(1.0f);
#endif

    remainderAttn = (float)((float)inputAttn * (float)curveSize);
    indexAttn =(int)remainderAttn;

    if (indexAttn == 0)
        return (curve[0]);
    else
    {
        remainderAttn = remainderAttn - (float)indexAttn;
        return (curve[indexAttn-1] + (curve[indexAttn] * remainderAttn));
    }

}

//just like the standard mixer, but optimized for speed since it runs at a much higher speed than normal servos
inline float InlineApplyMotorMixer3dUpright(pid_output pids[], float throttleIn)
{
	float idleNum1;
	float idleNum2;
	float highestMotor  = -100.0f;
	float lowestMotor   =  100.0f;
	float actuatorRange =  0.0f;
	volatile float rangedThrottle;
	volatile float throttle;
	float throttleOffset;
	int32_t i           = 0;

	//static int32_t threeDeeThrottleLatch = 1;

	for (i = activeMotorCounter; i >= 0; i--)
	{

		//-1 to 1
		motorOutput[i] = (
			(
				(pids[YAW].kp * 1 ) +
				(pids[YAW].kd * 1 ) +
				(pids[YAW].ki * 1 )
			) * motorMixer[i].yaw * -1.0f +
			(
				(pids[ROLL].kp * 1 ) +
				(pids[ROLL].kd * 1 ) +
				(pids[ROLL].ki * 1 )
			) * motorMixer[i].roll +
			(
				(pids[PITCH].kp * 1 ) +
				(pids[PITCH].kd * 1 ) +
				(pids[PITCH].ki * 1 )
			) * motorMixer[i].pitch
		);
		motorOutput[i] = InlineChangeRangef(motorOutput[i], 1.0, -1.0, 1.0, 0.0);
		if (motorOutput[i] > highestMotor) { highestMotor = motorOutput[i]; }
		if (motorOutput[i] < lowestMotor)  { lowestMotor  = motorOutput[i]; }
	}

	actuatorRange = highestMotor - lowestMotor;

	if (actuatorRange > 1.0f)
	{
		for (i = activeMotorCounter; i >= 0; i--)
		{
			motorOutput[i] /= actuatorRange;
		}
		throttle = 0.0f;
	}
	else
	{
		//put throttle range to same range as actuators. 0 to 1 from -1 to 1
		rangedThrottle = InlineChangeRangef(throttleIn, 1.00f, 0.1f, 1.0f, 0.0f);
		throttleOffset = actuatorRange / 2.0f;
		throttle = InlineConstrainf(rangedThrottle, throttleOffset, 1.0f - throttleOffset) - 0.5;

	}

	idleNum1 = (0.5f - (mainConfig.mixerConfig.idlePercent * 0.01f));
	idleNum2 = (1 - (0.5f - (mainConfig.mixerConfig.idlePercent * 0.01f)));
	for(i=7; i>=0; i--)
	{
		//motorOutput[i] = InlineChangeRangef(motorOutput[i]+throttle, 1.0f, 0.0f, 1.0f, 0.55f);
		//motorOutput[i] = InlineConstrainf(motorOutput[i], 1.0f, 0.55f);
		motorOutput[i] = InlineConstrainf(motorOutput[i]+throttle,0.0f,1.0f) * idleNum1 + idleNum2;
	}

	return(actuatorRange);

}

inline float InlineApplyMotorMixer3dInverted(pid_output pids[], float throttleIn)
{
	float idleNum;
	float highestMotor  = -100.0f;
	float lowestMotor   =  100.0f;
	float actuatorRange =  0.0f;
	volatile float rangedThrottle;
	volatile float throttle;
	float throttleOffset;
	int32_t i           = 0;

	//static int32_t threeDeeThrottleLatch = 1;

	for (i = activeMotorCounter; i >= 0; i--)
	{

		//-1 to 1
		motorOutput[i] = (
			(
				(pids[YAW].kp * 1 ) +
				(pids[YAW].kd * 1 ) +
				(pids[YAW].ki * 1 )
			) * motorMixer[i].yaw * -1.0f +
			(
				(pids[ROLL].kp * 1 ) +
				(pids[ROLL].kd * 1 ) +
				(pids[ROLL].ki * 1 )
			) * motorMixer[i].roll +
			(
				(pids[PITCH].kp * 1 ) +
				(pids[PITCH].kd * 1 ) +
				(pids[PITCH].ki * 1 )
			) * motorMixer[i].pitch
		);
		motorOutput[i] = InlineChangeRangef(motorOutput[i], 1.0, -1.0, 1.0, 0.0);
		if (motorOutput[i] > highestMotor) { highestMotor = motorOutput[i]; }
		if (motorOutput[i] < lowestMotor)  { lowestMotor  = motorOutput[i]; }
	}

	actuatorRange = highestMotor - lowestMotor;

	if (actuatorRange > 1.0f)
	{
		for (i = activeMotorCounter; i >= 0; i--)
		{
			motorOutput[i] /= actuatorRange;
		}
		throttle = 0.0f;
	}
	else
	{
		//put throttle range to same range as actuators. 0 to 1 from -1 to 1
		rangedThrottle = InlineChangeRangef(throttleIn, -0.1, -1.0, 1.0, 0.0);
		throttleOffset = actuatorRange / 2.0f;
		throttle = InlineConstrainf(rangedThrottle, throttleOffset, 1.0f - throttleOffset) - 0.5;

	}

	idleNum = (0.5f - (mainConfig.mixerConfig.idlePercentInverted * 0.01f));

	for(i=7; i>=0; i--)
	{
		motorOutput[i] = InlineConstrainf(motorOutput[i]+throttle,0.0f,1.0f) * idleNum;
	}

	return(actuatorRange);
}

inline float InlineApplyMotorMixer3dNeutral(pid_output pids[], float throttleIn)
{
	(void)(pids);
	(void)(throttleIn);
	int32_t i;
	for(i=7; i>=0; i--)
	{
		motorOutput[i] = 0.50f;
	}
	return(0.0f);
}

inline float ForeAftMixerFixer(float motorOutputFloat, float throttleFloat, uint32_t motorNumber)
{

	if (mainConfig.mixerConfig.foreAftMixerFixer == 1.0f)
	{
		//famx not active
		return(InlineConstrainf(motorOutputFloat+throttleFloat,0.0f,1.0f));
	}

	if (activeMotorCounter == 3)
	{
		//quad mixer. Motor 0 is always front left. Motor 1 is always front right. Motor 2 is always rear right. Motor 3 is always rear left.
		switch (motorNumber)
		{
			case 0:
			case 1:
				//front motors
				if (mainConfig.mixerConfig.foreAftMixerFixer < 1.0f)
				{
					//reduce front motors by mixer fixer
					throttleFloat = InlineChangeRangef(throttleFloat, motorMixer[motorNumber].throttle, 0.0f, (motorMixer[motorNumber].throttle * mainConfig.mixerConfig.foreAftMixerFixer), 0.0f);
				}
				break;
			case 2:
			case 3:
				//rear motors
				if (mainConfig.mixerConfig.foreAftMixerFixer > 1.0f)
				{
					//reduce rear motors by difference between mixer fixer and normal value (1.05 will reduce rear numbers to 95% at full throttle
					throttleFloat = InlineChangeRangef(throttleFloat, motorMixer[motorNumber].throttle, 0.0f, (motorMixer[motorNumber].throttle * ( 1.0f - (mainConfig.mixerConfig.foreAftMixerFixer - 1.0f ) ) ), 0.0f);
				}
				break;
		}
	}

	return(InlineConstrainf(motorOutputFloat+throttleFloat,0.0f,1.0f));
}

inline float InlineApplyMotorMixer(pid_output pids[], float throttleIn)
{

	float highestMotor  = -100.0f;
	float lowestMotor   =  100.0f;
	float actuatorRange =  0.0f;
	volatile float rangedThrottle;
	volatile float throttle;
	float throttleOffset;
	int32_t i           = 0;

	//static int32_t threeDeeThrottleLatch = 1;

	for (i = activeMotorCounter; i >= 0; i--)
	{

		//-1 to 1
		motorOutput[i] = (
			(
				(pids[YAW].kp * ApplyAttenuationCurve(motorOutput[i], kpAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[YAW].kd * ApplyAttenuationCurve(motorOutput[i], kdAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[YAW].ki * ApplyAttenuationCurve(motorOutput[i], kiAttenuationCurve, ATTENUATION_CURVE_SIZE ) )
			) * motorMixer[i].yaw +
			(
				(pids[ROLL].kp * ApplyAttenuationCurve(motorOutput[i], kpAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[ROLL].kd * ApplyAttenuationCurve(motorOutput[i], kdAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[ROLL].ki * ApplyAttenuationCurve(motorOutput[i], kiAttenuationCurve, ATTENUATION_CURVE_SIZE ) )
			) * motorMixer[i].roll +
			(
				(pids[PITCH].kp * ApplyAttenuationCurve(motorOutput[i], kpAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[PITCH].kd * ApplyAttenuationCurve(motorOutput[i], kdAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[PITCH].ki * ApplyAttenuationCurve(motorOutput[i], kiAttenuationCurve, ATTENUATION_CURVE_SIZE ) )
			) * motorMixer[i].pitch
		);
		motorOutput[i] = InlineChangeRangef(motorOutput[i], 1.0, -1.0, 1.0, 0.0);
		if (motorOutput[i] > highestMotor) { highestMotor = motorOutput[i]; }
		if (motorOutput[i] < lowestMotor)  { lowestMotor  = motorOutput[i]; }
	}

	actuatorRange = highestMotor - lowestMotor;

	if (actuatorRange > 1.0f)
	{
		for (i = activeMotorCounter; i >= 0; i--)
		{
			motorOutput[i] /= actuatorRange;
		}
		throttle = 0.0f;
	}
	else
	{
		//put throttle range to same range as actuators. 0 to 1 from -1 to 1
		rangedThrottle = InlineChangeRangef(throttleIn, 1.0, -1.0, 1.0, 0.0);
		throttleOffset = actuatorRange / 2.0f;
		throttle = InlineConstrainf(rangedThrottle, throttleOffset, 1.0f - throttleOffset) - 0.5;

	}

	for(i=7; i>=0; i--)
	{
		motorOutput[i] = ForeAftMixerFixer( motorOutput[i], throttle, i);
	}

	return(actuatorRange);

}

inline float InlineApplyMotorMixer1(pid_output pids[], float throttleIn)
 {
 	float highestMotor  = -100.0f;
 	float lowestMotor   =  100.0f;
 	float actuatorRange =    0.0f;
 	volatile float rangedThrottle;
 	float throttle;
 	float throttleOffset;
 	int32_t i           = 0;
 	for (i = activeMotorCounter; i >= 0; i--)
 	{
 		//-1 to 1
 		motorOutput[i] = (
 			(
 				(pids[YAW].kp * ApplyAttenuationCurve(motorOutput[i], kpAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
 				(pids[YAW].kd * ApplyAttenuationCurve(motorOutput[i], kdAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
 				(pids[YAW].ki * ApplyAttenuationCurve(motorOutput[i], kiAttenuationCurve, ATTENUATION_CURVE_SIZE ) )
 			) * motorMixer[i].yaw +
 			(
 				(pids[ROLL].kp * ApplyAttenuationCurve(motorOutput[i], kpAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
 				(pids[ROLL].kd * ApplyAttenuationCurve(motorOutput[i], kdAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
 				(pids[ROLL].ki * ApplyAttenuationCurve(motorOutput[i], kiAttenuationCurve, ATTENUATION_CURVE_SIZE ) )
 			) * motorMixer[i].roll +
 			(
 				(pids[PITCH].kp * ApplyAttenuationCurve(motorOutput[i], kpAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
 				(pids[PITCH].kd * ApplyAttenuationCurve(motorOutput[i], kdAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
 				(pids[PITCH].ki * ApplyAttenuationCurve(motorOutput[i], kiAttenuationCurve, ATTENUATION_CURVE_SIZE ) )
 			) * motorMixer[i].pitch
 		);
 		if (motorOutput[i] > highestMotor) { highestMotor = motorOutput[i]; }
 		if (motorOutput[i] < lowestMotor)  { lowestMotor  = motorOutput[i]; }
 	}
 	actuatorRange = highestMotor - lowestMotor;
 	if (actuatorRange > 1.0f)
 	{
 		for (i = activeMotorCounter; i >= 0; i--)
 		{
 			motorOutput[i] /= actuatorRange;
 		}
 		throttle = 0.0f;
 	}
 	else
 	{
 		//put throttle range to same range as actuators. 0 to 1 from -1 to 1
 		rangedThrottle = InlineChangeRangef(throttleIn, 1.0, -1.0, 1.0, 0.0);
 		throttleOffset = actuatorRange / 2.0f;
 		throttle = InlineConstrainf(rangedThrottle, throttleOffset, 1.0f - throttleOffset);

 	}
 	for(i=7; i>=0; i--)
 	{
 		motorOutput[i] = InlineConstrainf(motorOutput[i]+throttle,0.0f,1.0f);
 	}
 	return(actuatorRange);
 }

inline void InlineApplyMixer(pid_output pids[], float curvedRcCommandF[])
{
	(void)(pids);
	(void)(curvedRcCommandF);
}
