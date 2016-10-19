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

    BoardInit();

    HandleRfbl();

    LoadConfig(ADDRESS_CONFIG_START);

    InitializeMCUSettings();

    HandleFcStartupReg();

    InitBuzzer();
    InitLeds();
    InitUsb();
    InitRcData();
    InitMixer();
    InitFlightCode();
    InitPid();
    InitActuators();

    if (!accgyroInit(mainConfig.gyroConfig.loopCtrl)) {
        //ErrorHandler();
    }

    InitWatchdog(WATCHDOG_TIMEOUT_1S);

    buzzerStatus.status = STATE_BUZZER_OFF;
    ledStatus.status = LEDS_SLOW_BLINK;

    BoardUsartInit();

    bzero(serialRxBuffer, sizeof(serialRxBuffer));
    bzero(serialTxBuffer, sizeof(serialTxBuffer));

    while (1) {

    	scheduler(count--);

    	if (count == -1)
    		count = 16;

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
