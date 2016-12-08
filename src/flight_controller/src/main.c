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

    //TODO: Make automatic
	VectorIrqInit(ADDRESS_RFFW_START);

	//TODO Needs to pull parameters from flash here. For now we use defines
	getBoardHardwareDefs();

    BoardInit();

    HandleRfbl();

    LoadConfig(ADDRESS_CONFIG_START);

    InitializeMCUSettings();

    HandleFcStartupReg();

    //InitDmaInputOnMotors(board.motors[0]);

    InitBuzzer();
    InitLeds();

    //TODO: Only init if we have USB. Some F3s are serial only.
    InitUsb();

    //TODO: move the check into the init functions.
    if (board.flash[0].enabled) {
//    	InitFlashChip();
//    	InitFlightLogger();
    }

    InitRcData();
    InitMixer();
    InitFlightCode(); //flight code before PID code is a must since flight.c contains loop time settings the pid.c uses.
    InitPid();        //Relies on InitFlightCode for proper activations.

    DeInitActuators();
    InitActuators();      //Actuator init should happen after soft serial init.
    ZeroActuators(32000); //output actuators to idle after timers are stable;


    //TODO: Move these functions to an init function in DMA shenanigans. F3 and F7 won't need the SPORT function in DMA shenanigans.
    //LEDs don't work the same time as dshot. Actuator init should cancel out this activation if dshot is enabled, but for now we'll check the config
//    if ( (mainConfig.mixerConfig.escProtcol != ESC_DSHOT600) && (mainConfig.mixerConfig.escProtcol != ESC_DSHOT300) && (mainConfig.mixerConfig.escProtcol != ESC_DSHOT150) ) {
//    	//    Ws2812LedInit();
//    	InitDmaOutputForSoftSerial(DMA_OUTPUT_WS2812_LEDS, board.motors[6]); //Enable LEDs on actuator 6
//    }
    //if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS_SPORT) {

   	InitBoardUsarts(); //most important thing is activated last, the ability to control the craft.

   	//only works for sport right now
    if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS_SPORT)
    	InitAllowedSoftOutputs();

    if (mainConfig.rcControlsConfig.rxProtcol == USING_SPEKTRUM_TWO_WAY)
    	InitSpektrumTelemetry();
/*

		InitDmaOutputForSoftSerial(DMA_OUTPUT_SPORT, board.motors[7]); //Enable S.Port on actuator 7, in place of USART 1 RX pin
		uint32_t currentTime = Micros();
		__disable_irq();
    	//prepare soft serial buffer and index
		softSerialLastByteProcessedLocation = 0;
		softSerialCurBuf = 0;
		softSerialInd[softSerialCurBuf] = 0;
		softSerialBuf[softSerialCurBuf][softSerialInd[softSerialCurBuf]++] = currentTime;
		//prepare soft serial buffer and index
		__enable_irq();
    //}
*/




    if (!AccGyroInit(mainConfig.gyroConfig.loopCtrl)) {
        ErrorHandler(GYRO_INIT_FAILIURE);
    }

    InitWatchdog(WATCHDOG_TIMEOUT_16S);

    buzzerStatus.status = STATE_BUZZER_OFF;
    ledStatus.status = LEDS_SLOW_BLINK;

//    OneWireInit();

    while (1) {


    	scheduler(count--);

    	if (count == -1)
    		count = 16;

    }

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
        DelayMs(40);
		DoLed(0, 0);
		DoLed(1, 1);
        DelayMs(40);
    	ZeroActuators(10);
    }
}
