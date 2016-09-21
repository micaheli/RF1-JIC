#pragma once

//Single axis kalman filter
typedef struct {
	float q; //process noise covariance
	float r; //measurement noise covariance
	float x; //value
	float p; //estimation error covariance
	float k; //kalman gain
} kalman_state;

kalman_state kalman_init(float q, float r, float p, float intial_value);
void kalman_update(kalman_state *state, float measurement);

typedef struct filterStatePt1_s {
	float state;
	float RC;
	float constdT;
} filterStatePt1_t;

#define M_PI_FLOAT	3.14159265358979323846f

kalman_state kalman_init(float q, float r, float p, float intial_value);
void kalman_update(kalman_state *state, float measurement);
