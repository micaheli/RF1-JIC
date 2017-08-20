//#define STM32F405xx
#include "includes.h"

volatile int retValChk;

int main(void)
{
	//Absolutely no MCU specific code to go here.

    int count = 16;

    //TODO: Make automatic
	retValChk = VectorIrqInit(ADDRESS_RFFW_START);

	//TODO Needs to pull parameters from flash here. For now we use defines
	retValChk = GetBoardHardwareDefs();

    retValChk = InitializeMCUSettings();

    retValChk = BoardInit();

    //DshotInit(1);
    
    retValChk = HandleRfbl();

    retValChk = LoadConfig(ADDRESS_CONFIG_START);

    SpektrumBind(mainConfig.rcControlsConfig.bind);

    retValChk = HandleFcStartupReg();

    retValChk = InitBuzzer();
    retValChk = InitLeds();

    retValChk = InitUsb();

    retValChk = InitFlight();

    retValChk = InitWatchdog(WATCHDOG_TIMEOUT_32S);

    buzzerStatus.status = STATE_BUZZER_STARTUP;
    ledStatus.status    = LEDS_SLOW_BLINK;

    //InitializeGpio(ports[ENUM_PORTB], GPIO_PIN_1, 0);
    //InitializeGpio(ports[ENUM_PORTB], GPIO_PIN_0, 0);

    while (1)
    {

    	Scheduler(count--);

    	if (count == -1)
    		count = 16;

		//If 1wire is run, the gyro is disabled for the SPMFCF400.  This prevents the watchdog from ever resetting, causing the board to reset.
	    if (oneWireHasRun)
		    FeedTheDog();
    }

}
