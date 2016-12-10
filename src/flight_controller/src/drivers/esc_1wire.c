#include "includes.h"

uint8_t serialOutBuffer[256];
uint32_t timeInBuffer[256];
volatile uint32_t timeInBufferIdx = 0;

uint32_t currentEXTIPin;

void OneWireInit(void) {

	//need to deinit RX, Gyro, Flash... basically anything that uses DMA. Should make a skip for USART if it's being used for communication
	//need to reinit all this crap at the end.
	DisarmBoard();              //sets WD to 32S
	AccGyroDeinit();            //takes about 200ms maybe to run, this will also stop the flight code from running so no reason to stop that.
	DeInitBoardUsarts();        //deinit all the USARTs.
	DeInitActuators();          //deinit all the Actuators.
	DeInitAllowedSoftOutputs(); //deinit all the soft outputs

	OneWireMain(); //enter OneWireMain loop.

}

void SendHello(motor_type actuator) {
	uint32_t outputLength=sizeof("RACEFLIGHT");
	memcpy(serialOutBuffer,"RACEFLIGHT",outputLength);
	OutputSerialDmaByte(serialOutBuffer, outputLength, actuator, 0, 1);
}

void OneWireCallback(void) {
	uint32_t actuatorNumOutput;

	/* EXTI line interrupt detected */
	if(__HAL_GPIO_EXTI_GET_IT(currentEXTIPin) != RESET)
	{
		//timeInBuffer[timeInBufferIdx++] = Micros();
		//if ((timeInBufferIdx == 49) && (timeInBuffer[0] >0) )
		//	timeInBufferIdx = 0;

		__HAL_GPIO_EXTI_CLEAR_IT(currentEXTIPin);
	}
}

int OneWireMain(void) {

	uint32_t actuatorNumOutput;

	while (1) {
		for (actuatorNumOutput = 0; actuatorNumOutput < MAX_MOTOR_NUMBER; actuatorNumOutput++) {
			if (board.motors[actuatorNumOutput].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
				SetActiveDmaToActuatorDma(board.motors[actuatorNumOutput]);
				InitDmaOutputForSoftSerial(DMA_OUTPUT_ESC_1WIRE, board.motors[actuatorNumOutput]); //setup first motor for DMA output
				DelayMs(10); //hold line high for 10 ms
				SendHello(board.motors[actuatorNumOutput]);
				DelayMs(10); //hold line high for 10 ms starting from begining of message
				DeInitDmaOutputForSoftSerial(board.motors[actuatorNumOutput]);
				callbackFunctionArray[board.motors[actuatorNumOutput].EXTICallback]   = OneWireCallback;
				currentEXTIPin = board.motors[actuatorNumOutput].pin;
				EXTI_Init(ports[board.motors[actuatorNumOutput].port], board.motors[actuatorNumOutput].pin, board.motors[actuatorNumOutput].EXTIn, 1, 2, GPIO_MODE_IT_RISING_FALLING, GPIO_PULLUP);
				DelayMs(1);
			}
		}
	}

	return (0);
	uint32_t outputLength;
	uint8_t serialOutBuffer[256];
	//uint8_t *serialOutBuffer;

	outputLength=sizeof("RACEFLIGHT FTW!!!");

	InitOdd(board.motors[0]);
	//InitDmaInputOnMotors(board.motors[0]);

	while (1) {
		memcpy(serialOutBuffer,"RACEFLIGHT FTW!!!",outputLength);
		//outputLength=1;
		//serialOutBuffer[0] = BitReverse8(0);
		//OutputSerialDmaByte((uint8_t *)serialOutBuffer, outputLength, board.motors[3]);
		//serialOutBuffer[0] = BitReverse8(1);
		//OutputSerialDmaByte((uint8_t *)serialOutBuffer, outputLength, board.motors[3]);
		//serialOutBuffer[0] = BitReverse8(2);
		//OutputSerialDmaByte((uint8_t *)serialOutBuffer, outputLength, board.motors[3]);
		//serialOutBuffer[0] = BitReverse8('C');
		//OutputSerialDmaByte((uint8_t *)serialOutBuffer, outputLength, board.motors[3]);
		//DelayMs(10);
		//FeedTheDog();
/*
		serialOutBuffer[0] = BitReverse8(0);
		serialOutBuffer[1] = BitReverse8(1);
		serialOutBuffer[2] = BitReverse8(2);
		serialOutBuffer[3] = BitReverse8(3);
		serialOutBuffer[4] = BitReverse8(4);
		serialOutBuffer[5] = BitReverse8(5);
		serialOutBuffer[6] = BitReverse8(6);
		serialOutBuffer[7] = BitReverse8('C');
		serialOutBuffer[8] = BitReverse8('A');
		serialOutBuffer[9] = BitReverse8('T');
		serialOutBuffer[0] = BitReverse8(0);

		outputLength = 10;
*/
		//OutputSerialDma((uint8_t *)serialOutBuffer, outputLength, board.motors[0]);
		//OutputSerialDma((uint8_t *)serialOutBuffer, outputLength, board.motors[1]);
		//OutputSerialDma((uint8_t *)serialOutBuffer, outputLength, board.motors[2]);
//		OutputSerialDmaByte(serialOutBuffer, outputLength, board.motors[3]);
		DelayMs(20);
		FeedTheDog();
		//break;

	}

	return 1;

}

void OneWireDeinit(void) {
	DeInitBoardUsarts(); //deinit all the USARTs.
	DeInitActuators();  //deinit all the Actuators.
	InitActuators();    //init all the Actuators.
	InitBoardUsarts();   //init all the USARTs.
	if (!AccGyroInit(mainConfig.gyroConfig.loopCtrl)) {
		ErrorHandler(GYRO_INIT_FAILIURE);
	}
}
