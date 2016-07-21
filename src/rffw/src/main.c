/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "includes.h"

#include "usbd_hid.h"
#include "usb_device.h"

#include "accgyro/invensense_bus.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
//static int8_t hidBuffer[4];

/* Private function prototypes -----------------------------------------------*/
void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* Private functions ---------------------------------------------------------*/

int main(void)
{
    //uint8_t i = 0;

	BoardInit();

	InitializeLED(LED0_GPIO_PORT, LED0_PIN);
	InitializeLED(LED1_GPIO_PORT, LED1_PIN);

	USB_DEVICE_Init();

    if (!accgyroInit()) {
        ErrorHandler();
    }

	while (1) {
		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);

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
        HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_PIN);
        HAL_Delay(40);
    }
}
