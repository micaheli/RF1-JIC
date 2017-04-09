#include "includes.h"



int main(void)
{

	int32_t count;
	int32_t x;

	count = 16;

	VectorIrqInit(0x08000000);
    BoardInit();

	InitLeds();
	DelayMs(200);
	InitFets();
	DelayMs(5);

	for (x = 0; x < 5; x++) {
		Beep(2000 + x * 6000, 10, 10);
		DelayMs(10);
	}

    while (1)
    {

		inlineDigitalHi(LED0_GPIO, LED0_PIN);
		DelayMs(100);
		inlineDigitalHi(LED1_GPIO, LED1_PIN);
		DelayMs(100);
		inlineDigitalHi(LED2_GPIO, LED2_PIN);
		DelayMs(100);

		inlineDigitalLo(LED0_GPIO, LED0_PIN);
		DelayMs(100);
		inlineDigitalLo(LED1_GPIO, LED1_PIN);
		DelayMs(100);
		inlineDigitalLo(LED2_GPIO, LED2_PIN);
		DelayMs(100);

    	Scheduler(count--);

    	if (count == -1)
    		count = 16;
    }

}
