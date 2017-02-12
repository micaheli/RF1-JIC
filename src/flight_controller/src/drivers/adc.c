#include "includes.h"


uint32_t adcVoltage;


static void ConvertAdcVoltage(uint32_t rawAdcVoltage);


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
	(void)(AdcHandle);
  /* Get the converted value of regular channel */
  //uhADCxConvertedValue = HAL_ADC_GetValue(AdcHandle);
}

//Poll ADC is run by the task scheduler.
void PollAdc(void)
{
	if (HAL_ADC_PollForConversion(&adcHandle[board.boardADC[1].adcHandle], 2) == HAL_OK)
	{
		ConvertAdcVoltage( HAL_ADC_GetValue(&adcHandle[board.boardADC[1].adcHandle]) );
	}
	else
	{
		adcVoltage = 0;
	}
}

static void ConvertAdcVoltage(uint32_t rawAdcVoltage)
{
	adcVoltage = rawAdcVoltage * 10;
}

void InitAdc(void)
{
	GPIO_InitTypeDef gpioInit;
	ADC_ChannelConfTypeDef sConfig;

	gpioInit.Pin = board.boardADC[1].pin;
	gpioInit.Mode = GPIO_MODE_ANALOG;
	gpioInit.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ports[board.boardADC[1].port], &gpioInit);

	/*##-1- Configure the ADC peripheral #######################################*/
	adcHandle[board.boardADC[1].adcHandle].Instance                   = adcInstance[board.boardADC[1].adcInstance];
	adcHandle[board.boardADC[1].adcHandle].Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
	adcHandle[board.boardADC[1].adcHandle].Init.Resolution            = ADC_RESOLUTION_12B;
	adcHandle[board.boardADC[1].adcHandle].Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	adcHandle[board.boardADC[1].adcHandle].Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode disabled to have only 1 conversion at each conversion trig */
	adcHandle[board.boardADC[1].adcHandle].Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
	adcHandle[board.boardADC[1].adcHandle].Init.NbrOfDiscConversion   = 0;
	adcHandle[board.boardADC[1].adcHandle].Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;        /* Conversion start trigged at each external event */
	adcHandle[board.boardADC[1].adcHandle].Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
	adcHandle[board.boardADC[1].adcHandle].Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	adcHandle[board.boardADC[1].adcHandle].Init.NbrOfConversion       = 1;
	adcHandle[board.boardADC[1].adcHandle].Init.DMAContinuousRequests = ENABLE;
	adcHandle[board.boardADC[1].adcHandle].Init.EOCSelection          = DISABLE;

	if (HAL_ADC_Init(&adcHandle[board.boardADC[1].adcHandle]) != HAL_OK)
	{
		/* ADC initialization Error */
		ErrorHandler(ADC_INIT_FAILIURE);
	}

	/*##-2- Configure ADC regular channel ######################################*/
	sConfig.Channel      = board.boardADC[1].adcChannel;
	sConfig.Rank         = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset       = 0;

	if (HAL_ADC_ConfigChannel(&adcHandle[board.boardADC[1].adcHandle], &sConfig) != HAL_OK)
	{
		/* Channel Configuration Error */
		ErrorHandler(ADC_INIT_FAILIURE);
	}

	HAL_ADC_Start(&adcHandle[board.boardADC[1].adcHandle]);


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
