#include "includes.h"

int main(void)
{

	int32_t count;

	count = 16;

	VectorIrqInit(0x08060000);
	GetBoardHardwareDefs();
    InitializeMCUSettings();
    BoardInit();
    InitLeds();

    while (1)
    {
    	Scheduler(count--);

    	if (count == -1)
    		count = 16;
    }

}
