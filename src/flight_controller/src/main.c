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

//#define XDC

int main(void)
{
	//Absolutely no MCU specific code to go here.

    int32_t count = 16;

    //TODO: Make automatic
	VectorIrqInit(ADDRESS_RFFW_START);

	//TODO Needs to pull parameters from flash here. For now we use defines
	getBoardHardwareDefs();

    InitializeMCUSettings();

    BoardInit();

    HandleRfbl();

    LoadConfig(ADDRESS_CONFIG_START);

    //force these defaults every startup
#ifdef XDC
    mainConfig.pidConfig[YAW].kp   = 420.00;
	mainConfig.pidConfig[ROLL].kp  = 390.00;
	mainConfig.pidConfig[PITCH].kp = 600.00;

	mainConfig.pidConfig[YAW].ki   = 666.00;
	mainConfig.pidConfig[ROLL].ki  = 425.00;
	mainConfig.pidConfig[PITCH].ki = 555.00;

	mainConfig.pidConfig[YAW].kd   = 1000.00;
	mainConfig.pidConfig[ROLL].kd  = 1400.00;
	mainConfig.pidConfig[PITCH].kd = 1600.00;

	mainConfig.pidConfig[YAW].ga   = 3.00;
	mainConfig.pidConfig[ROLL].ga  = 3.00;
	mainConfig.pidConfig[PITCH].ga = 3.00;

	mainConfig.pidConfig[YAW].wc   = 0;
	mainConfig.pidConfig[ROLL].wc  = 0;
	mainConfig.pidConfig[PITCH].wc = 0;

	mainConfig.filterConfig[YAW].gyro.r   = 170.00;
	mainConfig.filterConfig[ROLL].gyro.r  = 170.00;
	mainConfig.filterConfig[PITCH].gyro.r = 170.00;

	mainConfig.filterConfig[YAW].gyro.q   = 0.015;
	mainConfig.filterConfig[ROLL].gyro.q  = 0.015;
	mainConfig.filterConfig[PITCH].gyro.q = 0.015;

	mainConfig.filterConfig[YAW].gyro.p    = 0.005;
	mainConfig.filterConfig[ROLL].gyro.p   = 0.005;
	mainConfig.filterConfig[PITCH].gyro.p  = 0.005;

	mainConfig.filterConfig[YAW].kd.r      = 85.0;
	mainConfig.filterConfig[ROLL].kd.r     = 85.0;
	mainConfig.filterConfig[PITCH].kd.r    = 70.0;



	mainConfig.rcControlsConfig.midRc[PITCH]         = 1024;
	mainConfig.rcControlsConfig.midRc[ROLL]          = 1024;
	mainConfig.rcControlsConfig.midRc[YAW]           = 1024;
	mainConfig.rcControlsConfig.midRc[THROTTLE]      = 1024;
	mainConfig.rcControlsConfig.midRc[AUX1]          = 1024;
	mainConfig.rcControlsConfig.midRc[AUX2]          = 1024;
	mainConfig.rcControlsConfig.midRc[AUX3]          = 1024;
	mainConfig.rcControlsConfig.midRc[AUX4]          = 1024;

	mainConfig.rcControlsConfig.minRc[PITCH]         = 22;
	mainConfig.rcControlsConfig.minRc[ROLL]          = 22;
	mainConfig.rcControlsConfig.minRc[YAW]           = 22;
	mainConfig.rcControlsConfig.minRc[THROTTLE]      = 22;
	mainConfig.rcControlsConfig.minRc[AUX1]          = 342;
	mainConfig.rcControlsConfig.minRc[AUX2]          = 1706;
	mainConfig.rcControlsConfig.minRc[AUX3]          = 342;
	mainConfig.rcControlsConfig.minRc[AUX4]          = 0;

	mainConfig.rcControlsConfig.maxRc[PITCH]         = 2025;
	mainConfig.rcControlsConfig.maxRc[ROLL]          = 2025;
	mainConfig.rcControlsConfig.maxRc[YAW]           = 2025;
	mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 2025;
	mainConfig.rcControlsConfig.maxRc[AUX1]          = 1706;
	mainConfig.rcControlsConfig.maxRc[AUX2]          = 342;
	mainConfig.rcControlsConfig.maxRc[AUX3]          = 1706;
	mainConfig.rcControlsConfig.maxRc[AUX4]          = 1000000;

	mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
	mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
	mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
	mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
	mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
	mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
	mainConfig.rcControlsConfig.channelMap[AUX3]     = 6;
	mainConfig.rcControlsConfig.channelMap[AUX4]     = 100;
	mainConfig.rcControlsConfig.channelMap[AUX5]     = 100;
	mainConfig.rcControlsConfig.channelMap[AUX6]     = 100;
	mainConfig.rcControlsConfig.channelMap[AUX7]     = 100;
	mainConfig.rcControlsConfig.channelMap[AUX8]     = 100;
	mainConfig.rcControlsConfig.channelMap[AUX9]     = 100;
	mainConfig.rcControlsConfig.channelMap[AUX10]    = 100;
	mainConfig.rcControlsConfig.channelMap[AUX11]    = 100;
	mainConfig.rcControlsConfig.channelMap[AUX12]    = 100; //junk channel

	mainConfig.rcControlsConfig.rcCalibrated         = 1;

	mainConfig.rcControlsConfig.rxUsart              = ENUM_USART3;
	mainConfig.rcControlsConfig.rxProtcol            = USING_SPEKTRUM_TWO_WAY; //this is used by serial.c
//	mainConfig.gyroConfig.boardCalibrated            = 1; //this is used by serial.c
//	mainConfig.gyroConfig.gyroRotation               = 0; //this is used by serial.c
#endif

    HandleFcStartupReg();

    InitBuzzer();
    InitLeds();

    //TODO: Only init if we have USB. Some F3s are serial only.
    InitUsb();

    InitFlight();
    DelayMs(20);

    InitWatchdog(WATCHDOG_TIMEOUT_32S);

    buzzerStatus.status = STATE_BUZZER_STARTUP;
    ledStatus.status    = LEDS_SLOW_BLINK;

    while (1)
    {

    	scheduler(count--);

    	if (count == -1)
    		count = 16;

    }

}

void InitFlight(void) {

    //TODO: move the check into the init functions.
    if (board.flash[0].enabled) {
    	InitFlashChip();
    	InitFlightLogger();
    }

    InitRcData();
    InitMixer();
    InitFlightCode();     //flight code before PID code is a must since flight.c contains loop time settings the pid.c uses.
    InitPid();            //Relies on InitFlightCode for proper activations.
    DeInitActuators();    //Deinit before Init is a shotgun startup
    InitActuators();      //Actuator init should happen after soft serial init.
    ZeroActuators(32000); //output actuators to idle after timers are stable;

    InitBoardUsarts();    //most important thing is activated last, the ability to control the craft.

	if (!AccGyroInit(mainConfig.gyroConfig.loopCtrl)) {
		ErrorHandler(GYRO_INIT_FAILIURE);
	}

	InitTelemtry();
	InitWs2812();
	DelayMs(20);

}

void ErrorHandler(uint32_t error)
{
	switch (error) {
		case TIMER_INPUT_INIT_FAILIURE:
		case MSP_DMA_GYRO_RX_INIT_FAILIURE:
		case MSP_DMA_GYRO_TX_INIT_FAILIURE:
		case MSP_DMA_SPI_RX_INIT_FAILIURE:
		case MSP_DMA_SPI_TX_INIT_FAILIURE:
			//ping warning to user here, may not a valid reason to crash the board though
			return;
			break;
		case SERIAL_HALF_DUPLEX_INIT_FAILURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case SERIAL_INIT_FAILURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case FLASH_SPI_INIT_FAILIURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case WS2812_LED_INIT_FAILIURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case HARD_FAULT:  //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		case MEM_FAULT:   //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		case BUS_FAULT:   //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		case USAGE_FAULT: //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		case GYRO_SPI_INIT_FAILIURE: //gyro failed to init. Can't fly like this.
		case GYRO_INIT_FAILIURE: //gyro failed to init. Can't fly like this.
		case GYRO_SETUP_COMMUNICATION_FAILIURE: //gyro init success, but setting up register failed. Can't fly like this.
		default:
			break;
	}

	//bad errors will fall through here
	ZeroActuators(32000);
    while (1) {
		DoLed(0, 1);
		DoLed(1, 0);
		simpleDelay_ASM(50000);
		DoLed(0, 0);
		DoLed(1, 1);
		simpleDelay_ASM(75000);
    	ZeroActuators(10);
    }
}
