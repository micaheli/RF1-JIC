/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "includes.h"

#include "usbd_hid.h"
#include "usb_device.h"

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
    //uint8_t i = 0;

    BoardInit();
    LedInit();

    USB_DEVICE_Init();

    if (!accgyroInit(LOOP_H8)) {
        ErrorHandler();
    }

    while (1) {
        LED1_TOGGLE;

        HAL_Delay(100);
    }
}

void ErrorHandler(void)
{
    while (1) {
        LED1_ON;
        LED2_OFF;
        HAL_Delay(40);
        LED1_OFF;
        LED2_ON;
        HAL_Delay(40);
    }
}
