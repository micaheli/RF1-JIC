#include "includes.h"

//default is quad xl 1234
//stinky yaw

const actuator_mixer CONST_MIXER_X1234[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{ 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1
	{-1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2
	{ 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 3
	{-1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 4
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 5
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 6
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 7
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 8
};

const actuator_mixer CONST_MIXER_X1234_REVERSE_YAW[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{-1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1
	{ 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2
	{-1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 3
	{ 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 4
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 5
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 6
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 7
	{ 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 8
};

actuator_mixer motorMixer[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{ 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1
	{-1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2
	{ 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 3
	{-1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 4
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

static float stabilizerAttenuation;
float motorOutput[MAX_MOTOR_NUMBER];
float servoOutput[MAX_SERVO_NUMBER];
int32_t activeMotorCounter = -1; //number of active motors minus 1
float kiAttenuationCurve[ATTENUATION_CURVE_SIZE] = {1.15, 1.05, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
float kpAttenuationCurve[ATTENUATION_CURVE_SIZE] = {1.15, 1.0, 1.0, 1.0, .95, .85, 1.0, .80, .85};
float kdAttenuationCurve[ATTENUATION_CURVE_SIZE] = {.85, .98, 1.0, 1.0, .95, .85, 1.0, .80, .85};

void InitMixer(void) {
	int32_t i;

	stabilizerAttenuation = 0;
	bzero(motorOutput, sizeof(motorOutput));
	bzero(servoOutput, sizeof(servoOutput));

	switch (mainConfig.mixerConfig.mixerType) {
		case MIXER_X1234_REVERSE_YAW:
			memcpy(motorMixer, CONST_MIXER_X1234_REVERSE_YAW, sizeof(motorMixer));
			break;
		case MIXER_CUSTOM:
			//fill mixer customer here, for now it defaults to MIXER_X1234
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

inline float ApplyAttenuationCurve (float inputAttn, float curve[], int curveSize) {

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
inline float InlineApplyMotorMixer(pid_output pids[], float curvedRcCommandF[], float motorOutput[]) {

	int x;

	float highestMotor  = -100.0f;
	float lowestMotor   =  100.0f;
	float actuatorRange =    0.0f;
	float throttle      = curvedRcCommandF[THROTTLE];
	int32_t i           = 0;

	uint32_t threeDeeMode = 0;

	if (!threeDeeMode) {
		throttle = (throttle+1) * 0.5; //0 to +1
	}

	for (i = activeMotorCounter; i >= 0; i--) {
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

	if (lowestMotor < 0) {

		//add the negative motor to the throttle, throttle has to at least make all motors 0%
		for (i = activeMotorCounter; i >= 0; i--) { //throttle is not zero, so we can add throttle.
			motorOutput[i] += (-1 * lowestMotor);
		}

	} //this makes sure actuator is at least 0 on each motor

	if (actuatorRange > 1) { //this should never happen since the PIDC actively monitors the actuator range

		//no need to apply throttle since throttle is automatically at 50% and cannot go lower or higher once an actuator is out of bounds

		//(1 / highestMotor); //this give us the multiplier we apply to each motor

		for (i = activeMotorCounter; i >= 0; i--) { //throttle is not zero, so we can add throttle.
			motorOutput[i] *= (1 / actuatorRange); //this squashes the mixer to fit within range.
			//this is a horrible way to do it though, so we need to make sure the PIDC doesn't allow this to happen.
			//we do this by communicating the actuator condition back to the PIDC
		}
		throttle=0;

	} else {

		throttle=InlineConstrainf( throttle, 0, (1-(actuatorRange * 0.5))); //constrain throttle to maximum amount of throttle we can apply.
		//throttle can't make actuator go out of bounds at this point since actuator range is between 0 and 1.
		//throttle needs to shift actuators up to the point they at least hit 0.
		//this can't make actuator go out of bounds since the range is less than 1;
		//for this reason we can do an if, else if here since both conditions can never be true at the same time since we check the total range first.

	}

	for(x=7; x>=0; x--)
	{
		motorOutput[x] = InlineConstrainf(motorOutput[x]+throttle,0.0f,1.0f);
	}

	return actuatorRange;

}

inline void InlineApplyMixer(pid_output pids[], float curvedRcCommandF[]) {
	(void)(pids);
	(void)(curvedRcCommandF);
}
