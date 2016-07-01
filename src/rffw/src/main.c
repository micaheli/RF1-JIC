/* Includes ------------------------------------------------------------------*/
#include "includes.h"

//#include "usbd_hid.h"
//#include "usb_device.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
//static int8_t hidBuffer[4];

/* Private function prototypes -----------------------------------------------*/
void BoardInit(void);
void SystemClock_Config(void);
void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* Private functions ---------------------------------------------------------*/

int main(void)
{
//    uint8_t i = 0;

	BoardInit();
//	MX_USB_DEVICE_Init();
	InitializeLED(LED1_GPIO_PORT, LED1_PIN);

	while (1) {
		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
		HAL_GPIO_TogglePin(SERVO1_GPIO_PORT, SERVO1_PIN);
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
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_Pin;

	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);
}
