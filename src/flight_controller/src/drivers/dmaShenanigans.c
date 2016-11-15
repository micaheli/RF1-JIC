#include "includes.h"

// each led takes 3 colors, each color takes 8 bits
// requires 50uS (42 cycles at 1.2uS) dead time between transmissions
#define WS2812_EXTRA_CYCLES 42
#define WS2812_BUFSIZE      (8*3*WS2812_MAX_LEDS+WS2812_EXTRA_CYCLES)


ws2812Led_t WS2812_IO_colors[WS2812_MAX_LEDS];
uint32_t WS2812_IO_framedata[WS2812_BUFSIZE];
ws2812Led_t colorTable[MAX_LED_COLORS];
uint8_t lastLEDMode = 0;
uint8_t ledColor = 254;
DMA_HandleTypeDef ws2812_led;
TIM_HandleTypeDef pwmTimerBase;


ws2812Led_t red = {
	.r = 0xff,
	.g = 0x00,
	.b = 0x00,
};
ws2812Led_t yellow = {
	.r = 0xff,
	.g = 0xff,
	.b = 0x00,
};
ws2812Led_t white = {
	.r = 0xff,
	.g = 0xff,
	.b = 0xff,
};
ws2812Led_t green = {
	.r = 0x00,
	.g = 0xff,
	.b = 0x00,
};
ws2812Led_t cyan = {
	.r = 0x00,
	.g = 0xff,
	.b = 0xff,
};
ws2812Led_t blue = {
	.r = 0x00,
	.g = 0x00,
	.b = 0xff,
};
ws2812Led_t purple = {
	.r = 0xff,
	.g = 0x00,
	.b = 0xff,
};



void ws2812_led_update(uint32_t nLeds) {
    // set the output data buffer, moving bytes to bits
    // note that the timer is running at 24 MHZ, with a period of 30 cycles
    // a "1" must be high for ~700 ns, which corresponds to roughly 17 timer cycles
    // a "0" must be high for ~350 ns, which corresponds to roughly 8 timer cycles

    int32_t   bitIdx;
    uint32_t  ledIdx;
    uint32_t  bufferIdx = 0;

    // don't overflow the led or framebuffer arrays
    if (nLeds > WS2812_MAX_LEDS) {
        nLeds = WS2812_MAX_LEDS;
    }

    for (ledIdx = 0; ledIdx < nLeds; ledIdx++) {        // send green, red, blue, MSB first
        uint32_t grb = (WS2812_IO_colors[ledIdx].g << 16) | (WS2812_IO_colors[ledIdx].r << 8) | WS2812_IO_colors[ledIdx].b;
        for (bitIdx = 23; bitIdx >= 0; bitIdx--) {
            WS2812_IO_framedata[bufferIdx++] = (grb & (1 << bitIdx)) ? 17 : 8;
        }
    }

    while (bufferIdx < WS2812_BUFSIZE) {
        WS2812_IO_framedata[bufferIdx++] = 0;
    }

    // only start an update if the previous update is finished
    //134392657
    volatile uint32_t cat = __HAL_DMA_GET_COUNTER(&ws2812_led);
    if (cat == 0) {
    	__HAL_DMA_SET_COUNTER(&ws2812_led, 24 * nLeds + WS2812_EXTRA_CYCLES);
        __HAL_DMA_ENABLE(&ws2812_led);

        __HAL_TIM_SET_COUNTER(&pwmTimerBase, 0);
        __HAL_TIM_ENABLE(&pwmTimerBase);
    }
}



void SetLEDColor(uint8_t newColor)
{
	uint8_t x;
	if (newColor >= MAX_LED_COLORS)
	{
		ledColor = 0;
	}
	else
	{
		ledColor = newColor;
	}
	for (x = 0; x < mainConfig.ledConfig.ledCount; x++)
	{
		memcpy(&WS2812_IO_colors[x], &colorTable[ledColor], 3);
	}
}



void Ws2812LedInit( void )
{

	uint16_t timerPrescaler;
	uint32_t timerHz = 24000000;
	uint32_t pwmHz   = 800000;
	uint32_t timerChannel;
	TIM_TypeDef *timer;

	memcpy(&colorTable[0], &red, 3);
	memcpy(&colorTable[1], &yellow, 3);
	memcpy(&colorTable[2], &green, 3);
	memcpy(&colorTable[3], &cyan, 3);
	memcpy(&colorTable[4], &blue, 3);
	memcpy(&colorTable[5], &purple, 3);
	memcpy(&colorTable[6], &white, 3);

    // default all LEDs to white
	memset(WS2812_IO_colors, 0xff, sizeof(WS2812_IO_colors));


	/*
		board.motors[3].timer      = MOTOR4_TIM;
		board.motors[3].pin        = MOTOR4_PIN;
		board.motors[3].port       = MOTOR4_GPIO;
		board.motors[3].AF         = MOTOR4_ALTERNATE;
		board.motors[3].timChannel = MOTOR4_TIM_CH;
		board.motors[3].timCCR     = MOTOR4_TIM_CCR;
		board.motors[3].polarity   = MOTOR4_POLARITY;
	*/
    // GPIO Init
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(ports[board.motors[3].port], board.motors[3].pin);

    GPIO_InitStructure.Pin       = board.motors[3].pin;
    GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = board.motors[3].AF;

    HAL_GPIO_Init(ports[board.motors[3].port], &GPIO_InitStructure);

    HAL_GPIO_WritePin(ports[board.motors[3].port], board.motors[3].pin, GPIO_PIN_SET);

//			InitActuatorTimer(ports[board.motors[motorNum].port], board.motors[motorNum].pin, timers[board.motors[motorNum].timer], board.motors[motorNum].timChannel, board.motors[motorNum].AF, board.motors[motorNum].polarity, disarmPulseValue, pwmHz, timerHz);

    timer        = timers[board.motors[3].timer];
    timerChannel = board.motors[3].timChannel;

	if(timer == TIM1 || timer == TIM8 || timer == TIM9|| timer == TIM10|| timer == TIM11) {
		timerPrescaler = (SystemCoreClock / timerHz) - 1;
	} else {
		timerPrescaler = (SystemCoreClock / 2 / timerHz) - 1;
	}


    // Timer Init
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef      sConfigOC;
	TIM_ClockConfigTypeDef  sClockSourceConfig;

	pwmTimerBase.Instance           = timer;
	pwmTimerBase.Init.Prescaler     = timerPrescaler;
	pwmTimerBase.Init.CounterMode   = TIM_COUNTERMODE_UP;
	pwmTimerBase.Init.Period        = (timerHz/pwmHz)-1;
	pwmTimerBase.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&pwmTimerBase);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&pwmTimerBase, &sClockSourceConfig);

	HAL_TIM_PWM_Init(&pwmTimerBase);

//	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//	HAL_TIMEx_MasterConfigSynchronization(&pwmTimerBase, &sMasterConfig);

	sConfigOC.OCMode      = TIM_OCMODE_PWM1;
	sConfigOC.Pulse       = 0;
	sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH; //board.motors[3].polarity
	sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_OC_ConfigChannel(&pwmTimerBase, &sConfigOC, timerChannel);
	HAL_TIM_Base_Start(&pwmTimerBase);
	HAL_TIM_PWM_Start(&pwmTimerBase, timerChannel);


	//DMA INIT
	//DMA1 Ch3, St1
	ws2812_led.Instance                 = DMA1_Stream1;
	ws2812_led.Init.Channel             = DMA_CHANNEL_3;
	ws2812_led.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	ws2812_led.Init.PeriphInc           = DMA_PINC_DISABLE;
	ws2812_led.Init.MemInc              = DMA_MINC_ENABLE;
	ws2812_led.Init.PeriphDataAlignment = DMA_MDATAALIGN_WORD;
	ws2812_led.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	ws2812_led.Init.Mode                = DMA_NORMAL;
	ws2812_led.Init.Priority            = DMA_PRIORITY_HIGH;
//	ws2812_led.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

    if (HAL_DMA_Init(&ws2812_led) != HAL_OK) {
        ErrorHandler();
    }

    __HAL_LINKDMA(&pwmTimerBase,hdma[TIM_DMA_ID_CC1],ws2812_led);


//    //NVIC INIT
//	HAL_NVIC_SetPriority(USARTx_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(USARTx_IRQn);

//    NVIC_InitTypeDef NVIC_InitStructure;
//    /* configure DMA Channel interrupt */
//    NVIC_InitStructure.NVIC_IRQChannel = WS2812_LED_DMA_IRQ;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init( &NVIC_InitStructure );
    /* enable DMA Channel transfer complete interrupt */

    SetLEDColor(mainConfig.ledConfig.ledColor);

}
//DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler
//DMA1_Channel4_5_6_7_IRQHandler
void DMA1_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_5_6_7_IRQn 0 */

  /* USER CODE END DMA1_Channel4_5_6_7_IRQn 0 */
  HAL_DMA_IRQHandler(&ws2812_led);
  /* USER CODE BEGIN DMA1_Channel4_5_6_7_IRQn 1 */

  /* USER CODE END DMA1_Channel4_5_6_7_IRQn 1 */
}

/*
void WS2812_LED_DMA_IRQ_HANDLER(void)
{
    if (DMA_GetFlagStatus(WS2812_LED_DMA_ST, WS2812_LED_DMA_FLAG)) {
        DMA_Cmd(WS2812_LED_DMA_ST, DISABLE);

        TIM_Cmd(WS2812_LED_TIM, DISABLE);
        DMA_ClearITPendingBit(WS2812_LED_DMA_ST, WS2812_LED_DMA_FLAG);
    }
}
*/
