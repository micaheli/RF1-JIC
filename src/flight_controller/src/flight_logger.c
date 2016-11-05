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
	DumbWriteToFlash(  (int8_t)( (data & 0xff00) >>  8) );
	DumbWriteToFlash(  (int8_t)(data & 0x00ff) );
}

inline void DumbWriteToFlash (int8_t data) {

	if (flashInfo.bufferStatus == BUFFER_STATUS_FILLING_A) {

		flashInfo.txBufferA[flashInfo.txBufferAPtr++] = data; //add data to buffer on byte at a time.

		//check if buffer is full
		if (flashInfo.txBufferAPtr > FLASH_CHIP_BUFFER_WRITE_DATA_END) { //filled buffer
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
			M25p16DmaWritePage(flashInfo.currentWriteAddress, flashInfo.txBufferA, flashInfo.rxBufferA); //write buffer to flash using DMA
			flashInfo.currentWriteAddress += FLASH_CHIP_BUFFER_WRITE_DATA_SIZE; //add pointer to address
			flashInfo.txBufferBPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.bufferStatus = BUFFER_STATUS_FILLING_A;
			if (flashInfo.currentWriteAddress >= flashInfo.totalSize)
				flashInfo.enabled = 0; //check if flash is full. Disable flash if it is full
		}

	}

}


void UpdateBlackbox(pid_output *flightPids, float flightSetPoints[] ) {

	if (curvedRcCommandF[AUX2] > 1500) {
		ledStatus.status = LEDS_FAST_BLINK;
		LoggingEnabled = 1;
	} else {
		ledStatus.status = LEDS_SLOW_BLINK;
		LoggingEnabled = 0;
	}

	if ( (LoggingEnabled) && (flashInfo.enabled) ) {

		if (firstLogging) {

			firstLogging = 0;

			DumbWriteToFlash(  'y' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  'y' );
			DumbWriteToFlash(  'i' );
			DumbWriteToFlash(  'y' );
			DumbWriteToFlash(  'd' );

			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'i' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'd' );

			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  'i' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  'd' );

			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'y' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  'p' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  't' );

			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'y' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'r' );
			DumbWriteToFlash(  's' );
			DumbWriteToFlash(  'p' );

		} else {

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

	}
}
