#include "includes.h"


typedef enum {
    BLHBLM_BLHELI_SILABS = 1,
	BLHBLM_BLHELI_ATMEL  = 2,
	BLHBLM_SIMONK_ATMEL  = 3,
} esc_bootloader_mode;

typedef struct {
    uint16_t len;
    uint16_t addr;
    uint8_t *data;
} ioMem_t;

/*
typedef struct {
    uint32_t (*Disconnect)(void);
    uint32_t (*PollReadReady)(void);
    uint32_t (*ReadFlash)(ioMem_t*);
    uint32_t (*WriteFlash)(ioMem_t*);
    uint32_t (*ReadEEprom)(ioMem_t*);
    uint32_t (*WriteEEprom)(ioMem_t*);
    uint32_t (*PageErase)(ioMem_t*);
    uint32_t (*EepromErase)(ioMem_t*);
} esc1WireProtocol_t;
*/

typedef struct {
//    uint32_t (*Disconnect)(void);
//    uint32_t (*PollReadReady)(void);
    uint32_t (*ReadFlash)(motor_type actuator, uint16_t address, uint32_t timeout);
//    uint32_t (*WriteFlash)(ioMem_t*);
    uint32_t (*ReadEEprom)(motor_type actuator, uint32_t timeout);
//    uint32_t (*WriteEEprom)(ioMem_t*);
//    uint32_t (*PageErase)(ioMem_t*);
//    uint32_t (*EepromErase)(ioMem_t*);
} esc1WireProtocol_t;

typedef struct {

	uint32_t            escSignature;
	uint32_t            bootVersion;
	uint32_t            bootPages;
	esc_bootloader_mode escBootloaderMode;
	const esc1WireProtocol_t  *esc1WireProtocol;

	enum {
		OW_IDLE                   = 0,
		OW_AWAITING_BOOT_MESSAGE  = 1,
		OW_AWAITING_READ_EEPROM   = 2,
		OW_PREPARING_ACTUATOR     = 3,
		OW_ACTUATOR_READY_TO_SEND = 4,
		OW_SENDING_DATA           = 5,
		OW_RECEIVING_DATA         = 6,
		OW_ERROR_UNKNOWN_BOOT_MSG = 7,
		OW_CONNECT_TO_BOOTLOADER  = 8,
	} oneWireState;

} esc_one_wire_status;


//buffer size, 256 plus two bytes for CRC
#define ESC_BUF_SIZE 256+2

// Bootloader commands
#define RestartBootloader  0
#define ExitBootloader     1

#define CMD_RUN            0x00
#define CMD_PROG_FLASH     0x01
#define CMD_ERASE_FLASH    0x02
#define CMD_READ_FLASH_SIL 0x03
#define CMD_VERIFY_FLASH   0x03
#define CMD_READ_EEPROM    0x04
#define CMD_PROG_EEPROM    0x05
#define CMD_READ_SRAM      0x06
#define CMD_READ_FLASH_ATM 0x07
#define CMD_KEEP_ALIVE     0xFD
#define CMD_SET_ADDRESS    0xFF
#define CMD_SET_BUFFER     0xFE
#define CMD_BOOTINIT       0x07
#define CMD_BOOTSIGN       0x08

// Bootloader result codes
#define RET_SUCCESS        0x30
#define RET_ERRORCOMMAND   0xC1
#define RET_ERRORCRC       0xC2
#define RET_NONE           0xFF



uint8_t  oneWireOutBuffer[ESC_BUF_SIZE];
uint8_t  oneWireInBuffer[ESC_BUF_SIZE];
uint32_t oneWireInBufferIdx;

static const uint8_t  bootInit[] = {0, 0, 0, 0, 0, 0, 0, 0, 0x0D, 'B', 'L', 'H', 'e', 'l', 'i', 0xF4, 0x7D};
static const uint16_t signaturesAtmel[]  = {0x9307, 0x930A, 0x930F, 0x940B, 0};
static const uint16_t signaturesSilabs[] = {0xF310, 0xF330, 0xF410, 0xF390, 0xF850, 0xE8B1, 0xE8B2, 0};


static uint32_t OneWireMain() __attribute__ ((unused));
static uint32_t SignatureMatch(uint16_t signature, const uint16_t *list, uint32_t listSize);
static void     AppendBlHeliCrc(uint8_t outBuffer[], uint32_t len);
//static void     SendHello(void);
static uint16_t Crc16Byte(uint16_t from, uint8_t byte);
//static uint32_t DisconnectBLHeli(void);
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
static uint32_t SendReadCommand(motor_type actuator, uint8_t cmd, uint16_t address, uint16_t length, uint32_t timeout) __attribute__ ((unused));
static uint32_t ReadEEpromSiLabsBLHeli(motor_type actuator, uint32_t timeout);
static uint32_t ReadFlashSiLabsBLHeli(motor_type actuator, uint16_t address, uint32_t timeout);
static uint32_t SendCmdSetBuffer(motor_type actuator, uint8_t outBuffer[], uint16_t length) __attribute__ ((unused));
static uint32_t SendCmdSetAddress(motor_type actuator, uint16_t address) __attribute__ ((unused));
static uint32_t HandleEscOneWire(uint8_t serialBuffer[], uint32_t outputLength)  __attribute__ ((unused));

/*

const esc1WireProtocol_t BLHeliAtmelProtocol = {
    .disconnect    = DisconnectBLHeli,
    .pollReadReady = PollReadReadyBLHeli,
    .readFlash     = ReadFlashAtmelBLHeli,
    .writeFlash    = WriteFlashBLHeli,
    .readEEprom    = ReadEEpromAtmelBLHeli,
    .writeEEprom   = WriteEEpromAtmelBLHeli,
    .pageErase     = PageEraseAtmelBLHeli,
    .eepromErase   = PageEraseAtmelBLHeli,
};

const esc1WireProtocol_t BLHeliSiLabsProtocol = {
    .disconnect    = DisconnectBLHeli,
    .pollReadReady = PollReadReadyBLHeli,
    .readFlash     = ReadFlashSiLabsBLHeli,
    .writeFlash    = WriteFlashBLHeli,
    .readEEprom    = ReadEEpromSiLabsBLHeli,
    .writeEEprom   = WriteEEpromSiLabsBLHeli,
    .pageErase     = PageEraseSiLabsBLHeli,
    .eepromErase   = EepromEraseSiLabsBLHeli,
};

*/

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
//    .disconnect    = DisconnectBLHeli,
//    .pollReadReady = PollReadReadyBLHeli,
    .ReadFlash     = ReadFlashSiLabsBLHeli,
//    .writeFlash    = WriteFlashBLHeli,
    .ReadEEprom    = ReadEEpromSiLabsBLHeli,
//    .writeEEprom   = WriteEEpromSiLabsBLHeli,
//    .pageErase     = PageEraseSiLabsBLHeli,
//    .eepromErase   = EepromEraseSiLabsBLHeli,
};

esc_one_wire_status escOneWireStatus[16];



void OneWireInit(void)
{

	uint32_t x;

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


	DelayMs(100); //delay while debugging since motors need time to startup.

	//OneWireMain(); //enter OneWireMain loop.

	while (1) {
		for (x = 0; x < MAX_MOTOR_NUMBER; x++) {

			if (board.motors[x].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
				if (ConnectToBlheliBootloader(board.motors[x], 35)) {
					//TODO: Make work with the protocol struct
					//const esc1WireProtocol_t *proto = escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].esc1WireProtocol;
					//proto->ReadEEprom(board.motors[actuatorNumOutput], 25);
					escOneWireStatus[board.motors[x].actuatorArrayNum].esc1WireProtocol->ReadEEprom(board.motors[x], 125);

				}
			}
		}
	}

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
			escOneWireStatus[actuator.actuatorArrayNum].escBootloaderMode = BLHBLM_BLHELI_ATMEL;
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

	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(oneWireOutBuffer, 6, oneWireInBuffer, actuator, 150); //150ms timeout is way more than we need

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
    return (ReadFlashSiLabsBLHeli(actuator, 0x1A00, timeout));

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

static uint32_t ReadFlashSiLabsBLHeli(motor_type actuator, uint16_t address, uint32_t timeout) {

	if (!SendCmdSetAddress(actuator, address))
		return (0);

	oneWireOutBuffer[0] = CMD_READ_FLASH_SIL;
	oneWireOutBuffer[1] = 0;
	oneWireOutBuffer[2] = (address >> 8);
	oneWireOutBuffer[3] = (address & 0xff);

	AppendBlHeliCrc(oneWireOutBuffer, 4);

	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(oneWireOutBuffer, 6, oneWireInBuffer, actuator, timeout); //25ms timeout is way more than we need

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


static uint32_t OneWireMain() {

	uint32_t actuatorNumOutput;


	while (1) {

		//scheduler for 1wire.
		//1. connect to all ESCs and ask for bootloader and config info
		//2. wait for further information.
		//for (actuatorNumOutput = 0; actuatorNumOutput < MAX_MOTOR_NUMBER; actuatorNumOutput++) {
//		actuatorNumOutput = 0;
//		if (board.motors[actuatorNumOutput].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {

			//
//			if (ConnectToBlheliBootloader(board.motors[actuatorNumOutput], 25)) {
//				//TODO: Make work with the protocol struct
//				//const esc1WireProtocol_t *proto = escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].esc1WireProtocol;
//				//proto->ReadEEprom(board.motors[actuatorNumOutput], 25);
//				escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].esc1WireProtocol->ReadEEprom(board.motors[actuatorNumOutput], 25);
//
//			}

//			while ( HandleSoftSerial() ); //wait until actuator is idle;
//			GetEscConfig(board.motors[actuatorNumOutput]);
//			ioMem_t ioMem = {
//				.addr = 0,
//				.len  = ESC_BUF_SIZE,
//				.data = serialOutBuffer,
//			};
//			escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].esc1WireProtocol->readEEprom(&ioMem);
//		}


		for (actuatorNumOutput = 0; actuatorNumOutput < MAX_MOTOR_NUMBER; actuatorNumOutput++) {

			if (board.motors[actuatorNumOutput].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
				if (ConnectToBlheliBootloader(board.motors[actuatorNumOutput], 25)) {
					//TODO: Make work with the protocol struct
					//const esc1WireProtocol_t *proto = escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].esc1WireProtocol;
					//proto->ReadEEprom(board.motors[actuatorNumOutput], 25);
					escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].esc1WireProtocol->ReadEEprom(board.motors[actuatorNumOutput], 25);

				}
			}

//			escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].oneWireState = OW_AWAITING_BOOT_MESSAGE;
//
//			//soft serial is in the IDLE state, let's switch to another actuator and prepare it for soft serial
//			if (board.motors[actuatorNumOutput].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {
//
//
//
//				softSerialStatus.currentActuator = board.motors[actuatorNumOutput];
//				escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].oneWireState = OW_CONNECT_TO_BOOTLOADER;
//				//while ( HandleEscOneWire() );
//
//				//while ( HandleSoftSerial() );
//				//Actuator is Idle. At this point we either DeInit it and move on or do something with it again.
//				//It's currently in the RX state, but a line idle has occurred so we consider it IDLE//
//
//				//HandleEscOneWire(); //does nothing right now.
//			}

		}

	}

	return (0);

}

void OneWireDeinit(void) {

	DeInitBoardUsarts(); //deinit all the USARTs.
	DeInitActuators();   //deinit all the Actuators.
	InitActuators();     //init all the Actuators.
	InitBoardUsarts();   //init all the USARTs.
	if (!AccGyroInit(mainConfig.gyroConfig.loopCtrl)) {
		ErrorHandler(GYRO_INIT_FAILIURE);
	}

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

	outBuffer[i+1] = (crc & 0xff);
	outBuffer[i+2] = (crc >> 8);

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
