#include "includes.h"



//todo: need to move the soft serial stuff to a soft serial or the serial driver.
//Soft Serial relies entirely on the Micros() function... which seems very accurate in RF1.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////1wire header above, soft serial header below... todo: split them up into two files
/////////////////////////////////////////blheli 1wire header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    uint32_t (*ReadFlash)(motor_type*, uint32_t timeout);
//    uint32_t (*WriteFlash)(ioMem_t*);
    uint32_t (*ReadEEprom)(motor_type*, uint32_t timeout);
//    uint32_t (*WriteEEprom)(ioMem_t*);
//    uint32_t (*PageErase)(ioMem_t*);
//    uint32_t (*EepromErase)(ioMem_t*);
} esc1WireProtocol_t;

typedef struct {

	uint32_t            escSignature;
	uint32_t            bootVersion;
	uint32_t            bootPages;
	esc_bootloader_mode escBootloaderMode;
	esc1WireProtocol_t  esc1WireProtocol;

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

static const uint8_t  bootInit[] = {0, 0, 0, 0, 0x0D, 'B', 'L', 'H', 'e', 'l', 'i', 0xF4, 0x7D};
static const uint16_t signaturesAtmel[]  = {0x9307, 0x930A, 0x930F, 0x940B, 0};
static const uint16_t signaturesSilabs[] = {0xF310, 0xF330, 0xF410, 0xF390, 0xF850, 0xE8B1, 0xE8B2, 0};


static uint32_t OneWireMain(void);
static uint32_t SignatureMatch(uint16_t signature, uint16_t *list, uint32_t listSize);
static void     AppendBlHeliCrc(uint8_t outBuffer[], uint32_t len);
static void     SendHello(void);
static uint16_t Crc16Byte(uint16_t from, uint8_t byte);
static uint32_t DisconnectBLHeli(void);
static uint32_t PollReadReadyBLHeli(void);
static uint32_t ReadFlashAtmelBLHeli(ioMem_t *ioMem);
static uint32_t ReadFlashSiLabsBLHeli(ioMem_t *ioMem);
static uint32_t WriteFlashBLHeli(ioMem_t *ioMem);
static uint32_t ReadEEpromAtmelBLHeli(ioMem_t *ioMem);
static uint32_t ReadEEpromSiLabsBLHeli(ioMem_t *ioMem);
static uint32_t WriteEEpromAtmelBLHeli(ioMem_t *ioMem);
static uint32_t WriteEEpromSiLabsBLHeli(ioMem_t *ioMem);
static uint32_t PageEraseAtmelBLHeli(ioMem_t *ioMem);
static uint32_t PageEraseSiLabsBLHeli(ioMem_t *ioMem);
static uint32_t EepromEraseSiLabsBLHeli(ioMem_t *ioMem);


static uint32_t ConnectToBlheliBootloader(motor_type actuator, uint32_t timeout);
static uint32_t SendReadCommand(motor_type actuator, uint16_t address, uint32_t timeout);
static uint32_t ReadEEpromSiLabsBLHeli(motor_type actuator, uint32_t timeout);
static uint32_t ReadFlashSiLabsBLHeli(motor_type actuator, uint32_t timeout);

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

const esc1WireProtocol_t BLHeliAtmelProtocol = {
//    .disconnect    = DisconnectBLHeli,
//    .pollReadReady = PollReadReadyBLHeli,
    .readFlash     = ReadFlashAtmelBLHeli,
//    .writeFlash    = WriteFlashBLHeli,
    .readEEprom    = ReadEEpromAtmelBLHeli,
//    .writeEEprom   = WriteEEpromAtmelBLHeli,
//    .pageErase     = PageEraseAtmelBLHeli,
//    .eepromErase   = PageEraseAtmelBLHeli,
};

const esc1WireProtocol_t BLHeliSiLabsProtocol = {
//    .disconnect    = DisconnectBLHeli,
//    .pollReadReady = PollReadReadyBLHeli,
    .readFlash     = ReadFlashSiLabsBLHeli,
//    .writeFlash    = WriteFlashBLHeli,
    .readEEprom    = ReadEEpromSiLabsBLHeli,
//    .writeEEprom   = WriteEEpromSiLabsBLHeli,
//    .pageErase     = PageEraseSiLabsBLHeli,
//    .eepromErase   = EepromEraseSiLabsBLHeli,
};

esc_one_wire_status escOneWireStatus[16];





//ss
typedef struct {

	motor_type	      currentActuator;
	volatile uint32_t actuatorState; //can be changed by an ISR, so it's volatile.
	float             bitWidth;
	float             byteWidth;
	float		      lineIdleTime;
	uint32_t          buadRate;
	uint32_t          bitsPerByte; //including frame bits
	uint32_t          inverted;
	volatile uint32_t dataInBuffer;
	volatile uint32_t timeOfActivation;   //Anything time related should be made volatile.
	volatile uint32_t timeOfLastActivity; //do we need this? //can be changed by an ISR, so it's volatile if we need it.

	enum {
		SS_IDLE                    = 0,
		SS_PREPARING_ACTUATOR      = 1,
		SS_ACTUATOR_READY_TO_SEND  = 2,
		SS_SENDING_DATA            = 3,
		SS_RECEIVING_DATA          = 4,
		SS_ERROR_TIME_IN_BUFFER_OF = 5,
	} softSerialState;

} soft_serial_status;



#define SOFT_SERIAL_BUF_SIZE 256
#define SOFT_SERIAL_TIME_BUFFER_SIZE (SOFT_SERIAL_BUF_SIZE * 10) //This is huge! Can probably do smaller than this size safely since the chance that nothing but 0xAA is sent is next to 0;



soft_serial_status softSerialStatus;
static const uint16_t bitLookup[] = {0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF};
volatile softserial_function_pointer softserialCallbackFunctionArray[1];
uint8_t  serialOutBuffer[SOFT_SERIAL_BUF_SIZE];
uint8_t  serialInBuffer[SOFT_SERIAL_BUF_SIZE];
uint8_t  serialInBufferIdx=0;
volatile uint32_t timeInBuffer[SOFT_SERIAL_TIME_BUFFER_SIZE];
volatile uint32_t timeInBufferIdx = 0;




static float    FindSoftSerialBitWidth(uint32_t baudRate);
static float    FindSoftSerialByteWidth(float bitWidth, uint32_t bitsPerByte);
static float    FindSoftSerialLineIdleTime(float byteWidth);
static void     SendSoftSerialActuator(motor_type actuator);
static void     ProcessSoftSerialLineIdle(void);
static void     NumberOfBits(uint32_t time2, uint32_t time1, uint32_t bitsInByte, float bitWidth, uint16_t *numberOfBits, uint32_t workingOnByte);
static uint32_t ProcessSerialBits(void);
static uint32_t IsSoftSerialLineIdle(void);
static uint32_t HandleSoftSerial(void);





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

	for (x=0;x<sizeof(timeInBuffer);x++)
		timeInBuffer[x] = 0;

	softserialCallbackFunctionArray[0] = HandleEscOneWire;

	DelayMs(100); //delay while debugging since motors need time to startup.
	OneWireMain(); //enter OneWireMain loop.

}

void SendOutput(void) {

	//set softSerialStatus to SENDING_DATA state
	softSerialStatus.softSerialState = SS_SENDING_DATA;

	//set the DMA callback function to the OneWireDmaCallback
	callbackFunctionArray[softSerialStatus.currentActuator.DmaCallback] = SoftSerialDmaCallback;

	//set the data to be sent
	OutputSerialDmaByte(serialOutBuffer, softSerialStatus.dataInBuffer, softSerialStatus.currentActuator, 0, 1);

}

static uint32_t ConnectToBlheliBootloader(motor_type actuator) {

	//connect to ESC and get bootloader info

	memcpy(oneWireOutBuffer, bootInit, sizeof(bootInit));
	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(oneWireOutBuffer, sizeof(bootInit), oneWireInBuffer, actuator, 100);

	escOneWireStatus[actuator.actuatorArrayNum].escBootloaderMode = 0;

	//if ( (oneWireInBuffer[0] == '4') && (oneWireInBuffer[1] == '7') && (oneWireInBuffer[2] == '1') )
	if ( !strncmp((char *)oneWireInBuffer, "471", 3) && (oneWireInBufferIdx > 7) )
	{
		//4, 7, 1, d, 0xE8, 0xB2, 0x06, 0x01, 0x30
		// Format = BootMsg("471c"), SIGNATURE_HIGH, SIGNATURE_LOW, BootVersion (always 6), BootPages (,ACK)
		escOneWireStatus[actuator.actuatorArrayNum].escSignature = ( (oneWireInBuffer[4] << 8) | oneWireInBuffer[5] );
		escOneWireStatus[actuator.actuatorArrayNum].bootVersion  = oneWireInBuffer[6];
		escOneWireStatus[actuator.actuatorArrayNum].bootPages    = oneWireInBuffer[7];

		if (SignatureMatch(escOneWireStatus[actuator.actuatorArrayNum].escSignature, &signaturesSilabs, (sizeof(signaturesSilabs)/2)))
		{
			escOneWireStatus[actuator.actuatorArrayNum].escBootloaderMode = BLHBLM_BLHELI_SILABS;
			escOneWireStatus[actuator.actuatorArrayNum].esc1WireProtocol  = &BLHeliSiLabsProtocol;

		}
		else if (SignatureMatch(escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escSignature, &signaturesAtmel, (sizeof(signaturesAtmel)/2)))
		{
			escOneWireStatus[actuator.actuatorArrayNum].escBootloaderMode = BLHBLM_BLHELI_ATMEL;
			escOneWireStatus[actuator.actuatorArrayNum].esc1WireProtocol  = &BLHeliAtmelProtocol;
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

static uint32_t ReadEEpromSiLabsBLHeli(motor_type actuator) {

	// SiLabs has no EEPROM, just a flash section at 0x1A00
    return (ReadFlashSiLabsBLHeli(actuator, 0x1A00));

}

static uint32_t CheckCrc(uint8_t inBuffer[], uint32_t length, uint16_t crc) {

}

static uint32_t SendReadCommand(motor_type actuator, uint8_t cmd, uint16_t address, uint16_t length) {

	uint8_t cmdBuffer[] = {cmd,(length & 0xff), 0, 0};

	if (!SendCmdSetAddress(actuator, address))
			return (0);

	AppendBlHeliCrc(cmdBuffer, 2);

	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(cmdBuffer, 4, oneWireInBuffer, actuator, 150); //19200, reading 258 bytes should take around 135 ms

	//fills oneWireInBuffer to length 256 + 2 crc bytes
	if ( oneWireInBufferIdx == (length + 2) ) { //did we get the amount of data we expected?
		if (CheckCrc(oneWireInBuffer, length, (uint16_t)(oneWireInBuffer[length-1] >> 8) | (oneWireInBuffer[length] & 0xff) ) ) {
			//check the CRC
//cruiser
		}
	}

//    return readBufBLHeli(esc, ioMem->data, len, true);

}

static uint32_t ReadFlashSiLabsBLHeli(motor_type actuator, uint16_t address) {

	if (!SendCmdSetAddress(actuator, address))
		return (0);

	oneWireOutBuffer[0] = CMD_READ_FLASH_SIL;
	oneWireOutBuffer[1] = 0;
	oneWireOutBuffer[2] = (address >> 8);
	oneWireOutBuffer[3] = (address & 0xff);

	AppendBlHeliCrc(oneWireOutBuffer, 4);

	oneWireInBufferIdx = SoftSerialSendReceiveBlocking(oneWireOutBuffer, 6, oneWireInBuffer, actuator, 25); //25ms timeout is way more than we need

	return (oneWireInBufferIdx);
}

void SoftSerialExtiCallback(void) {

	// EXTI line interrupt detected
	if(__HAL_GPIO_EXTI_GET_IT(softSerialStatus.currentActuator.pin) != RESET)
	{

		//record time of IRQ in microseconds
		timeInBuffer[timeInBufferIdx++] = Micros();
		if (timeInBufferIdx == SOFT_SERIAL_TIME_BUFFER_SIZE) {
			timeInBufferIdx = 0;
			softSerialStatus.softSerialState = SS_ERROR_TIME_IN_BUFFER_OF;
		}

		__HAL_GPIO_EXTI_CLEAR_IT(softSerialStatus.currentActuator.pin);

	}

}

uint32_t SoftSerialSendReceiveBlocking(uint8_t serialOutBuffer[], uint32_t serialOutBufferLength, uint8_t inBuffer[], motor_type actuator, uint32_t timeoutMs) {

	uint32_t timeout = timeoutMs + InlineMillis();

	//send serial, block until we get line idle, fill in buffer with the reply
	SendSoftSerialActuator(actuator);

	//set softSerialStatus to SENDING_DATA state
	softSerialStatus.softSerialState = SS_SENDING_DATA;

	//set the DMA callback function to the OneWireDmaCallback
	callbackFunctionArray[softSerialStatus.currentActuator.DmaCallback] = SoftSerialDmaCallback;

	//set the data to be sent
	OutputSerialDmaByte(serialOutBuffer, serialOutBufferLength, actuator, 0, 1); //send outbuffer, xx bytes, this actuator, 0=LSB, 1=serial frame

	//SS state is in SS_SENDING_DATA. Once data output completes it goes to SS_RECEIVING_DATA. While SS_SENDING_DATA we check to see if timeout time has passed.
	while (softSerialStatus.softSerialState == SS_SENDING_DATA) {
		if (timeout > InlineMillis()) {
			return (0); //timeout occurred, return failure
		}
	}

	//When in RECEIVING we wait for a line idle to occur. We allow up to timeout for this to happen.
	while (softSerialStatus.softSerialState == SS_RECEIVING_DATA) {
		if (timeout > InlineMillis()) {
			return (0); //timeout occurred, return failure
		}
		if ( IsSoftSerialLineIdle() ) {
			ProcessSoftSerialLineIdle(0); //proccess
			memcpy(inBuffer, serialInBuffer, serialInBufferIdx);
			return (serialInBufferIdx);
		}
	}

}

void SoftSerilDmaCallback(void) {

	//DMA is done sending, let's switch GPIO to EXTI mode
	PutSoftSerialActuatorInReceiveState();

}

void ProcessSoftSerialLineIdle(uint32_t useCallback) {

	timeInBuffer[timeInBufferIdx] = timeInBuffer[timeInBufferIdx - 1] + lrintf(softSerialStatus.bitWidth); //put in last time so we can get the last byte. We need the last byte to calculate the frame.
	timeInBufferIdx++;

	//Process the serial data received and disabled the IRQ if there's a line idle sensed AFTER data has been received
	if ( ProcessSerialBits() ) {
		//data exit and we're in a line idle
		//Set Soft Serial Status to IDLE Set input buffers back to zero state and process the received data
		softSerialStatus.softSerialState = SS_IDLE;
		//process the received data
		if ( (useCallback) && (softserialCallbackFunctionArray[0]) )
			softserialCallbackFunctionArray[0](serialInBuffer, serialInBufferIdx);
	} else {
		//line idle exits, but no data found, how to handle this issue?
		//do not DeInit actuator and only error out if time since activation > greater than XX time
	}

}


void PutSoftSerialActuatorInReceiveState(void) {

	GPIO_InitTypeDef        GPIO_InitStruct;

	//switch GPIO from timer output to EXTI input without doing a deinit.
	GPIO_InitStruct.Pin  = softSerialStatus.currentActuator.pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = softSerialStatus.inverted ? GPIO_PULLDOWN : GPIO_PULLUP;
    HAL_GPIO_Init(ports[softSerialStatus.currentActuator.port], &GPIO_InitStruct);

    //set DMA callback to disabled since we're in EXTI mode now.
	callbackFunctionArray[softSerialStatus.currentActuator.DmaCallback] = 0;
	softSerialStatus.softSerialState = SS_RECEIVING_DATA;

	//reset reception buffer.
	timeInBufferIdx = 0;

}

static uint32_t IsSoftSerialLineIdle(void) {
	volatile float timeNow;

	timeNow = (float)Micros();

	if ( (timeNow - (float)timeInBuffer[timeInBufferIdx-1]) > softSerialStatus.lineIdleTime) {
	//have to use millis because micros can't be running constantly as it disables IRQs to function properly. We loose some precision so we have to ceil the bytewidth
	//if (InlineMillis() - lrintf((float)timeInBuffer[timeInBufferIdx] * 0.001) > ceilf(softSerialStatus.byteWidth * 0.001) )
		if (timeInBufferIdx > 3)
			return(1);
	}
	return(0);
}


static void NumberOfBits(uint32_t time2, uint32_t time1, uint32_t bitsInByte, float bitWidth, uint16_t *numberOfBits, uint32_t workingOnByte) {

	float timeD;
	float maxWidthPossible;

	*numberOfBits = 0;

	if (time2 <= time1) //no bits
	{
		return;
	}

	timeD = (float)(time2 - time1);
	maxWidthPossible = (bitWidth * (float)bitsInByte);

	if ( workingOnByte || (timeD < maxWidthPossible) ) //working on byte, so return line idle, or bits exist and this is a new byte
	{
		*numberOfBits = lrintf(round(timeD/(float)bitWidth));
	}

	return;

}

static uint32_t ProcessSerialBits(void) {

	uint32_t x;
	uint32_t fails;
	uint16_t bits;
	uint32_t currentBit;
	uint32_t bitsInByte;
	volatile uint32_t byte;
	uint16_t totalBitsFound;
	float    bitWidth;
	uint32_t byteFound;

	volatile uint32_t timeNow = Micros();
	byteFound  = 0;
	bitWidth   = 51.45;
	bitsInByte = 10;

	//bit time for 19200 is 51.45 us
	//max byte time including frames is 514.45 us. Call it 535 to be safe.

	//max byte size is

	fails=0;
	byte = 0;
	currentBit = 1;

	for (x = 0; x < timeInBufferIdx; x++) {

		NumberOfBits(timeInBuffer[x+1], timeInBuffer[x], bitsInByte, bitWidth, &bits, totalBitsFound);

		totalBitsFound = 0;

		if (totalBitsFound == 0) { //starting new byte string from line idle.

			currentBit = 1;

			while (totalBitsFound < bitsInByte) {

				if (currentBit) {

					if (!bits) {
						x++;
						NumberOfBits(timeInBuffer[x+1], timeInBuffer[x], bitsInByte, bitWidth, &bits, totalBitsFound);
					}

					if (bits > 0) {
						bits = CONSTRAIN(bits,1,(bitsInByte-totalBitsFound));
						//byte |= (bitLookup[ bits ] << totalBitsFound);
						byte &= ~(bitLookup[ bits ] << totalBitsFound);
						totalBitsFound += bits;
						currentBit=0;
						bits = 0;
					} else {
						//ignore this time as a corruption
						fails++;
					}

				} else {

					if (!bits) {
						x++;
						NumberOfBits(timeInBuffer[x+1], timeInBuffer[x], bitsInByte, bitWidth, &bits, totalBitsFound);
					}

					if (bits > 0) {
						bits = CONSTRAIN(bits,1,(bitsInByte-totalBitsFound));

						//byte &= ~(bitLookup[ bits ] << totalBitsFound);
						byte |= (bitLookup[ bits ] << totalBitsFound);
						totalBitsFound += bits;
						currentBit=1;
						bits = 0;
					} else {
						//ignore this time as a corruption
						fails++;
					}

				}

				if(fails > 10) {
					totalBitsFound = 0;
					break;
				}

				if (totalBitsFound >= 10) {
					//trim off frames
					serialInBuffer[serialInBufferIdx++] = (uint8_t)( (byte >> 1) & 0xFF );
					if (timeNow > 1) {
						totalBitsFound = 0;
					}
					byteFound = 1;
					break;
				}

			}

		}

	}

	return(byteFound);

}


inline static float FindSoftSerialBitWidth(uint32_t baudRate) {
	return ( (1.0 / (float)baudRate) * 1000.0 * 1000.0 );
}

inline static float FindSoftSerialByteWidth(float bitWidth, uint32_t bitsPerByte) {
	return (bitWidth * (float)bitsPerByte);
}

inline static float FindSoftSerialLineIdleTime(float byteWidth) {
	return (byteWidth * 1.10);
}


static void SendSoftSerialActuator(motor_type actuator) {

	//Prepares the soft serial actuator. Places it into SS_ACTUATOR_READY_TO_SEND state.

	//set current actuator
	softSerialStatus.currentActuator    = actuator;
	softSerialStatus.timeOfActivation   = Micros();
	softSerialStatus.softSerialState    = SS_PREPARING_ACTUATOR;
	softSerialStatus.buadRate           = 19200; //baud rate
	softSerialStatus.bitsPerByte        = 10;    //including frame bits
	softSerialStatus.inverted           = 0;     //including frame bits

	//calculate these values now and store the results. Stored as floats.
	softSerialStatus.bitWidth           = FindSoftSerialBitWidth(softSerialStatus.buadRate); //bit length in us
	softSerialStatus.byteWidth          = FindSoftSerialByteWidth(softSerialStatus.bitWidth, softSerialStatus.bitsPerByte);
	softSerialStatus.lineIdleTime       = FindSoftSerialLineIdleTime(softSerialStatus.byteWidth);


	//activate DMA output on actuator
	SetActiveDmaToActuatorDma(actuator);

	//Set EXTI callback function to the SoftSerialExtiCallback
	callbackFunctionArray[actuator.EXTICallback] = SoftSerialExtiCallback;

	//init EXTI, we immediately put the Actuator into output mode, but this saves a bit of time for when the actuator needs to receive.
	EXTI_Init(ports[actuator.port], actuator.pin, actuator.EXTIn, 1, 2, GPIO_MODE_IT_RISING_FALLING, softSerialStatus.inverted ? GPIO_PULLDOWN : GPIO_PULLUP); //pulldown if inverted, pullup if normal serial

	//Put actuator into Output state.
	InitDmaOutputForSoftSerial(DMA_OUTPUT_ESC_1WIRE, actuator);

}

static uint32_t HandleSoftSerial(void) {

	//return 1 to run this function again, return 0 to move onto the next actuator or finish up.
	//I think we need a timeout check here.

	FeedTheDog(); //don't allow a watchdog reboot to happen.

	switch (softSerialStatus.softSerialState) {

		case SS_IDLE:                  //Actuator is Idle
			return (0);
			break;

		case SS_PREPARING_ACTUATOR:    //Actuator is being prepared, after 200 us has passed (allow time for actuator to stabilize. We can send data.
			if (Micros() - softSerialStatus.timeOfActivation > 20)
				softSerialStatus.softSerialState = SS_ACTUATOR_READY_TO_SEND;
			return (1);
			break;

		case SS_ACTUATOR_READY_TO_SEND: //Actuator is prepared and ready to send data. Let's send init then return 1 to keep checking.
			return (1);
			break;

		case SS_RECEIVING_DATA:         //Actuator is in the reception state. We keep checking until we sense a line idle.
			if ( IsSoftSerialLineIdle() ) {
				ProcessSoftSerialLineIdle();
			}
			return (1);
			break;

		case SS_SENDING_DATA:           //Actuator is in the sending state. We keep checking until DMA does a callback or until error time elapses
			return (1);
			break;

		case SS_ERROR_TIME_IN_BUFFER_OF:
			//We received more IRQs than we can deal with, buffer is too small or there's a problem.
			//TODO: better handle this situation.
			DeInitDmaOutputForSoftSerial(softSerialStatus.currentActuator);
			return (0);
			break;

		default:
			return (1);

	}

}

uint32_t HandleEscOneWire(uint8_t serialBuffer[], uint32_t outputLength) {

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

				if (SignatureMatch(escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escSignature, &signaturesSilabs, (sizeof(signaturesSilabs)/2)))
				{
					escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escBootloaderMode = BLHBLM_BLHELI_SILABS;
					escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].esc1WireProtocol  = &BLHeliSiLabsProtocol;

				}
				else if (SignatureMatch(escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escSignature, &signaturesAtmel, (sizeof(signaturesAtmel)/2)))
				{
					escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escBootloaderMode = BLHBLM_BLHELI_ATMEL;
					escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].esc1WireProtocol  = &BLHeliAtmelProtocol;
				}
				else
				{
					escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].escBootloaderMode = 0;
				}

				HandleEscOneWire(serialBuffer, outputLength);
			}
			else
			{
				escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].oneWireState = OW_ERROR_UNKNOWN_BOOT_MSG;
			}
			break;

		case OW_AWAITING_READ_EEPROM:
			ioMem_t ioMem = {
				.addr = 0,
				.len  = ESC_BUF_SIZE,
				.data = serialOutBuffer,
			};
			escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].esc1WireProtocol->readEEprom(&ioMem);
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

	return (0);
}


static uint32_t OneWireMain(void) {

	uint32_t actuatorNumOutput;

	while (1) {

		//scheduler for 1wire.
		//1. connect to all ESCs and ask for bootloader and config info
		//2. wait for further information.
		//for (actuatorNumOutput = 0; actuatorNumOutput < MAX_MOTOR_NUMBER; actuatorNumOutput++) {
		actuatorNumOutput = 0;
		if (board.motors[actuatorNumOutput].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {

			//
			if (ConnectToBlheliBootloader(board.motors[actuatorNumOutput])) {
				//TODO: Make work with the protocol struct
				escOneWireStatus[board.motors[actuatorNumOutput]].esc1WireProtocol.readEEprom();
				if (GetEscConfig(board.motors[actuatorNumOutput])) {
					//put ESC config into structure;
					volatile uint32_t cat = 1;
				}
			}

//			while ( HandleSoftSerial() ); //wait until actuator is idle;
//			GetEscConfig(board.motors[actuatorNumOutput]);
//			ioMem_t ioMem = {
//				.addr = 0,
//				.len  = ESC_BUF_SIZE,
//				.data = serialOutBuffer,
//			};
//			escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].esc1WireProtocol->readEEprom(&ioMem);
		}


		for (actuatorNumOutput = 0; actuatorNumOutput < MAX_MOTOR_NUMBER; actuatorNumOutput++) {

			escOneWireStatus[board.motors[actuatorNumOutput].actuatorArrayNum].oneWireState = OW_AWAITING_BOOT_MESSAGE;

			//soft serial is in the IDLE state, let's switch to another actuator and prepare it for soft serial
			if (board.motors[actuatorNumOutput].enabled == ENUM_ACTUATOR_TYPE_MOTOR) {



				softSerialStatus.currentActuator = board.motors[actuatorNumOutput];
				escOneWireStatus[softSerialStatus.currentActuator.actuatorArrayNum].oneWireState = OW_CONNECT_TO_BOOTLOADER;
				while ( HandleEscOneWire() );

				while ( HandleSoftSerial() );
				//Actuator is Idle. At this point we either DeInit it and move on or do something with it again.
				//It's currently in the RX state, but a line idle has occurred so we consider it IDLE

				//HandleEscOneWire(); //does nothing right now.
			}

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





































static uint32_t SignatureMatch(uint16_t signature, uint16_t *list, uint32_t listSize)
{

	uint32_t x;

	for (x=0;x<listSize;x++)
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
