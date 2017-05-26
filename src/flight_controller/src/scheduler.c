#include "includes.h"

//this doesn't really belong here
volatile uint8_t tInBuffer[HID_EPIN_SIZE], tOutBuffer[HID_EPOUT_SIZE-1];

uint32_t skipTaskHandlePcComm = 0;
volatile uint32_t errorMask   = 0;

//scheduler timer
TIM_HandleTypeDef schedulerTimer;

//soft serial buffer handling. TODO: make a structure
volatile uint32_t softSerialEnabled = 0;
volatile uint32_t softSerialBuf[2][SOFT_SERIAL_BIT_TIME_ARRAY_SIZE];
volatile uint32_t softSerialInd[2];
volatile uint32_t softSerialCurBuf;
volatile uint32_t softSerialLastByteProcessedLocation;
volatile uint32_t softSerialSwitchBuffer;
volatile uint32_t turnOnVtxNow = 0;

uint8_t  proccesedSoftSerial[25]; //25 byte buffer enough?
uint32_t proccesedSoftSerialIdx   = 0;
uint32_t softSerialLineIdleSensed = 0;
uint32_t lastBitFound             = 0;

static void TaskProcessSoftSerial(void);
static void TaskTelemtry(void);
static void TaskWizard(void);
static void TaskHandlePcComm(void);
static void TaskLed(void);
static void TaskBuzzer(void);
static void TaskAdc(void);
static void TaskCheckVtx(void);
static void TaskCheckDelayedArming(void);
static void TaskProcessArmingStructure(void);
static void InitGeneralInterruptTimer(uint32_t pwmHz, uint32_t timerHz);
static void DeInitGeneralInterruptTimer(void);


static void DeInitGeneralInterruptTimer(void)
{
	HAL_TIM_Base_Stop_IT(&schedulerTimer);
	callbackFunctionArray[GetTimerCallbackFromTimerEnum(board.generalTimer[0].timer)] = 0;
}

static void InitGeneralInterruptTimer(uint32_t pwmHz, uint32_t timerHz)
{
	uint16_t timerPrescaler = 0;

	callbackFunctionArray[GetTimerCallbackFromTimerEnum(board.generalTimer[0].timer)] = GeneralInterruptTimerCallback;

	timerPrescaler = (uint16_t)(SystemCoreClock / TimerPrescalerDivisor(board.generalTimer[0].timer) / timerHz) - 1;

	// Initialize timer
	schedulerTimer.Instance           = timers[board.generalTimer[0].timer];
	schedulerTimer.Init.Prescaler     = timerPrescaler;
	schedulerTimer.Init.CounterMode   = TIM_COUNTERMODE_UP;
	schedulerTimer.Init.Period        = (timerHz / pwmHz) - 1;
	schedulerTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&schedulerTimer);
	HAL_TIM_Base_Start_IT(&schedulerTimer);

    HAL_NVIC_SetPriority(board.generalTimer[0].timerIRQn, 2, 0);
    HAL_NVIC_EnableIRQ(board.generalTimer[0].timerIRQn);
}

//TODO: Don't hardcode to timer 6, use a callback instead
void GeneralInterruptTimerCallback(uint32_t callbackNumber)
{

	(void)(callbackNumber);

    if (__HAL_TIM_GET_FLAG(&schedulerTimer, TIM_FLAG_UPDATE) != RESET)      //In case other interrupts are also running
    {

        if (__HAL_TIM_GET_ITSTATUS(&schedulerTimer, TIM_IT_UPDATE) != RESET)
        {

            __HAL_TIM_CLEAR_FLAG(&schedulerTimer, TIM_FLAG_UPDATE);

            //triggers DMA read which will trigger flight code
            GyroExtiCallback(0);

        }

    }

}

//DeInit the fake Gyro EXTI and attempt to reenable the actual EXTI, this checks that the gyro is interrupting and will init the fake EXTI if necessary.
void DeInitFakeGyroExti(void)
{
	DeInitGeneralInterruptTimer();
	InitGyroExti();
	DelayMs(2);
	gyroInterrupting = 0;
	DelayMs(2);
	if(!gyroInterrupting)
	{
		InitFakeGyroExti();
	}
}

//Deinit the real gyro EXTI and Init the fake gyro EXTI
void InitFakeGyroExti(void)
{
	//TODO: Don't just hardcode for 32KHz
	DeInitGyroExti();
	InitGeneralInterruptTimer(32000, 48000000);
}

void InitScheduler(void)
{
	//DeInit the fake Gyro EXTI and attempt to reenable the actual EXTI, this checks that the gyro is interrupting and will init the fake EXTI if necessary.
	DelayMs(2);
	gyroInterrupting = 0;
	DelayMs(2);
	if(!gyroInterrupting)
	{
		InitFakeGyroExti();
	}
	turnOnVtxNow = 0;

	return;
}

void Scheduler(int32_t count)
{

	switch (count) {

		case 0:
			TaskHandlePcComm();
			break;
		case 1:
			TaskLed();
			break;
		case 2:
			TaskBuzzer();
			break;
		case 3:
			TaskAdc();
			break;
		case 4:
			TaskProcessSoftSerial();
			break;
		case 5:
			TaskTelemtry();
			break;
		case 6:
			TaskWizard();
			break;
		case 7:
			TaskCheckVtx();
			break;
		case 8:
			TaskCheckDelayedArming();
			break;
		case 9:
			TaskProcessArmingStructure();
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		default:
			break;

	}

}

inline void TaskProcessArmingStructure(void)
{
	ProcessArmingStructure();
}

inline void TaskCheckDelayedArming(void)
{

 	//handles VTX enabling as well
 	if(armBoardAt)
 	{
 		//delayed arming window of 150 ms.
 		if ( (InlineMillis() > armBoardAt) && ((InlineMillis() - armBoardAt) < 250) )
 		{
 			if( mainConfig.telemConfig.telemSmartAudio && !ModeSet(M_VTXON) )
 			{
 				//only try turning on the VTX once per arming
 				turnOnVtxNow = 0;
 				VtxTurnOn();
 			}
 			armBoardAt = 0;
 			ArmBoard();
 		}
 	}
}

inline void TaskCheckVtx(void)
{

	static uint32_t modeLatch = 0;

	//don't do this task unless board is disarmed
	if (boardArmed)
		return;

	if (!mainConfig.telemConfig.telemSmartAudio)
		return;

	//don't change channel until prog mode is done
	if (progMode)
		return;

	if (ModeSet(M_VTXON) && ModeActive(M_VTXON) && !modeLatch)
	{
		//only try turning on the VTX once per mode enabling
		turnOnVtxNow = 1;
		modeLatch = 1;
	}
	else if (ModeSet(M_VTXON) && !ModeActive(M_VTXON))
	{
		modeLatch = 0;
	}

	if (turnOnVtxNow)
	{
		turnOnVtxNow = 0;
		VtxTurnOn(); //blocking of scheduler during send and receive
	}

	if (vtxRequested.vtxBandChannel != vtxRecord.vtxBandChannel)
	{
		VtxBandChannel(vtxRequested.vtxBandChannel);
	}

	if (vtxRequested.vtxPit != vtxRecord.vtxPit)
	{
		//if (vtxRecord.vtxPit == VTX_MODE_ACTIVE)
		//	VtxTurnOn();
		//else
		//	VtxTurnPit();
	}

	if (vtxRequested.vtxPower != vtxRecord.vtxPower)
	{
		VtxPower(vtxRequested.vtxPower);
	}

}

inline void TaskAdc(void)
{
	if (progMode)
		return;
	PollAdc();
	CheckBatteryCellCount();
}

inline void TaskProcessSoftSerial(void)
{
	 if (oneWireActive)
		 FeedTheDog();
}

inline void TaskWizard(void)
{
	switch(wizardStatus.currentWizard)
	{

		case WIZ_RC:
			if (wizardStatus.currentStep == 1) //step three needs to be polled by user/gui
				HandleWizRc();
			break;
		case 0:
		default:
			return;
	}
}

inline void TaskTelemtry(void)
{
	ProcessTelemtry();
}

inline void TaskHandlePcComm(void)
{
	uint32_t x;

	if (skipTaskHandlePcComm)
		return;

	if (tOutBuffer[0]==2)
	{ //we have a usb report
		ProcessCommand((char *)tOutBuffer);
		for (x=0;x<(HID_EPOUT_SIZE-1);x++)
			tOutBuffer[x] = 0;
	}

}

inline void TaskLed(void)
{
	UpdateLeds(); //update status LEDs
	UpdateWs2812Leds();
}

inline void TaskBuzzer(void)
{
	UpdateBuzzer();
}

void ErrorHandler(uint32_t error)
{
	errorMask |= (error);

	switch (error)
	{
		case TIMER_INPUT_INIT_FAILIURE:
		case ADC_INIT_FAILIURE:
		case ADC_DMA_INIT_FAILIURE:
		case MSP_DMA_GYRO_RX_INIT_FAILIURE:
		case MSP_DMA_GYRO_TX_INIT_FAILIURE:
		case MSP_DMA_SPI_RX_INIT_FAILIURE:
		case MSP_DMA_SPI_TX_INIT_FAILIURE:
			//ping warning to user here, may not a valid reason to crash the board though
			return;
			break;
		case SERIAL_HALF_DUPLEX_INIT_FAILURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case SERIAL_INIT_FAILURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case FLASH_SPI_INIT_FAILIURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case WS2812_LED_INIT_FAILIURE:
			//ping warning to user here, not a valid reason to crash the board though
			return;
			break;
		case GYRO_SPI_INIT_FAILIURE: //gyro failed to init. Can't fly like this.
		case GYRO_INIT_FAILIURE: //gyro failed to init. Can't fly like this.
		case GYRO_SETUP_COMMUNICATION_FAILIURE: //gyro init success, but setting up register failed. Can't fly like this.
			return;
			break;
		case HARD_FAULT:  //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		case MEM_FAULT:   //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		case BUS_FAULT:   //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		case USAGE_FAULT: //hard fault is bad, if we're in flight we should setup a restart, for now we crash the board
		default:
			break;
	}

	//bad errors will fall through here
	ZeroActuators(32000);

    while (1)
    {
		DoLed(0, 1);
		DoLed(1, 0);
		simpleDelay_ASM(50000);
		DoLed(0, 0);
		DoLed(1, 1);
		simpleDelay_ASM(75000);
    	ZeroActuators(10);
    }

}
