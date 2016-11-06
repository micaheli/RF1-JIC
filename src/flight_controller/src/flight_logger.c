#include "includes.h"

uint32_t LoggingEnabled;
uint32_t firstLogging;

int InitFlightLogger(void) {

	LoggingEnabled = 0;
	firstLogging = 1;
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


void UpdateBlackbox(pid_output *flightPids, float flightSetPoints[] ) {

	static uint16_t iteration = 0;

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

			firstLogging = 0;

			flashInfo.txBufferAPtr=0;
			flashInfo.rxBufferAPtr=0;
			flashInfo.txBufferBPtr=0;
			flashInfo.rxBufferBPtr=0;
			flashInfo.bufferStatus = BUFFER_STATUS_FILLING_A;
			bzero(flashInfo.txBufferA, FLASH_CHIP_BUFFER_SIZE);
			bzero(flashInfo.txBufferB, FLASH_CHIP_BUFFER_SIZE);

			//name,
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );

			DumbWriteToFlash(  'i' );
			DumbWriteToFlash(  't' );
			DumbWriteToFlash(  'u' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );

			DumbWriteToFlash(  'y' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  'y' );
			DumbWriteToFlash(  'i' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  'y' );
			DumbWriteToFlash(  'd' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );

			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'i' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'd' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );

			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  'i' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  'd' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );

			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'y' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  't' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );

			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'y' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'w' );
			DumbWriteToFlash(  ';' );

			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );
			DumbWriteToFlash(  ';' );



		} else {


			DumbWriteToFlash(  iteration++ );

			InlineWrite16To8(  (int16_t)(flightPids[YAW].kp   * 10000) );
			InlineWrite16To8(  (int16_t)(flightPids[YAW].ki   * 10000) );
			InlineWrite16To8(  (int16_t)(flightPids[YAW].kd   * 10000) );
			InlineWrite16To8(  (int16_t)(flightPids[ROLL].kp  * 10000) );
			InlineWrite16To8(  (int16_t)(flightPids[ROLL].ki  * 10000) );
			InlineWrite16To8(  (int16_t)(flightPids[ROLL].kd  * 10000) );
			InlineWrite16To8(  (int16_t)(flightPids[PITCH].kp * 10000) );
			InlineWrite16To8(  (int16_t)(flightPids[PITCH].ki * 10000) );
			InlineWrite16To8(  (int16_t)(flightPids[PITCH].kd * 10000) );

			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[YAW] * 10000) );
			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[ROLL] * 10000) );
			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[PITCH] * 10000) );
			InlineWrite16To8(  (int16_t)(smoothedRcCommandF[THROTTLE] * 10000) );

			InlineWrite16To8(  (int16_t)(flightSetPoints[YAW] * 10000) );
			InlineWrite16To8(  (int16_t)(flightSetPoints[ROLL] * 10000) );
			InlineWrite16To8(  (int16_t)(flightSetPoints[PITCH] * 10000) );

		}

	} else {
		ledStatus.status = LEDS_SLOW_BLINK;
	}
}
