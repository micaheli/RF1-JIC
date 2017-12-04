#include "includes.h"


void Beep(uint32_t hz, uint32_t length, uint32_t volume)
{

	uint32_t off = 1000 / (hz / 1000);
	uint32_t count = (hz / 1000) * length;
	uint32_t x;

	volume = CONSTRAIN(volume, 1, 15);

    FreeWheel();
    DelayMs(2);
    AFetLoOn();
	for (x = 0; x < count; x++)
	{
        AFetHiOn();
        delayUs(volume);
        AFetHiOff();
        delayUs(off);
    //inlineDigitalHi(B_FET_HI_GPIO, B_FET_HI_PIN);
    //delayUs(6);
    //inlineDigitalLo(B_FET_HI_GPIO, B_FET_HI_PIN);
    //delayUs(6);
	}
	FreeWheel();
}



void PlayC7(uint32_t ms)
{
    Beep(2093, ms, 10);
}

void PlayCs7(uint32_t ms)
{
    Beep(2217, ms, 10);
}

void PlayD7(uint32_t ms)
{
    Beep(2349, ms, 10);
}

void PlayDs7(uint32_t ms)
{
    Beep(2489, ms, 10);
}

void PlayE7(uint32_t ms)
{
    Beep(2637, ms, 10);
}

void PlayF7(uint32_t ms)
{
    Beep(2793, ms, 10);
}

void PlayFs7(uint32_t ms)
{
    Beep(2959, ms, 10);
}

void PlayG7(uint32_t ms)
{
    Beep(3135, ms, 10);
}

void PlayGs7(uint32_t ms)
{
    Beep(3322, ms, 10);
}

void PlayA7(uint32_t ms)
{
    Beep(2093, ms, 10);
}

void PlayAs7(uint32_t ms)
{
    Beep(2093, ms, 10);
}

void PlayB7(uint32_t ms)
{
    Beep(2093, ms, 10);
}