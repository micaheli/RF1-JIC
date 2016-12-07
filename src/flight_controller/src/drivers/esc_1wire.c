#include "includes.h"

uint8_t serialOutBuffer[10];

void OneWireInit(void) {

	//need to deinit RX, Gyro, Flash... basically anything that uses DMA. Should make a skip for USART if it's being used for communication
	//need to reinit all this crap at the end.
	DisarmBoard();      //sets WD to 32S
	AccGyroDeinit();    //takes about 200ms maybe to run, this will also stop the flight code from running so no reason to stop that.
	DeInitBoardUsarts(); //deinit all the USARTs.
	DeInitActuators();  //deinit all the Actuators.

//	InitDmaOutputOnMotors(DMA_OUTPUT_ESC_1WIRE);

	OneWireMain(); //enter OneWireMain loop.
}

int OneWireMain(void) {
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
