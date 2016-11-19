#pragma once

#define PIf       3.14159265358f
#define IPIf      0.31830988618f //inverse of Pi

//These were defined in a lib file?
#ifndef MIN
  #define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
  #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef ABS
  #define ABS(x) ((x) > 0 ? (x) : -(x))
#endif

#ifndef CONSTRAIN
  #define CONSTRAIN(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

float InlineConstrainf(float amt, float low, float high);
float InlineChangeRangef(float oldValue, float oldMax, float oldMin, float newMax, float newMin);
float CalculateSD(float data[]);
