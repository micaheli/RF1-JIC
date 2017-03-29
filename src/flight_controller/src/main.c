#include "includes.h"

int main(void)
{
	//Absolutely no MCU specific code to go here.

    int32_t count = 16;

    //TODO: Make automatic
	VectorIrqInit(ADDRESS_RFFW_START);

	//TODO Needs to pull parameters from flash here. For now we use defines
	GetBoardHardwareDefs();

    InitializeMCUSettings();

    BoardInit();

    HandleRfbl();

    LoadConfig(ADDRESS_CONFIG_START);

    SpektrumBind(mainConfig.rcControlsConfig.bind);

    HandleFcStartupReg();

    InitBuzzer();
    InitLeds();

    InitUsb();

    InitFlight();

    InitWatchdog(WATCHDOG_TIMEOUT_32S);

    InitSpektrumTelemetry();

    buzzerStatus.status = STATE_BUZZER_STARTUP;
    ledStatus.status    = LEDS_SLOW_BLINK;

    //InitializeGpio(ports[ENUM_PORTB], GPIO_PIN_1, 0);
    //InitializeGpio(ports[ENUM_PORTB], GPIO_PIN_0, 0);
    while (1)
    {

    	Scheduler(count--);

    	if (count == -1)
    		count = 16;

    }

}
