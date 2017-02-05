#pragma once

enum {
    PAGE_PROGRAM_IN_PROGRESS   = (1 << 0),
    MASS_ERASE_IN_PROGRESS     = (1 << 1),
    SECTOR_ERASE_IN_PROGRESS   = (1 << 2),
    CHIP_BUSY                  = (1 << 3),
    READ_STATUS_IN_PROGRESS    = (1 << 4),
	READ_ID_IN_PROGRESS        = (1 << 5),
	DMA_READ_ID_IN_PROGRESS    = (1 << 6),
	DMA_READ_COMPLETE          = (1 << 7),
	DMA_DATA_READ_IN_PROGRESS  = (1 << 8),
	DMA_DATA_WRITE_IN_PROGRESS = (1 << 9),
};

//data in read buffer goes from 5 to 260. the first
//data in write buffer goes from 4 to 259. the first
#define FLASH_CHIP_BUFFER_WRITE_DATA_SIZE  256
#define FLASH_CHIP_BUFFER_WRITE_DATA_START 4
#define FLASH_CHIP_BUFFER_WRITE_DATA_END   259
#define FLASH_CHIP_BUFFER_READ_DATA_SIZE   256
#define FLASH_CHIP_BUFFER_READ_DATA_START  4
#define FLASH_CHIP_BUFFER_READ_DATA_END    259
#define FLASH_CHIP_BUFFER_SIZE             260
#define BUFFER_STATUS_FILLING_A 1
#define BUFFER_STATUS_FILLING_B 2

#define FLASH_DISABLED 0
#define FLASH_ENABLED 1
#define FLASH_FULL 2

typedef struct {
	uint8_t rxBuffer[FLASH_CHIP_BUFFER_SIZE];
	uint8_t txBuffer[FLASH_CHIP_BUFFER_SIZE];
	volatile uint32_t txBufferPtr;
	volatile uint32_t rxBufferPtr;
} buffer_record;

typedef struct {
	volatile uint32_t enabled;
	volatile uint32_t chipId;
	volatile uint32_t flashSectors;
	volatile uint32_t pagesPerSector;
	volatile uint32_t sectorSize;
	volatile uint32_t totalSize;
	volatile uint32_t pageSize;
	volatile uint32_t status;
	volatile uint32_t bufferStatus;
	uint8_t commandRxBuffer[4]; //used for replies of commands
	uint8_t commandTxBuffer[4]; //used for sending chip commands. Needs to be separate of data buffer since both can be in use at once
/*
	uint8_t txBufferA[FLASH_CHIP_BUFFER_SIZE]; //tx buffer to chip. Should be 256 + 5 bytes since that's the page size.
	uint8_t rxBufferA[FLASH_CHIP_BUFFER_SIZE]; //rx buffer from chip. Should be 256 + 5 bytes since we use it for command and dummy bytes while reading a page
	uint8_t txBufferB[FLASH_CHIP_BUFFER_SIZE]; //double buffer. While one write, one is filled
	uint8_t rxBufferB[FLASH_CHIP_BUFFER_SIZE]; //double buffer. While one write, one is filled
*/

	buffer_record buffer[2];
	uint8_t bufferNum;

	/*
	volatile uint32_t txBufferAPtr;
	volatile uint32_t rxBufferAPtr;
	volatile uint32_t txBufferBPtr;
	volatile uint32_t rxBufferBPtr;
	*/
	volatile uint32_t currentWriteAddress;
} flash_info_record;

extern flash_info_record flashInfo;

extern void FlashDeinit(void);
extern void FlashDmaRxCallback(void);
extern int  InitFlashChip(void);
extern void DataFlashBeginProgram(uint32_t address);
extern int  FlashChipWriteData(uint8_t *data, uint8_t length);
extern int  FlashChipReadData(uint32_t address, uint8_t *buffer, int length);
extern void DataFlashProgramPage(uint32_t address, uint8_t *data, uint16_t length);
extern void WriteEnableDataFlash(void);
extern int  MassEraseDataFlashByPage(int blocking);
extern int  MassEraseDataFlash(int blocking);
extern void M25p16DmaWritePage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer);
extern int  M25p16ReadPage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer);
extern int  WriteEnableDataFlashDma(void);
