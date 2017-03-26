#pragma once

//Single axis paf filter
typedef struct {
	float q; //process noise covariance
	float r; //measurement noise covariance
	float x; //value
	float p; //estimation error covariance
	float k; //paf gain
	float stdDev[32]; //standard deviation counter
	uint32_t stdDevCnt; //standard deviation counter
	float output; //paf gain
} paf_state;

//config structure which is loaded by config
typedef struct {
    float q;
    float r;
    float p;
} paf_filter_config_record;

typedef struct {
	uint32_t filterMod;
	paf_filter_config_record kd;
	paf_filter_config_record gyro;
	paf_filter_config_record acc;
} filter_device;

typedef struct {
    float a0, a1, a2, a3, a4;
    float x1, x2, y1, y2;
} biquad_state;

typedef struct {
	float state;
	float rC;
	float dT;
} lpf_state;

typedef struct
{
	float x; //state matrix
	float a;
	float u; //control variable matrix
	float b;
	float w;
} kalman_state;

extern paf_state pafGyroStates[];

extern void InitPaf(paf_state *pafState, float q, float r, float p, float intial_value);
extern void PafUpdate(paf_state *state, float measurement);

extern void  InitBiquad(float filterCutFreq, biquad_state *newState, float refreshRateSeconds, uint32_t filterType, biquad_state *oldState, float bandwidth);
extern float BiquadUpdate(float sample, biquad_state *bQstate);
extern void  LpfInit(lpf_state *filter, float frequencyCut, float refreshRateSeconds);
extern float LpfUpdate(float input, lpf_state *filter);

#define M_LN2_FLOAT	0.69314718055994530942f
#define M_PI_FLOAT	3.14159265358979323846f
#define BIQUAD_BANDWIDTH 1.92f

#define FILTER_TYPE_LOWPASS  0
#define FILTER_TYPE_NOTCH    1
#define FILTER_TYPE_PEEK     2
#define FILTER_TYPE_HIGHPASS 3
