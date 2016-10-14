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

    if (rtc_read_backup_reg(FC_STATUS_REG) == FC_STATUS_INFLIGHT) { //FC crashed while inflight.
    	debugU32[7]=6;
    	//boot_to_app();
    } else {
    	debugU32[7]=7;
    }

    debugU32[5]=7;

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
    	if (count == -1) {
    		count = 16;
/*
    		if(uartHandle.gState != HAL_UART_STATE_BUSY_TX)
    		{
				aTxBuffer[0]=1;
				aTxBuffer[1]=2;
				aTxBuffer[2]=3;
				aTxBuffer[3]=4;
				aTxBuffer[4]=5;
				aTxBuffer[5]=6;
				aTxBuffer[6]=7;
				aTxBuffer[7]=8;
				aTxBuffer[8]=9;
				aTxBuffer[9]=10;

				if(HAL_UART_Transmit_DMA(&uartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
				{
					ErrorHandler();
				}
    		}
*/
/*
    		if(uartHandle.RxState != HAL_UART_STATE_BUSY_RX)
    		{

				// ##-2- Put UART peripheral in reception process ###########################
				if(HAL_UART_Receive_DMA(&uartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
				{
					ErrorHandler();
				}
				for (unsigned char i=0;i<63;i++) {
					tInBuffer[i] = aRxBuffer[i];
				}
				tInBuffer[0] = 1;

			    USBD_HID_SendReport (&hUsbDeviceFS, tInBuffer, HID_EPIN_SIZE);
    		}
*/
    	}


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
