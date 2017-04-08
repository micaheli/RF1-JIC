#include "includes.h"


void Beep(uint32_t hz, uint32_t length, uint32_t volume)
{

	uint32_t off = 10 / hz;
	uint32_t count = khz * length / 3;
	uint32_t x;

	volume = CONSTRAIN(volume, 1, 100);

    FreeWheel();
    DelayMs(2);
	for (x = 0; x < count; x++)
	{
        AFetLoOn();
        AFetHiOn();
        delayUs(volume);
        AFetHiOff();
        AFetLoOff();
        delayUs(off);
        BFetLoOn();
        BFetHiOn();
        delayUs(volume);
        BFetHiOff();
        BFetLoOff();
        delayUs(off);
        CFetLoOn();
        CFetHiOn();
        delayUs(volume);
        CFetHiOff();
        CFetLoOff();
        delayUs(off);
    //inlineDigitalHi(B_FET_HI_GPIO, B_FET_HI_PIN);
    //delayUs(6);
    //inlineDigitalLo(B_FET_HI_GPIO, B_FET_HI_PIN);
    //delayUs(6);
	}
	FreeWheel();

}