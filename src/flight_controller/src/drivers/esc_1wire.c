#include "includes.h"

uint8_t serialOutBuffer[10];

void OneWireInit(void) {

	//need to deinit RX, Gyro, Flash... basically anything that uses DMA. Should make a skip for USART if it's being used for communication
	//need to reinit all this crap at the end.
	DisarmBoard();      //sets WD to 32S
	AccGyroDeinit();    //takes about 200ms maybe to run, this will also stop the flight code from running so no reason to stop that.
	BoardUsartDeinit(); //deinit all the USARTs.

	InitDmaOutputOnMotors(DMA_OUTPUT_ESC_1WIRE);

	OneWireMain(); //enter OneWireMain loop.
}

int OneWireMain(void) {
	uint32_t outputLength;
	uint8_t serialOutBuffer[10];

	while (1) {
		serialOutBuffer[0] = 'R';
		serialOutBuffer[1] = 'A';
		serialOutBuffer[2] = 'C';
		serialOutBuffer[3] = 'E';
		serialOutBuffer[4] = 'F';
		serialOutBuffer[5] = 'L';
		serialOutBuffer[6] = 'I';
		serialOutBuffer[7] = 'G';
		serialOutBuffer[8] = 'H';
		serialOutBuffer[9] = 'T';
		outputLength = 10;
		OutputSerialDma((uint8_t *)serialOutBuffer, outputLength, board.motors[0]);
		OutputSerialDma((uint8_t *)serialOutBuffer, outputLength, board.motors[1]);
		OutputSerialDma((uint8_t *)serialOutBuffer, outputLength, board.motors[2]);
		OutputSerialDma((uint8_t *)serialOutBuffer, outputLength, board.motors[3]);
		DelayMs(10);
		FeedTheDog();
		//break;
	}
	return 1;

}
void OneWireDeinit(void) {
	BoardUsartInit(); //deinit all the USARTs.
	if (!AccGyroInit(mainConfig.gyroConfig.loopCtrl)) {
		ErrorHandler(GYRO_INIT_FAILIURE);
	}
}
