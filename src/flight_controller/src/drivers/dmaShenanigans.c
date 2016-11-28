#include "includes.h"

// each led takes 3 colors, each color takes 8 bits
// requires 50uS (42 cycles at 1.2uS) dead time between transmissions
#define WS2812_EXTRA_CYCLES 44
#define WS2812_BUFSIZE      (8*3*WS2812_MAX_LEDS+WS2812_EXTRA_CYCLES)

TIM_HandleTypeDef  softSerialClockTimer;
DMA_HandleTypeDef  softSerialDmaHandle;

ws2812Led_t WS2812_IO_colors[WS2812_MAX_LEDS];
uint32_t WS2812_IO_framedata[WS2812_BUFSIZE];
ws2812Led_t colorTable[MAX_LED_COLORS];
uint8_t lastLEDMode = 0;
uint8_t ledColor = 254;
DMA_HandleTypeDef  ws2812_led;
TIM_HandleTypeDef  pwmTimerBase;
TIM_OC_InitTypeDef sConfigOC;
uint32_t dmaTriggered = 0;
uint32_t onePulseWidth[16];
uint32_t zeroPulseWidth[16];

uint32_t testBuffer[88];

/* Captured Value */
__IO uint32_t            uwIC2Value = 0;
__IO uint32_t            uwIC2Value2 = 0;
/* Duty Cycle Value */
__IO uint32_t            uwDutyCycle = 0;
/* Frequency Value */
__IO uint32_t            uwFrequency = 0;





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


static void TimDmaInit(TIM_HandleTypeDef *htim, uint32_t handlerIndex, board_dma actuatorDma);
static void InitOutputForDma(motor_type actuator, uint32_t pwmHz, uint32_t timerHz);

void OutputSerialDmaByte(uint8_t *serialOutBuffer, uint32_t outputLength, motor_type actuator) {

	int32_t  bitIdx;
	uint32_t bufferIdx = 0;
	uint32_t outputIndex;
	uint32_t bitsPerFrame = 8;

    for (outputIndex = 0; outputIndex < outputLength; outputIndex++) { //Send Data MSB
    	serialOutBuffer[outputIndex] = BitReverse8(serialOutBuffer[outputIndex]); //for LSB
        WS2812_IO_framedata[bufferIdx++] = zeroPulseWidth[actuator.timerHandle];//frame start
        for (bitIdx = (bitsPerFrame - 1); bitIdx >= 0; bitIdx--) {
            WS2812_IO_framedata[bufferIdx++] = (serialOutBuffer[outputIndex] & (1 << bitIdx)) ? onePulseWidth[actuator.timerHandle] : zeroPulseWidth[actuator.timerHandle]; //load data into framedata one bit at a time
        }
        WS2812_IO_framedata[bufferIdx++] = onePulseWidth[actuator.timerHandle]; //stop bit

    }

	HAL_TIM_PWM_Stop(&pwmTimers[actuator.timerHandle], actuator.timChannel);
	HAL_TIM_PWM_ConfigChannel(&pwmTimers[actuator.timerHandle], &sConfigOC, actuator.timChannel);
	HAL_TIM_PWM_Start_DMA(&pwmTimers[actuator.timerHandle], actuator.timChannel, (uint32_t *)WS2812_IO_framedata, bufferIdx);

}


void TIM8_CC_IRQHandler(void) {
	HAL_TIM_IRQHandler(&softSerialClockTimer);
}

void DMA2_Stream1_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream1_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_1]);
}

void DMA2_Stream4_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream4_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_4]);
}

void InitOdd(motor_type actuator) {

	GPIO_InitTypeDef        GPIO_InitStruct;
	TIM_TypeDef            *timer;
	TIM_IC_InitTypeDef      sConfig;
	TIM_SlaveConfigTypeDef  sSlaveConfig;
	TIM_ClockConfigTypeDef  sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	uint16_t timerPrescaler;
	uint32_t timerHz;
	uint32_t pwmHz;


	bzero(testBuffer, sizeof(testBuffer));
	//setup internal timer clock
	timerHz   = 48000000; //48 MHz frequency is perfectly fine for 19200 Baud.
	pwmHz     = 19200;   //baudrate
	//2500

    //Timer Init
    //timer = timers[actuator.timer];
    timer = TIM8;
    __TIM8_CLK_ENABLE();

	if(timer == TIM1 || timer == TIM8 || timer == TIM9|| timer == TIM10|| timer == TIM11) {
		timerPrescaler = (SystemCoreClock / timerHz) - 1;
	} else {
		timerPrescaler = (SystemCoreClock / 2 / timerHz) - 1;
	}

	//Use timer8 channel three for oc to trigger interrupt for DMA
	HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 5, 1);
	HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);

	HAL_TIM_Base_DeInit(&softSerialClockTimer);
	softSerialClockTimer.Instance           = timer;
	softSerialClockTimer.Init.Period     	= (timerHz/pwmHz)-1;
	softSerialClockTimer.Init.Prescaler   	= timerPrescaler;
	softSerialClockTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	softSerialClockTimer.Init.CounterMode 	= TIM_COUNTERMODE_UP;

	sConfigOC.OCMode      = TIM_OCMODE_TIMING;
	sConfigOC.Pulse       = 350; //trigger 250 counts in
	sConfigOC.OCPolarity  = TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_OC_ConfigChannel(&softSerialClockTimer, &sConfigOC, TIM_CHANNEL_3);
	HAL_TIM_PWM_Init(&softSerialClockTimer);
	return;
//	if (HAL_TIM_Base_Init(&softSerialClockTimer) != HAL_OK) {
//		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
//	}

//#define TIM_DMA_Update                     ((uint16_t)0x0100)
//#define TIM_DMA_CC1                        ((uint16_t)0x0200)
//#define TIM_DMA_CC2                        ((uint16_t)0x0400)
//#define TIM_DMA_CC3                        ((uint16_t)0x0800)
	//timer->DIER |= ((uint16_t)0x0100); //TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE );
	//if (HAL_TIM_Base_Start_IT(&softSerialClockTimer) != HAL_OK) {
	//	ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	//}

	softSerialDmaHandle.Instance                 = DMA2_Stream4; //or DMA2_Stream7
	softSerialDmaHandle.Init.Channel             = DMA_CHANNEL_7;
	softSerialDmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY; //DMA_PERIPH_TO_MEMORY //DMA_MEMORY_TO_MEMORY
	softSerialDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
	softSerialDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
	softSerialDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	softSerialDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	softSerialDmaHandle.Init.Mode                = DMA_NORMAL;
	softSerialDmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;
	softSerialDmaHandle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	softSerialDmaHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	softSerialDmaHandle.Init.MemBurst     	     = DMA_MBURST_SINGLE;
	softSerialDmaHandle.Init.PeriphBurst         = DMA_PBURST_SINGLE;

	//softSerialDmaHandle.XferCpltCallback  = dmaComplete;
	//softSerialDmaHandle.XferErrorCallback = dmaError;

    /* Configure DMA Stream destination address */
	softSerialDmaHandle.Instance->PAR = (uint32_t)&ports[actuator.port]->IDR; //input is GPIO Input Data Register //(uint32_t)&GPIOC->IDR;

    /* Configure DMA Stream source address */
	softSerialDmaHandle.Instance->M0AR = (uint32_t)&testBuffer; //output is the buffer

	HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
	if (HAL_DMA_Init(&softSerialDmaHandle) != HAL_OK)
    {
      ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
    }
	__HAL_LINKDMA(&softSerialClockTimer,hdma[TIM_DMA_ID_CC3],softSerialDmaHandle);
	HAL_TIM_OC_Start(&softSerialClockTimer, TIM_CHANNEL_3);


	if (HAL_DMA_Start_IT(&softSerialDmaHandle, (uint32_t)&testBuffer, (uint32_t)&ports[actuator.port]->IDR, 2) != HAL_OK)
    {
      ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
    }

	return;


    //if (HAL_TIM_IC_Start_DMA(&softSerialClockTimer, TIM_CHANNEL_1, (uint32_t *)testBuffer, 88) != HAL_OK)
    //{
    //  ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
    //}

    /* Configure DMA Stream destination address */
	softSerialDmaHandle.Instance->PAR = (uint32_t)&ports[actuator.port]->IDR; //input is GPIO Input Data Register //(uint32_t)&GPIOC->IDR;

    /* Configure DMA Stream source address */
	softSerialDmaHandle.Instance->M0AR = (uint32_t)&testBuffer; //output is the buffer

	return;

    if (HAL_DMA_Init(&softSerialDmaHandle) != HAL_OK)
    {
    	ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
    }

	if (HAL_TIM_Base_Start_DMA(&softSerialClockTimer, (uint32_t *)testBuffer, sizeof(testBuffer)) != HAL_OK) {
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}
    HAL_GPIO_DeInit(ports[actuator.port], actuator.pin);

    InitializeGpio(ports[actuator.port], actuator.pin, 0);






  //  softSerialDmaHandle

//
//HAL_TIM_OC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length)










    //GPIO_InitStruct.Pin       = actuator.pin;
    //GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP; //GPIO_MODE_AF_PP
    //GPIO_InitStruct.Pull      = GPIO_PULLUP; //GPIO_PULLUP //pull up for non inverted, pull down for inverted
    //GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    //GPIO_InitStruct.Alternate = actuator.AF;



	//TIM8_UP_TIM13_IRQn
    // GPIO Init
	/*
    HAL_GPIO_DeInit(ports[actuator.port], actuator.pin);

    GPIO_InitStruct.Pin       = actuator.pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP; //GPIO_MODE_AF_PP
    GPIO_InitStruct.Pull      = GPIO_PULLUP; //GPIO_PULLUP //pull up for non inverted, pull down for inverted
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = actuator.AF;

    HAL_GPIO_Init(ports[actuator.port], &GPIO_InitStruct);
*/
    /*##-2- Configure the NVIC for TIMx #########################################*/
    HAL_TIM_Base_DeInit(&pwmTimers[actuator.timerHandle]);
	pwmTimers[actuator.timerHandle].Instance           	= timer;
	pwmTimers[actuator.timerHandle].Init.Period     	= (timerHz/pwmHz)-1;
	pwmTimers[actuator.timerHandle].Init.Prescaler   	= timerPrescaler;
	pwmTimers[actuator.timerHandle].Init.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;
	pwmTimers[actuator.timerHandle].Init.CounterMode 	= TIM_COUNTERMODE_UP;
    if (HAL_TIM_Base_Init(&pwmTimers[actuator.timerHandle]) != HAL_OK) {
        ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
    }

    if (HAL_TIM_Base_Start_IT(&pwmTimers[actuator.timerHandle]) != HAL_OK) {
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}
//    if (HAL_TIM_Base_Start_DMA(&pwmTimers[actuator.timerHandle]) != HAL_OK) {
//		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
//	}

    return;
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&pwmTimers[actuator.timerHandle], &sClockSourceConfig) != HAL_OK) {
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&pwmTimers[actuator.timerHandle], &sMasterConfig) != HAL_OK) {
    	ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}

    HAL_NVIC_SetPriority(actuator.timerIRQn, 5, 1);
	HAL_NVIC_EnableIRQ(actuator.timerIRQn);


	pwmTimers[actuator.timerHandle].State = HAL_TIM_STATE_RESET;
	__TIM3_CLK_ENABLE();
	HAL_TIM_Base_Init(&pwmTimers[actuator.timerHandle]);
	HAL_TIM_IC_Init(&pwmTimers[actuator.timerHandle]);


















}
void InitDmaInputOnMotors(motor_type actuator) {

	GPIO_InitTypeDef       GPIO_InitStruct;
	TIM_TypeDef           *timer;
	TIM_IC_InitTypeDef     sConfig;
	TIM_SlaveConfigTypeDef sSlaveConfig;

	timer = timers[actuator.timer];

    // GPIO Init
    HAL_GPIO_DeInit(ports[actuator.port], actuator.pin);

    GPIO_InitStruct.Pin       = actuator.pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP; //GPIO_MODE_AF_PP
    GPIO_InitStruct.Pull      = GPIO_PULLUP; //GPIO_PULLUP //pull up for non inverted, pull down for inverted
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = actuator.AF;

    HAL_GPIO_Init(ports[actuator.port], &GPIO_InitStruct);

    /*##-2- Configure the NVIC for TIMx #########################################*/
    //HAL_NVIC_SetPriority(actuator.timerIRQn, 5, 0);
    //HAL_NVIC_EnableIRQ(actuator.timerIRQn);

    HAL_TIM_Base_DeInit(&pwmTimers[actuator.timerHandle]);
	pwmTimers[actuator.timerHandle].Instance           	= timer;
	pwmTimers[actuator.timerHandle].Init.Period     	= 0xFFFF;
	pwmTimers[actuator.timerHandle].Init.Prescaler   	= 191; //1MHz
	pwmTimers[actuator.timerHandle].Init.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;
	pwmTimers[actuator.timerHandle].Init.CounterMode 	= TIM_COUNTERMODE_UP;

	pwmTimers[actuator.timerHandle].State               = HAL_TIM_STATE_RESET;
	__TIM3_CLK_ENABLE();
	HAL_TIM_Base_Init(&pwmTimers[actuator.timerHandle]);
	HAL_TIM_IC_Init(&pwmTimers[actuator.timerHandle]);

	// Configure the Input Capture channels
	sConfig.ICPrescaler = TIM_ICPSC_DIV1;
	sConfig.ICFilter    = 0x3;
	sConfig.ICPolarity  = TIM_ICPOLARITY_FALLING;
	//TIM_ICPOLARITY_FALLING
	//TIM_ICPOLARITY_RISING
	//TIM_ICPOLARITY_BOTHEDGE
	sConfig.ICSelection = TIM_ICSELECTION_INDIRECTTI;
	if(HAL_TIM_IC_ConfigChannel(&pwmTimers[actuator.timerHandle], &sConfig, actuator.timChannelC) != HAL_OK)
	{
		/* Configuration Error */
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}

	sConfig.ICPolarity = TIM_ICPOLARITY_RISING;
	sConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
	if(HAL_TIM_IC_ConfigChannel(&pwmTimers[actuator.timerHandle], &sConfig, actuator.timChannel) != HAL_OK)
	{
		/* Configuration Error */
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}
	/*##-3

	/* Select the slave Mode: Reset Mode */
	sSlaveConfig.SlaveMode     = TIM_SLAVEMODE_RESET;
	sSlaveConfig.InputTrigger  = TIM_TS_TI2FP2;
	if(HAL_TIM_SlaveConfigSynchronization(&pwmTimers[actuator.timerHandle], &sSlaveConfig) != HAL_OK)
	{
		/* Configuration Error */
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}

	HAL_NVIC_SetPriority(actuator.timerIRQn,5,1);
	HAL_NVIC_EnableIRQ(actuator.timerIRQn);
	/*##-4- Start the Input Capture in interrupt mode ##########################*/
	if(HAL_TIM_IC_Start_IT(&pwmTimers[actuator.timerHandle], actuator.timChannel) != HAL_OK)
	{
		/* Starting Error */
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}

	/*##-5- Start the Input Capture in interrupt mode ##########################*/
	if(HAL_TIM_IC_Start_IT(&pwmTimers[actuator.timerHandle], actuator.timChannelC) != HAL_OK)
	{
		/* Starting Error */
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}


	return;
	if(HAL_TIM_IC_Init(&pwmTimers[actuator.timerHandle]) != HAL_OK)
	{
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}

	/*##-2- Configure the Input Capture channels ###############################*/
	/* Common configuration */
	sConfig.ICPrescaler = TIM_ICPSC_DIV1;
	sConfig.ICFilter = 0x0; //0x0 to 0xF

	/* Configure the Input Capture of channel of actuator */
	sConfig.ICPolarity  = TIM_ICPOLARITY_FALLING;     //trigger on falling and rising edge
	//TIM_ICPOLARITY_BOTHEDGE
	//TIM_ICPOLARITY_RISING
	//TIM_ICPOLARITY_FALLING
	sConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
	//sConfig.ICFilter    = 0xF;  //0x0 - 0xF
	//TIM_ICSELECTION_TRC
	//TIM_ICSELECTION_INDIRECTTI
	//TIM_ICSELECTION_DIRECTTI
	if(HAL_TIM_IC_ConfigChannel(&pwmTimers[actuator.timerHandle], &sConfig, actuator.timChannel) != HAL_OK)
	{
		/* Configuration Error */
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}

	/*##-3- Configure the slave mode ###########################################*/
	/* Select the slave Mode: Reset Mode */
	sSlaveConfig.SlaveMode     = TIM_SLAVEMODE_TRIGGER;
	//TIM_SLAVEMODE_TRIGGER
	//TIM_SLAVEMODE_RESET
	//TIM_SLAVEMODE_GATED
	//TIM_SLAVEMODE_DISABLE
	//TIM_SLAVEMODE_EXTERNAL1
	sSlaveConfig.InputTrigger  = TIM_TS_TI2FP2;
	//sSlaveConfig.TriggerPolarity
	if(HAL_TIM_SlaveConfigSynchronization(&pwmTimers[actuator.timerHandle], &sSlaveConfig) != HAL_OK)
	{
		/* Configuration Error */
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}

	/*##-4- Start the Input Capture in interrupt mode ##########################*/
	if(HAL_TIM_IC_Start_IT(&pwmTimers[actuator.timerHandle], actuator.timChannel) != HAL_OK)
	{
		/* Starting Error */
		ErrorHandler(TIMER_INPUT_INIT_FAILIURE);
	}

	HAL_TIM_Base_Init(&pwmTimers[actuator.timerHandle]);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	return;
	//actuator.timChannel
	if (htim->Channel == board.motors[0].activeTim)
	{
		/* Get the Input Capture value */
		uwIC2Value = HAL_TIM_ReadCapturedValue(htim, board.motors[0].timChannel);

    if (uwIC2Value != 0)
    {
      /* Duty cycle computation */
    	uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, board.motors[0].timChannelC);
    	uint32_t diff = uwIC2Value - uwIC2Value2;
    	uwDutyCycle = ((HAL_TIM_ReadCapturedValue(htim, board.motors[0].timChannelC)) * 100) / uwIC2Value;

      /* uwFrequency computation
      TIM4 counter clock = (RCC_Clocks.HCLK_Frequency)/2 */
      uwFrequency = (HAL_RCC_GetHCLKFreq())/2 / uwIC2Value;

    }
    else
    {
      uwDutyCycle = 0;
      uwFrequency = 0;
    }
  }
}

void InitDmaOutputOnMotors(uint32_t usedFor) {

	uint32_t timerHz;
	uint32_t pwmHz;
	uint32_t onePulse;
	uint32_t zeroPulse;

	if (usedFor == DMA_OUTPUT_WS2812_LEDS) {
		timerHz   = 24000000;
		pwmHz     = 800000;
		onePulse  = 17;
		zeroPulse = 8;
	} else if (usedFor == DMA_OUTPUT_ESC_1WIRE) {
		//17 / 24 = 0.708 us
	    // note that the timer is running at 24 MHZ, with a period of 30 cycles
	    // a "1" must be high for ~700 ns, which corresponds to roughly 17 timer cycles
	    // a "0" must be high for ~350 ns, which corresponds to roughly 8 timer cycles
		//At 09600, bit time is 104.166666666666 microseconds. 104166 ns
		//At 19200, bit time is 52.083333333333 microseconds. 52083 ns
		//0.0416666666666667 us per tick with timerHz at 24000000 and pwmHz at 800000 with
		//1 MHz timer is 1us per tick. :)
		//52 ticks is a 1 and 0 ticks is a 0 for serial

		//19200KBAUD
		timerHz   = 48000000; //48 MHz frequency is perfectly fine for 19200 Baud.
		pwmHz     = 19200;   //baudrate
		onePulse  = 1;
		zeroPulse = 2490; //2500 max, but we can't fill the CCR

		//100KBAUD
		//timerHz   = 48000000; //48 MHz frequency is perfectly fine for 19200 Baud.
		//pwmHz     = 100000;   //baudrate
		//onePulse  = 1;
		//zeroPulse = 470; //480 max, but we can't fill the CCR

		//200KBAUD
		//timerHz   = 48000000; //48 MHz frequency is perfectly fine for 19200 Baud.
		//pwmHz     = 200000;   //baudrate
		//onePulse  = 1;
		//zeroPulse = 230; //240 max, but we can't fill the CCR
	} else if (usedFor == DMA_OUTPUT_DSHOT) {
		timerHz   = 24000000;
		pwmHz     = 800000;
		onePulse  = 17; //need to find pulse width for dshot.
		zeroPulse = 8;
	}

	for (uint32_t motorNum = 0; motorNum < MAX_MOTOR_NUMBER; motorNum++) {
		if ( (board.motors[motorNum].enabled) && (board.dmasMotor[board.motors[motorNum].Dma].enabled) ) {
			onePulseWidth[board.motors[motorNum].timerHandle]  = onePulse;
			zeroPulseWidth[board.motors[motorNum].timerHandle] = zeroPulse;
			InitOutputForDma(board.motors[motorNum], pwmHz, timerHz);
		}
	}

}

static void InitOutputForDma(motor_type actuator, uint32_t pwmHz, uint32_t timerHz) {

    GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t timerPrescaler;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_ClockConfigTypeDef  sClockSourceConfig;
	TIM_TypeDef *timer;

    // GPIO Init
    HAL_GPIO_DeInit(ports[actuator.port], actuator.pin);

    GPIO_InitStructure.Pin       = actuator.pin;
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP; //GPIO_MODE_AF_PP
    GPIO_InitStructure.Pull      = GPIO_PULLUP; //GPIO_PULLUP //pull up for non inverted, pull down for inverted
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = actuator.AF;

    HAL_GPIO_Init(ports[actuator.port], &GPIO_InitStructure);

    if (actuator.polarity == TIM_OCPOLARITY_LOW)
    {
    	inlineDigitalLo(ports[actuator.port], actuator.pin);
    }
    else
    {
    	inlineDigitalHi(ports[actuator.port], actuator.pin);
    }




    //Timer Init
    timer        = timers[actuator.timer];

	if(timer == TIM1 || timer == TIM8 || timer == TIM9|| timer == TIM10|| timer == TIM11) {
		timerPrescaler = (SystemCoreClock / timerHz) - 1;
	} else {
		timerPrescaler = (SystemCoreClock / 2 / timerHz) - 1;
	}

	pwmTimers[actuator.timerHandle].Instance           	= timer;
	pwmTimers[actuator.timerHandle].Init.Prescaler     	= timerPrescaler;
	pwmTimers[actuator.timerHandle].Init.CounterMode   	= TIM_COUNTERMODE_UP;
	pwmTimers[actuator.timerHandle].Init.Period        	= (timerHz/pwmHz)-1;
	pwmTimers[actuator.timerHandle].Init.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&pwmTimers[actuator.timerHandle]);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&pwmTimers[actuator.timerHandle], &sClockSourceConfig);

	HAL_TIM_PWM_Init(&pwmTimers[actuator.timerHandle]);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&pwmTimers[actuator.timerHandle], &sMasterConfig);

	sConfigOC.OCMode      = TIM_OCMODE_PWM1;
	sConfigOC.Pulse       = 0;
	sConfigOC.OCPolarity  = actuator.polarity;
	//sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_PWM_ConfigChannel(&pwmTimers[actuator.timerHandle], &sConfigOC, actuator.timChannel);


	//DMA INIT
	TimDmaInit(&pwmTimers[actuator.timerHandle], actuator.CcDmaHandle, board.dmasMotor[actuator.Dma]);

	bzero(WS2812_IO_framedata,sizeof(WS2812_IO_framedata));
	WS2812_IO_framedata[0]=0;
	WS2812_IO_framedata[1]=0;
	WS2812_IO_framedata[2]=0;
	WS2812_IO_framedata[3]=0;
	WS2812_IO_framedata[4]=0;


	HAL_TIM_PWM_Start_DMA(&pwmTimers[actuator.timerHandle], actuator.timChannel, (uint32_t *)WS2812_IO_framedata, 5);
    HAL_NVIC_SetPriority(actuator.timerIRQn, 3, 0);
    HAL_NVIC_EnableIRQ(actuator.timerIRQn);

}

static void TimDmaInit(TIM_HandleTypeDef *htim, uint32_t handlerIndex, board_dma actuatorDma) {

	if (htim->hdma[handlerIndex] != 0)
	{
		HAL_DMA_DeInit(htim->hdma[handlerIndex]);
	}

	dmaHandles[actuatorDma.dmaHandle].Instance                 = dmaStream[actuatorDma.dmaStream];
	dmaHandles[actuatorDma.dmaHandle].Init.Channel             = actuatorDma.dmaChannel;
	dmaHandles[actuatorDma.dmaHandle].Init.Direction           = actuatorDma.dmaDirection;
	dmaHandles[actuatorDma.dmaHandle].Init.PeriphInc           = actuatorDma.dmaPeriphInc;
	dmaHandles[actuatorDma.dmaHandle].Init.MemInc              = actuatorDma.dmaMemInc;
	dmaHandles[actuatorDma.dmaHandle].Init.PeriphDataAlignment = actuatorDma.dmaPeriphAlignment;
	dmaHandles[actuatorDma.dmaHandle].Init.MemDataAlignment    = actuatorDma.dmaMemAlignment;
	dmaHandles[actuatorDma.dmaHandle].Init.Mode                = actuatorDma.dmaMode;
	dmaHandles[actuatorDma.dmaHandle].Init.Priority            = actuatorDma.dmaPriority;
	dmaHandles[actuatorDma.dmaHandle].Init.FIFOMode            = actuatorDma.fifoMode;
	dmaHandles[actuatorDma.dmaHandle].Init.FIFOThreshold       = actuatorDma.fifoThreshold;
	dmaHandles[actuatorDma.dmaHandle].Init.MemBurst     	   = actuatorDma.MemBurst;
	dmaHandles[actuatorDma.dmaHandle].Init.PeriphBurst         = actuatorDma.PeriphBurst;

	/* Associate the initialized DMA handle to the TIM handle */
	__HAL_LINKDMA(htim, hdma[handlerIndex], dmaHandles[actuatorDma.dmaHandle]);

	HAL_NVIC_SetPriority(actuatorDma.dmaIRQn, 3, 3);
	HAL_NVIC_EnableIRQ(actuatorDma.dmaIRQn);

	if (HAL_DMA_Init(&dmaHandles[actuatorDma.dmaHandle]) != HAL_OK) {
		ErrorHandler(WS2812_LED_INIT_FAILIURE);
	}

}












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
        uint32_t grb = (WS2812_IO_colors[ledIdx].g << 16) | (WS2812_IO_colors[ledIdx].r << 8) | WS2812_IO_colors[ledIdx].b;
        //uint32_t grb = (0xAA << 16) | (0xAA << 8) | 0xAA;
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


		HAL_TIM_PWM_Stop(&pwmTimerBase, board.motors[3].timChannel);
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

void Ws2812LedInit( void )
{
//todo: this function will no longer work.
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
	sConfigOC.Pulse       = 54;
	sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH; //board.motors[3].polarity
	//sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

    SetLEDColor(mainConfig.ledConfig.ledColor);

	HAL_TIM_PWM_ConfigChannel(&pwmTimerBase, &sConfigOC, timerChannel);


	//DMA INIT
	//TimDmaInit(&pwmTimerBase, TIM_DMA_ID_CC3);

	bzero(WS2812_IO_framedata,sizeof(WS2812_IO_framedata));
	HAL_TIM_PWM_Start_DMA(&pwmTimerBase, board.motors[3].timChannel, (uint32_t *)WS2812_IO_framedata, 1);

    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

}



void TIM2_IRQHandler(void)
{
  //HAL_TIM_IRQHandler(&TimHandle);
}

void TIM3_IRQHandler(void)
{
	volatile uint32_t cat = Micros();

  HAL_TIM_IRQHandler(&pwmTimers[board.motors[0].timerHandle]);
}

void TIM8_UP_TIM13_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&softSerialClockTimer);
//	if (inlineIsPinStatusHi(ports[board.motors[0].port], board.motors[0].pin))
//		inlineDigitalHi(ports[board.motors[0].port], board.motors[0].pin);
//	else
//		inlineDigitalLo(ports[board.motors[0].port], board.motors[0].pin);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM8)
    {
    	//HAL_TIM_IRQHandler(&softSerialClockTimer);
    	//if (inlineIsPinStatusHi(ports[board.motors[0].port], board.motors[0].pin))
    	//	inlineDigitalHi(ports[board.motors[0].port], board.motors[0].pin);
    	//else
    	//	inlineDigitalLo(ports[board.motors[0].port], board.motors[0].pin);
//        HAL_TIM_Base_Stop_IT(htim);
        //HAL_GPIO_WritePin(GPIOG,GPIO_PIN_2,GPIO_PIN_RESET);

    }

}














//TODO: add back support for DMA
/*
void DMA1_Stream1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&ws2812_led);
	HAL_TIM_PWM_Stop(&pwmTimerBase, board.motors[3].timChannel);
	dmaTriggered = 1;
}
*/
