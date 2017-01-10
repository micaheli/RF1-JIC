#include "includes.h"

volatile uint32_t telemEnabled  = 0;
volatile uint32_t lastTimeSPort = 0;
volatile uint32_t okToSendSPort = 0;
volatile uint32_t sPortExtiSet  = 0;

volatile uint8_t telemtryRxBuffer[10];
volatile uint32_t telemtryRxBufferIdx;

motor_type sbusActuator;
uint32_t timeToSend = 0;
uint32_t readyToSend = 0;
uint8_t sPortPacket[8];

void ProcessTelemtry(void) {

	//This function is run by the task manager quite often.
	//is telemetry bidirectional or unidirectional

	//if bidirectional, which serial do we listen to?
	//if line is idle, check the data in it's RX buffer
	//check if it's time and what we need to send

	//if unidirectional, check if it's safe to send
	//if it's time to send figure out what to send and send it.

	//SoftSerialReceiveBlocking(uint8_t inBuffer[], motor_type actuator, uint32_t timeoutMs, uint32_t baudRate, uint32_t bitLength, uint32_t inverted);
}


void InitTelemtry(void)
{

	if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS_SPORT)
		InitSoftSport();
	//	InitAllowedSoftOutputs();

	//if (mainConfig.rcControlsConfig.rxProtcol == USING_SPEKTRUM_TWO_WAY)
	//	InitSpektrumTelemetry();

}

void TelemtryRxCallback(uint8_t serialBuffer[], uint32_t outputLength)
{

	volatile uint32_t switchIt = 0;

	if (outputLength > 0)
	{
		if ( (serialBuffer[0] == 0x7E) && (serialBuffer[1] == 0x1B) )
		{
			if (switchIt)
			{
				ledStatus.status = LEDS_OFF;
				DoLed(0, 0);
				switchIt = 0;
			}
			else
			{
				ledStatus.status = LEDS_OFF;
				DoLed(0, 1);
				switchIt = 1;
			}
			//sport output happens here
			//SoftSerialReceiveNonBlocking(board.motors[outputNumber]);
			SmartPortSendPackage(0x0700, (int32_t)(filteredAccData[ACCX] * 100), sPortPacket );
			//SoftSerialSendNonBlocking(sPortPacket, 8, sbusActuator);
			//send data, line will go back to receive state after data is sent
			//SoftSerialReceiveNonBlocking(sbusActuator);
			return;
		}
	}

	//SoftSerialReceiveNonBlocking(sbusActuator);


}

void TelemtryTxCallback(uint8_t serialBuffer[], uint32_t outputLength)
{
	(void)(serialBuffer);
	(void)(outputLength);
}

void InitSoftSport(void)
{

	//set RX callback to Send sbus data if
	//if
	uint32_t actuatorNumOutput;
	uint32_t actuatorNumCheck;
	uint32_t okayToEnable = 1;
	uint32_t outputNumber;

	for (actuatorNumOutput = 0; actuatorNumOutput < MAX_MOTOR_NUMBER; actuatorNumOutput++)
	{
		outputNumber = mainConfig.mixerConfig.motorOutput[actuatorNumOutput];
		switch (board.motors[outputNumber].enabled)
		{
			case ENUM_ACTUATOR_TYPE_WS2812:
			case ENUM_ACTUATOR_TYPE_SPORT:
				for (actuatorNumCheck = 0; actuatorNumCheck < MAX_MOTOR_NUMBER; actuatorNumCheck++) { //make sure soft sport and soft ws2812 don't interfer with active motor configuration

					if (DoesDmaConflictWithActiveDmas(board.motors[outputNumber]))
					{
						okayToEnable = 0;
					}

				}
				if (okayToEnable)
				{ //this actuator is safe to enable

					if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_SPORT)
					{
						//TODO: make telemetry and soft serial setup smarter

						sbusActuator = board.motors[outputNumber];
						//buffer location to write to, buffer index, actuator to RX on.
						//once line idle occurs the callback function will be called
						//set rx and tx callbacks.
						//callbackFunctionArray[board.motors[outputNumber].EXTICallback] = TelemtryRxCallback;
						//callbackFunctionArray[board.motors[outputNumber].DmaCallback]  = TelemtryTxCallback;
						softserialCallbackFunctionArray[0] = TelemtryRxCallback; //this function is called once line idle is sensed after data reception
						SoftSerialReceiveNonBlocking(board.motors[outputNumber], 57600, 10, 1);

						//PutSoftSerialActuatorInReceiveState(board.motors[outputNumber]);

						softSerialEnabled = 1;
						telemEnabled      = 1;
						uint32_t currentTime = Micros();

						__disable_irq();
						//prepare soft serial buffer and index
						softSerialLastByteProcessedLocation = 0;
						softSerialCurBuf = 0;
						softSerialInd[softSerialCurBuf] = 0;
						softSerialBuf[softSerialCurBuf][softSerialInd[softSerialCurBuf]++] = currentTime;
						//prepare soft serial buffer and index
						__enable_irq();

						SetActiveDmaToActuatorDma(board.motors[outputNumber]);
						InitDmaOutputForSoftSerial(board.motors[outputNumber].enabled, board.motors[outputNumber]);
					}

				}
				break;
			default:
				break;
		}
	}
}
