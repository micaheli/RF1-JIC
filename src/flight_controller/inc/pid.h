#pragma once

#define AXIS_NUMBER 3
#define VECTOR_NUMBER 3
#define KD_RING_BUFFER_SIZE 256
#define MAX_KD 0.3231f

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
} pid_terms;

extern float pidSetpoint[AXIS_NUMBER];    //3 axis for pidc. range is in DPS.
extern pid_output pids[AXIS_NUMBER];
extern float currentKdFilterConfig[AXIS_NUMBER];

void InitPid (void);
//void InlineInitPidFilters(void);
void InlinePidController (float filteredGyroData[], float filteredGyroDataKd[], float flightSetPoints[], pid_output flightPids[], float actuatorRange, pid_terms pidConfig[]);
uint32_t SpinStopper(int32_t axis, float pidError);
void InlineUpdateWitchcraft(pid_terms pidConfig[]);
