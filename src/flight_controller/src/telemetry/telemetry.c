#include "includes.h"

volatile uint32_t telemEnabled  = 0;
volatile uint32_t lastTimeSPort = 0;
volatile uint32_t okToSendSPort = 0;
volatile uint32_t sPortExtiSet  = 0;

volatile uint8_t telemtryRxBuffer[10];
volatile uint32_t telemtryRxBufferIdx;


void ProcessTelemtry(void) {

	//This function is run by the task manager quite often.
	//is telemetry bidirectional or unidirectional

	//if bidirectional, which serial do we listen to?
	//if line is idle, check the data in it's RX buffer
	//check if it's time and what we need to send

	//if unidirectional, check if it's safe to send
	//if it's time to send figure out what to send and send it.


}


void InitTelemtry(void) {

	//if (mainConfig.rcControlsConfig.rxProtcol == USING_SBUS_SPORT)
	//	InitAllowedSoftOutputs();

	//if (mainConfig.rcControlsConfig.rxProtcol == USING_SPEKTRUM_TWO_WAY)
	//	InitSpektrumTelemetry();

}

void TelemtryRxCallback(uint8_t serialInBuffer[], uint32_t *serialInBufferIdx) {
	(void)(serialInBuffer);
	(void)(serialInBufferIdx);
}

void InitSoftSport(void) {

	//set RX callback to Send sbus data if
	//if
	uint32_t actuatorNumOutput;
	uint32_t actuatorNumCheck;
	uint32_t okayToEnable = 1;
	uint32_t outputNumber;

	for (actuatorNumOutput = 0; actuatorNumOutput < MAX_MOTOR_NUMBER; actuatorNumOutput++) {
		outputNumber = mainConfig.mixerConfig.motorOutput[actuatorNumOutput];
		switch (board.motors[outputNumber].enabled) {
			case ENUM_ACTUATOR_TYPE_WS2812:
			case ENUM_ACTUATOR_TYPE_SPORT:
				for (actuatorNumCheck = 0; actuatorNumCheck < MAX_MOTOR_NUMBER; actuatorNumCheck++) { //make sure soft sport and soft ws2812 don't interfer with active motor configuration

					if (!DoesDmaConflictWithActiveDmas(board.motors[outputNumber])) {
						okayToEnable = 0;
					}

				}
				if (okayToEnable) { //this actuator is safe to enable

					if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_SPORT) {
						//TODO: make telemetry and soft serial setup smarter

						//buffer location to write to, buffer index, actuator to RX on.
						//once line idle occurs the callback function will be called
						softserialCallbackFunctionArray[0] = TelemtryRxCallback; //this function is called once line idle is sensed after data reception
						SoftSerialReceiveNonBlocking(telemtryRxBuffer, &telemtryRxBufferIdx, board.motors[outputNumber]);

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
