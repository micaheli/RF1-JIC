#include "includes.h"

int main(void)
{

	int32_t count;

	count = 2;

	VectorIrqInit(bootloaderAddress);
	GetBoardHardwareDefs();
    InitializeMCUSettings();
    BoardInit();
    InitRfbl();
    //FC will either boot into app or continue on here after the InitRfbl function runs

    while (1)
    {
    	Scheduler(count--);

    	if (count == -1)
    		count = 2;
    }

}
