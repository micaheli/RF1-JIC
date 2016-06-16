/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void BoardInit(void);
void SystemClock_Config(void);
void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void InitializeSERVO(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* Private functions ---------------------------------------------------------*/

int main(void)
	{

	BoardInit();
	InitializeLED(LED1_GPIO_PORT, LED1_PIN);

	while (1)
	{
		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
		HAL_GPIO_TogglePin(SERVO1_GPIO_PORT, SERVO1_PIN);
		HAL_Delay(100);

	}
}

void BoardInit(void)
{
	HAL_Init();

	SystemClock_Config();
 
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

}

void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_Pin;

	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);

}