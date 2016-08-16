#pragma once

extern float trueRcCommandF[4];     //4 sticks. range is -1 to 1, directly related to stick position
extern float curvedRcCommandF[4];   //4 sticks. range is -1 to 1, this is the rcCommand after the curve is applied
extern float smoothedRcCommandF[4]; //4 sticks. range is -1 to 1, this is the smoothed rcCommand

void collectRcCommand (uint16_t rcData[4], float *trueRcCommandF, float *curvedRcCommandF);
float applyRcCommandCurve (float rcCommand);

