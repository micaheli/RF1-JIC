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
uint32_t onePulseWidth[16];
uint32_t zeroPulseWidth[16];

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

void OutputSerialDma(uint8_t *serialOutBuffer, uint32_t outputLength, motor_type actuator) {

	int32_t  bitIdx;
	uint32_t bufferIdx = 0;
	uint32_t outputIndex;
	uint32_t bitsPerFrame = 10;

	//frame, data bits * 8, no parity, 1 stop bit; between 10 and 12 bits per frame. We store that into a buffer

    for (outputIndex = 0; outputIndex < outputLength; outputIndex++) { //Send Data MSB
        WS2812_IO_framedata[bufferIdx++] = zeroPulseWidth[actuator.timerHandle];//frame start
        for (bitIdx = bitsPerFrame; bitIdx >= 0; bitIdx--) {
            WS2812_IO_framedata[bufferIdx++] = (serialOutBuffer[outputIndex] & (1 << bitIdx)) ? onePulseWidth[actuator.timerHandle] : zeroPulseWidth[actuator.timerHandle]; //load data into framedata one bit at a time
        }
        WS2812_IO_framedata[bufferIdx++] = onePulseWidth[actuator.timerHandle]; //stop bit
        WS2812_IO_framedata[bufferIdx++] = onePulseWidth[actuator.timerHandle]; //idle
    }

	HAL_TIM_PWM_Stop(&pwmTimers[actuator.timerHandle], actuator.timChannel);
	HAL_TIM_PWM_ConfigChannel(&pwmTimers[actuator.timerHandle], &sConfigOC, actuator.timChannel);
	HAL_TIM_PWM_Start_DMA(&pwmTimers[actuator.timerHandle], actuator.timChannel, (uint32_t *)WS2812_IO_framedata, bufferIdx);

	DelayMs(10);
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
		timerHz   = 1000000; //1 MHz frequency is perfectly fine for 19200 Baud, but not perfect, 52.08 cycles before overrun. We can reset the timer after XX cycles though.
		pwmHz     = 19200;   //baudrate
		onePulse  = 52;
		zeroPulse = 0;
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
	uint32_t polarityForDma;


    // GPIO Init
    HAL_GPIO_DeInit(ports[actuator.port], actuator.pin);

    GPIO_InitStructure.Pin       = actuator.pin;
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP; //GPIO_MODE_AF_PP
    GPIO_InitStructure.Pull      = GPIO_PULLDOWN; //GPIO_PULLUP
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = actuator.AF;

    HAL_GPIO_Init(ports[actuator.port], &GPIO_InitStructure);

    if (actuator.polarity == TIM_OCPOLARITY_LOW)
    {
    	polarityForDma = TIM_OCPOLARITY_HIGH;
    	inlineDigitalLo(ports[actuator.port], actuator.pin);
    }
    else
    {
    	polarityForDma = TIM_OCPOLARITY_LOW;
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
	sConfigOC.Pulse       = 55;
	sConfigOC.OCPolarity  = polarityForDma;
	//sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_PWM_ConfigChannel(&pwmTimers[actuator.timerHandle], &sConfigOC, actuator.timChannel);


	//DMA INIT
	TimDmaInit(&pwmTimers[actuator.timerHandle], actuator.CcDmaHandle, board.dmasMotor[actuator.Dma]);

	bzero(WS2812_IO_framedata,sizeof(WS2812_IO_framedata));
	WS2812_IO_framedata[0]=100;
	WS2812_IO_framedata[1]=50;
	WS2812_IO_framedata[2]=25;
	WS2812_IO_framedata[3]=50;
	WS2812_IO_framedata[4]=100;


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
	sConfigOC.Pulse       = 52;
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


//TODO: add back support for DMA
/*
void DMA1_Stream1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&ws2812_led);
	HAL_TIM_PWM_Stop(&pwmTimerBase, board.motors[3].timChannel);
	dmaTriggered = 1;
}
*/
