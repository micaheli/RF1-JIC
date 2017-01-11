#include "includes.h"




void ProcessTelemtry(void) {


	//This function is run by the task manager quite often.
	//is telemetry bidirectional or unidirectional

	//if bidirectional, which serial do we listen to?
	//if line is idle, check the data in it's RX buffer
	//check if it's time and what we need to send

	//if unidirectional, check if it's safe to send
	//if it's time to send figure out what to send and send it.

	//SoftSerialReceiveBlocking(uint8_t inBuffer[], motor_type actuator, uint32_t timeoutMs, uint32_t baudRate, uint32_t bitLength, uint32_t inverted);

	if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS_SPORT)
	{
		if ( (sendSmartPortAt) && (sendSmartPortAt > Micros()) )
		{
			sendSmartPortAt = 0; //reset send time to 0 which disables it
			SendSmartPort();     //send the data. Blind of soft or hard s.port

		}
		else
		{
			CheckIfSportReadyToSend(); //sets sendSmartPortAt if it needs to.
		}
	}

}


void InitTelemtry(void)
{

	if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS_SPORT)
		InitSoftSport();

	if (mainConfig.rcControlsConfig.rxProtcol == USING_SPEKTRUM_TWO_WAY)
		InitSpektrumTelemetry();

}


