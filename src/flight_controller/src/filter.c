#include "includes.h"

paf_state InitPaf(float q, float r, float p, float intial_value)
{
	paf_state result;
	result.q = q;
	result.r = r;
	result.p = p;
	result.x = intial_value;

	return result;
}

void PafUpdate(paf_state *state, float measurement)
{
	//prediction update
	//omit x = x
	state->p = state->p + state->q;

	//measurement update
	state->k = state->p / (state->p + state->r);
	float x = state->x + state->k * (measurement - state->x);
	if (x > -1000)
		state->x = x;

	state->p = (1 - state->k) * state->p;
}



void InitBiquad(float filterCutFreq, biquad_state *newState, float refreshRateSeconds, uint32_t filterType)
{

	float samplingRate;
    float omega, sn, cs, alpha;
    float a0, a1, a2, b0, b1, b2;

    samplingRate = (1 / refreshRateSeconds);

	omega = 2 * (float)M_PI_FLOAT * (float) filterCutFreq / samplingRate;
	sn = (float)sinf((float)omega);
	cs = (float)cosf((float)omega);
	alpha = sn * (float)sinf( (float)((float)M_LN2_FLOAT / 2 * (float)BIQUAD_BANDWIDTH * (omega / sn)) );

	if (filterType) {
	    b0 = 1;
	    b1 = -2 * cs;
	    b2 = 1;
	    a0 = 1 + alpha;
	    a1 = -2 * cs;
	    a2 = 1 - alpha;
	} else {
		b0 = (1.0 - cs) / 2;
		b1 = 1.0 - cs;
		b2 = (1.0 - cs) / 2;
		a0 = 1.0 + alpha;
		a1 = -2.0 * cs;
		a2 = 1.0 - alpha;
	}



    // precompute the coefficients
    newState->a0 = b0 / a0;
    newState->a1 = b1 / a0;
    newState->a2 = b2 / a0;
    newState->a3 = a1 / a0;
    newState->a4 = a2 / a0;

    // zero initial samples
    //todo: make updateable on the fly
    newState->x1 = 0;
    newState->x2 = 0;
    newState->y1 = 0;
    newState->y2 = 0;

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
