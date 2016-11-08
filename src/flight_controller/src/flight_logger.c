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

inline void InlineWrite16To8 (int16_t data) {
	DumbWriteToFlash(  (uint8_t)( (data & 0xff00) >>  8) );
	DumbWriteToFlash(  (uint8_t)(data & 0x00ff) );
}

inline void DumbWriteToFlash (uint8_t data) {

	if (flashInfo.bufferStatus == BUFFER_STATUS_FILLING_A) {

		flashInfo.txBufferA[flashInfo.txBufferAPtr++] = data; //add data to buffer on byte at a time.

		//check if buffer is full
		if (flashInfo.txBufferAPtr > FLASH_CHIP_BUFFER_WRITE_DATA_END) { //filled buffer
			bzero(flashInfo.rxBufferA, FLASH_CHIP_BUFFER_SIZE);
			M25p16DmaWritePage(flashInfo.currentWriteAddress, flashInfo.txBufferA, flashInfo.rxBufferA); //write buffer to flash using DMA
			flashInfo.currentWriteAddress += FLASH_CHIP_BUFFER_WRITE_DATA_SIZE; //add pointer to address
			flashInfo.txBufferAPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.bufferStatus = BUFFER_STATUS_FILLING_B;
			if (flashInfo.currentWriteAddress >= flashInfo.totalSize)
				flashInfo.enabled = 0; //check if flash is full. Disable flash if it is full
		}

	} else if (flashInfo.bufferStatus == BUFFER_STATUS_FILLING_B) {

		flashInfo.txBufferB[flashInfo.txBufferBPtr++] = data; //add data to buffer on byte at a time.

		//check if buffer is full
		if (flashInfo.txBufferBPtr > FLASH_CHIP_BUFFER_WRITE_DATA_END) { //filled buffer
			bzero(flashInfo.rxBufferB, FLASH_CHIP_BUFFER_SIZE);
			M25p16DmaWritePage(flashInfo.currentWriteAddress, flashInfo.txBufferB, flashInfo.rxBufferB); //write buffer to flash using DMA
			flashInfo.currentWriteAddress += FLASH_CHIP_BUFFER_WRITE_DATA_SIZE; //add pointer to address
			flashInfo.txBufferBPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.bufferStatus = BUFFER_STATUS_FILLING_A;
			if (flashInfo.currentWriteAddress >= flashInfo.totalSize)
				flashInfo.enabled = 0; //check if flash is full. Disable flash if it is full
		}

	}

}

inline int DumbWriteString(char *string, int sizeOfString) {

	static int x;

	for (x=0; x < sizeOfString; x++)
		DumbWriteToFlash( string[x] );

	return sizeOfString;
}

#define STARTLOG "STARTLOG"
#define ITERATION "iteration"

inline int () {
	if ((UPDATE_BB_TOTAL_HEADER_SIZE - ( flashInfo.currentWriteAddress % UPDATE_BB_TOTAL_HEADER_SIZE)) < UPDATE_BB_TOTAL_HEADER_SIZE)
		flashInfo.currentWriteAddress += UPDATE_BB_TOTAL_HEADER_SIZE - ( flashInfo.currentWriteAddress % UPDATE_BB_TOTAL_HEADER_SIZE);

	return
}

void UpdateBlackbox(pid_output *flightPids, float flightSetPoints[] ) {

	static uint16_t iteration = 0;
	int bytesSent, finishX;
	char charString[UPDATE_BB_CHAR_STRING_SIZE];

	if (curvedRcCommandF[AUX2] < 0) {
		ledStatus.status = LEDS_FAST_BLINK;
		LoggingEnabled = 1;
	} else if (curvedRcCommandF[AUX2] >= 0) {
		ledStatus.status = LEDS_SLOW_BLINK;
		LoggingEnabled = 0;
		firstLogging = 1;
	}


	if ( (LoggingEnabled) && (flashInfo.enabled) ) {

		ledStatus.status = LEDS_FAST_BLINK;
		if (firstLogging) {

			//make sure flashInfo.currentWriteAddress is aligned to a page (multiple of 256)

			if ((UPDATE_BB_TOTAL_HEADER_SIZE - ( flashInfo.currentWriteAddress % UPDATE_BB_TOTAL_HEADER_SIZE)) < UPDATE_BB_TOTAL_HEADER_SIZE)
				flashInfo.currentWriteAddress += UPDATE_BB_TOTAL_HEADER_SIZE - ( flashInfo.currentWriteAddress % UPDATE_BB_TOTAL_HEADER_SIZE);


			firstLogging = 0;

			flashInfo.txBufferAPtr=FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.rxBufferAPtr=FLASH_CHIP_BUFFER_READ_DATA_START;
			flashInfo.txBufferBPtr=FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.rxBufferBPtr=FLASH_CHIP_BUFFER_READ_DATA_START;
			flashInfo.bufferStatus = BUFFER_STATUS_FILLING_A;
			bzero(flashInfo.txBufferA, FLASH_CHIP_BUFFER_SIZE);
			bzero(flashInfo.txBufferB, FLASH_CHIP_BUFFER_SIZE);


			bytesSent = 0;

			//start of header




			bytesSent += DumbWriteString(HEADER, strlen(HEADER)+1);
			//BYTE 256 is a null character
			//pages are aligned with data at all times if we keep this at 256

			flashInfo.currentWriteAddress += ( UPDATE_BB_TOTAL_HEADER_SIZE - (strlen(HEADER)+1) );

		} else {

			//align data to flash
			finishX = (flashInfo.currentWriteAddress % UPDATE_BB_DATA_SIZE);
			if (finishX != 0) {
				flashInfo.currentWriteAddress += finishX;
			}

			//pages are aligned with data at all times if we keep this at 256
			InlineWrite16To8(  (int16_t)(flightPids[YAW].kp   * 10000) ); //2
			InlineWrite16To8(  (int16_t)(flightPids[YAW].ki   * 10000) ); //4
			InlineWrite16To8(  (int16_t)(flightPids[YAW].kd   * 10000) ); //6

			InlineWrite16To8(  (int16_t)(flightPids[ROLL].kp  * 10000) ); //8
			InlineWrite16To8(  (int16_t)(flightPids[ROLL].ki  * 10000) ); //10
			InlineWrite16To8(  (int16_t)(flightPids[ROLL].kd  * 10000) ); //12

			InlineWrite16To8(  (int16_t)(flightPids[PITCH].kp * 10000) ); //14
			InlineWrite16To8(  (int16_t)(flightPids[PITCH].ki * 10000) ); //16
			InlineWrite16To8(  (int16_t)(flightPids[PITCH].kd * 10000) ); //18

			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[YAW] * 10000) ); //20
			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[ROLL] * 10000) ); //22
			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[PITCH] * 10000) ); //24
			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[THROTTLE] * 10000) ); //26

			InlineWrite16To8(  (int16_t)(flightSetPoints[YAW] * 10000) ); //28
			InlineWrite16To8(  (int16_t)(flightSetPoints[ROLL] * 10000) ); //30
			InlineWrite16To8(  (int16_t)(flightSetPoints[PITCH] * 10000) ); //32
			//32

		}

	} else {
		ledStatus.status = LEDS_SLOW_BLINK;
	}
}
