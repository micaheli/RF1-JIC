#include "includes.h"

// each led takes 3 colors, each color takes 8 bits
// requires 50uS (42 cycles at 1.2uS) dead time between transmissions
#define WS2812_EXTRA_CYCLES 44
#define WS2812_BUFSIZE      (8*3*WS2812_MAX_LEDS+WS2812_EXTRA_CYCLES)


ws2812Led_t WS2812_IO_colors[WS2812_MAX_LEDS];
uint32_t WS2812_IO_framedata[WS2812_BUFSIZE];
ws2812Led_t colorTable[MAX_LED_COLORS];
uint8_t lastLEDMode = 0;
uint8_t ledColor = 254;
DMA_HandleTypeDef  ws2812_led;
TIM_HandleTypeDef  pwmTimerBase;
TIM_OC_InitTypeDef sConfigOC;
uint32_t dmaTriggered = 0;

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

	//HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
	//HAL_TIM_PWM_Start_DMA(&TimHandle, TIM_CHANNEL_1, (uint32_t *) LEDbuffer, LED_BUFFER_SIZE);
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

    WS2812_IO_framedata[0] = 0;
    WS2812_IO_framedata[1] = 0;
    WS2812_IO_framedata[2] = 0;
    WS2812_IO_framedata[3] = 0;
    for (ledIdx = 0; ledIdx < nLeds; ledIdx++) {        // send green, red, blue, MSB first
        //uint32_t grb = (WS2812_IO_colors[ledIdx].g << 16) | (WS2812_IO_colors[ledIdx].r << 8) | WS2812_IO_colors[ledIdx].b;
        uint32_t grb = (0xAA << 16) | (0xAA << 8) | 0xAA;
        for (bitIdx = 23; bitIdx >= 0; bitIdx--) {
            WS2812_IO_framedata[4+(bufferIdx++)] = (grb & (1 << bitIdx)) ? 17 : 8;
        }
    }

    while (bufferIdx < (WS2812_BUFSIZE - 4)) {
        WS2812_IO_framedata[(bufferIdx++)+4] = 0;
    }

    // only start an update if the previous update is finished
    //134392657

    if (dmaTriggered) {
    	//HAL_TIM_PWM_ConfigChannel(&pwmTimerBase, &sConfigOC, board.motors[3].timChannel);
    	//HAL_TIM_PWM_Start_DMA(&pwmTimerBase, board.motors[3].timChannel, (uint32_t *) WS2812_IO_framedata, WS2812_BUFSIZE);

    	//timer = timers[board.motors[3].timer];
    	//timer->EGR |= TIM_EGR_UG;
    	//__HAL_DMA_SET_COUNTER(&ws2812_led, 0);
    	//__HAL_TIM_SET_COUNTER(&pwmTimerBase, 0);

    	HAL_TIM_PWM_ConfigChannel(&pwmTimerBase, &sConfigOC, board.motors[3].timChannel);
    	HAL_TIM_PWM_Start_DMA(&pwmTimerBase, board.motors[3].timChannel, (uint32_t *)WS2812_IO_framedata, WS2812_BUFSIZE);

    	dmaTriggered = 0;


//    	__HAL_DMA_SET_COUNTER(pwmTimerBase.hdma[TIM_DMA_ID_CC3], 24 * nLeds + WS2812_EXTRA_CYCLES);
//    	uint32_t cat = __HAL_DMA_GET_COUNTER(pwmTimerBase.hdma[TIM_DMA_ID_CC3]);
//        __HAL_DMA_ENABLE(pwmTimerBase.hdma[TIM_DMA_ID_CC3]);

        //__HAL_TIM_SET_COUNTER(&pwmTimerBase, 0);
        //__HAL_TIM_ENABLE(&pwmTimerBase);
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

static void TimDmaInit(TIM_HandleTypeDef *htim, uint32_t handlerIndex) {

	if (htim->hdma[handlerIndex] != 0)
	{
		HAL_DMA_DeInit(htim->hdma[handlerIndex]);
	}

	//DMA1 Ch3, St1
	ws2812_led.Instance                 = DMA1_Stream1;
	ws2812_led.Init.Channel             = DMA_CHANNEL_3;
	ws2812_led.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	ws2812_led.Init.PeriphInc           = DMA_PINC_DISABLE;
	ws2812_led.Init.MemInc              = DMA_MINC_ENABLE;
	ws2812_led.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	ws2812_led.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	ws2812_led.Init.Mode                = DMA_NORMAL;
	ws2812_led.Init.Priority            = DMA_PRIORITY_HIGH;
	ws2812_led.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	ws2812_led.Init.MemBurst     	    = DMA_MBURST_SINGLE;
	ws2812_led.Init.PeriphBurst         = DMA_PBURST_SINGLE;

	/* Associate the initialized DMA handle to the TIM handle */
	__HAL_LINKDMA(htim, hdma[handlerIndex], ws2812_led);

	//HAL_DMA_Start( &ws2812_led,  (uint32_t)WS2812_IO_framedata, ccr[board.motors[3].timCCR], WS2812_BUFSIZE);
	HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 3, 3);
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

	//__HAL_DMA_ENABLE_IT(&ws2812_led, DMA_IT_TC);

	//HAL_DMA_Init(htim->hdma[handlerIndex]);
	if (HAL_DMA_Init(&ws2812_led) != HAL_OK) {
		ErrorHandler();
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

    // GPIO Init
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(ports[board.motors[3].port], board.motors[3].pin);

    GPIO_InitStructure.Pin       = board.motors[3].pin;
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP; //GPIO_MODE_AF_PP
    GPIO_InitStructure.Pull      = GPIO_PULLDOWN; //GPIO_PULLUP
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = board.motors[3].AF;

    HAL_GPIO_Init(ports[board.motors[3].port], &GPIO_InitStructure);

    HAL_GPIO_WritePin(ports[board.motors[3].port], board.motors[3].pin, GPIO_PIN_RESET);

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
	TIM_ClockConfigTypeDef  sClockSourceConfig;

	pwmTimerBase.Instance           	= timer;
	pwmTimerBase.Init.Prescaler     	= timerPrescaler;
	pwmTimerBase.Init.CounterMode   	= TIM_COUNTERMODE_UP;
	pwmTimerBase.Init.Period        	= (timerHz/pwmHz)-1;
	pwmTimerBase.Init.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&pwmTimerBase);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&pwmTimerBase, &sClockSourceConfig);

	HAL_TIM_PWM_Init(&pwmTimerBase);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&pwmTimerBase, &sMasterConfig);

	sConfigOC.OCMode      = TIM_OCMODE_PWM1;
	sConfigOC.Pulse       = 0;
	sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH; //board.motors[3].polarity
	//sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

    SetLEDColor(mainConfig.ledConfig.ledColor);

	HAL_TIM_PWM_ConfigChannel(&pwmTimerBase, &sConfigOC, timerChannel);
	//HAL_TIM_PWM_Start_DMA(&pwmTimerBase, timerChannel, (uint32_t *) WS2812_IO_framedata, WS2812_BUFSIZE);
	//HAL_TIM_OC_Start_DMA(&pwmTimerBase, timerChannel, (uint32_t *) WS2812_IO_framedata, WS2812_BUFSIZE);
//	HAL_TIM_PWM_Start_IT(&pwmTimerBase, timerChannel);
//	uint32_t pData[2]={0xFF,0x00};
	//HAL_TIM_OC_Start_DMA(&pwmTimerBase, timerChannel ,(uint32_t *)pData,2);



	//DMA INIT
	TimDmaInit(&pwmTimerBase, TIM_DMA_ID_CC3);

	HAL_TIM_PWM_Start_DMA(&pwmTimerBase, board.motors[3].timChannel, (uint32_t *)WS2812_IO_framedata, WS2812_BUFSIZE);
	//uint16_t pData[10]={4,2,4,2,4,2,4,2,4,2};
	//HAL_TIM_PWM_Start_DMA(&pwmTimerBase, timerChannel, (uint32_t *)pData, 10);

//	HAL_TIM_OC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
//	HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
//	//HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
//	//HAL_TIM_Encoder_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData1, uint32_t *pData2, uint16_t Length);

    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);


//	__HAL_DMA_SET_COUNTER(pwmTimerBase.hdma[TIM_DMA_ID_CC3], 24 * mainConfig.ledConfig.ledCount + WS2812_EXTRA_CYCLES);
//    __HAL_DMA_ENABLE(pwmTimerBase.hdma[TIM_DMA_ID_CC3]);

//    __HAL_TIM_SET_COUNTER(&pwmTimerBase, 0);
//    __HAL_TIM_ENABLE(&pwmTimerBase);
}


//void DMA1_Stream0_IRQHandler(void) {
//}
extern DMA_HandleTypeDef dma_spi1_rx;
extern DMA_HandleTypeDef dma_spi1_tx;

void DMA1_Stream1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&ws2812_led);
	dmaTriggered = 1;
}
//void DMA1_Stream2_IRQHandler(void) {
//	fish();
//}
//void DMA1_Stream3_IRQHandler(void) {
//}
//void DMA1_Stream4_IRQHandler(void) {
//	fish();
//}
//void DMA1_Stream5_IRQHandler(void) {
//	fish();
//}
//void DMA1_Stream6_IRQHandler(void) {
//}

//void DMA1_Stream1_IRQHandler(void)
//{
  /* USER CODE BEGIN DMA1_Channel4_5_6_7_IRQn 0 */

  /* USER CODE END DMA1_Channel4_5_6_7_IRQn 0 */
//  HAL_DMA_IRQHandler(&ws2812_led);
  /* USER CODE BEGIN DMA1_Channel4_5_6_7_IRQn 1 */

  /* USER CODE END DMA1_Channel4_5_6_7_IRQn 1 */
//}

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
