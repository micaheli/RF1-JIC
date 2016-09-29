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

    uint8_t count = 0;

    //need to move this to mcu specific code file
	SCB->VTOR = ADDRESS_FLASH_START; //set vector register to firmware start
	__enable_irq(); // enable interrupts

    BoardInit();
    LedInit();
    UsbInit();

    LoadConfig();

    InitRcData();
    InitMixer();

    if (!accgyroInit(gyroConfig.loopCtrl)) {
        //ErrorHandler();
    }

    ledStatus.status = LEDS_SLOW_BLINK;

    BoardUsartInit();

    bzero(aRxBuffer, sizeof(aRxBuffer));
    bzero(aTxBuffer, sizeof(aTxBuffer));

    while (1) {
    	scheduler(count++);
    	if (count == 16) {
    		count = 0;

        	DelayMs(500);

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


    }

}

void ErrorHandler(void)
{
    while (1) {
        LED1_ON;
        LED2_OFF;
        DelayMs(40);
        LED1_OFF;
        LED2_ON;
        DelayMs(40);
    }
}
