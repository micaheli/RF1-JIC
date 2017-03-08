#include "includes.h"


volatile uint32_t sendSmartPortAt = 0;
volatile uint32_t sendSmartPortLuaAt = 0;
volatile uint32_t sendSpektrumTelemtryAt = 0;

volatile uint32_t telemEnabled = 1;

volatile uint32_t vtxRecord;

void ProcessTelemtry(void)
{

	if (!telemEnabled)
		return;
	//This function is run by the task manager quite often.
	//is telemetry bidirectional or unidirectional

	//if bidirectional, which serial do we listen to?
	//if line is idle, check the data in it's RX buffer
	//check if it's time and what we need to send

	//if unidirectional, check if it's safe to send
	//if it's time to send figure out what to send and send it.

	//SoftSerialReceiveBlocking(uint8_t inBuffer[], motor_type actuator, uint32_t timeoutMs, uint32_t baudRate, uint32_t bitLength, uint32_t inverted);

	if (mainConfig.telemConfig.telemSport)
	{
		if ( (sendSmartPortAt) && ( sendSmartPortAt > Micros() ) )
		{
			sendSmartPortAt = 0; //reset send time to 0 which disables it
			SendSmartPort();     //send the data. Blind of soft or hard s.port

		}
		else if ( (sendSmartPortLuaAt) && ( sendSmartPortLuaAt > Micros() ) )
		{
			sendSmartPortLuaAt = 0; //reset send time to 0 which disables it
			SendSmartPortLua();     //send the data. Blind of soft or hard s.port

		}
		else
		{
			CheckIfSportReadyToSend(); //sets sendSmartPortAt if it needs to.
		}
	}

	if (mainConfig.telemConfig.telemSpek)
	{
		if ( (sendSpektrumTelemtryAt) && ( sendSpektrumTelemtryAt > Micros() ) )
		{
			sendSpektrumTelemtryAt = 0; //reset send time to 0 which disables it
			sendSpektrumTelem();     //send the data. Blind of soft or hard s.port
		}
	}
}

void InitMavlink(uint32_t serialPort)
{
	(void)(serialPort);
}

void InitTelemtry(void)
{

	uint32_t successNote;

/*
	vtxRecord.vtxDevice = VTX_DEVICE_NONE;

	//init SA and fill vtxRecord if successful
	switch(mainConfig.telemConfig.telemSmartAudio)
	{
		case TELEM_ACTUATOR1:
		case TELEM_ACTUATOR2:
		case TELEM_ACTUATOR3:
		case TELEM_ACTUATOR4:
		case TELEM_ACTUATOR5:
		case TELEM_ACTUATOR6:
		case TELEM_ACTUATOR7:
		case TELEM_ACTUATOR8:
			successNote = InitSoftSmartAudio();
			break;
		case TELEM_USART1:
			successNote = InitSmartAudio(ENUM_USART1);
			break;
		case TELEM_USART2:
			successNote = InitSmartAudio(ENUM_USART2);
			break;
		case TELEM_USART3:
			successNote = InitSmartAudio(ENUM_USART3);
			break;
		case TELEM_USART4:
			successNote = InitSmartAudio(ENUM_USART4);
			break;
		case TELEM_USART5:
			successNote = InitSmartAudio(ENUM_USART5);
			break;
		case TELEM_USART6:
			successNote = InitSmartAudio(ENUM_USART6);
			break;
		default:
			break;
	}
*/
	switch(mainConfig.telemConfig.telemSport)
	{
		case TELEM_ACTUATOR1:
		case TELEM_ACTUATOR2:
		case TELEM_ACTUATOR3:
		case TELEM_ACTUATOR4:
		case TELEM_ACTUATOR5:
		case TELEM_ACTUATOR6:
		case TELEM_ACTUATOR7:
		case TELEM_ACTUATOR8:
			InitSoftSport();
			break;
		case TELEM_USART1:
			InitSport(ENUM_USART1);
			break;
		case TELEM_USART2:
			InitSport(ENUM_USART2);
			break;
		case TELEM_USART3:
			InitSport(ENUM_USART3);
			break;
		case TELEM_USART4:
			InitSport(ENUM_USART4);
			break;
		case TELEM_USART5:
			InitSport(ENUM_USART5);
			break;
		case TELEM_USART6:
			InitSport(ENUM_USART6);
			break;
		default:
			break;
	}

	switch(mainConfig.telemConfig.telemMsp)
	{
		//soft msp not supported right now
		case TELEM_ACTUATOR1:
		case TELEM_ACTUATOR2:
		case TELEM_ACTUATOR3:
		case TELEM_ACTUATOR4:
		case TELEM_ACTUATOR5:
		case TELEM_ACTUATOR6:
		case TELEM_ACTUATOR7:
		case TELEM_ACTUATOR8:
			break;
		case TELEM_USART1:
			InitMsp(ENUM_USART1);
			break;
		case TELEM_USART2:
			InitMsp(ENUM_USART2);
			break;
		case TELEM_USART3:
			InitMsp(ENUM_USART3);
			break;
		case TELEM_USART4:
			InitMsp(ENUM_USART4);
			break;
		case TELEM_USART5:
			InitMsp(ENUM_USART5);
			break;
		case TELEM_USART6:
			InitMsp(ENUM_USART6);
			break;
		default:
			break;
	}

	switch(mainConfig.telemConfig.telemMav)
	{
		//soft msp not supported right now
		case TELEM_ACTUATOR1:
		case TELEM_ACTUATOR2:
		case TELEM_ACTUATOR3:
		case TELEM_ACTUATOR4:
		case TELEM_ACTUATOR5:
		case TELEM_ACTUATOR6:
		case TELEM_ACTUATOR7:
		case TELEM_ACTUATOR8:
			break;
		case TELEM_USART1:
			InitMavlink(ENUM_USART1);
			break;
		case TELEM_USART2:
			InitMavlink(ENUM_USART2);
			break;
		case TELEM_USART3:
			InitMavlink(ENUM_USART3);
			break;
		case TELEM_USART4:
			InitMavlink(ENUM_USART4);
			break;
		case TELEM_USART5:
			InitMavlink(ENUM_USART5);
			break;
		case TELEM_USART6:
			InitMavlink(ENUM_USART6);
			break;
		default:
			break;
	}

	//not needed
	if (mainConfig.telemConfig.telemSpek)
		InitSpektrumTelemetry();

}


