#include "includes.h"

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = FC_PLLM;
  RCC_OscInitStruct.PLL.PLLN = FC_PLLN;
  RCC_OscInitStruct.PLL.PLLP = FC_PLLP;
  RCC_OscInitStruct.PLL.PLLQ = FC_PLLQ;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }
  
  /* Activate the OverDrive to reach the 216 Mhz Frequency */
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    while(1);
  }


  /* Select PLLSAI output as USB clock source */
  PeriphClkInitStruct.PLLSAI.PLLSAIN = FC_PLL_SAIN;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = FC_PLL_SAIQ;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = FC_PLL_SAIP;
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
  PeriphClkInitStruct.Clk48ClockSelection  = RCC_CLK48SOURCE_PLLSAIP;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct)  != HAL_OK)
  {
    while(1);
  }


  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    while(1);
  }


	PeriphClkInitStruct.PeriphClockSelection  = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_USART6|RCC_PERIPHCLK_UART4|RCC_PERIPHCLK_UART5|RCC_PERIPHCLK_UART7|RCC_PERIPHCLK_UART8|RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C3|RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_I2C4;
	PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
	PeriphClkInitStruct.Uart4ClockSelection  = RCC_UART4CLKSOURCE_PCLK1;
	PeriphClkInitStruct.Uart5ClockSelection  = RCC_UART5CLKSOURCE_PCLK1;
	PeriphClkInitStruct.Usart6ClockSelection = RCC_USART6CLKSOURCE_PCLK2;
	PeriphClkInitStruct.Uart7ClockSelection  = RCC_UART7CLKSOURCE_PCLK1;
	PeriphClkInitStruct.Uart8ClockSelection  = RCC_UART8CLKSOURCE_PCLK1;
	PeriphClkInitStruct.I2c1ClockSelection   = RCC_I2C1CLKSOURCE_PCLK1;
	PeriphClkInitStruct.I2c2ClockSelection   = RCC_I2C2CLKSOURCE_PCLK1;
	PeriphClkInitStruct.I2c3ClockSelection   = RCC_I2C3CLKSOURCE_PCLK1;
	PeriphClkInitStruct.I2c4ClockSelection   = RCC_I2C4CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
	   while(1);
	}

 // Activating the timerprescalers while the APBx prescalers are 1/2/4 will connect the TIMxCLK to HCLK which has been configured to 216MHz
 __HAL_RCC_TIMCLKPRESCALER(RCC_TIMPRES_ACTIVATED);

  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

  systemUsTicks = (HAL_RCC_GetHCLKFreq()/1000000);
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  SysTick->CTRL = 0;
  SysTick->VAL = 0; /* Load the SysTick Counter Value */
  SysTick->CTRL = (SysTick_CTRL_TICKINT_Msk   |  /* Enable SysTick exception */
                   SysTick_CTRL_ENABLE_Msk) |    /* Enable SysTick system timer */
                   SysTick_CTRL_CLKSOURCE_Msk;   /* Use processor clock source */

  /* SysTick_IRQn interrupt configuration */
  //HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
  //HAL_NVIC_EnableIRQ(SysTick_IRQn);

}

void BoardInit(void)
{
	//SCB->VTOR = ADDRESS_FLASH_START; //set vector register to firmware start
	//__enable_irq(); // enable interrupts

	HAL_Init();

	SystemClock_Config();

	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();

	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();


	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();
	__HAL_RCC_TIM5_CLK_ENABLE();
	__HAL_RCC_TIM6_CLK_ENABLE();
	__HAL_RCC_TIM7_CLK_ENABLE();
	__HAL_RCC_TIM8_CLK_ENABLE();

	__USART1_CLK_ENABLE();
	__USART3_CLK_ENABLE();
	__UART4_CLK_ENABLE();
	__USART6_CLK_ENABLE();
}

void USBInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Configure USB FS GPIOs */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure DM DP Pins */
    GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Enable USB FS Clock */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    /* Set USBFS Interrupt priority */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 0, 0);

    /* Enable USBFS Interrupt */
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

}

void USBDeInit(void)
{
    /* Peripheral clock disable */
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

    /**USB_OTG_FS GPIO Configuration
    PA9     ------> USB_OTG_FS_VBUS
    PA11     ------> USB_OTG_FS_DM
    PA12     ------> USB_OTG_FS_DP
    */
    //HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_12);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
}

uint32_t TimerPrescalerDivisor(uint32_t timer)
{
	(void)(timer);
	return(1);
}
