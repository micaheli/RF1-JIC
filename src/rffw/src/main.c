/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "includes.h"

#include "usbd_hid.h"
#include "usb_device.h"

#include "drivers/invensense_bus.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
//static int8_t hidBuffer[4];

/* Private function prototypes -----------------------------------------------*/
void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void DMA_Init(void);

/* Private functions ---------------------------------------------------------*/

int main(void)
{
    //uint8_t i = 0;

	BoardInit();

#if LEDn >= 1
    InitializeLED(LED1_GPIO_Port, LED1_GPIO_Pin);
#endif
#if LEDn >= 2
    InitializeLED(LED2_GPIO_Port, LED2_GPIO_Pin);
#endif
#if LEDn >= 3
    InitializeLED(LED3_GPIO_Port, LED3_GPIO_Pin);
#endif

	USB_DEVICE_Init();

    DMA_Init();

    if (!accgyroInit()) {
        ErrorHandler();
    }

	while (1) {
        LED2_TOGGLE;

        /*
        hidBuffer[0] = 0; // buttons
        if (i < 10) {
            hidBuffer[1] = 0x10;
            hidBuffer[2] = 0x00;
        } else if (i < 20) {
            hidBuffer[1] = 0x00;
            hidBuffer[2] = 0x10;
        } else if (i < 30) {
            hidBuffer[1] = -0x10;
            hidBuffer[2] = 0x00;
        } else {
            hidBuffer[1] = 0x00;
            hidBuffer[2] = -0x10;
        }
        i = (i + 1) % 40;

        USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidBuffer, 4);
        */

        HAL_Delay(100);
	}
}

void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(GPIOx, GPIO_Pin);

	GPIO_InitStructure.Pin = GPIO_Pin;

	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);

	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
}

void ErrorHandler(void)
{
    while (1) {
        LED1_TOGGLE;
        HAL_Delay(40);
    }
}
