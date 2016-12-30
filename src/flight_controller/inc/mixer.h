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
    uint32_t motorMixer;
    uint32_t motorOutput[8];
} mixer_config;

enum {MIXER_X1234=0,MIXER_X1234RY,MIXER_X1234I,MIXER_CUSTOM,MIXER_END};
enum {ESC_MULTISHOT=0,ESC_ONESHOT,ESC_PWM,ESC_ONESHOT42,ESC_DSHOT150,ESC_DSHOT300, ESC_DSHOT600, ESC_MEGAVOLT,ESC_PROTOCOL_END};

#define ATTENUATION_CURVE_SIZE 9


extern int motorNumber;
extern int servoNumber;
extern volatile float motorOutput[];
extern volatile float servoOutput[];
extern actuator_mixer servoMixer[];
extern actuator_mixer motorMixer[];


void InitMixer(void);
float InlineApplyMotorMixer(pid_output pids[], float curvedRcCommandF[], volatile float motorOutput[]);
float InlineApplyMotorMixer2(pid_output pids[], float curvedRcCommandF[], volatile float motorOutput[]);
void InlineApplyMixer(pid_output pids[], float curvedRcCommandF[]);
float ApplyAttenuationCurve (float input, float curve[], int curveSize);
