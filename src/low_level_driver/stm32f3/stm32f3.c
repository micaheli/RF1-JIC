#include "includes.h"

int FULL_32 = 0;
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV                     = RCC_PREDIV_DIV1 (1)
  *            PLLMUL                         = RCC_PLL_MUL9 (9)
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  clocks dividers */
  RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

	/* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

  systemUsTicks = (HAL_RCC_GetHCLKFreq()/1000000);
}

void BoardInit(void)
{
	SCB->VTOR = ADDRESS_FLASH_START; //set vector register to firmware start
	__enable_irq(); // enable interrupts

	HAL_Init();

	SystemClock_Config();

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();


  __HAL_RCC_DMA1_CLK_ENABLE();
  /*
  __USART1_CLK_ENABLE();
  __USART2_CLK_ENABLE();
  __USART3_CLK_ENABLE();
  __UART4_CLK_ENABLE();
  __UART5_CLK_ENABLE();
  __USART6_CLK_ENABLE();
  */
  __HAL_RCC_TIM1_CLK_ENABLE();
  __HAL_RCC_TIM2_CLK_ENABLE();
  __HAL_RCC_TIM6_CLK_ENABLE();

}

void VectorIrqInit(uint32_t address)
{
	SCB->VTOR = address; //set vector register to firmware start
	__enable_irq(); // enable interrupts

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


void USB_DEVICE_Init(void)
{

}
void USB_DEVICE_DeInit(void)
{

}

void USBInit(void)
{
}

void USBDeInit(void)
{
}

uint32_t TimerPrescalerDivisor(uint32_t timer)
{
	(void)(timer);
	return(1);
}
