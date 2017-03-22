#include "includes.h"


volatile uint32_t sendSmartPortAt = 0;
volatile uint32_t sendSmartPortLuaAt = 0;
volatile uint32_t sendSpektrumTelemtryAt = 0;

volatile uint32_t telemEnabled = 1;

volatile vtx_record vtxRequested;
volatile vtx_record vtxRecord;

static const uint32_t vtxBandChannelToFrequencyLookup[] = {
	5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, //Boscam A
	5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, //Boscam B
	5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, //Boscam E
	5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, //FatShark
	5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, //RaceBand
};

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

uint32_t VtxBandChannelToFrequency(uint32_t bandChannel)
{
	return(vtxBandChannelToFrequencyLookup[bandChannel]);
}

void VtxChannelToBandAndChannel(uint32_t inChannel, volatile uint32_t *vtxBand, volatile uint32_t *channel)
{

	uint32_t bandMultiplier;

	bandMultiplier = (inChannel) / 8;

	(*vtxBand) = bandMultiplier;
	(*channel) = inChannel - (bandMultiplier * 8);

}

uint32_t VtxBandAndChannelToBandChannel(volatile uint32_t vtxBand, volatile uint32_t channel)
{

	return ( (channel + (8 * vtxBand)) );

}


uint32_t VtxTurnOn(void)
{
	uint32_t returnValue;
	static uint32_t mutex = 0;
	if (mutex)
		return(0);
	mutex = 1;

	switch(vtxRecord.vtxDevice)
	{
		case VTX_DEVICE_SMARTV1:
		case VTX_DEVICE_SMARTV2:
			returnValue = SmartAudioVtxTurnOn();
			mutex = 0;
			return( returnValue );
			break;
		case VTX_DEVICE_NONE:
		default:
			return(0);
			break;
	}

	return(0);

}

uint32_t VtxTurnPit(void)
{
	uint32_t returnValue;
	static uint32_t mutex = 0;
	if (mutex)
		return(0);
	mutex = 1;

	switch(vtxRecord.vtxDevice)
	{
		case VTX_DEVICE_SMARTV1:
		case VTX_DEVICE_SMARTV2:
			returnValue = SmartAudioVtxTurnPit();
			mutex = 0;
			return( returnValue );
			break;
		case VTX_DEVICE_NONE:
		default:
			return(0);
			break;
	}

	return(0);

}

uint32_t VtxBandChannel(uint32_t bandChannel)
{
	uint32_t returnValue;
	static uint32_t mutex = 0;
	if (mutex)
		return(0);
	mutex = 1;

	switch(vtxRecord.vtxDevice)
	{
		case VTX_DEVICE_SMARTV1:
		case VTX_DEVICE_SMARTV2:
			returnValue = SmartAudioVtxBandChannel(bandChannel);
			mutex = 0;
			return( returnValue );
			break;
		case VTX_DEVICE_NONE:
		default:
			return(0);
			break;
	}

	return(0);

}

uint32_t VtxPower(uint32_t power)
{
	uint32_t returnValue;
	static uint32_t mutex = 0;
	if (mutex)
		return(0);
	mutex = 1;

	switch(vtxRecord.vtxDevice)
	{
		case VTX_DEVICE_SMARTV1:
		case VTX_DEVICE_SMARTV2:
			returnValue = SmartAudioVtxPower(power);
			mutex = 0;
			return( returnValue );
			break;
		case VTX_DEVICE_NONE:
		default:
			return(0);
			break;
	}

	return(0);

}

void InitTelemtry(void)
{

	vtxRecord.vtxDevice      = VTX_DEVICE_NONE;

	//try twice to init smart audi if it's enabled
	InitSmartAudio();
    if(mainConfig.telemConfig.telemSmartAudio && !vtxRecord.vtxDevice)
    {
    	DelayMs(1500);
    	InitSmartAudio();
    }

	if (vtxRecord.vtxDevice != VTX_DEVICE_NONE)
	{
		vtxRequested.vtxDevice      = vtxRecord.vtxDevice;
		vtxRequested.vtxBand        = vtxRecord.vtxBand;
		vtxRequested.vtxChannel     = vtxRecord.vtxChannel;
		vtxRequested.vtxBandChannel = vtxRecord.vtxBandChannel;
		vtxRequested.vtxPower       = vtxRecord.vtxPower;
		vtxRequested.vtxPit         = vtxRecord.vtxPit;
		vtxRequested.vtxRegion      = vtxRecord.vtxRegion;
		vtxRequested.vtxFrequency   = vtxRecord.vtxFrequency;
	}

	InitAllSport();

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


