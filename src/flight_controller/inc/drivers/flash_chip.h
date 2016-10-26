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

//data in read buffer goes from 5 to 260
#define FLASH_CHIP_BUFFER_SIZE 261
#define BUFFER_STATUS_FILLING_A 1
#define BUFFER_STATUS_FILLING_B 2

typedef struct {
	uint32_t enabled;
	uint32_t chipId;
	uint32_t flashSectors;
	uint32_t pagesPerSector;
	uint32_t sectorSize;
	uint32_t totalSize;
	uint32_t pageSize;
	uint32_t status;
	uint32_t bufferStatus;
	uint8_t commandRxBuffer[4]; //used for replies of commands
	uint8_t commandTxBuffer[4]; //used for sending chip commands. Needs to be separate of data buffer since both can be in use at once
	uint8_t txBufferA[FLASH_CHIP_BUFFER_SIZE]; //tx buffer to chip. Should be 256 + 5 bytes since that's the page size.
	uint8_t rxBufferA[FLASH_CHIP_BUFFER_SIZE]; //rx buffer from chip. Should be 256 + 5 bytes since we use it for command and dummy bytes while reading a page
	uint8_t txBufferB[FLASH_CHIP_BUFFER_SIZE]; //double buffer. While one write, one is filled
	uint8_t rxBufferB[FLASH_CHIP_BUFFER_SIZE]; //double buffer. While one write, one is filled
} flash_info_record;

extern DMA_HandleTypeDef dma_flash_rx;
extern DMA_HandleTypeDef dma_flash_tx;
extern flash_info_record flashInfo;

extern int InitFlashChip(void);
extern void DataFlashBeginProgram(uint32_t address);
extern int FlashChipWriteData(uint8_t *data, uint8_t length);
extern int FlashChipReadData(uint32_t address, uint8_t *buffer, int length);
extern void DataFlashProgramPage(uint32_t address, uint8_t *data, uint16_t length);
extern void WriteEnableDataFlash(void);
