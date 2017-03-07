#include "includes.h"

enum {
	STOP_BITS_1_0 = 0,
	STOP_BITS_1_5 = 1,
	STOP_BITS_2_0 = 2,
};

uint32_t softSerialRxTimeBuffer;

static const uint16_t bitLookup[] = {0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF};

TIM_HandleTypeDef softSerialTimer;
uint32_t onOffBitArray[10];

static void     NumberOfBits(uint32_t time2, uint32_t time1, uint32_t bitsInByte, float bitWidth, uint16_t *numberOfBits, uint32_t workingOnByte);
static float    FindSoftSerialBitWidth(uint32_t baudRate);
static float    FindSoftSerialByteWidth(float bitWidth, uint32_t bitsPerByte);
static float    FindSoftSerialLineIdleTime(float byteWidth);
static uint32_t IsSoftSerialLineIdle(void);

static void     InitSoftSerialTimer(uint32_t pwmHz, uint32_t timerHz);


static void InitSoftSerialTimer(uint32_t pwmHz, uint32_t timerHz)
{
	uint32_t timer = ENUM_TIM7;

	uint16_t timerPrescaler = 0;

	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_ClockConfigTypeDef  sClockSourceConfig;

	timerPrescaler = (uint16_t)(SystemCoreClock / TimerPrescalerDivisor(timer) / timerHz) - 1;

	// Initialize timer
	softSerialTimer.Instance           = timers[timer];
	softSerialTimer.Init.Prescaler     = timerPrescaler;
	softSerialTimer.Init.CounterMode   = TIM_COUNTERMODE_UP;
	softSerialTimer.Init.Period        = (timerHz / pwmHz) - 1;
	softSerialTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&softSerialTimer);
	HAL_TIM_Base_Start(&softSerialTimer);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&softSerialTimer, &sClockSourceConfig);

	HAL_TIM_PWM_Init(&softSerialTimer);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&softSerialTimer, &sMasterConfig);

}

uint32_t SendSoftSerialByteBlocking(uint8_t byte, uint32_t startBit, uint32_t stopBits)
{
	//start bit is a zero, end bit is a 1
	//baudrate, timer rate, 48MHz is good on f4s running at 192 MHz

	uint32_t x;
	uint32_t bitNumber;

	bitNumber = 0;

	//fill timer array
	if (startBit)
		onOffBitArray[bitNumber++] = 0;

	for (x=0;x<8;x++)
		onOffBitArray[bitNumber++] = BIT_CHECK(byte, x);

	switch(stopBits)
	{
		case STOP_BITS_1_0:
			onOffBitArray[bitNumber++] = 1;
			break;
		case STOP_BITS_1_5:
		case STOP_BITS_2_0:
			onOffBitArray[bitNumber++] = 1;
			onOffBitArray[bitNumber++] = 1;
			break;
	}

	//start timer and start sending bits:

}


uint32_t SendSoftSerialBlocking(uint32_t baudrate, uint32_t inverted, uint32_t stopBits, uint32_t startBit, uint32_t port, uint32_t pin, uint8_t byteArray[], uint32_t numBytesToSend)
{
	uint32_t x;

	//init GPIO
	//inverted

	//to send
	//baudrate, timer rate, 48MHz is good on f4s running at 192 MHz
	InitSoftSerialTimer(baudrate, 48000000);

	for(x=0;x<numBytesToSend;x++)
	{
		//SendSoftSerialByteBlocking(byte, startBit, stopBits);
	}

	return(1);
}

//uint32_t ReceiveSoftSerialBlocking()
//{
//
//}

//static uint32_t IsSoftSerialLineIdle(void)
//{
//	volatile float timeNow;
//	timeNow = (float)Micros();
//	if ( (timeNow - (float)telemtryRxTimerBuffer[telemtryRxTimerBufferIdx-1]) > (200))
//	{
//		if (telemtryRxTimerBufferIdx > 1)
//			return(1);
//	}
//	return(0);
//}

uint32_t NewProcessSoftSerialBits(volatile uint32_t timerBuffer[], volatile uint32_t *timerBufferIndex, volatile uint8_t serialBuffer[], volatile uint32_t *serialBufferIndex, float bitWidthUs, uint32_t bitsInByte)
{

	uint32_t x;
	uint32_t fails;
	uint16_t bits;
	uint32_t currentBit;
	volatile uint32_t byte;
	uint16_t totalBitsFound;
	uint32_t bytesFound;
	volatile uint32_t timeNow = Micros();

	bytesFound     = 0;

	fails          = 0;
	byte           = 0;
	currentBit     = 1;

	static volatile uint8_t byte1;
	static volatile uint8_t byte2;
	//at higher baud rates, the last bit doesn't always calculate correctly, so we hadd a fake byte to the end of the times and remove iut after calculations
	//put in last time so we can get the last byte. We need the last byte to calculate the frame.
	//put in a fake byte at the end and drop it after we calculate
	//last good byte time is timerBuffer[(*timerBufferIndex) - 1]
	//fake byte will have a stop bit which is timerBuffer[(*timerBufferIndex)] = timerBuffer[(*timerBufferIndex) - 1] + lrintf(bitWidthUs);
	//then put in start bit + 8 zeros which is lrintf(bitWidthUs * 9)

	//timerBuffer[(*timerBufferIndex)] = timerBuffer[(*timerBufferIndex) - 1] + lrintf(bitWidthUs * 10); //fake start bit at least 10 bits away
	//(*timerBufferIndex)++; //increment
	//timerBuffer[(*timerBufferIndex)] = timerBuffer[(*timerBufferIndex) - 1] + lrintf(bitWidthUs * 9); //fake zero byte
	//(*timerBufferIndex)++; //increment

	totalBitsFound = 0;
	currentBit     = 1;

	for (x = 1; x < (*timerBufferIndex); x++)
	{

		if (currentBit)
		{

			NumberOfBits(timerBuffer[x], timerBuffer[x-1], bitsInByte, bitWidthUs, &bits, totalBitsFound);

			if ( (bits > 0) )
			{
				if ( x== ( (*timerBufferIndex)-1) )
					bits = (bitsInByte-totalBitsFound);
				else
					bits = CONSTRAIN(bits,1,(bitsInByte-totalBitsFound));
				byte &= ~(bitLookup[ bits ] << totalBitsFound);
				totalBitsFound += bits;
				currentBit=0;
				bits = 0;
			}
			else
			{
				//ignore this time as a corruption
				fails++;
			}
		}
		else
		{

			NumberOfBits(timerBuffer[x], timerBuffer[x-1], bitsInByte, bitWidthUs, &bits, totalBitsFound);

			if ( (bits > 0) )
			{
				if ( x== ( (*timerBufferIndex)-1) )
					bits = (bitsInByte-totalBitsFound);
				else
					bits = CONSTRAIN(bits,1,(bitsInByte-totalBitsFound));
				byte |= (bitLookup[ bits ] << totalBitsFound);
				totalBitsFound += bits;
				currentBit=1;
				bits = 0;
			}
			else
			{
				//ignore this time as a corruption
				volatile uint32_t t1 = timerBuffer[x-1];
				volatile uint32_t t2 = timerBuffer[x];
				volatile uint32_t t3 = t2 - t1;
				fails++;
			}
		}

		//if(fails > 10)
		//{
		//	totalBitsFound = 0;
		//	break;
		//}

		if (totalBitsFound >= 10)
		{
			//trim off frames
			if (bytesFound == 0)
			{
				byte1 = (uint8_t)( (byte >> 1) & 0xFF );
			}
			else
			{
				if (byte1 != 0xFE)
					byte2 = (uint8_t)( (byte >> 1) & 0xFF );
				if (byte2 == 0x1b)
				{
					volatile uint32_t time[10];
					time[0] = timerBuffer[0];
					time[1] = timerBuffer[1];
					time[2] = timerBuffer[2];
					time[3] = timerBuffer[3];
					time[4] = timerBuffer[4];
					time[5] = timerBuffer[5];
					time[6] = timerBuffer[6];
					time[7] = timerBuffer[7];
					time[8] = timerBuffer[8];
					time[9] = timerBuffer[9];
					time[10] = timerBuffer[10];
					byte2 = 0x1b;
				}
			}
			serialBuffer[(*serialBufferIndex)++] = (uint8_t)( (byte >> 1) & 0xFF );
			totalBitsFound = 0;

			bytesFound++;
		}

	}

	(*timerBufferIndex) = 0; //set time buffer index to zero
	(*serialBufferIndex) = 0; //set rx buffer index to zero
	return(bytesFound); //return number of bytes found
}

inline static void NumberOfBits(uint32_t time2, uint32_t time1, uint32_t bitsInByte, float bitWidth, uint16_t *numberOfBits, uint32_t workingOnByte)
{
	float timeD;
	float maxWidthPossible;
	*numberOfBits = 0;
	if (time2 <= time1) //no bits
	{
		return;
	}
	timeD = (float)(time2 - time1);
	maxWidthPossible = (bitWidth * (float)bitsInByte);
	if ( workingOnByte || (timeD < maxWidthPossible) ) //working on byte, so return line idle, or bits exist and this is a new byte
	{
		*numberOfBits = lrintf(round(timeD/(float)bitWidth));
	}
	return;
}

inline static float FindSoftSerialBitWidth(uint32_t baudRate)
{
	return ( (1.0 / (float)baudRate) * 1000.0 * 1000.0 );
}

inline static float FindSoftSerialByteWidth(float bitWidth, uint32_t bitsPerByte)
{
	return (bitWidth * (float)bitsPerByte);
}

inline static float FindSoftSerialLineIdleTime(float byteWidth)
{
	return (byteWidth * 1.50);
}
