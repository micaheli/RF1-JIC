#pragma once


enum { MOTOR1 = 0,MOTOR2,MOTOR3,MOTOR4,MOTOR5,MOTOR6,MOTOR7,MOTOR8 };
enum { SERVO1 = 0,SERVO2,SERVO3,SERVO4,SERVO5,SERVO6,SERVO7,SERVO8 };


typedef struct {
    float yaw;
    float roll;
    float pitch;
    float throttle;
    float aux1;
    float aux2;
    float aux3;
    float aux4;
} actuator_mixer;

typedef struct {
    uint32_t mixerType;
    uint32_t escProtcol;
    uint32_t escUpdateFrequency;
    float    idlePercent;
    float    idlePercentInverted;
    uint32_t motorOutput[8];
} mixer_config;

enum {
	MIXER_X1234     =0,
	MIXER_X1234RY   =1,
	MIXER_X4213     =2,
	MIXER_X4213RY   =3,
	MIXER_X1234_3D  =4,
	MIXER_X1234RY_3D=5,
	MIXER_X4213_3D  =6,
	MIXER_X4213RY_3D=7,
	MIXER_CUSTOM,
	MIXER_END};

enum {
	ESC_MULTISHOT=0,
	ESC_ONESHOT=1,
	ESC_PWM=2,
	ESC_ONESHOT42=3,
	ESC_DSHOT150=4,
	ESC_DSHOT300=5,
	ESC_DSHOT600=6,
	ESC_SPOOKY_ACTION_AT_A_DISTANCE=7,
	ESC_MULTISHOT25=8,
	ESC_MULTISHOT125=9,
	ESC_PROTOCOL_END=10,
};

#define ATTENUATION_CURVE_SIZE 9


extern uint32_t threeDeeMode;
extern int motorNumber;
extern int servoNumber;
extern volatile float motorOutput[];
extern volatile float servoOutput[];
extern actuator_mixer servoMixer[];
extern actuator_mixer motorMixer[];


extern void  InitMixer(void);
extern void  InlineApplyMixer(pid_output pids[], float curvedRcCommandF[]);

extern float InlineApplyMotorMixer(pid_output pids[], float throttleIn);
extern float InlineApplyMotorMixer3dUpright(pid_output pids[], float throttleIn);
extern float InlineApplyMotorMixer3dInverted(pid_output pids[], float throttleIn);
extern float InlineApplyMotorMixer3dNeutral(pid_output pids[], float throttleIn);
