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
	state->x = state->x + state->k * (measurement - state->x);
	state->p = (1 - state->k) * state->p;
}
