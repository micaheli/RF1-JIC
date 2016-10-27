/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "includes.h"

//#include "usbd_hid.h"
//#include "usb_device.h"

#include "input/gyro.h"
#include "drivers/invensense_bus.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t tInBuffer[HID_EPIN_SIZE], tOutBuffer[HID_EPOUT_SIZE-1];

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

int main(void)
{
	//Absolutely no MCU specific code to go here.

    int32_t count = 16;

    VectorIrqInit(ADDRESS_FLASH_START);

	//TODO Needs to pull parameters from flash here. For now we use defines
	getBoardHardwareDefs();

    BoardInit();

    HandleRfbl();

    LoadConfig(ADDRESS_CONFIG_START);

    InitializeMCUSettings();

    HandleFcStartupReg();

    InitBuzzer();
    InitLeds();
    InitUsb();
    InitFlashChip();
    InitFlightLogger();
    InitRcData();
    InitMixer();
    InitFlightCode();
    InitPid();
    InitActuators();

    if (!accgyroInit(mainConfig.gyroConfig.loopCtrl)) {
        ErrorHandler();
    }

    InitWatchdog(WATCHDOG_TIMEOUT_1S);

    buzzerStatus.status = STATE_BUZZER_OFF;
    ledStatus.status = LEDS_SLOW_BLINK;

    BoardUsartInit();

    bzero(serialRxBuffer, sizeof(serialRxBuffer));
    bzero(serialTxBuffer, sizeof(serialTxBuffer));
	
	uint8_t calibrated1 = 0;
	uint8_t calibrated2 = 0;
    while (1) {


    	scheduler(count--);

    	if (count == -1)
    		count = 16;
			/*
	    	if (InlineMillis() > 2000 && calibrated1==0)
	    	{
		    	SetCalibrate1();
		    	calibrated1 = 1;
	    	}
	    	if (InlineMillis() > 4000 && calibrated2 == 0)
	    	{
		    	if(SetCalibrate2())
		    		SaveConfig(ADDRESS_CONFIG_START);
		    	calibrated2 = 1;
	    	}
			*/

    }

}

void ErrorHandler(void)
{
	ZeroActuators();

    while (1) {
		DoLed(0, 1);
		DoLed(1, 0);
        DelayMs(40);
		DoLed(0, 0);
		DoLed(1, 1);
        DelayMs(40);
    }
}
