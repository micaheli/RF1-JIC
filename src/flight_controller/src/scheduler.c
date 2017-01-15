#include "includes.h"

extern uint8_t tOutBuffer[];
extern uint8_t tInBuffer[];
uint32_t skipTaskHandlePcComm   = 0;
uint32_t failsafeStage          = 0;
uint32_t autoSaveTimer          = 0;
uint32_t firstRun               = 1;


//soft serial buffer handling. TODO: make a structure
volatile uint32_t softSerialEnabled = 0;
volatile uint32_t softSerialBuf[2][SOFT_SERIAL_BIT_TIME_ARRAY_SIZE];
volatile uint32_t softSerialInd[2];
volatile uint32_t softSerialCurBuf;
volatile uint32_t softSerialLastByteProcessedLocation;
volatile uint32_t softSerialSwitchBuffer;
static uint8_t    proccesedSoftSerial[25]; //25 byte buffer enough?
static uint32_t   proccesedSoftSerialIdx = 0;
static uint32_t   softSerialLineIdleSensed = 0;
static uint32_t   lastBitFound = 0;


static void TaskProcessSoftSerial(void);
static void TaskTelemtry(void);
static void TaskAutoSaveConfig(void);
static void TaskHandlePcComm(void);
static void TaskLed(void);
static void TaskBuzzer(void);

void scheduler(int32_t count)
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
			TaskAutoSaveConfig();
			break;
		case 4:
			TaskProcessSoftSerial();
			break;
		case 5:
			TaskTelemtry();
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			firstRun = 0;
			break;
		default:
			break;

	}

}

 void TaskProcessSoftSerial(void) {

	 if (oneWireActive)
		 FeedTheDog();

	 //SoftSerialCheckLineIdle();

/*
	//TODO: Move to softSerial.c or serial.c
	uint32_t x;
	uint32_t currentTime;
	uint32_t bytesExist;
	uint32_t currentBits[10];
	uint32_t packetBitSize;
	uint32_t totalBitSize;
	uint32_t currentBitsIdx;
	uint32_t currentBitsIdxCtr;
	uint32_t timeToUse;
	uint32_t bitTimeReplacement;
	uint32_t bitIdx;
	uint16_t byte;
	uint32_t high;

	//soft serial task. Feed the dog while
	if (oneWireOngoing) {
		FeedTheDog();
	}

	if (!softSerialEnabled)
		return;

	currentTime = Micros();

	//assuming 57600 BAUD for now. 17.25 us per bit on average, 1 stop bit, no parity. max number of interrupts for this byte would be 10, min number would be 2
	//about 172 us per byte, assume the byte is complete 180 us after a pause of 200us
	//a bit is going to be
	packetBitSize = 10; //frame 1 then 8 bits of data in lsb format then frame 0
	//assuming 200 us space between packets for now
	//assuming inverted serial for now. Always starts with a high (0) after a long pause

	if ((currentTime - lastBitFound > 200) && (proccesedSoftSerialIdx > 0) ) //we have data and line is at idle
		softSerialLineIdleSensed = 1;

	if (softSerialSwitchBuffer)
		return;

	if (softSerialInd[softSerialCurBuf] > 60) {
		softSerialSwitchBuffer = 1;
		return;
	}

	//look for bytes first:
	bytesExist = 0;
	for (x = softSerialLastByteProcessedLocation;x < softSerialInd[softSerialCurBuf]; x++) {

		timeToUse = softSerialBuf[softSerialCurBuf][x+1];
		if (timeToUse < softSerialBuf[softSerialCurBuf][x]) {
			timeToUse = currentTime;
		}

		if ( (timeToUse - softSerialBuf[softSerialCurBuf][x] > 200) ) { //first bit, let look for end of byte
			if (currentTime - timeToUse > 180) { //do we possibly have a full byte?
				softSerialLastByteProcessedLocation = x+1;
				bytesExist = 1;

				break;
			} else {
				return;
			}
		}
	}

	if (bytesExist) {

		currentBitsIdx = 0;
		totalBitSize = 0;

		for (x = 0;x < packetBitSize; x++) {

			timeToUse = softSerialBuf[softSerialCurBuf][softSerialLastByteProcessedLocation+1];
			if (timeToUse < softSerialBuf[softSerialCurBuf][softSerialLastByteProcessedLocation]) {
				timeToUse = currentTime;
			}

			//			if (timeToUse == currentTime) //last bits are all zero

			currentBits[currentBitsIdx] = (uint32_t)roundf( (float)( timeToUse - softSerialBuf[softSerialCurBuf][softSerialLastByteProcessedLocation] ) / 17.25 );
			totalBitSize += currentBits[currentBitsIdx];



			if (totalBitSize > packetBitSize) {
				currentBits[currentBitsIdx] -= (packetBitSize - totalBitSize);
				break;
			} else if (totalBitSize == packetBitSize) {
				break;
			}
			softSerialLastByteProcessedLocation++;
			currentBitsIdx++;
		}

	} else {
		return;
	}

	bitIdx = 0;
	byte   = 0;
	high   = 1;// high pulse is first when it's an inverted signal, but a high pulse is considered a 0;
	//this record the bits in the order they come in. LSB or MSB are ignored.
	for (currentBitsIdxCtr = 0; currentBitsIdxCtr < currentBitsIdx; currentBitsIdxCtr++) { //for each pack of bits, first is high then next is low
		if (high) {
			for (x = 0; x < currentBits[currentBitsIdxCtr]; x++)
				byte |= ( (1 << bitIdx++) ); //for inverted signal a low pulse is a 1. We & it with the bit position;
			high = 0; //set next operation to low
		} else {
			for (x = 0; x < currentBits[currentBitsIdxCtr]; x++)
				bitIdx++; //byte |= ( (0 << bitIdx++) ); //for inverted signal a high pulse is a 0. We & it with the bit position;
			high = 1; //set next operation to high
		}
	}


	softSerialBuf[softSerialCurBuf][softSerialLastByteProcessedLocation] = bitTimeReplacement = softSerialBuf[softSerialCurBuf][softSerialLastByteProcessedLocation-currentBitsIdx]-30; //make processing next byte easier by making sure difference is greater than 200 (assuming 57600)


	proccesedSoftSerial[proccesedSoftSerialIdx++] = ~(uint8_t)((byte >> 1) & 0xFF);

	lastBitFound = currentTime;

	if (proccesedSoftSerialIdx == 25) {
		proccesedSoftSerialIdx = 0;
	}

*/

}


void TaskTelemtry(void) {

	ProcessTelemtry();
/*
	//TODO: Move all this to telemetry.c

	GPIO_InitTypeDef GPIO_InitStructure;
	static uint32_t telemCount = 0;
	volatile uint32_t currentTime;

	if (!telemEnabled)
		return;

	if (softSerialLineIdleSensed) {

		if ( (proccesedSoftSerial[0] == 0x7E) && (proccesedSoftSerial[1] == 0x1B) ) {
			//send telemetry here
			lastTimeSPort = InlineMillis();

			//HAL_GPIO_DeInit(ports[board.motors[7].port], board.motors[7].pin);

			//Set pin to timer now that IRQ has occurred.
			GPIO_InitStructure.Pin       = board.motors[7].pin;
			GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP; //GPIO_MODE_AF_PP
			GPIO_InitStructure.Pull      = GPIO_PULLUP; //GPIO_PULLUP //pull up for non inverted, pull down for inverted
			GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
			GPIO_InitStructure.Alternate = board.motors[7].AF;

			HAL_GPIO_Init(ports[board.motors[7].port], &GPIO_InitStructure);
			okToSendSPort = 1;
			sPortExtiSet = 0;
		}
		for (uint32_t x=0;x<25;x++)
			proccesedSoftSerial[x] = 0;

		proccesedSoftSerialIdx   = 0;
		softSerialLineIdleSensed = 0;
		softSerialLineIdleSensed = 0;

	}
	currentTime = InlineMillis();
	if ( ( currentTime - lastTimeSPort  > 2 ) && ( okToSendSPort ) ) { //interrupt 5 ms ago. (last updated is greater than 20)
		okToSendSPort = 0; //latch sending s.port off. EXTI turns it back on.

		if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS_SPORT) {

			switch(telemCount++) {
				case 0:
					SmartPortSendPackage(0x0700, (int32_t)(filteredAccData[ACCX] * 100) );
					break;
				case 1:
					SmartPortSendPackage(0x0710, (int32_t)(filteredAccData[ACCY] * 100) );
					break;
				case 2:
					SmartPortSendPackage(0x0720, (int32_t)(filteredAccData[ACCZ] * 100) );
					break;
				case 3:
					SmartPortSendPackage(0x0701, (int32_t)(filteredGyroData[PITCH]) );
					break;
				case 4:
					SmartPortSendPackage(0x0711, (int32_t)(filteredGyroData[ROLL]) );
					break;
				case 5:
					SmartPortSendPackage(0x0721, (int32_t)(filteredGyroData[YAW]) );
					telemCount = 0;
					break;
				case 6:
					telemCount = 0;
					break;

			}

		}
	}

	//TODO: Soft serial needs to be similar to serial. I think soft serial needs to go into serial.c
	if ( (currentTime - lastTimeSPort > 8) && (!sPortExtiSet) ) { //15 ms since EXTI occurred, let's look for another EXTI now
		sPortExtiSet = 1;
		EXTI_Init(ports[board.motors[7].port], board.motors[7].pin, EXTI9_5_IRQn, 0, 1, GPIO_MODE_IT_RISING_FALLING, GPIO_PULLDOWN);
	}

*/
}

inline void TaskAutoSaveConfig(void) {
	autoSaveTimer = 0;
	return;
	if (!boardArmed) {
		if ( autoSaveTimer && ( InlineMillis() - autoSaveTimer > 1000) ) {
			autoSaveTimer = 0;
			SaveConfig(ADDRESS_CONFIG_START);
		}
	}
}

inline void TaskHandlePcComm(void)
{
	if (skipTaskHandlePcComm)
		return;

	if (tOutBuffer[0]==2) { //we have a usb report

		ProcessCommand((char *)tOutBuffer);
		bzero(tOutBuffer, HID_EPIN_SIZE);

	}

}

inline void TaskLed(void)
{
	static uint32_t ms100Counter = 0;
	static uint32_t lastUpdate = 0;
	static uint32_t lastColors = 0xFFFFFFFF;
	uint32_t currentColors;
	uint32_t x;
	uint32_t y;
	uint8_t  rgbArray[96];

	UpdateLeds(); //update status LEDs

	//Update WS2812 LEDs
	if ( ( InlineMillis() - lastUpdate ) > 100 )
	{
		ms100Counter++;
		lastUpdate = InlineMillis();

		currentColors = (((uint8_t)mainConfig.ledConfig.ledGreen<<16) | ((uint8_t)mainConfig.ledConfig.ledRed<<8) | ((uint8_t)mainConfig.ledConfig.ledBlue<<0));
		if (ws2812LedRecord.enabled && ( (currentColors != lastColors) || (ms100Counter == 10) ))
		{
			ms100Counter = 0;
			lastColors = currentColors;
			y = 0;

			mainConfig.ledConfig.ledCount = CONSTRAIN(mainConfig.ledConfig.ledCount, 1, WS2812_MAX_LEDS);

			for (x = 0; x < mainConfig.ledConfig.ledCount; x++)
			{
				rgbArray[y++] = ~(uint8_t)mainConfig.ledConfig.ledGreen;
				rgbArray[y++] = ~(uint8_t)mainConfig.ledConfig.ledRed;
				rgbArray[y++] = ~(uint8_t)mainConfig.ledConfig.ledBlue;
			}

			OutputSerialDmaByte(rgbArray, y, ws2812LedRecord.ws2812Actuator, 1, 0, 0);

		}
	}

}

inline void TaskBuzzer(void)
{
	UpdateBuzzer();
}
