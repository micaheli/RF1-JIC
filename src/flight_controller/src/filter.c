#include "includes.h"

//enum
//{
//	PX = 0,
//	PY = 1,
//	PZ = 2,
//	RX = 3,
//	RY = 4,
//	RZ = 5,
//};

//enum
//{
//	AX = 0,
//	AY = 1,
//	AZ = 2,
//};


void InitKalman(kalman_state *kalmanState)
{
/*

	//halfGyrodTsquared

	X(0) = A*X(-1)

	Matrix A
	1,  0,  0, dT,  0,  0
	0,  1,  0,  0, dT,  0
	0,  0,  1,  0,  0, dT
	0,  0,  0,  1,  0,  0
	0,  0,  0,  0,  1,  0
	0,  0,  0,  0,  0,  1

	State Matrix
	pX
	pY
	pZ
	rX
	rY
	rZ

	Matrix B
	0.5dT^2,       0,       0
	      0, 0.5dT^2,       0
	      0,       0, 0.5dT^2
	     dt,       0,       0
	      0,      dt,       0
	      0,       0,      dt

	Control Variable Matrix
	aX
	aY
	aZ


	State Matrix
	pX
	pY
	pZ
	rX
	rY
	rZ

 */
	//kalmanState->x[0][0] = 1.0f;
}

void InitPaf(paf_state *pafState, float q, float r, float p, float intial_value)
{

	//pafState->q = q * 0.000001;
	//pafState->r = r * 0.001;
	//pafState->p = p * 0.001;
	//pafState->x = intial_value;

	pafState->q = q;
	pafState->r = r;
	pafState->p = p;
	pafState->x = intial_value;

	//reset counter. Not need to reset the array since we use the counter and that would be too slow
	pafState->stdDevCnt = 31;

}

void PafUpdate(paf_state *state, float measurement)
{
	/*
	float modifier;

	switch (mainConfig.filterConfig[2].filterMod)
	{
		case 0:
			modifier = 1;
			break;
		case 1:
			modifier = 16.4;
			break;
		case 2:
			modifier = 164.0;
			break;
		case 3:
			modifier = 328.0;
			break;
		case 4:
			modifier = 656.0;
			break;
		case 5:
			modifier = 1312.0;
			break;
		case 6:
		default:
			modifier = 2624.0;
			break;

	}
	*/


	//update stdDev
	if(state->stdDevCnt)
		state->stdDev[state->stdDevCnt--] = measurement;

	//prediction update
	state->p = state->p + state->q;

	//measurement update
	state->k = state->p / (state->p + state->r);

	//if (ABS(measurement) > 1990.0f)
	//	state->k = 0.0f;
//	state->x = state->x + state->k * (measurement * modifier - state->x);
	state->x = state->x + state->k * (measurement - state->x);
	state->p = (1 - state->k) * state->p;

//	state->output = (state->x / modifier);
	state->output = state->x;
}



void InitBiquad(float filterCutFreq, biquad_state *newState, float refreshRateSeconds, uint32_t filterType, biquad_state *oldState, float bandwidth)
{

	float samplingRate;
    float bigA, omega, sn, cs, alpha, beta;
    float a0, a1, a2, b0, b1, b2;

    float dbGain = 4.0;

    samplingRate = (1 / refreshRateSeconds);

	omega = 2 * (float)M_PI_FLOAT * (float) filterCutFreq / samplingRate;
	sn    = (float)sinf((float)omega);
	cs    = (float)cosf((float)omega);
	alpha = sn * (float)sinf( (float)((float)M_LN2_FLOAT / 2 * (float)bandwidth * (omega / sn)) );

	(void)(beta);
    //bigA  = powf(10, dbGain /40);
	//beta  = arm_sqrt_f32(bigA + bigA);

	switch (filterType)
	{
		case FILTER_TYPE_LOWPASS:
			b0 = (1 - cs) /2;
			b1 = 1 - cs;
			b2 = (1 - cs) /2;
			a0 = 1 + alpha;
			a1 = -2 * cs;
			a2 = 1 - alpha;
			break;
		case FILTER_TYPE_NOTCH:
			b0 = 1;
			b1 = -2 * cs;
			b2 = 1;
			a0 = 1 + alpha;
			a1 = -2 * cs;
			a2 = 1 - alpha;
			break;
		case FILTER_TYPE_PEEK:
		    bigA = powf(10, dbGain /40);
			b0   = 1 + (alpha * bigA);
			b1   = -2 * cs;
			b2   = 1 - (alpha * bigA);
			a0   = 1 + (alpha / bigA);
			a1   = -2 * cs;
			a2   = 1 - (alpha / bigA);
			break;
		 case FILTER_TYPE_HIGHPASS:
			b0 = (1 + cs) /2;
			b1 = -(1 + cs);
			b2 = (1 + cs) /2;
			a0 = 1 + alpha;
			a1 = -2 * cs;
			a2 = 1 - alpha;
			break;
	}

    // precompute the coefficients
    newState->a0 = b0 / a0;
    newState->a1 = b1 / a0;
    newState->a2 = b2 / a0;
    newState->a3 = a1 / a0;
    newState->a4 = a2 / a0;

    // zero initial samples
    //todo: make updateable on the fly
    newState->x1 =  oldState->x1;
    newState->x2 =  oldState->x2;
    newState->y1 =  oldState->y1;
    newState->y2 =  oldState->y1;

}

void LpfInit(lpf_state *filter, float frequencyCut, float refreshRateSeconds)
{
	filter->dT = refreshRateSeconds;
	filter->rC = 1.0f / ( 2.0f * M_PI * frequencyCut );
	filter->state = 0.0;
}

float LpfUpdate(float input, lpf_state *filter)
{

	filter->state = filter->state + filter->dT / (filter->rC + filter->dT) * (input - filter->state);

	return (filter->state);
}

float BiquadUpdate(float sample, biquad_state *state)
{
    float result;

    /* compute result */
    result = state->a0 * (float)sample + state->a1 * state->x1 + state->a2 * state->x2 -
            state->a3 * state->y1 - state->a4 * state->y2;

    /* shift x1 to x2, sample to x1 */
    state->x2 = state->x1;
    state->x1 = (float)sample;
    /* shift y1 to y2, result to y1 */
    state->y2 = state->y1;
    state->y1 = result;

    return (float)result;

}
