#include "includes.h"

#define CHARGED_VOLTAGE 4.0
#define RUNNING_VOLTAGE 3.80
//#define DEAD_VOLTAGE    3.65

#define NORMAL_VOLTAGE 3.33

#ifdef SPMFC400
	#define HIGH_RESISTOR 68.00
	#define LOW_RESISTOR 3.30
	#undef NORMAL_VOLTAGE
	#define NORMAL_VOLTAGE 3.30
#else
	#define HIGH_RESISTOR 100.00
	#define LOW_RESISTOR 10.00
#endif

VoltageStorageRec voltageStorage[3];
float adcVoltage=0;
float adcCurrent=0;
float adcMAh=0;
uint32_t cellCount=0;
float averageVoltage=0;
float lowVoltage = 0;
float runningVoltage = 0;
float fullVoltage = 0;
volatile uint32_t adcDmaBuffer[2]; //first one is voltage, second one is current

static void ConvertAdcVoltage(uint32_t rawAdcVoltage, float highResistor, float lowResistor);
static void ConvertAdcCurrent(uint32_t rawAdcVoltage, float adcCurrFactor);

//IRQ based conversion complete
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
	//we don't care when conversion is done, we just poll the data since the DMA fills the buffer
	(void)(AdcHandle);
}

//Poll ADC is run by the task scheduler.
void PollAdc(void)
{
	ConvertAdcCurrent( adcDmaBuffer[0], mainConfig.telemConfig.adcCurrFactor );
	ConvertAdcVoltage( adcDmaBuffer[1], HIGH_RESISTOR, LOW_RESISTOR );
}

float ProcessVoltage() //takes adc voltage and returns usable voltage for telemtry
{
    for (int32_t x=2;x>=0;x--)
    {
        if (adcVoltage > voltageStorage[x].voltage)
        {
            voltageStorage[x].voltage = adcVoltage;
            voltageStorage[x].storageTime = InlineMillis();
        }
    }

    if (voltageStorage[2].storageTime < (InlineMillis() - 3000))
    {
        voltageStorage[2].storageTime = voltageStorage[1].storageTime;
        voltageStorage[2].voltage = voltageStorage[1].voltage;
    }

    if (voltageStorage[1].storageTime < (InlineMillis() - 2000))
    {
        voltageStorage[1].storageTime = voltageStorage[0].storageTime;
        voltageStorage[1].voltage = voltageStorage[0].voltage;
    }

    if (voltageStorage[0].storageTime < (InlineMillis() - 1000))
    {
        voltageStorage[0].storageTime = InlineMillis();
        voltageStorage[0].voltage = adcVoltage;
    }

    return(voltageStorage[2].voltage);
}

static void ConvertAdcVoltage(uint32_t rawAdcVoltage, float highResistor, float lowResistor)
{
	static uint32_t lastTime=0;
	if (adcVoltage == 0 )
	{
		averageVoltage = adcVoltage = (float)rawAdcVoltage * (float)((float)NORMAL_VOLTAGE/4096.00) * (float)(((float)highResistor+(float)lowResistor)/(float)lowResistor);
	}
	else
	{
		adcVoltage = (float)rawAdcVoltage * (float)((float)NORMAL_VOLTAGE/4096.00) * (float)(((float)highResistor+(float)lowResistor)/(float)lowResistor);

		if (InlineMillis()-lastTime > 250 )
		{
			lastTime=InlineMillis();
			averageVoltage = ProcessVoltage();
		}
	}
}

static void ConvertAdcCurrent(uint32_t rawAdcVoltage, float adcCurrFactor)
{
	static uint32_t lastTime=0;

	adcCurrent = (float)rawAdcVoltage * (float)((float)NORMAL_VOLTAGE/4096.00) * adcCurrFactor;

	if (lastTime > 0)
	{
		//ma / hour
		//3600000000 ma / hour
		//3600000000 us in an hour
		//1000 ma in an amp
		//amps / (3600000000 / 1000) = mah

		adcMAh = adcMAh +  ( ( Micros() - lastTime ) * adcCurrent / 3600000.0f );
	}
	lastTime=Micros();
}

void CheckBatteryCellCount()
{
	if (InlineMillis() > 1000 && cellCount == 0)
	{
		if ( (averageVoltage > 21.5) && (averageVoltage< 25.8) )
			cellCount=6;
		if ( (averageVoltage > 17.5) && (averageVoltage< 21.5) )
			cellCount=5;
		if ( (averageVoltage > 13.5) && (averageVoltage < 17.5) )
			cellCount=4;
		if ( (averageVoltage > 8.6 ) && (averageVoltage< 13.5) )
			cellCount=3;
		if ( (averageVoltage > 4.5) && (averageVoltage< 8.6) )
			cellCount=2;
		if (averageVoltage < 4.5)
			cellCount=1;

		fullVoltage = (CHARGED_VOLTAGE * cellCount);
		runningVoltage = (RUNNING_VOLTAGE * cellCount);
		lowVoltage = (mainConfig.telemConfig.vbatCutoff * cellCount);
	}
	if ( (averageVoltage<lowVoltage) && boardArmed && (averageVoltage > 2.0f) && mainConfig.telemConfig.vbatbuzzer )
	{
		//turn buzzer on
		buzzerStatus.status = STATE_BUZZER_DEADBAT;
	}

	/*
	if ( (averageVoltage>lowVoltage) && (averageVoltage<runningVoltage)  && (averageVoltage > 2.0f) && mainConfig.telemConfig.vbatbuzzer )
	{
		//turn buzzer on
		buzzerStatus.status = STATE_BUZZER_LOWBAT;
	}
	*/
}

void InitAdc(void)
{
	GPIO_InitTypeDef gpioInit;
	ADC_ChannelConfTypeDef sConfig;

	gpioInit.Pin = board.boardADC[1].pin;
	gpioInit.Mode = GPIO_MODE_ANALOG;
	gpioInit.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ports[board.boardADC[1].port], &gpioInit);

	gpioInit.Pin = board.boardADC[0].pin;
	gpioInit.Mode = GPIO_MODE_ANALOG;
	gpioInit.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ports[board.boardADC[0].port], &gpioInit);

	//##-1- Configure the ADC peripheral #######################################
	adcHandle[board.boardADC[1].adcHandle].Instance                   = adcInstance[board.boardADC[1].adcInstance];
	adcHandle[board.boardADC[1].adcHandle].Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
	adcHandle[board.boardADC[1].adcHandle].Init.Resolution            = ADC_RESOLUTION_12B;
	adcHandle[board.boardADC[1].adcHandle].Init.ScanConvMode          = ENABLE;
	adcHandle[board.boardADC[1].adcHandle].Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode disabled to have only 1 conversion at each conversion trig */
	adcHandle[board.boardADC[1].adcHandle].Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
	adcHandle[board.boardADC[1].adcHandle].Init.NbrOfDiscConversion   = 0;
	adcHandle[board.boardADC[1].adcHandle].Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;        /* Conversion start trigged at each external event */
	adcHandle[board.boardADC[1].adcHandle].Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
	adcHandle[board.boardADC[1].adcHandle].Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	adcHandle[board.boardADC[1].adcHandle].Init.NbrOfConversion       = 2;
	adcHandle[board.boardADC[1].adcHandle].Init.DMAContinuousRequests = ENABLE;
	adcHandle[board.boardADC[1].adcHandle].Init.EOCSelection          = ADC_EOC_SEQ_CONV;

	if (HAL_ADC_Init(&adcHandle[board.boardADC[1].adcHandle]) != HAL_OK)
	{
		// ADC initialization Error
		ErrorHandler(ADC_INIT_FAILIURE);
	}

	//##-2- Configure ADC first channel ######################################
	sConfig.Channel      = board.boardADC[0].adcChannel;
	sConfig.Rank         = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset       = 0;

	if (HAL_ADC_ConfigChannel(&adcHandle[board.boardADC[1].adcHandle], &sConfig) != HAL_OK)
	{
		// Channel Configuration Error
		ErrorHandler(ADC_INIT_FAILIURE);
	}

	//##-3- Configure ADC second channel ######################################
	sConfig.Channel      = board.boardADC[1].adcChannel;
	sConfig.Rank         = 2;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset       = 0;

	if (HAL_ADC_ConfigChannel(&adcHandle[board.boardADC[1].adcHandle], &sConfig) != HAL_OK)
	{
		// Channel Configuration Error
		ErrorHandler(ADC_INIT_FAILIURE);
	}

	//fill active DMA
 	board.dmasActive[ENUM_DMA2_STREAM_1].enabled            = 1;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaStream          = ENUM_DMA2_STREAM_1;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaChannel         = DMA_CHANNEL_2;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaPeriphAlignment = DMA_PDATAALIGN_WORD;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaMemAlignment    = DMA_MDATAALIGN_WORD;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaMode            = DMA_CIRCULAR;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaPriority        = DMA_PRIORITY_LOW;
	board.dmasActive[ENUM_DMA2_STREAM_1].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaIRQn            = DMA2_Stream1_IRQn;
	board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle          = ENUM_DMA2_STREAM_1;
	board.dmasActive[ENUM_DMA2_STREAM_1].priority           = 6;

	//set DMA handle
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Instance                 = dmaStream[board.dmasActive[ENUM_DMA2_STREAM_1].dmaStream];
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.Channel             = board.dmasActive[ENUM_DMA2_STREAM_1].dmaChannel;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.Direction           = board.dmasActive[ENUM_DMA2_STREAM_1].dmaDirection;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.PeriphInc           = board.dmasActive[ENUM_DMA2_STREAM_1].dmaPeriphInc;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.MemInc              = board.dmasActive[ENUM_DMA2_STREAM_1].dmaMemInc;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.PeriphDataAlignment = board.dmasActive[ENUM_DMA2_STREAM_1].dmaPeriphAlignment;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.MemDataAlignment    = board.dmasActive[ENUM_DMA2_STREAM_1].dmaMemAlignment;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.Mode                = board.dmasActive[ENUM_DMA2_STREAM_1].dmaMode;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.Priority            = board.dmasActive[ENUM_DMA2_STREAM_1].dmaPriority;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.FIFOMode            = board.dmasActive[ENUM_DMA2_STREAM_1].fifoMode;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.FIFOThreshold       = board.dmasActive[ENUM_DMA2_STREAM_1].fifoThreshold;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.MemBurst            = board.dmasActive[ENUM_DMA2_STREAM_1].MemBurst;
	dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle].Init.PeriphBurst         = board.dmasActive[ENUM_DMA2_STREAM_1].PeriphBurst;

	//link the DMA
	__HAL_LINKDMA(&adcHandle[board.boardADC[1].adcHandle], DMA_Handle, dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle]);

	//init the DMA
	HAL_DMA_Init(&dmaHandles[board.dmasActive[ENUM_DMA2_STREAM_1].dmaHandle]);

	//Set NVIC and IRQ
	HAL_NVIC_SetPriority(board.dmasActive[ENUM_DMA2_STREAM_1].dmaIRQn, board.dmasActive[ENUM_DMA2_STREAM_1].priority, 0);
	HAL_NVIC_EnableIRQ(board.dmasActive[ENUM_DMA2_STREAM_1].dmaIRQn);

	//Start the DMA
	HAL_ADC_Start_DMA(&adcHandle[board.boardADC[1].adcHandle], (uint32_t *)&adcDmaBuffer, 2);

	/*##-3- Start the conversion process #######################################*/
	/* Note: Considering IT occurring after each number of ADC conversions      */
	/*       (IT by DMA end of transfer), select sampling time and ADC clock    */
	/*       with sufficient duration to not create an overhead situation in    */
	/*        IRQHandler. */
//	if(HAL_ADC_Start_DMA(&adcHandle, (uint32_t*)&uhADCxConvertedValue, 1) != HAL_OK)
//	{
//		/* Start Conversation Error */
//		ErrorHandler(ADC_DMA_INIT_FAILIURE);
//	}
}
