#pragma once

#define PIf       3.14159265358f
#define IPIf      0.31830988618f //inverse of Pi
#define I180      0.00555555555f

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

extern uint8_t BitReverse8(uint8_t byteToConvert);
extern uint32_t BigToLittleEndian32(uint32_t numberIn);
extern float InlineConstrainf(float amt, float low, float high);
extern float InlineChangeRangef(float oldValue, float oldMax, float oldMin, float newMax, float newMin);
extern float CalculateSD(float data[]);
extern float CalculateSDSize(float data[], uint32_t size);
