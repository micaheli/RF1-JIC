#include "includes.h"


esc_one_wire_status escOneWireStatus[16];


// EEPROM layout - BLHeli rev 20, bumped in 14.0
const BLHeli_EEprom_t BLHeli20_EEprom = {
    .BL_GOV_P_GAIN = 0,
    .BL_GOV_I_GAIN = 1,
    .BL_GOV_MODE = 2,
    .BL_MOT_GAIN = 4,
    .BL_STARTUP_PWR = 6,
    .BL_PWM_FREQ = 7,
    .BL_DIRECTION = 8,
    .BL_INPUT_POL = 9,
    .BL_INIT_L = 10,
    .BL_INIT_H = 11,
    .BL_ENABLE_TX = 12,
    .BL_COMM_TIMING = 18,
    .BL_PPM_MIN_THROTLE = 22,
    .BL_PPM_MAX_THROTLE = 23,
    .BL_BEEP_STRENGTH = 24,
    .BL_BEACON_STRENGTH = 25,
    .BL_BEACON_DELAY = 26,
    .BL_DEMAG_COMP = 28,
    .BL_BEC_VOLTAGE_HIGH = 29,
    .BL_PPM_CENTER = 30,
    .BL_TEMP_PROTECTION = 32,
    .BL_ENABLE_POWER_PROT = 33,
    .BL_ENABLE_PWM_INPUT = 34,
    .BL_PWM_DITHER = 35,
    .BL_BRAKE_ON_STOP = NO_CMD,
    .BL_LED_CONTROL = NO_CMD,
};
// EEPROM layout - BLHeli rev 21, bumped in 14.5
const BLHeli_EEprom_t BLHeli21_EEprom = {
    .BL_GOV_P_GAIN = 0,
    .BL_GOV_I_GAIN = 1,
    .BL_GOV_MODE = 2,
    .BL_MOT_GAIN = 4,
    .BL_STARTUP_PWR = 6,
    .BL_PWM_FREQ = 7,
    .BL_DIRECTION = 8,
    .BL_INPUT_POL = 9,
    .BL_INIT_L = 10,
    .BL_INIT_H = 11,
    .BL_ENABLE_TX = 12,
    .BL_COMM_TIMING = 18,
    .BL_PPM_MIN_THROTLE = 22,
    .BL_PPM_MAX_THROTLE = 23,
    .BL_BEEP_STRENGTH = 24,
    .BL_BEACON_STRENGTH = 25,
    .BL_BEACON_DELAY = 26,
    .BL_DEMAG_COMP = 28,
    .BL_BEC_VOLTAGE_HIGH = 29,
    .BL_PPM_CENTER = 30,
    .BL_TEMP_PROTECTION = 32,
    .BL_ENABLE_POWER_PROT = 33,
    .BL_ENABLE_PWM_INPUT = 34,
    .BL_PWM_DITHER = 35,
    .BL_BRAKE_ON_STOP = 36,
    .BL_LED_CONTROL = NO_CMD,
};
// EEPROM layout - BLHeli_S rev 32
const BLHeli_EEprom_t BLHeliS32_EEprom = {
    .BL_GOV_P_GAIN = NO_CMD,
    .BL_GOV_I_GAIN = NO_CMD,
    .BL_GOV_MODE = NO_CMD,
    .BL_MOT_GAIN = NO_CMD,
    .BL_STARTUP_PWR = 6,
    .BL_PWM_FREQ = NO_CMD,
    .BL_DIRECTION = 8,
    .BL_INPUT_POL = NO_CMD,
    .BL_INIT_L = 10,
    .BL_INIT_H = 11,
    .BL_ENABLE_TX = 12,
    .BL_COMM_TIMING = 18,
    .BL_PPM_MIN_THROTLE = 22,
    .BL_PPM_MAX_THROTLE = 23,
    .BL_BEEP_STRENGTH = 24,
    .BL_BEACON_STRENGTH = 25,
    .BL_BEACON_DELAY = 26,
    .BL_DEMAG_COMP = 28,
    .BL_BEC_VOLTAGE_HIGH = NO_CMD,
    .BL_PPM_CENTER = 30,
    .BL_TEMP_PROTECTION = 32,
    .BL_ENABLE_POWER_PROT = 33,
    .BL_ENABLE_PWM_INPUT = 34,
    .BL_PWM_DITHER = NO_CMD,
    .BL_BRAKE_ON_STOP = 36,
    .BL_LED_CONTROL = 37,
};



uint8_t  oneWireOutBuffer[ESC_BUF_SIZE];
uint8_t  oneWireInBuffer[ESC_BUF_SIZE];
volatile uint32_t oneWireInBufferIdx;
volatile uint32_t oneWireOngoing;

static const uint8_t  bootInit[] = {0, 0, 0, 0, 0, 0, 0, 0, 0x0D, 'B', 'L', 'H', 'e', 'l', 'i', 0xF4, 0x7D};
static const uint16_t signaturesAtmel[]  = {0x9307, 0x930A, 0x930F, 0x940B, 0};
static const uint16_t signaturesSilabs[] = {0xF310, 0xF330, 0xF410, 0xF390, 0xF850, 0xE8B1, 0xE8B2, 0};


static uint32_t OneWireMain() __attribute__ ((unused));
static uint32_t SignatureMatch(uint16_t signature, const uint16_t *list, uint32_t listSize);
static void     AppendBlHeliCrc(uint8_t outBuffer[], uint32_t len);
//static void     SendHello(void);
static uint16_t Crc16Byte(uint16_t from, uint8_t byte);
//static uint32_t PollReadReadyBLHeli(void);
//static uint32_t ReadFlashAtmelBLHeli(ioMem_t *ioMem);
//static uint32_t ReadFlashSiLabsBLHeli(motor_type actuator, uint16_t address, uint32_t timeout);
//static uint32_t WriteFlashBLHeli(ioMem_t *ioMem);
//static uint32_t ReadEEpromAtmelBLHeli(ioMem_t *ioMem);
//static uint32_t ReadEEpromSiLabsBLHeli(motor_type actuator, uint32_t timeout);
//static uint32_t WriteEEpromAtmelBLHeli(ioMem_t *ioMem);
//static uint32_t WriteEEpromSiLabsBLHeli(ioMem_t *ioMem);
//static uint32_t PageEraseAtmelBLHeli(ioMem_t *ioMem);
//static uint32_t PageEraseSiLabsBLHeli(ioMem_t *ioMem);
//static uint32_t EepromEraseSiLabsBLHeli(ioMem_t *ioMem);


static uint32_t ConnectToBlheliBootloader(motor_type actuator, uint32_t timeout);
static uint32_t DisconnectBLHeli(motor_type actuator, uint32_t timeout);
static uint32_t SendReadCommand(motor_type actuator, uint8_t cmd, uint16_t address, uint16_t length, uint32_t timeout) __attribute__ ((unused));
static uint32_t ReadEEpromSiLabsBLHeli(motor_type actuator, uint32_t timeout);
static uint32_t ReadFlashSiLabsBLHeli(motor_type actuator, uint16_t address, uint16_t length, uint32_t timeout);
static uint32_t SendCmdSetBuffer(motor_type actuator, uint8_t outBuffer[], uint16_t length) __attribute__ ((unused));
static uint32_t SendCmdSetAddress(motor_type actuator, uint16_t address) __attribute__ ((unused));
static uint32_t HandleEscOneWire(uint8_t serialBuffer[], uint32_t outputLength)  __attribute__ ((unused));

const BLHeli_EEprom_t* GetBLHeliEEpromLayout(uint8_t data[], uint32_t dataLength);

/*
const esc1WireProtocol_t BLHeliAtmelProtocol = {
//    .disconnect    = DisconnectBLHeli,
//    .pollReadReady = PollReadReadyBLHeli,
    .ReadFlash     = ReadFlashAtmelBLHeli,
//    .writeFlash    = WriteFlashBLHeli,
    .ReadEEprom    = ReadEEpromAtmelBLHeli,
//    .writeEEprom   = WriteEEpromAtmelBLHeli,
//    .pageErase     = PageEraseAtmelBLHeli,
//    .eepromErase   = PageEraseAtmelBLHeli,
};
*/

const esc1WireProtocol_t BLHeliSiLabsProtocol = {
    .Disconnect    = DisconnectBLHeli,
//    .pollReadReady = PollReadReadyBLHeli,
    .ReadFlash     = ReadFlashSiLabsBLHeli,
//    .writeFlash    = WriteFlashBLHeli,
    .ReadEEprom    = ReadEEpromSiLabsBLHeli,
//    .writeEEprom   = WriteEEpromSiLabsBLHeli,
//    .pageErase     = PageEraseSiLabsBLHeli,
//    .eepromErase   = EepromEraseSiLabsBLHeli,
};




uint32_t OneWireInit(void)
{

	uint32_t x;

	oneWireOngoing = 1;
	//need to deinit RX, Gyro, Flash... basically anything that uses DMA. Should make a skip for USART if it's being used for communication
	//need to reinit all this crap at the end.
	DisarmBoard();              //sets WD to 32S
	AccGyroDeinit();            //takes about 200ms maybe to run, this will also stop the flight code from running so no reason to stop that.
	DeInitBoardUsarts();        //deinit all the USARTs.
	DeInitActuators();          //deinit all the Actuators.

	DeInitAllowedSoftOutputs(); //deinit all the soft outputs


//	motor_type actuator = board.motors[0];
//				if (actuator.enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
//					volatile uint32_t cat = 0;
//					DelayMs(100); //delay while debugging since motors need time to startup.
//
//				}
//	for (x=0;x<sizeof(timeInBuffer);x++)
//		timeInBuffer[x] = 0;

	//softserialCallbackFunctionArray[0] = HandleEscOneWire;


	//OneWireMain(); //enter OneWireMain loop.

	uint32_t tries = 0;
	uint32_t allWork = 1;
	uint32_t atLeastOneWorks = 0;

	while (tries < 5) {
		allWork = 1;
		tries++;
		for (x = 0; x < MAX_MOTOR_NUMBER; x++) {
			InitDmaOutputForSoftSerial(DMA_OUTPUT_ESC_1WIRE, board.motors[x]);
		}
		DelayMs(600);
		for (x = 0; x < MAX_MOTOR_NUMBER; x++) {
			if (board.motors[x].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
				escOneWireStatus[board.motors[x].actuatorArrayNum].enabled = 0;
				if (ConnectToBlheliBootloader(board.motors[x], 35)) {
					atLeastOneWorks++;
					escOneWireStatus[board.motors[x].actuatorArrayNum].esc1WireProtocol->ReadEEprom(board.motors[x], 125);
				} else {
					allWork = 0;
				}
				DeInitDmaOutputForSoftSerial(board.motors[x]);
			}
		}
		if (allWork) //all active motors returned
		{
			return (atLeastOneWorks);
		}
		else if (atLeastOneWorks || tries < 2) //try at least twice, even if not a single ESC detected
		{
			InitActuators();
			DelayMs(7000);
			DeInitActuators();
		}
		else if (atLeastOneWorks == 0 && tries == 2 ) //tried twice, not a single ESC detected
		{
			return (0);
		}
	}

	return (atLeastOneWorks); //tried all used up, return what we've found.

	for (x = 0; x < MAX_MOTOR_NUMBER; x++) {
		//ConnectToBlheliBootloader(DMA_OUTPUT_ESC_1WIRE, board.motors[x]);
	}

	for (x = 0; x < MAX_MOTOR_NUMBER; x++) {

		//InitDmaOutputForSoftSerial(board.motors[x]);
		if (board.motors[x].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
			escOneWireStatus[board.motors[x].actuatorArrayNum].enabled = 0;
			if (ConnectToBlheliBootloader(board.motors[x], 35)) {
				//TODO: Make work with the protocol struct
				//const esc1WireProtocol_t *proto = escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].esc1WireProtocol;
				//proto->ReadEEprom(board.motors[actuatorNumOutput], 25);
				escOneWireStatus[board.motors[x].actuatorArrayNum].esc1WireProtocol->ReadEEprom(board.motors[x], 200);
			}
			DeInitDmaOutputForSoftSerial(board.motors[x]);
		}

	}

	/*
	for (x = 0; x < MAX_MOTOR_NUMBER; x++) {

		escOneWireStatus[board.motors[x].actuatorArrayNum].enabled = 0;
		if (board.motors[x].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
			if (ConnectToBlheliBootloader(board.motors[x], 35)) {
				//TODO: Make work with the protocol struct
				//const esc1WireProtocol_t *proto = escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].esc1WireProtocol;
				//proto->ReadEEprom(board.motors[actuatorNumOutput], 25);
				escOneWireStatus[board.motors[x].actuatorArrayNum].esc1WireProtocol->ReadEEprom(board.motors[x], 125);
			}
		}
	}
	*/
}

//void SendOutput(void) {
//
//	//set softSerialStatus to SENDING_DATA state
//	softSerialStatus.softSerialState = SS_SENDING_DATA;
//
//	//set the DMA callback function to the OneWireDmaCallback
//	callbackFunctionArray[softSerialStatus.currentActuator.DmaCallback] = SoftSerialDmaCallback;
//
//	//set the data to be sent
//	OutputSerialDmaByte(serialOutBuffer, softSerialStatus.dataInBuffer, softSerialStatus.currentActuator, 0, 1);
//
//}

static uint32_t ConnectToBlheliBootloader(motor_type actuator, uint32_t timeout) {

	//connect to ESC and get bootloader info

	memcpy(oneWireOutBuffer, bootInit, sizeof(bootInit));
	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(oneWireOutBuffer, sizeof(bootInit), oneWireInBuffer, actuator, timeout);

	escOneWireStatus[actuator.actuatorArrayNum].escBootloaderMode = 0;

	//if ( (oneWireInBuffer[0] == '4') && (oneWireInBuffer[1] == '7') && (oneWireInBuffer[2] == '1') )
	if ( !strncmp((char *)oneWireInBuffer, "471", 3) && (oneWireInBufferIdx > 7) )
	{
		//4, 7, 1, d, 0xE8, 0xB2, 0x06, 0x01, 0x30
		// Format = BootMsg("471c"), SIGNATURE_HIGH, SIGNATURE_LOW, BootVersion (always 6), BootPages (,ACK)
		escOneWireStatus[actuator.actuatorArrayNum].escSignature = ( (oneWireInBuffer[4] << 8) | oneWireInBuffer[5] );
		escOneWireStatus[actuator.actuatorArrayNum].bootVersion  = oneWireInBuffer[6];
		escOneWireStatus[actuator.actuatorArrayNum].bootPages    = oneWireInBuffer[7];

		if (SignatureMatch(escOneWireStatus[actuator.actuatorArrayNum].escSignature, signaturesSilabs, (sizeof(signaturesSilabs)/2)))
		{
			escOneWireStatus[actuator.actuatorArrayNum].escBootloaderMode = BLHBLM_BLHELI_SILABS;
			escOneWireStatus[actuator.actuatorArrayNum].esc1WireProtocol  = &BLHeliSiLabsProtocol;

		}
		else if (SignatureMatch(escOneWireStatus[actuator.actuatorArrayNum].escSignature, signaturesAtmel, (sizeof(signaturesAtmel)/2)))
		{
//			escOneWireStatus[actuator.actuatorArrayNum].escBootloaderMode = BLHBLM_BLHELI_ATMEL;
//			escOneWireStatus[actuator.actuatorArrayNum].esc1WireProtocol  = &BLHeliAtmelProtocol;
		}
		else
		{
			escOneWireStatus[actuator.actuatorArrayNum].escBootloaderMode = 0;
		}

	}

	return (escOneWireStatus[actuator.actuatorArrayNum].escBootloaderMode);

}

static uint32_t SendCmdSetAddress(motor_type actuator, uint16_t address) {

	if ((address == 0xffff)) {
		return (1);
	}

	oneWireOutBuffer[0] = CMD_SET_ADDRESS;
	oneWireOutBuffer[1] = 0;
	oneWireOutBuffer[2] = (address >> 8);
	oneWireOutBuffer[3] = (address & 0xff);

	//write to ESC, wait for ACK.
	AppendBlHeliCrc(oneWireOutBuffer, 4);

	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(oneWireOutBuffer, 6, oneWireInBuffer, actuator, 10); //150ms timeout is way more than we need

	if ( (oneWireInBufferIdx > 0) && (oneWireInBuffer[0] == RET_SUCCESS) ) {
		return (1);
	}

	return (0);
}

static uint32_t SendCmdSetBuffer(motor_type actuator, uint8_t outBuffer[], uint16_t length) {

	uint8_t cmdBuffer[] = {CMD_SET_BUFFER, 0, (length >> 8), (length & 0xff), 0, 0};

	//write to ESC, wait for ACK.
	AppendBlHeliCrc(cmdBuffer, 4);

	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(cmdBuffer, 6, oneWireInBuffer, actuator, 150); //150ms timeout is way more than we need

	if ( (oneWireInBufferIdx > 0) && (oneWireInBuffer[0] != RET_NONE) ) {

		oneWireInBufferIdx = SoftSerialSendReceiveBlocking(outBuffer, 6, oneWireInBuffer, actuator, 150); //150ms timeout is way more than we need

		if ( (oneWireInBufferIdx > 0) && (oneWireInBuffer[0] == RET_SUCCESS) ) {
			return (1);
		}

	}

	return (0);
}

static uint32_t ReadEEpromSiLabsBLHeli(motor_type actuator, uint32_t timeout) {

	// SiLabs has no EEPROM, just a flash section at 0x1A00

	uint32_t hasData;

	hasData = ReadFlashSiLabsBLHeli(actuator, 0x1A00, 120, timeout);

	if (hasData > 109)
	{

		escOneWireStatus[actuator.actuatorArrayNum].enabled = 1;
		bzero(escOneWireStatus[actuator.actuatorArrayNum].nameStr,      sizeof(escOneWireStatus[actuator.actuatorArrayNum].nameStr));
		bzero(escOneWireStatus[actuator.actuatorArrayNum].fwStr,        sizeof(escOneWireStatus[actuator.actuatorArrayNum].fwStr));
		bzero(escOneWireStatus[actuator.actuatorArrayNum].versionStr,   sizeof(escOneWireStatus[actuator.actuatorArrayNum].versionStr));
		memcpy( escOneWireStatus[actuator.actuatorArrayNum].nameStr,    oneWireInBuffer + 64, 16 );
		memcpy( escOneWireStatus[actuator.actuatorArrayNum].fwStr,      oneWireInBuffer + 80, 16 );
		memcpy( escOneWireStatus[actuator.actuatorArrayNum].versionStr, oneWireInBuffer + 95, 16 );
		escOneWireStatus[actuator.actuatorArrayNum].version = ( (oneWireInBuffer[0] << 8) | (oneWireInBuffer[1]) ) ;

	}
	else
	{

		escOneWireStatus[actuator.actuatorArrayNum].enabled = 0;

	}

	return(escOneWireStatus[actuator.actuatorArrayNum].enabled);

}

//static uint32_t CheckCrc(uint8_t inBuffer[], uint32_t length, uint16_t crc) {
//
//}

static uint32_t SendReadCommand(motor_type actuator, uint8_t cmd, uint16_t address, uint16_t length, uint32_t timeout) {

	uint8_t cmdBuffer[] = {cmd,(length & 0xff), 0, 0};

	if (!SendCmdSetAddress(actuator, address))
			return (0);

	AppendBlHeliCrc(cmdBuffer, 2);

	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(cmdBuffer, 4, oneWireInBuffer, actuator, timeout); //19200, reading 258 bytes should take around 135 ms

	//fills oneWireInBuffer to length 256 + 2 crc bytes
	if ( oneWireInBufferIdx == (uint32_t)(length + 2) ) { //did we get the amount of data we expected?
//		if (CheckCrc(oneWireInBuffer, length, (uint16_t)(oneWireInBuffer[length-1] >> 8) | (oneWireInBuffer[length] & 0xff) ) ) {
			//check the CRC

//cruiser
//		}
	}

//    return readBufBLHeli(esc, ioMem->data, len, true);

}

static uint32_t DisconnectBLHeli(motor_type actuator, uint32_t timeout)
{
	oneWireOutBuffer[0] = CMD_RUN;
	oneWireOutBuffer[1] = 0;

	AppendBlHeliCrc(oneWireOutBuffer, 2);

	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(oneWireOutBuffer, 4, oneWireInBuffer, actuator, timeout);

	return (oneWireInBufferIdx);
}

static uint32_t ReadFlashSiLabsBLHeli(motor_type actuator, uint16_t address, uint16_t length, uint32_t timeout) {

	if (!SendCmdSetAddress(actuator, address))
		return (0);

	oneWireOutBuffer[0] = CMD_READ_FLASH_SIL;
	oneWireOutBuffer[1] = (length & 0xff);

	AppendBlHeliCrc(oneWireOutBuffer, 2);

	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(oneWireOutBuffer, 4, oneWireInBuffer, actuator, timeout);

	return (oneWireInBufferIdx);
}

static uint32_t HandleEscOneWire(uint8_t serialBuffer[], uint32_t outputLength) {

	(void)(serialBuffer);
	(void)(outputLength);
	/*
	switch (escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].oneWireState) {

		case OW_AWAITING_BOOT_MESSAGE:
			if ( (serialBuffer[0] == '4') && (serialBuffer[1] == '7') && (serialBuffer[2] == '1') )
			{
				//4, 7, 1, d, 0xE8, 0xB2, 0x06, 0x01, 0x30
				// Format = BootMsg("471c"), SIGNATURE_HIGH, SIGNATURE_LOW, BootVersion (always 6), BootPages (,ACK)
				escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escSignature = ( (serialBuffer[4] << 8) | serialBuffer[5] );
				escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].bootVersion  = serialBuffer[6];
				escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].bootPages    = serialBuffer[7];
				escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].oneWireState = OW_AWAITING_READ_EEPROM;

				if (SignatureMatch(escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escSignature, signaturesSilabs, (sizeof(signaturesSilabs)/2)))
				{
					escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escBootloaderMode = BLHBLM_BLHELI_SILABS;
					escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].esc1WireProtocol  = &BLHeliSiLabsProtocol;

				}
				else if (SignatureMatch(escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escSignature, signaturesAtmel, (sizeof(signaturesAtmel)/2)))
				{
					escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escBootloaderMode = BLHBLM_BLHELI_ATMEL;
//					escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].esc1WireProtocol  = &BLHeliAtmelProtocol;
				}
				else
				{
					escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escBootloaderMode = 0;
				}

				//HandleEscOneWire(serialBuffer, outputLength);
			}
			else
			{
				escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].oneWireState = OW_ERROR_UNKNOWN_BOOT_MSG;
			}
			break;

		case OW_AWAITING_READ_EEPROM:
			//ioMem_t ioMem = {
			//	.addr = 0,
			//	.len  = ESC_BUF_SIZE,
			//	.data = serialOutBuffer,
			//};
			//escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].esc1WireProtocol->readEEprom(&ioMem);
			break;

		case OW_CONNECT_TO_BOOTLOADER:
			break;

		case OW_IDLE:
		case OW_ACTUATOR_READY_TO_SEND:
		case OW_SENDING_DATA:
		case OW_RECEIVING_DATA:
		default:
			break;

	}

*/
	return (0);
}


uint32_t OneWireMain() {

	uint32_t x;

	while (1) {
		for (x = 0; x < MAX_MOTOR_NUMBER; x++) {

			if (board.motors[x].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
				FeedTheDog(); //feed the dog each time we change motors
				if (ConnectToBlheliBootloader(board.motors[x], 35)) {
					escOneWireStatus[board.motors[x].actuatorArrayNum].esc1WireProtocol->ReadEEprom(board.motors[x], 125);
				} else {
					InitActuators();
					DelayMs(5000);
					DeInitActuators();
					if (ConnectToBlheliBootloader(board.motors[x], 35)) {
						escOneWireStatus[board.motors[x].actuatorArrayNum].esc1WireProtocol->ReadEEprom(board.motors[x], 125);
					}
				}
			} else {
				escOneWireStatus[board.motors[x].actuatorArrayNum].enabled = 0;
			}
			DeInitDmaOutputForSoftSerial(board.motors[x]);
		}
	}

	return (0);

}

void OneWireDeinit(void) {

	uint32_t x;

	for (x = 0; x < MAX_MOTOR_NUMBER; x++)
	{

		//if ( (board.motors[x].enabled == ENUM_ACTUATOR_TYPE_MOTOR) && (escOneWireStatus[board.motors[x].actuatorArrayNum].enabled) )
		if (board.motors[x].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
		{

			escOneWireStatus[board.motors[x].actuatorArrayNum].esc1WireProtocol->Disconnect(board.motors[x], 22);

			DeInitDmaOutputForSoftSerial(board.motors[x]);

		}

	}

	DeInitAllowedSoftOutputs(); //deinit all the soft outputs

	InitFlight();

}











const BLHeli_EEprom_t* GetBLHeliEEpromLayout(uint8_t data[], uint32_t dataLength) {

	uint32_t layoutVersion;

    if (dataLength < BLHELI_EEPROM_HEAD) {
        return 0;
    }

    layoutVersion = data[2];

    if (layoutVersion == 32 || layoutVersion == 33)
    {
        return &BLHeliS32_EEprom;
    }
    else if (layoutVersion == 20)
    {
        return &BLHeli20_EEprom;
    }
    else if (layoutVersion == 21)
    {
        return &BLHeli21_EEprom;
    }

    return NULL;

}

static uint32_t SignatureMatch(uint16_t signature, const uint16_t *list, uint32_t listSize)
{

	uint32_t x;

	for (x=0;x<listSize;x++)
	{
	    if (signature == list[x]) {
            return(x+1);
        }
    }

	return(0);

}

static uint16_t Crc16Byte(uint16_t from, uint8_t byte) {
    uint16_t crc16 = from;
    for (int i = 0; i < 8; i++) {
        if (((byte & 0x01) ^ (crc16 & 0x0001)) != 0) {
            crc16 >>= 1;
            crc16 ^= 0xA001;
        } else {
            crc16 >>= 1;
        }
        byte >>= 1;
    }
    return crc16;
}

static void AppendBlHeliCrc(uint8_t outBuffer[], uint32_t len) {

	uint16_t crc = 0;
    uint32_t i;

    for(i = 0; i < len; i++) {
        crc = Crc16Byte(crc, outBuffer[i]);
    }

	outBuffer[i] = (crc & 0xff);
	outBuffer[i+1] = (crc >> 8);

}

/*

//BLHeli Protocol
static uint16_t Crc16Byte(uint16_t from, uint8_t byte) {
    uint16_t crc16 = from;
    for (int i = 0; i < 8; i++) {
        if (((byte & 0x01) ^ (crc16 & 0x0001)) != 0) {
            crc16 >>= 1;
            crc16 ^= 0xA001;
        } else {
            crc16 >>= 1;
        }
        byte >>= 1;
    }
    return crc16;
}
//////////////////////////////////////////////////////////////////////
// reading
static int ReadByteBLHeli(motor_type actuator) {
    uint32_t btime;
    uint32_t start_time;
    if (!PollReadReadyBLHeli()) {
        return -1;
    }
    // start bit
    start_time = micros();
    btime = start_time + START_BIT_TIME;
    uint16_t bitmask = 0;
    for(int bit = 0; bit < 10; bit++) {
        while (cmp32(micros(), btime) < 0);
        if (getEscState(esc)) {
            bitmask |= (1 << bit);
        }
        btime += BIT_TIME;
    }
    // check start bit and stop bit
    if ((bitmask & (1 << 0)) || (!(bitmask & (1 << 9)))) {
        return -1;
    }
    return bitmask >> 1;
}
static uint8_t ReadBufBLHeli(escHardware_t *esc, uint8_t *pstring, int len, bool checkCrc) {
    int crc = 0;
    int c;
    RX_LED_ON;
    uint8_t lastACK = RET_NONE;
    for (int i = 0; i < len; i++) {
        if ((c = readByteBLHeli(esc)) < 0) goto timeout;
        crc = Crc16Byte(crc, c);
        pstring[i] = c;
    }
    if (checkCrc) {
        // With CRC, read 2 more for checksum
        for (int i = 0; i < 2; i++) {  // checksum 2 CRC bytes
            if ((c = readByteBLHeli(esc)) < 0) goto timeout;
            crc = Crc16Byte(crc, c);
        }
    }
    // get ack
    if ((c = readByteBLHeli(esc)) < 0) goto timeout;
    lastACK = c;
    if (checkCrc) {
        if (crc != 0) {  // CRC of correct message is 0
            lastACK = RET_ERRORCRC;
        }
    }
timeout:
    RX_LED_OFF;
    return lastACK == RET_SUCCESS;
}
static uint8_t GetAck(escHardware_t *esc, int retry) {
    int c;
    while ((c = readByteBLHeli(esc)) < 0) {
        if (--retry < 0) {    // timeout=1 -> 1 retry
            return RET_NONE;
        }
    }
    return c;
}
//////////////////////////////////////////////////////////////////////
// writing
//static void WriteByteBLHeli(uint8_t byte) {
//    // send one idle bit first (stopbit from previous byte)
//    uint16_t bitmask = (byte << 2) | (1 << 0) | (1 << 10);
//    uint32_t btime = micros();
//    while ((1)) {
//        setEscState(esc, bitmask & 1);
//        btime += BIT_TIME;
//        bitmask >>= 1;
//        if (bitmask == 0) {
//            break; // stopbit shifted out - but don't wait
//        }
//        while (cmp32(micros(), btime) < 0);
//    }
//}

static void AppendBlHeliCrc(uint8_t outBuffer[], uint32_t len) {

	uint16_t crc = 0;
    uint32_t i;

    for(i = 0; i < len; i++) {
        crc = Crc16Byte(crc, outBuffer[i]);
    }

	outBuffer[i+1] = (crc & 0xff);
	outBuffer[i+2] = (crc >> 8);

}

static void WriteBufBLHeli(uint8_t inBuffer[], uint8_t outBuffer[], uint32_t len, uint32_t appendCrc) {

	uint16_t crc = 0;
    uint32_t i;

    for(i = 0; i < len; i++) {
    	outBuffer[i] = inBuffer[i];
        crc = Crc16Byte(crc, inBuffer[i]);
    }

    if (appendCrc) {
    	outBuffer[i+1] = (crc & 0xff);
    	outBuffer[i+2] = (crc >> 8);
    }

}
//////////////////////////////////////////////////////////////////////
// commands
static uint8_t SendCmdSetAddress(ioMem_t *ioMem) { //supports only 16 bit Adr
    // skip if adr == 0xFFFF
    if ((ioMem->addr == 0xffff)) {
        return (1);
    }
    uint8_t sCMD[] = {CMD_SET_ADDRESS, 0, ioMem->addr >> 8, ioMem->addr & 0xff};

    //write to ESC, wait for ACK.
    //WriteBufBLHeli needs to write to the serialOutBuffer
    WriteBufBLHeli(sCMD, sizeof(sCMD), 1);
    SendSoftSerialActuator(softSerialStatus.currentActuator);

    return GetAck(2) == RET_SUCCESS;
}
static uint8_t SendCmdSetBuffer(escHardware_t *esc, ioMem_t *ioMem) {
    uint16_t len = ioMem->len;
    uint8_t sCMD[] = {CMD_SET_BUFFER, 0, len >> 8, len & 0xff};
    WriteBufBLHeli(sCMD, sizeof(sCMD), (1));
    if (GetAck(esc, 2) != RET_NONE) {
        return 0;
    }
    WriteBufBLHeli(ioMem->data, len, (1));
    return ( GetAck(esc, 40) == RET_SUCCESS );
}
//////////////////////////////////////////////////////////////////////
// reading and writing
static uint8_t SendReadCommand(uint8_t cmd, ioMem_t *ioMem) {
    if (!SendCmdSetAddress(ioMem)) {
        return (0);
    }
    unsigned len = ioMem->len;
    uint8_t sCMD[] = {cmd, len & 0xff};    // 0x100 is sent as 0x00 here
    WriteBufBLHeli(sCMD, sizeof(sCMD), 1);
    return ( readBufBLHeli(ioMem->data, len, 1) );

	//set softSerialStatus to SENDING_DATA state
	softSerialStatus.softSerialState = SS_SENDING_DATA;

	//set the DMA callback function to the OneWireDmaCallback
	callbackFunctionArray[softSerialStatus.currentActuator.DmaCallback] = SoftSerialDmaCallback;

	//send bootInit message to the DMA buffer and init DMA
	memcpy(serialOutBuffer, bootInit, sizeof(bootInit));
	OutputSerialDmaByte(serialOutBuffer, sizeof(bootInit), softSerialStatus.currentActuator, 0, 1);

}
static uint8_t SendWriteCommand(escHardware_t *esc, uint8_t cmd, ioMem_t *ioMem) {
    if (!sendCmdSetAddress(esc, ioMem)) {
        return (0);
    }
    if (!sendCmdSetBuffer(esc, ioMem)) {
        return (0);
    }
    uint8_t sCMD[] = {cmd, 0x01};
    writeBufBLHeli(sCMD, sizeof(sCMD), 1);
    return GetAck(esc) == RET_SUCCESS;
}
//////////////////////////////////////////////
// protocol
static uint32_t DisconnectBLHeli(escHardware_t *escHardware)
{
    uint8_t sCMD[] = {CMD_RUN, 0};
    writeBufBLHeli(sCMD, sizeof(sCMD), (1));
    return (1);
}
static uint32_t PollReadReadyBLHeli(void) {
    uint32_t wait_time = micros() + timeout;
    while (getEscState(escHardware)) {
        // check for start bit begin
        if (cmp32(micros(), wait_time) > 0) {
            return (0);
        }
    }
    return (1);
}
static uint32_t ReadFlashAtmelBLHeli(ioMem_t *ioMem) {
    return SendReadCommand(CMD_READ_FLASH_ATM, ioMem);
}
static uint32_t ReadFlashSiLabsBLHeli(ioMem_t *ioMem) {
    return (SendReadCommand(CMD_READ_FLASH_SIL, ioMem));
}
static uint32_t WriteFlashBLHeli(ioMem_t *ioMem) {
    return SendWriteCommand(CMD_PROG_FLASH, ioMem);
}
static uint32_t ReadEEpromAtmelBLHeli(ioMem_t *ioMem) {
    return SendReadCommand(CMD_READ_EEPROM, ioMem);
}
static uint32_t ReadEEpromSiLabsBLHeli(ioMem_t *ioMem) {
    // SiLabs has no EEPROM, just a flash section at 0x1A00
    ioMem->addr += 0x1A00;
    return (ReadFlashSiLabsBLHeli(ioMem));
}
static uint32_t WriteEEpromAtmelBLHeli(ioMem_t *ioMem) {
	//3000U * 1000U / START_BIT_TIMEOUT may possibly overflow. We confine the value if it does.
	uint32_t timeout = 3000U * 1000U / START_BIT_TIMEOUT;
	if (timeout > 255) {
		timeout = 255;
	}
    return SendWriteCommand(CMD_PROG_EEPROM, ioMem);
}
static uint32_t WriteEEpromSiLabsBLHeli(ioMem_t *ioMem) {
    // SiLabs has no EEPROM, just a flash section at 0x1A00
    ioMem->addr += 0x1A00;
    return WriteFlashBLHeli(ioMem);
}
static uint32_t PageEraseAtmelBLHeli(ioMem_t *ioMem) {
    (void)(ioMem);
    // page erase only required on silabs mcu's
    return (1);
}
static uint32_t PageEraseSiLabsBLHeli(ioMem_t *ioMem) {
    if (!SendCmdSetAddress(ioMem)) {
        return (0);
    }
    uint8_t sCMD[] = {CMD_ERASE_FLASH, 0x01};
    writeBufBLHeli(sCMD, sizeof(sCMD), 1);
    return getAck(40 * 1000 / START_BIT_TIMEOUT) == RET_SUCCESS;
}
static uint32_t EepromEraseSiLabsBLHeli(ioMem_t *ioMem) {
    // SiLabs has no EEPROM, just a flash section at 0x1A00
    ioMem->addr += 0x1A00;
    return pageEraseSiLabsBLHeli(ioMem);
}
*/
