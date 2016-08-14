/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "includes.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t tInBuffer[HID_EPIN_SIZE], tOutBuffer[HID_EPOUT_SIZE-1];

/* Private function prototypes -----------------------------------------------*/
void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* Private functions ---------------------------------------------------------*/

int main(void)
{
    //uint8_t i = 0;

    BoardInit();

//todo: Do we want to init LEDs like this?
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


    while (1) {
        LED2_TOGGLE;

        HAL_Delay(200);
        if (tOutBuffer[0]==2) {
        	LED1_TOGGLE;
        	tOutBuffer[0] = 0; //clear buffer id, only need to clear first byte.
    		tInBuffer[0] = 1;
    		tInBuffer[1]=0x52;
    		tInBuffer[2]=0x61;
    		tInBuffer[3]=0x63;
    		tInBuffer[4]=0x65;
    		tInBuffer[5]=0x46;
    		tInBuffer[6]=0x6c;
    		tInBuffer[7]=0x69;
    		tInBuffer[8]=0x67;
    		tInBuffer[9]=0x68;
    		tInBuffer[10]=0x74;
    		tInBuffer[11]=0x20;
    		tInBuffer[12]=0x46;
    		tInBuffer[13]=0x54;
    		tInBuffer[14]=0x57;
    		tInBuffer[15]=0x21;
    		tInBuffer[16]=0x21;
            USBD_HID_SendReport (&hUsbDeviceFS, tInBuffer, HID_EPIN_SIZE);
        }
    }
}

void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(GPIOx, GPIO_Pin);

    GPIO_InitStructure.Pin = GPIO_Pin;

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
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
