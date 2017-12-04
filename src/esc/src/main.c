/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
int BoardInit(void);
void SystemClock_Config(void);
void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* Private functions ---------------------------------------------------------*/

int main(void)
{

	BoardInit();
	InitializeLED(LED1_GPIO_PORT, LED1_PIN);

	while (1) {
		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
		HAL_GPIO_TogglePin(SERVO1_GPIO_PORT, SERVO1_PIN);

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
