#include "includes.h"

uint32_t errorMask = 0;
uint8_t tInBuffer[HID_EPIN_SIZE], tOutBuffer[HID_EPOUT_SIZE-1];

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

    //TODO: Only init if we have USB. Some F3s are serial only.
    InitUsb();

    InitFlight();

    InitWatchdog(WATCHDOG_TIMEOUT_32S);

    buzzerStatus.status = STATE_BUZZER_STARTUP;
    ledStatus.status    = LEDS_SLOW_BLINK;

    while (1)
    {

    	Scheduler(count--);

    	if (count == -1)
    		count = 16;

    }

}

void InitVbusSensing(void)
{
#ifdef VBUS_SENSING
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(ports[VBUS_SENSING_GPIO], VBUS_SENSING_PIN);

    GPIO_InitStructure.Pin   = VBUS_SENSING_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    HAL_GPIO_Init(ports[VBUS_SENSING_GPIO], &GPIO_InitStructure);
#endif
}

uint32_t IsUsbConnected(void)
{
#ifdef VBUS_SENSING
	return(!inlineIsPinStatusHi(ports[VBUS_SENSING_GPIO], VBUS_SENSING_PIN));
#else
	return(0);
#endif

}

void InitFlight(void) {

    //TODO: move the check into the init functions.

	DeInitAllowedSoftOutputs();

    if (board.flash[0].enabled)
    {
    	InitFlashChip();
    	InitFlightLogger();
    }

    InitVbusSensing();
    InitRcData();
    InitMixer();
    InitFlightCode();     //flight code before PID code is a must since flight.c contains loop time settings the pid.c uses.
    InitPid();            //Relies on InitFlightCode for proper activations.
    DeInitActuators();    //Deinit before Init is a shotgun startup
    InitActuators();      //Actuator init should happen after soft serial init.
    ZeroActuators(5000);  //output actuators to idle after timers are stable;

	InitAdc();
    InitModes();          //set flight modes mask to zero.
    InitBoardUsarts();    //most important thing is activated last, the ability to control the craft.

	if (!AccGyroInit(mainConfig.gyroConfig.loopCtrl))
	{
		ErrorHandler(GYRO_INIT_FAILIURE);
	}

	InitTelemtry();

#ifndef SPMFC400
	if (!IsUsbConnected())
	{
		InitWs2812();
	}
#else	InitWs2812();
#endif
	//InitTransponderTimer();
	DelayMs(2);

}
