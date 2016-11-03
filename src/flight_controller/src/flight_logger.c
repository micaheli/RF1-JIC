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

inline void InlineWrite16To8 (uint16_t data) {
	DumbWriteToFlash(  (uint8_t)(data & 0x00ff) );
	DumbWriteToFlash(  (uint8_t)( (data & 0xff00) >>  8) );
}

inline void DumbWriteToFlash (uint8_t data) {

	if (flashInfo.bufferStatus == BUFFER_STATUS_FILLING_A) {

		flashInfo.txBufferA[flashInfo.txBufferAPtr++] = data; //add data to buffer on byte at a time.

		//check if buffer is full
		if (flashInfo.txBufferAPtr > FLASH_CHIP_BUFFER_WRITE_DATA_END) { //filled buffer
			M25p16DmaWritePage(flashInfo.currentWriteAddress, flashInfo.txBufferA, flashInfo.rxBufferA); //write buffer to flash using DMA
			flashInfo.currentWriteAddress += FLASH_CHIP_BUFFER_WRITE_DATA_SIZE; //add pointer to address
			flashInfo.txBufferAPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.bufferStatus = BUFFER_STATUS_FILLING_B;
		}

	} else if (flashInfo.bufferStatus == BUFFER_STATUS_FILLING_B) {

		flashInfo.txBufferB[flashInfo.txBufferBPtr++] = data; //add data to buffer on byte at a time.

		//check if buffer is full
		if (flashInfo.txBufferBPtr > FLASH_CHIP_BUFFER_WRITE_DATA_END) { //filled buffer
			M25p16DmaWritePage(flashInfo.currentWriteAddress, flashInfo.txBufferA, flashInfo.rxBufferA); //write buffer to flash using DMA
			flashInfo.currentWriteAddress += FLASH_CHIP_BUFFER_WRITE_DATA_SIZE; //add pointer to address
			flashInfo.txBufferBPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.bufferStatus = BUFFER_STATUS_FILLING_A;
		}

	}

}


void UpdateBlackbox(pid_output *flightPids) {

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

		} else {

			InlineWrite16To8(  (uint16_t)(flightPids[YAW].kp   * 1000) );
			InlineWrite16To8(  (uint16_t)(flightPids[YAW].ki   * 1000) );
			InlineWrite16To8(  (uint16_t)(flightPids[YAW].kd   * 1000) );
			InlineWrite16To8(  (uint16_t)(flightPids[ROLL].kp  * 1000) );
			InlineWrite16To8(  (uint16_t)(flightPids[ROLL].ki  * 1000) );
			InlineWrite16To8(  (uint16_t)(flightPids[ROLL].kd  * 1000) );
			InlineWrite16To8(  (uint16_t)(flightPids[PITCH].kp * 1000) );
			InlineWrite16To8(  (uint16_t)(flightPids[PITCH].ki * 1000) );
			InlineWrite16To8(  (uint16_t)(flightPids[PITCH].kd * 1000) );

		}

	}
}
