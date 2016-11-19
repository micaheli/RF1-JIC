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

inline float CalculateSD(float data[])
{
    float sum = 0.0, mean, standardDeviation = 0.0;

    int i;

    for(i=0; i<10; ++i)
    {
        sum += data[i];
    }

    mean = sum/10;

    for(i=0; i<10; ++i)
        standardDeviation += powf(data[i] - mean, 2);

    return (sqrtf(standardDeviation/10.0)) ;
}
