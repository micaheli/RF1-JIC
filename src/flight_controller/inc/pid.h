#pragma once

#define AXIS_NUMBER 3
#define VECTOR_NUMBER 3
#define KD_RING_BUFFER_SIZE 256

#define MAX_KP_FM1 0.90f
#define MAX_KI_FM1 0.30f
#define MAX_KD_FM1 0.55f

#define MAX_KP 0.75f
#define MAX_KI 0.25f
#define MAX_KD 0.30f


#define DEFAULT_PID_CONFIG_VALUE 30.0f
#define DEFAULT_YAW_KP           100.0f
#define DEFAULT_ROLL_KP          105.0f
#define DEFAULT_PITCH_KP         110.0f
#define DEFAULT_YAW_KI           1500.0f
#define DEFAULT_ROLL_KI          1100.0f
#define DEFAULT_PITCH_KI         1200.0f
#define DEFAULT_YAW_KD           900.0f
#define DEFAULT_ROLL_KD          700.0f
#define DEFAULT_PITCH_KD         900.0f

enum { KP = 0, KD, KI, PID_VARIABLE_COUNT }; // why is there a blank enum here kalyn?

typedef struct {
    float kp;
    float ki;
    float kd;
} pid_output;

typedef struct {
    float kp;
    float ki;
    float kd;
    uint32_t wc;
    uint32_t ga;
    float slp;
    float sli;
    float sla;
    float sld;
    float kdLimit;
    float kiLimit;
} pid_terms;

extern float pidSetpoint[AXIS_NUMBER];    //3 axis for pidc. range is in DPS.
extern pid_output pids[AXIS_NUMBER];
extern float currentKdFilterConfig[AXIS_NUMBER];

void InitPid (void);
//void InlineInitPidFilters(void);
uint32_t InlinePidController (float filteredGyroData[], float flightSetPoints[], pid_output flightPids[], float actuatorRange, pid_terms pidConfig[]);
uint32_t SpinStopper(int32_t axis, float pidError);
void InlineUpdateWitchcraft(pid_terms pidConfig[]);
