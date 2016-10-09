#pragma once

#define AXIS_NUMBER 3
#define KD_RING_BUFFER_SIZE 256

enum { KP = 0, KD, KI, PID_VARIABLE_COUNT };

typedef struct {
    float kp;
    float ki;
    float kd;
} pid_output;

typedef struct {
    float kp;
    float ki;
    float kd;
} pid_terms;

extern float pidSetpoint[AXIS_NUMBER];    //3 axis for pidc. range is in DPS.
extern pid_output pids[AXIS_NUMBER];



void InitPid (void);
void InlinePidController (float filteredGyroData[], float flightSetPoints[], pid_output flightPids[], float actuatorRange, pid_terms pidConfig[]);
