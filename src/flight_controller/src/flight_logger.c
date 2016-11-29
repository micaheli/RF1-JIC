#include "includes.h"


#define UPDATE_BB_CHAR_STRING_SIZE 10 //good for 100 days of logging
#define UPDATE_BB_TOTAL_HEADER_SIZE 256 //good for 100 days of logging
#define UPDATE_BB_DATA_SIZE 32 //good for 100 days of logging
#define HEADER "STARTLOG\n" \
		"VER=001\n\0"


uint32_t LoggingEnabled;
uint32_t firstLogging;
uint32_t flashAlign;

int InitFlightLogger(void) {

	LoggingEnabled = 0;
	firstLogging = 1;
	flashAlign = 0;
	return 1;

}

void EnableLogging(void) {
	LoggingEnabled = 1;
}

void DisableLogging(void) {
	LoggingEnabled = 0;
}


 void FinishPage(void) {
	uint32_t remaingBytes = (flashInfo.pageSize - (flashInfo.buffer[flashInfo.bufferNum].txBufferPtr - FLASH_CHIP_BUFFER_WRITE_DATA_START) );
	for (uint32_t x=0;x<remaingBytes;x++)
	{
		WriteByteToFlash('\0');
	}
}

 void FinishBlock(uint32_t count) {
	uint32_t finishY = ((flashInfo.currentWriteAddress + (flashInfo.buffer[flashInfo.bufferNum].txBufferPtr - FLASH_CHIP_BUFFER_WRITE_DATA_START)) % count);
	if (finishY != 0) {
		for (uint32_t x=0;x<(count - finishY);x++)
		{
			WriteByteToFlash('\0');
		}
	}
}

inline void InlineWrite16To8 (int16_t data) {
	WriteByteToFlash(  (uint8_t)( data >> 8 ) );
	WriteByteToFlash(  (uint8_t)(data & 0xff) );
}

 void WriteByteToFlash (uint8_t data) {

	buffer_record *buffer = &flashInfo.buffer[flashInfo.bufferNum];

	buffer->txBuffer[buffer->txBufferPtr++] = data;

	if (buffer->txBufferPtr > FLASH_CHIP_BUFFER_WRITE_DATA_END) {
		if (flashInfo.bufferNum == 0)
		{
			flashInfo.bufferNum = 1;
		}
		else
		{
			flashInfo.bufferNum = 0;
		}
		flashInfo.buffer[flashInfo.bufferNum].txBufferPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;

		M25p16DmaWritePage(flashInfo.currentWriteAddress, buffer->txBuffer, buffer->rxBuffer); //write buffer to flash using DMA
		flashInfo.currentWriteAddress += FLASH_CHIP_BUFFER_WRITE_DATA_SIZE; //add pointer to address
		if (flashInfo.currentWriteAddress >= flashInfo.totalSize)
			flashInfo.enabled = FLASH_FULL; //check if flash is full. Disable flash if it is full

	}

}

inline int DumbWriteString(char *string, int sizeOfString) {
	for (int x=0; x < sizeOfString; x++)
		WriteByteToFlash( string[x] );

	return sizeOfString;
}

#define STARTLOG "STARTLOG"
#define ITERATION "iteration"


void UpdateBlackbox(pid_output *flightPids, float flightSetPoints[], float dpsGyroArray[], float filteredGyroData[], float filteredAccData[] ) {

	int finishX;
	static int loggingStartedLatch = 0;

	if ( (mainConfig.rcControlsConfig.rcCalibrated) && (boardArmed) && (trueRcCommandF[AUX2] > 0.5) && (flashInfo.enabled == FLASH_ENABLED) ) {
		ledStatus.status = LEDS_FASTER_BLINK;
		LoggingEnabled = 1;
		loggingStartedLatch = 1;
	} else if (trueRcCommandF[AUX2] <= 0.5) {
		if (boardArmed)
			ledStatus.status = LEDS_MED_BLINK;
		else
			ledStatus.status = LEDS_SLOW_BLINK;

		LoggingEnabled = 0;
		firstLogging = 1;
		if (loggingStartedLatch) {
			loggingStartedLatch = 0;
			FinishPage();
		}
	}


	if ( (LoggingEnabled) && (flashInfo.enabled == FLASH_ENABLED) ) {

		if (firstLogging) {

			flashInfo.buffer[0].txBufferPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.buffer[1].txBufferPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.buffer[0].rxBufferPtr = 0;
			flashInfo.buffer[1].rxBufferPtr = 0;
			flashInfo.bufferNum = 0;

			finishX = (flashInfo.currentWriteAddress % UPDATE_BB_DATA_SIZE);
			if (finishX != 0) {
				flashInfo.currentWriteAddress += (UPDATE_BB_DATA_SIZE - finishX);
			}

			DumbWriteString(HEADER, strlen(HEADER)+1);
			FinishPage();
			firstLogging = 0;

		} else {

			//pages are aligned with data at all times if we keep this at 256
			InlineWrite16To8(  (int16_t)(flightPids[YAW].kp           * 10000) ); //2
			InlineWrite16To8(  (int16_t)(flightPids[YAW].ki           * 10000) ); //4
			InlineWrite16To8(  (int16_t)(flightPids[YAW].kd           * 10000) ); //6

			InlineWrite16To8(  (int16_t)(flightPids[ROLL].kp          * 10000) ); //8
			InlineWrite16To8(  (int16_t)(flightPids[ROLL].ki          * 10000) ); //10
			InlineWrite16To8(  (int16_t)(flightPids[ROLL].kd          * 10000) ); //12

			InlineWrite16To8(  (int16_t)(flightPids[PITCH].kp         * 10000) ); //14
			InlineWrite16To8(  (int16_t)(flightPids[PITCH].ki         * 10000) ); //16
			InlineWrite16To8(  (int16_t)(flightPids[PITCH].kd         * 10000) ); //18

			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[YAW]      * 10000) ); //20
			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[ROLL]     * 10000) ); //22
			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[PITCH]    * 10000) ); //24
			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[THROTTLE] * 10000) ); //26

			//-2000.0 to 2000.0 DPS
			InlineWrite16To8(  (int16_t)(flightSetPoints[YAW]    * 10) ); //28
			InlineWrite16To8(  (int16_t)(flightSetPoints[ROLL]   * 10) ); //30
			InlineWrite16To8(  (int16_t)(flightSetPoints[PITCH]  * 10) ); //32

			//-2000.0 to 2000.0 DPS
			InlineWrite16To8(  (int16_t)(dpsGyroArray[YAW]       * 10) ); //34
			InlineWrite16To8(  (int16_t)(dpsGyroArray[ROLL]      * 10) ); //36
			InlineWrite16To8(  (int16_t)(dpsGyroArray[PITCH]     * 10) ); //38

			//-2000.0 to 2000.0 DPS
			InlineWrite16To8(  (int16_t)(filteredGyroData[YAW]   * 10) ); //40
			InlineWrite16To8(  (int16_t)(filteredGyroData[ROLL]  * 10) ); //42
			InlineWrite16To8(  (int16_t)(filteredGyroData[PITCH] * 10) ); //44

			//-16 to 16 DPS
			InlineWrite16To8(  (int16_t)(filteredAccData[ACCX]   * 1000) ); //46
			InlineWrite16To8(  (int16_t)(filteredAccData[ACCY]   * 1000) ); //48
			InlineWrite16To8(  (int16_t)(filteredAccData[ACCZ]   * 1000) ); //50

			//0 TO 1
			InlineWrite16To8(  (int16_t)(motorOutput[0]          * 10000) ); //52
			InlineWrite16To8(  (int16_t)(motorOutput[1]          * 10000) ); //54
			InlineWrite16To8(  (int16_t)(motorOutput[2]          * 10000) ); //56
			InlineWrite16To8(  (int16_t)(motorOutput[3]          * 10000) ); //58

			InlineWrite16To8(  (int16_t)(accNoise[2]             * 1000) ); //60
			InlineWrite16To8(  (int16_t)(accNoise[3]             * 1000) ); //62
			InlineWrite16To8(  (int16_t)(accNoise[4]             * 1000) ); //64

			FinishBlock(64);

		}

	} else {
		ledStatus.status = LEDS_SLOW_BLINK;
	}
}
