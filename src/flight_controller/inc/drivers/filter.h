#pragma once

//Single axis paf filter
typedef struct {
	float q; //process noise covariance
	float r; //measurement noise covariance
	float x; //value
	float p; //estimation error covariance
	float k; //paf gain
} paf_state;

//config structure which is loaded by config
typedef struct {
    float q;
    float r;
    float p;
} filter_config;

typedef struct {
	filter_config kd;
	filter_config gyro;
} filter_device;

paf_state InitPaf(float q, float r, float p, float intial_value);
void PafUpdate(paf_state *state, float measurement);

#define M_PI_FLOAT	3.14159265358979323846f
