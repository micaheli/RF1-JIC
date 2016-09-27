#include "includes.h"

//default quad
int motorNumber = 4;

//default quad
int servoNumber = 0;

//default is quad xl 1234
actuator_mixer motorMixer[MAX_MOTOR_NUMBER] =  {
	//yaw, roll, pitch, throttle, aux1, aux2, aux3, aux4
	{ 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 1
	{-1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f}, //motor 2
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



void InitMixer(void) {
	stabilizerAttenuation = 0;
	bzero(motorOutput, sizeof(motorOutput));
	bzero(servoOutput, sizeof(servoOutput));
}

inline float ApplyAttenuationKdCurve (float motorOutput) {
	return motorOutput;
}

inline float ApplyAttenuationKpCurve (float motorOutput) {
	return motorOutput;
}

//just like the standard mixer, but optimized for speed since it runs at a much higher speed than normal servos
inline float InlineApplyMotorMixer(pid_output pids[], float curvedRcCommandF[]) {

	float highestMotor  = -100.0f;
	float lowestMotor   =  100.0f;
	float actuatorRange =    0.0f;
	float throttle      = curvedRcCommandF[THROTTLE];
	unsigned char i     = 0;

	for (i = 0; i < motorNumber; i++) {
		stabilizerAttenuation = ApplyAttenuationKpCurve( motorOutput[i] );
		motorOutput[i] = (
				( (stabilizerAttenuation * pids[YAW].kp + pids[YAW].kd) + pids[YAW].ki )  * motorMixer[i].yaw +
				( (stabilizerAttenuation * pids[ROLL].kp + pids[ROLL].kd) + pids[ROLL].ki )  * motorMixer[i].roll +
				( (stabilizerAttenuation * pids[PITCH].kp + pids[PITCH].kd) + pids[PITCH].ki )  * motorMixer[i].pitch
		);
		if (motorOutput[i] > highestMotor) { highestMotor = motorOutput[i]; }
		if (motorOutput[i] < lowestMotor)  { lowestMotor  = motorOutput[i]; }
	}

	actuatorRange = highestMotor - lowestMotor;

	if (actuatorRange > 1) { //this should never happen since the PIDC actively monitors the actuator range

		//no need to apply throttle since throttle is automatically at 50% and cannot go lower or higher once an actuator is out of bounds

		if (lowestMotor < 0) {
			throttle += (-1 * lowestMotor); //add the negative motor to the throttle, throttle has to at least make all motors 0%
			for (i = 0; i < motorNumber; i++) { //throttle is not zero, so we can add throttle.
				motorOutput[i] += throttle;
			}
		} //this makes sure actuator is at least 0 on each motor

		//(1 / highestMotor); //this give us the multiplier we apply to each motor

		for (i = 0; i < motorNumber; i++) { //throttle is not zero, so we can add throttle.
			motorOutput[i] *= (1 / highestMotor); //this squashes the mixer to fit within range.
			//this is a horrible way to do it though, so we need to make sure the PIDC doesn't allow this to happen.
			//we do this by communicating the actuator condition back to the PIDC
		}

	} else {

		//throttle can't make actuator go out of bounds at this point since actuator range is between 0 and 1.
		//throttle needs to shift actuators up to the point they at least hit 0.
		//this can't make actuator go out of bounds since the range is less than 1;
		//for this reason we can do an if, else if here since both conditions can never be true at the same time since we check the total range first.
		if (lowestMotor < 0) {
			throttle += (-1 * lowestMotor); //add the negative motor to the throttle, throttle has to at least make all motors 0%
		} else if ( (throttle + actuatorRange) > 1 ) { //if throttle makes actuator go out of bounds then we limit throttle to keep actuator within bounds
			throttle -= (throttle + actuatorRange) - 1;
		}

		for (i = 0; i < motorNumber; i++) { //throttle is not zero, so we can add throttle.
			motorOutput[i] += throttle;
		}

	}

	return actuatorRange;

}

inline void InlineApplyMixer(pid_output pids[], float curvedRcCommandF[]) {
	(void)(pids);
	(void)(curvedRcCommandF);
}
