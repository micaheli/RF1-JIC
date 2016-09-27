#include "includes.h"

inline float InlineConstrainf(float amt, float low, float high)
{
    if (amt < low)
        return low;
    else if (amt > high)
        return high;
    else
        return amt;
}

inline float InlineChangeRangef(float oldValue, float oldMax, float oldMin, float newMax, float newMin)
{
	float oldRange = (oldMax - oldMin);
	float newRange = (newMax - newMin);
	return (((oldValue - oldMin) * newRange) / oldRange) + newMin;
}
