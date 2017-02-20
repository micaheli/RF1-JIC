#include "includes.h"


#define UPDATE_BB_CHAR_STRING_SIZE 10 //good for 100 days of logging
#define UPDATE_BB_TOTAL_HEADER_SIZE 256 //good for 100 days of logging
#define UPDATE_BB_DATA_SIZE 32 //good for 100 days of logging
#define HEADER      \
	"STARTLOG\n"    \
	"VER=002\n"     \
	"STYLE=RAT\n\0" \

/*
static const char blackboxHeader[] =
    "H Product:Blackbox flight recorder\n"
    "H Data version:2\n"
    "H I interval:1\n"
	"H Field I name:loopIteration,time," //2
	"axisP[2],axisI[2],axisD[2]," //5
	"axisP[0],axisI[0],axisD[0]," //8
	"axisP[1],axisI[1],axisD[1]," //11
	"rcCommand[0],rcCommand[1],rcCommand[2],rcCommand[3]," //15
	"debug[2],debug[0],debug[1]," //18
	"gyroADC[2],gyroADC[0],gyroADC[1]," //21
	"ugyroADC[2],ugyroADC[0],ugyroADC[1]," //24
	"accSmooth[2],accSmooth[0],accSmooth[1]," //27
	"motor[0],motor[1],motor[2],motor[3]\n" //31

	"H Field I signed:"    "0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
	"H Field I predictor:" "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
	"H Field I encoding:"  "1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
	"H Field P predictor:" "6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
	"H Field P encoding:"  "9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
	"H Field S name:flightModeFlags,stateFlags,failsafePhase,rxSignalReceived,rxFlightChannelsValid\n"
	"H Field S signed:0,0,0,0,0\n"
	"H Field S predictor:0,0,0,0,0\n"
	"H Field S encoding:1,1,7,7,7\n"
	"H Firmware type:Raceflight\n"
	"H Firmware revision:One\n"
	"H Firmware date:Oct 31 2015 22:44:00\n"
	"P interval:1/1\n"
	"H rcRate:100\n"
	"H minthrottle:1100\n"
	"H maxthrottle:2000\n"
	"H gyro.scale:0x41600000\n"
	"H acc_1G:1\n"
;
*/
uint32_t LoggingEnabled;
uint32_t firstLogging;
uint32_t flashAlign;
uint32_t logItteration;
int32_t  logItterationCounter;
uint32_t logRateTime;
uint32_t logStartMicros;
uint32_t flashCountdownFake;

pid_output lastFlightPids[AXIS_NUMBER];
float      lastFlightSetPoints[AXIS_NUMBER];
float      lastFilteredGyroData[AXIS_NUMBER];
float      lastDpsGyroArray[AXIS_NUMBER];
float      lastFilteredAccData[AXIS_NUMBER];
float      lastMotorOutput[AXIS_NUMBER];


int InitFlightLogger(void)
{

	flashCountdownFake = 0;
	LoggingEnabled = 0;
	firstLogging   = 1;
	flashAlign     = 0;
	logItteration  = 0;
	logStartMicros = 0;
	logRateTime    = 1000;
	bzero(lastFlightPids, sizeof(pid_output));
	bzero(lastFlightSetPoints, sizeof(lastFlightSetPoints));
	bzero(lastFilteredGyroData, sizeof(lastFilteredGyroData));
	bzero(lastDpsGyroArray, sizeof(lastDpsGyroArray));
	bzero(lastFilteredAccData, sizeof(lastFilteredAccData));
	bzero(lastMotorOutput, sizeof(lastMotorOutput));
	return (1);

}

void EnableLogging(void)
{
	LoggingEnabled = 1;
}

void DisableLogging(void)
{
	LoggingEnabled = 0;
}


 void FinishPage(void)
 {
	uint32_t remaingBytes = (flashInfo.pageSize - (flashInfo.buffer[flashInfo.bufferNum].txBufferPtr - FLASH_CHIP_BUFFER_WRITE_DATA_START) );
	for (uint32_t x=0;x<remaingBytes;x++)
	{
		WriteByteToFlash('\0');
	}
}

 void FinishBlock(uint32_t count)
 {
	uint32_t finishY = ((flashInfo.currentWriteAddress + (flashInfo.buffer[flashInfo.bufferNum].txBufferPtr - FLASH_CHIP_BUFFER_WRITE_DATA_START)) % count);
	if (finishY != 0)
	{
		for (uint32_t x=0;x<(count - finishY);x++)
		{
			WriteByteToFlash('\0');
		}
	}
}

inline void InlineWrite16To8 (int16_t data)
{
	WriteByteToFlash(  (uint8_t)( data >> 8 ) );
	WriteByteToFlash(  (uint8_t)(data & 0xff) );
}

 void WriteByteToFlash (uint8_t data)
 {

	buffer_record *buffer = &flashInfo.buffer[flashInfo.bufferNum];

	buffer->txBuffer[buffer->txBufferPtr++] = data;

	if (buffer->txBufferPtr > FLASH_CHIP_BUFFER_WRITE_DATA_END)
	{
		if (flashInfo.bufferNum == 0)
		{
			flashInfo.bufferNum = 1;
		}
		else
		{
			flashInfo.bufferNum = 0;
		}
		flashInfo.buffer[flashInfo.bufferNum].txBufferPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;

		if (flashInfo.status != DMA_DATA_WRITE_IN_PROGRESS)
		{
			//only write and increment write address is flash chip s not busy to prevent blocks of FFFFFFF
			M25p16DmaWritePage(flashInfo.currentWriteAddress, buffer->txBuffer, buffer->rxBuffer); //write buffer to flash using DMA
			flashInfo.currentWriteAddress += FLASH_CHIP_BUFFER_WRITE_DATA_SIZE; //add pointer to address
		}
		if (flashInfo.currentWriteAddress >= flashInfo.totalSize)
			flashInfo.enabled = FLASH_FULL; //check if flash is full. Disable flash if it is full

	}

}

inline int DumbWriteString(char *string, int sizeOfString)
{
	for (int x=0; x < sizeOfString; x++)
		WriteByteToFlash( string[x] );

	return sizeOfString;
}

#define STARTLOG "STARTLOG"
#define ITERATION "iteration"

uint32_t ZigzagEncode(int32_t value)
{
    return ( (uint32_t)((value << 1) ^ (value >> 31)) );
}

void BlackboxWriteUnsignedVB(uint32_t value)
{
    while (value > 127) { // one acceptable use of while loop, because 3 shifts is 0, guaranteeed P.G.
    	WriteByteToFlash((uint8_t) (value | 0x80)); // Set the high bit to mean "more bytes follow"
        value >>= 7;
    }
    WriteByteToFlash( (uint8_t)value );
}

void BlackboxWriteSignedVB(int32_t value)
{
    //ZigZag encode to make the value always positive
    BlackboxWriteUnsignedVB(ZigzagEncode(value));
}

void UpdateBlackbox(pid_output flightPids[], float flightSetPoints[], float dpsGyroArray[], float filteredGyroData[], float filteredAccData[] )
{

	uint32_t        finishX;
	static uint32_t loggingStartedLatch = 0;
	static int32_t  disarmLast = 0;
	uint32_t        recordJunkData = 0;

	pid_output currFlightPids[AXIS_NUMBER];
	float      currFlightSetPoints[AXIS_NUMBER];
	float      currFilteredGyroData[AXIS_NUMBER];
	float      currDpsGyroArray[AXIS_NUMBER];
	float      currFilteredAccData[AXIS_NUMBER];
	float      currMotorOutput[4];

	if ( (mainConfig.rcControlsConfig.rcCalibrated) && (boardArmed) && (ModeActive(M_LOGGING)) && (flashInfo.enabled == FLASH_ENABLED) )
	{
		ledStatus.status    = LEDS_FASTER_BLINK;
		LoggingEnabled      = 1;
		loggingStartedLatch = 1;
		disarmLast          = 25;
		recordJunkData      = 0;
	}
	else if ( (mainConfig.rcControlsConfig.rcCalibrated) && (boardArmed) && (!ModeSet(M_LOGGING)) && (flashInfo.enabled == FLASH_ENABLED) )
	{
		ledStatus.status    = LEDS_FASTER_BLINK;
		LoggingEnabled      = 1;
		loggingStartedLatch = 1;
		disarmLast          = 25;
		recordJunkData      = 0;
	}
	else if ( flashCountdownFake > 1 )
	{
		flashCountdownFake--;
		ledStatus.status    = LEDS_FASTER_BLINK;
		LoggingEnabled      = 1;
		loggingStartedLatch = 1;
		disarmLast          = 25;
		recordJunkData      = 0;
	}
	else
	{
		if (boardArmed)
			ledStatus.status = LEDS_MED_BLINK;
		else
			ledStatus.status = LEDS_SLOW_BLINK;

		if (disarmLast-- < 1)
		{
			disarmLast = -1;
			LoggingEnabled = 0;
			firstLogging = 1;
			if (loggingStartedLatch)
			{
				loggingStartedLatch = 0;
				FinishPage();
			}
		}
		recordJunkData = 1;
	}


	if ( (LoggingEnabled) && (flashInfo.enabled == FLASH_ENABLED) )
	{
		logItteration++;

		if (firstLogging)
		{
			logStartMicros = Micros();

			flashInfo.buffer[0].txBufferPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.buffer[1].txBufferPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
			flashInfo.buffer[0].rxBufferPtr = 0;
			flashInfo.buffer[1].rxBufferPtr = 0;
			flashInfo.bufferNum             = 0;

			finishX = (flashInfo.currentWriteAddress % UPDATE_BB_DATA_SIZE);
			if (finishX != 0)
			{
				flashInfo.currentWriteAddress += (UPDATE_BB_DATA_SIZE - finishX);
			}

			DumbWriteString(HEADER, strlen(HEADER)+1);
			//DumbWriteString(blackboxHeader, strlen(blackboxHeader)+1);
			//FinishPage();
			firstLogging = 0;
			logItterationCounter = 0;

		}
		else
		{

			
			if(logItterationCounter < 1)
			{
				logItterationCounter = 1;	//TODO make this configurable value. Capture rate = 1khz/value

				//average all values
				for (finishX = 0; finishX < AXIS_NUMBER; finishX++)
				{
					currFlightPids[finishX].kp    = ( (flightPids[finishX].kp + lastFlightPids[finishX].kp) * 0.5);
					currFlightPids[finishX].ki    = ( (flightPids[finishX].ki + lastFlightPids[finishX].ki) * 0.5);
					currFlightPids[finishX].kd    = ( (flightPids[finishX].kd + lastFlightPids[finishX].kd) * 0.5);

					currFlightSetPoints[finishX]  = ( (flightSetPoints[finishX] + lastFlightSetPoints[finishX]) * 0.5);

					currFilteredGyroData[finishX] = ( (filteredGyroData[finishX] + lastFilteredGyroData[finishX]) * 0.5);

					currDpsGyroArray[finishX]     = ( (dpsGyroArray[finishX] + lastDpsGyroArray[finishX]) * 0.5);

					currFilteredAccData[finishX]  = ( (filteredAccData[finishX] + lastFilteredAccData[finishX]) * 0.5);
				}

				currMotorOutput[0]      = ( (motorOutput[0] + lastMotorOutput[0]) * 0.5);
				currMotorOutput[1]      = ( (motorOutput[1] + lastMotorOutput[1]) * 0.5);
				currMotorOutput[2]      = ( (motorOutput[2] + lastMotorOutput[2]) * 0.5);
				currMotorOutput[3]      = ( (motorOutput[3] + lastMotorOutput[3]) * 0.5);

				//copy current value to last values.
				memcpy(lastFlightPids, flightPids, sizeof(pid_output));

				memcpy(lastFlightSetPoints, flightSetPoints, sizeof(lastFlightSetPoints));
				memcpy(lastFilteredGyroData, filteredGyroData, sizeof(lastFilteredGyroData));
				memcpy(lastDpsGyroArray, dpsGyroArray, sizeof(lastDpsGyroArray));
				memcpy(lastFilteredAccData, filteredAccData, sizeof(lastFilteredAccData));

				lastMotorOutput[0] = motorOutput[0];
				lastMotorOutput[1] = motorOutput[1];
				lastMotorOutput[2] = motorOutput[2];
				lastMotorOutput[3] = motorOutput[3];

				//write iframe
				WriteByteToFlash( 'I' );

				BlackboxWriteUnsignedVB(logItteration);
				BlackboxWriteUnsignedVB(logStartMicros + (logItteration * logRateTime) );

				if (recordJunkData)
				{
					BlackboxWriteSignedVB( (int32_t)(999) );
					BlackboxWriteSignedVB( (int32_t)(999) );
					BlackboxWriteSignedVB( (int32_t)(999) );

					BlackboxWriteSignedVB( (int32_t)(rx_timeout) );
					BlackboxWriteSignedVB( (int32_t)(rx_timeout) );
					BlackboxWriteSignedVB( (int32_t)(rx_timeout) );

					BlackboxWriteSignedVB( (int32_t)(activeModes) );
					BlackboxWriteSignedVB( (int32_t)(activeModes) );
					BlackboxWriteSignedVB( (int32_t)(activeModes) );
				}
				else
				{
					BlackboxWriteSignedVB( (int32_t)(currFlightPids[YAW].kp           * 1000) );
					BlackboxWriteSignedVB( (int32_t)(currFlightPids[YAW].ki           * 1000) );
					BlackboxWriteSignedVB( (int32_t)(currFlightPids[YAW].kd           * 1000) );

					BlackboxWriteSignedVB( (int32_t)(currFlightPids[ROLL].kp          * 1000) );
					BlackboxWriteSignedVB( (int32_t)(currFlightPids[ROLL].ki          * 1000) );
					BlackboxWriteSignedVB( (int32_t)(currFlightPids[ROLL].kd          * 1000) );

					BlackboxWriteSignedVB( (int32_t)(currFlightPids[PITCH].kp         * 1000) );
					BlackboxWriteSignedVB( (int32_t)(currFlightPids[PITCH].ki         * 1000) );
					BlackboxWriteSignedVB( (int32_t)(currFlightPids[PITCH].kd         * 1000) );
				}

				//-1 to 1 to -500 to 500
				BlackboxWriteSignedVB( (int32_t)( ((smoothedRcCommandF[YAW])      * 500)) ); //20
				BlackboxWriteSignedVB( (int32_t)( ((smoothedRcCommandF[ROLL])     * 500)) ); //22
				BlackboxWriteSignedVB( (int32_t)( ((smoothedRcCommandF[PITCH])    * 500)) ); //24
				//-1 to 1 to 1000 to 2000
				BlackboxWriteSignedVB( (int32_t)( ((smoothedRcCommandF[THROTTLE] + 1) * 500) + 1000) ); //26

				//-2000.0 to 2000.0 DPS
				BlackboxWriteSignedVB( (int32_t)(currFlightSetPoints[YAW]    * 16.4) ); //28
				BlackboxWriteSignedVB( (int32_t)(currFlightSetPoints[ROLL]   * 16.4) ); //30
				BlackboxWriteSignedVB( (int32_t)(currFlightSetPoints[PITCH]  * 16.4) ); //32

				//-2000.0 to 2000.0 DPS
				//BlackboxWriteSignedVB( (int32_t)(currDpsGyroArray[YAW]       * 16.4) ); //34
				//BlackboxWriteSignedVB( (int32_t)(currDpsGyroArray[ROLL]      * 16.4) ); //36
				//BlackboxWriteSignedVB( (int32_t)(currDpsGyroArray[PITCH]     * 16.4) ); //38
				(void)(dpsGyroArray);
				(void)(currDpsGyroArray);

				//-2000.0 to 2000.0 DPS
				BlackboxWriteSignedVB( (int32_t)(currFilteredGyroData[YAW]   * 16.4) ); //40
				BlackboxWriteSignedVB( (int32_t)(currFilteredGyroData[ROLL]  * 16.4) ); //42
				BlackboxWriteSignedVB( (int32_t)(currFilteredGyroData[PITCH] * 16.4) ); //44

				//-16 to 16 Gees to crappy number
				BlackboxWriteSignedVB( (int32_t)(currFilteredAccData[ACCX]   * 2048) ); //46
				BlackboxWriteSignedVB( (int32_t)(currFilteredAccData[ACCY]   * 2048) ); //48
				BlackboxWriteSignedVB( (int32_t)(currFilteredAccData[ACCZ]   * 2048) ); //50

				//0 TO 1 to 1000 to 2000
				BlackboxWriteSignedVB( (int32_t)( (currMotorOutput[0] + 1)   * 1000) ); //52
				BlackboxWriteSignedVB( (int32_t)( (currMotorOutput[1] + 1)   * 1000) ); //54
				BlackboxWriteSignedVB( (int32_t)( (currMotorOutput[2] + 1)   * 1000) ); //56
				BlackboxWriteSignedVB( (int32_t)( (currMotorOutput[3] + 1)   * 1000) ); //58
			}

			logItterationCounter--;

/*
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
*/
		}

	}
	else
	{
		ledStatus.status = LEDS_SLOW_BLINK;
	}
}
