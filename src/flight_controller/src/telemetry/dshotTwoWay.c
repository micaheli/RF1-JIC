#include "includes.h"

#define DSHOT_OUT_BUFFER_SIZE 48 //16 * 3 bytes
int dShotFeedTheDog = 0;
int dshotOutBuffer[DSHOT_OUT_BUFFER_SIZE];  

/*
static void DshotTimerInit(uint32_t timer, uint32_t pwmHz, uint32_t timerHz); //make general function?
static void DshotDmaInit(void);
static void DshotTransferComplete(uint32_t callbackNumber);

//DSHOT timer
TIM_HandleTypeDef dshotTimer;

void DshotInit(int offlineMode)
{

    TIM_ClockConfigTypeDef  sClockSourceConfig; //set, timer variable
    TIM_MasterConfigTypeDef sMasterConfig;      //set, timer variable
    int outputNumber;                           //set, motor output based on board orrientation, from quad's POV: 0 is top left, 1 is top right, etc...
    int motorNum;                               //set, motor output based on board arrat, from board's POV: 0 is top left, 1 is top right, etc...

    //deinit flight and treat motors independantly, make this compatible with old board
    if(offlineMode)
    {
        dShotFeedTheDog = 1; //feed the dog in the scheduler
        DeinitFlight();      //disable gyro, acc, motor outputs, serial, and soft serial
    }

    //set motor outputs as GPIOs
    //motorNum = 0; //test motor 1
    //outputNumber = mainConfig.mixerConfig.motorOutput[motorNum]; //get output number from quad's POV
    //InitializeGpio(ports[board.motors[outputNumber].port], board.motors[outputNumber].pin, 0); //init GPIO and set output to low
    InitializeGpio(ports[ENUM_PORTB], GPIO_PIN_5, 0); //use B5 to test output on LED


    //init timer
    //DshotTimerInit(TIM1, 15, 24000000);
    DshotTimerInit(TIM1, 15000000, 24000000); //slow way down for visual test
    DshotDmaInit();

    //start the timer
    if ((HAL_TIM_Base_Start(&htim1)) != HAL_OK)
    {
      while(1);
    }

    //TESTING, output to GPIOB for testing.
    //start the transfer:
    if (HAL_DMA_Start_IT(&dmaHandles[ENUM_DMA2_STREAM_5], (uint32_t)&aSRC_Const_Buffer, (uint32_t)&GPIOB->ODR, DSHOT_OUT_BUFFER_SIZE) != HAL_OK)
    {
        while(1);
    }

}

static void DshotTransferComplete(uint32_t callbackNumber)
{
  transferCompleteDetected = 1;
  TIM1->DIER = TIM_DIER_UDE;
  if (HAL_DMA_Start_IT(&dmaHandles[ENUM_DMA2_STREAM_5], (uint32_t)&aSRC_Const_Buffer, (uint32_t)&GPIOB->ODR, BUFFER_SIZE) != HAL_OK)
    {
        while(1);
    }
}


static void DshotDmaInit(void) //make general function?
{
    //init DMA
    //for testing we hard code DMA2 Stream 5
    dmaHandles[ENUM_DMA2_STREAM_5].Instance                 = DMA2_Stream5;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.Channel             = DMA_CHANNEL_6;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.Direction           = DMA_MEMORY_TO_PERIPH;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.PeriphInc           = DMA_PINC_DISABLE;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.MemInc              = DMA_MINC_ENABLE;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.Mode                = DMA_NORMAL;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.Priority            = DMA_PRIORITY_HIGH;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

    //only need the first one of these?
    __HAL_LINKDMA(&dshotTimer,hdma[TIM_DMA_ID_UPDATE],dmaHandles[ENUM_DMA2_STREAM_5]);
    __HAL_LINKDMA(&dshotTimer,hdma[TIM_DMA_ID_CC3],dmaHandles[ENUM_DMA2_STREAM_5]);

    if (HAL_DMA_Init(&dmaHandles[ENUM_DMA2_STREAM_5]) != HAL_OK)
    {
      while(1);
    }

    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 2);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

    //register the callback function
    callbackFunctionArray[FP_DMA2_S5] = DshotTransferComplete;
}

static void DshotTimerInit(uint32_t timer, uint32_t pwmHz, uint32_t timerHz) //make general function?
{

    //run at 24 MHz, need a bitrate of 3 bits per byte:
    //three types of "bytes"
    //one, zero, nothing:
    //one is XXx.
    //zero is Xxx,
    //nothing is xxx,
    //Bit length (total timing period) is 1.67 microseconds
    //For a bit to be 0, the pulse width is 625 nanoseconds
    //For a bit to be 1, the pulse width is 1250 nanoseconds
    //For DSHOT 600:
    //15 and 24000000 MHz will generate a timer event ever 625 ns when the timer is active
    //(15 / 24,000,000) = 0.000,000,625 seconds
    //3 timer events per bit, 000 = nothing, 100 = zero, 110 = one

    dshotTimer.Instance           = timer;
    dshotTimer.Init.Prescaler     = (uint16_t)(SystemCoreClock / TimerPrescalerDivisor(timer) / timerHz) - 1;
    dshotTimer.Init.CounterMode   = TIM_COUNTERMODE_UP;
    dshotTimer.Init.Period        = (timerHz / pwmHz) - 1;;
    dshotTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&dshotTimer) != HAL_OK)
    {
        while(1);
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&dshotTimer, &sClockSourceConfig) != HAL_OK)
    {
        while(1);
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&dshotTimer, &sMasterConfig) != HAL_OK)
    {
        while(1);
    }

}

void BlockingReadGPIO()
{
}
*/