#include "includes.h"

#define DSHOT_OUT_BUFFER_SIZE 48 //16 * 3 bytes
int dShotFeedTheDog = 0;
uint32_t dshotOutBuffer[DSHOT_OUT_BUFFER_SIZE];  
uint32_t dshotInBuffer[DSHOT_OUT_BUFFER_SIZE*8];  


static void DshotTimerInit(uint32_t timer, uint32_t pwmHz, uint32_t timerHz); //make general function?
static void DshotDmaInit(void);
static void DshotTransferComplete(uint32_t callbackNumber);
static void FillDshotBuffer(uint16_t data, uint32_t gpio);

//DSHOT timer
TIM_HandleTypeDef dshotTimer;


void FillDshotBuffer(uint16_t data, uint32_t gpio)
{
    int bitIndex;

    #define BITS_PER_DSHOT_PACKET 16

    for (bitIndex = (BITS_PER_DSHOT_PACKET - 1); bitIndex >= 0; bitIndex--)
    {
        if (data & (1 << bitIndex))
        {
            dshotOutBuffer[bitIndex*3]   |= gpio;
            dshotOutBuffer[bitIndex*3+1] |= gpio;
            dshotOutBuffer[bitIndex*3+2] &= ~(gpio);
        }
        else
        {
            dshotOutBuffer[bitIndex*3]   |= gpio;
            dshotOutBuffer[bitIndex*3+1] &= ~(gpio);
            dshotOutBuffer[bitIndex*3+2] &= ~(gpio);
        }
    }

}

void DshotInit(int offlineMode)
{

    //todo change from single buffer method maybe
    //int outputNumber;   //set, motor output based on board orrientation, from quad's POV: 0 is top left, 1 is top right, etc...
    int motorNum;       //set, motor output based on board arrat, from board's POV: 0 is top left, 1 is top right, etc...

    //deinit flight and treat motors independantly, make this compatible with old board
    if(offlineMode)
    {
        dShotFeedTheDog = 1; //feed the dog in the scheduler
        //DeinitFlight();      //disable gyro, acc, motor outputs, serial, and soft serial
    }

    //init timer
    DshotTimerInit(ENUM_TIM1, 15, 24000000);
    //DshotTimerInit(ENUM_TIM1, 512, 1024); //slow way down for visual test
    DshotDmaInit();
    if ((HAL_TIM_Base_Start(&dshotTimer)) != HAL_OK)
    {
      while(1);
    }

    bzero(dshotOutBuffer, sizeof(dshotOutBuffer));
    for(motorNum=0;motorNum<MAX_MOTOR_NUMBER;motorNum++)
    {
        /*
        outputNumber = mainConfig.mixerConfig.motorOutput[motorNum]; //get output number from quad's POV
        if(board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
        {
            InitializeGpio(ports[board.motors[outputNumber].port], board.motors[outputNumber].pin, 0);
            //todo this can be changed to a constant lookup table
            FillDshotBuffer(0xAAAA, board.motors[outputNumber].pin);

            //source is dshot buffer, destination is motor GPIO (LED for first testing)
            if (HAL_DMA_Start_IT(&dmaHandles[ENUM_DMA2_STREAM_5], (uint32_t)&dshotOutBuffer, (uint32_t)&ports[board.motors[outputNumber].port]->ODR, DSHOT_OUT_BUFFER_SIZE) != HAL_OK)
            {
                while(1);
            }
        }
        */

    }
    InitializeGpio(ports[ENUM_PORTA], GPIO_PIN_2, 1); //use B5 to test output on LED
    FillDshotBuffer(0xAAAA, GPIO_PIN_2);
    //if (HAL_DMA_Start_IT(&dmaHandles[ENUM_DMA2_STREAM_5], (uint32_t)&dshotOutBuffer, (uint32_t)&GPIOA->ODR, DSHOT_OUT_BUFFER_SIZE) != HAL_OK)
    //{
    //    while(1);
    //}    
    if (HAL_DMA_Start_IT(&dmaHandles[ENUM_DMA2_STREAM_5], (uint32_t)&dshotOutBuffer, (uint32_t)&GPIOA->ODR, DSHOT_OUT_BUFFER_SIZE) != HAL_OK)
    {
        while(1);
    }
    TIM1->DIER |=  0x00000100;
    //hdma->Instance->CR  &= ~(DMA_IT_TC);
    //TIM1->DIER = TIM_DIER_UDE;
    //set motor outputs as GPIOs
    //motorNum = 0; //test motor 1
    //outputNumber = mainConfig.mixerConfig.motorOutput[motorNum]; //get output number from quad's POV
    //InitializeGpio(ports[board.motors[outputNumber].port], board.motors[outputNumber].pin, 0); //init GPIO and set output to low
    //InitializeGpio(ports[ENUM_PORTB], GPIO_PIN_2, 0); //use B5 to test output on LED


    //init timer
    //DshotTimerInit(TIM1, 15, 24000000);
    //DshotTimerInit(ENUM_TIM1, 15000000, 24000000); //slow way down for visual test
    //DshotDmaInit();

    //start the timer
    //if ((HAL_TIM_Base_Start(&dshotTimer)) != HAL_OK)
    //{
    //  while(1);
    //}

    //TESTING, output to GPIOA for testing.
    //start the transfer:
    DelayMs(6000);
}

static void DshotTransferComplete(uint32_t callbackNumber)
{
    static int fisherton = 0;
    (void)(callbackNumber);
    TIM1->DIER &= ~(0x00000100);
    volatile int transferCompleteDetected = 1;
    //1.55us from completion to this beingtriggered
    //inlineDigitalHi(ports[ENUM_PORTA], GPIO_PIN_2);
    if(fisherton)
    {
        fisherton = 0;
        FillDshotBuffer(0xAAAA, GPIO_PIN_2);
    }
    else
    {
        fisherton = 1;
        FillDshotBuffer(0x5555, GPIO_PIN_2);        
    }
    (void)(transferCompleteDetected);
    if (HAL_DMA_Start_IT(&dmaHandles[ENUM_DMA2_STREAM_5], (uint32_t)&dshotOutBuffer, (uint32_t)&GPIOA->ODR, DSHOT_OUT_BUFFER_SIZE) != HAL_OK)
    {
        while(1);
    }
    //inlineDigitalHi(ports[ENUM_PORTA], GPIO_PIN_2);
    TIM1->DIER |= 0x00000100;
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
    dmaHandles[ENUM_DMA2_STREAM_5].Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.Mode                = DMA_NORMAL;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.Priority            = DMA_PRIORITY_HIGH;
    dmaHandles[ENUM_DMA2_STREAM_5].Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

    HAL_DMA_UnRegisterCallback(&dmaHandles[ENUM_DMA2_STREAM_5], HAL_DMA_XFER_ALL_CB_ID);
    
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

    TIM_ClockConfigTypeDef  sClockSourceConfig; //set, timer variable
    TIM_MasterConfigTypeDef sMasterConfig;      //set, timer variable

    (void)(pwmHz);
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

    dshotTimer.Instance           = timers[timer];
    dshotTimer.Init.Prescaler     = (uint16_t)(SystemCoreClock / TimerPrescalerDivisor(timer) / timerHz) - 1;
    //dshotTimer.Init.CounterMode   = TIM_COUNTERMODE_UP;
    //dshotTimer.Init.Period        = (timerHz / pwmHz) - 1;;
    //dshotTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    //dshotTimer.Init.Prescaler     = 1024;
    dshotTimer.Init.CounterMode   = TIM_COUNTERMODE_UP;
    dshotTimer.Init.Period        = 3;
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
