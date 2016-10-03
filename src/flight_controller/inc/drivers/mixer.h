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


#define MAX_MOTOR_NUMBER 8
#define MAX_SERVO_NUMBER 8


extern int motorNumber;
extern int servoNumber;
extern float motorOutput[];
extern float servoOutput[];
extern actuator_mixer servoMixer[];
extern actuator_mixer motorMixer[];


void InitMixer(void);
float InlineApplyMotorMixer(pid_output pids[], float curvedRcCommandF[], float motorOutput[]);
void InlineApplyMixer(pid_output pids[], float curvedRcCommandF[]);
