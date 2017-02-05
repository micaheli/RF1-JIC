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
static float stabilizerAttenuation;
volatile float motorOutput[MAX_MOTOR_NUMBER];
volatile float servoOutput[MAX_SERVO_NUMBER];
int32_t activeMotorCounter = -1; //number of active motors minus 1
float kiAttenuationCurve[ATTENUATION_CURVE_SIZE] = {1.35, 1.30, 1.25, 1.20, 1.15, 1.10, 1.05, 1.00, 0.95};
float kpAttenuationCurve[ATTENUATION_CURVE_SIZE] = {1.3, 1.15, 1.0, 1.0, 0.9, 0.9, 0.85, 0.80, 0.75};
float kdAttenuationCurve[ATTENUATION_CURVE_SIZE] = {1.3, 1.15, 1.0, 1.0, 0.9, 0.9, 0.85, 0.80, 0.75};

static float ApplyAttenuationCurve (float input, float curve[], int curveSize);

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

static float ApplyAttenuationCurve (float inputAttn, float curve[], int curveSize) {

	//return(1.0f);
    uint32_t indexAttn;
    float remainderAttn;

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
/*
inline float InlineApplyMotorMixer(pid_output pids[], float curvedRcCommandF[], volatile float motorOutputHere[])
{

	int x;

	float highestMotor  = -100.0f;
	float lowestMotor   =  100.0f;
	float actuatorRange =    0.0f;
	float throttle      = curvedRcCommandF[THROTTLE];
	int32_t i           = 0;

	uint32_t threeDeeMode = 0;

	if (!threeDeeMode)
	{
		throttle = (throttle+1) * 0.5; //0 to +1
	}

	for (i = activeMotorCounter; i >= 0; i--)
	{
		motorOutputHere[i] = (
			(
				(pids[YAW].kp * ApplyAttenuationCurve(motorOutputHere[i], kpAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[YAW].kd * ApplyAttenuationCurve(motorOutputHere[i], kdAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[YAW].ki * ApplyAttenuationCurve(motorOutputHere[i], kiAttenuationCurve, ATTENUATION_CURVE_SIZE ) )
			) * motorMixer[i].yaw +
			(
				(pids[ROLL].kp * ApplyAttenuationCurve(motorOutputHere[i], kpAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[ROLL].kd * ApplyAttenuationCurve(motorOutputHere[i], kdAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[ROLL].ki * ApplyAttenuationCurve(motorOutputHere[i], kiAttenuationCurve, ATTENUATION_CURVE_SIZE ) )
			) * motorMixer[i].roll +
			(
				(pids[PITCH].kp * ApplyAttenuationCurve(motorOutputHere[i], kpAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[PITCH].kd * ApplyAttenuationCurve(motorOutputHere[i], kdAttenuationCurve, ATTENUATION_CURVE_SIZE ) ) +
				(pids[PITCH].ki * ApplyAttenuationCurve(motorOutputHere[i], kiAttenuationCurve, ATTENUATION_CURVE_SIZE ) )
			) * motorMixer[i].pitch
		);
		if (motorOutputHere[i] > highestMotor) { highestMotor = motorOutputHere[i]; }
		if (motorOutputHere[i] < lowestMotor)  { lowestMotor  = motorOutputHere[i]; }
	}

	actuatorRange = highestMotor - lowestMotor;

	if (lowestMotor < 0)
	{

		//add the negative motor to the throttle, throttle has to at least make all motors 0%
		for (i = activeMotorCounter; i >= 0; i--)
		{ //throttle is not zero, so we can add throttle.
			motorOutputHere[i] += (-1 * lowestMotor);
		}

	} //this makes sure actuator is at least 0 on each motor

	if (actuatorRange > 1)
	{ //this should never happen since the PIDC actively monitors the actuator range

		//no need to apply throttle since throttle is automatically at 50% and cannot go lower or higher once an actuator is out of bounds

		//(1 / highestMotor); //this give us the multiplier we apply to each motor

		for (i = activeMotorCounter; i >= 0; i--)
		{ //throttle is not zero, so we can add throttle.
			motorOutputHere[i] *= (1 / actuatorRange); //this squashes the mixer to fit within range.
			//this is a horrible way to do it though, so we need to make sure the PIDC doesn't allow this to happen.
			//we do this by communicating the actuator condition back to the PIDC
		}
		throttle=0;

	}
	else
	{

		throttle=InlineConstrainf( throttle, 0, (1-(actuatorRange * 0.5))); //constrain throttle to maximum amount of throttle we can apply.
		//throttle can't make actuator go out of bounds at this point since actuator range is between 0 and 1.
		//throttle needs to shift actuators up to the point they at least hit 0.
		//this can't make actuator go out of bounds since the range is less than 1;
		//for this reason we can do an if, else if here since both conditions can never be true at the same time since we check the total range first.

	}

	for(x=7; x>=0; x--)
	{
		motorOutputHere[x] = InlineConstrainf(motorOutputHere[x]+throttle,0.0f,1.0f);
	}

	return actuatorRange;

}
*/

//just like the standard mixer, but optimized for speed since it runs at a much higher speed than normal servos
float InlineApplyMotorMixer3dUpright(pid_output pids[], float throttleIn)
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

float InlineApplyMotorMixer3dInverted(pid_output pids[], float throttleIn)
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

float InlineApplyMotorMixer3dNeutral(pid_output pids[], float throttleIn)
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

 float InlineApplyMotorMixer(pid_output pids[], float throttleIn)
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
		motorOutput[i] = InlineConstrainf(motorOutput[i]+throttle,0.0f,1.0f);
	}

	return(actuatorRange);

}

inline void InlineApplyMixer(pid_output pids[], float curvedRcCommandF[])
{
	(void)(pids);
	(void)(curvedRcCommandF);
}
