#include "includes.h"

volatile uint32_t telemEnabled  = 0;
volatile uint32_t lastTimeSPort = 0;
volatile uint32_t okToSendSPort = 0;
volatile uint32_t sPortExtiSet  = 0;




void ProcessTelemtry(void) {

	//This function is run by the task manager quite often.
	//is telemetry bidirectional or unidirectional

	//if bidirectional, which serial do we listen to?
	//if line is idle, check the data in it's RX buffer
	//check if it's time and what we need to send

	//if unidirectional, check if it's safe to send
	//if it's time to send figure out what to send and send it.


}
