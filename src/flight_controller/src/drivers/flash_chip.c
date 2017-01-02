#include "includes.h"


flash_info_record flashInfo;


#define M25P16_RDID              0x9F
#define M25P16_READ_BYTES        0x03
#define M25P16_READ_BYTES_FAST   0x0B
#define M25P16_READ_STATUS_REG   0x05
#define M25P16_WRITE_STATUS_REG  0x01
#define M25P16_WRITE_ENABLE      0x06
#define M25P16_WRITE_DISABLE     0x04
#define M25P16_PAGE_PROGRAM      0x02
#define M25P16_SECTOR_ERASE      0xD8
#define M25P16_BULK_ERASE        0xC7


#define M25P16_WRITE_IN_PROGRESS 0x01
#define M25P16_WRITE_ENABLED     0x02


#define M25P16_PAGESIZE          256


#define ID_MICRON_M25P16         0x202015
#define ID_MICRON_N25Q064        0x20BA17
#define ID_WINBOND_W25Q64        0xEF4017
#define ID_MICRON_N25Q128        0x20ba18
#define ID_WINBOND_W25Q128       0xEF4018


static void SpiInit(uint32_t baudRatePrescaler);
static int M25p16ReadIdSetFlashRecord(void);
static unsigned int M25p16ReadIdSetFlashRecordDma(void);
static uint8_t M25p16ReadStatus(void);
static int FlashChipReadWriteDataSpiDma(uint8_t *txData, uint8_t *rxData, uint16_t length);
static int M25p16DmaReadPage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer);
extern void M25p16DmaWritePage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer);


void M25p16DmaWritePage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer)
{

	//write data from txBuffer into flash chip using DMA.
	//command and dummy bytes are in rxBuffer

	//once the command is executed (by DMA handler when CS goes high), the chip will take between 0.8 and 5 ms to run
	//this means 256 bytes can be written every 160 cycles at 32 KHz worst case (5ms)
	//and that 256 bytes can be written every 5 cycles at 1 KHz worst case
	//1 KHz safe logging is 51 bytes per cycle. That's 12 full floats
	//typical case allows 6 times more data. 72 floats per 1 KHz.
	//12 floats per cycle at 1 KHz is about 48  KB per second. 48  KB per seconds will last 341 seconds (5.68 minutes of flight time).
	//72 floats per cycle at 1 KHz is about 288 KB per second. 288 KB per seconds will last 56 seconds (a bit under 1 minute of flight time).

	//rx buffer is just used as a dummy, we can completely ignore it

  	txBuffer[0] = M25P16_PAGE_PROGRAM;
  	txBuffer[1] = ((address >> 16) & 0xFF);
  	txBuffer[2] = ((address >> 8) & 0xFF);
  	txBuffer[3] = (address & 0xFF);

	if (HAL_DMA_GetState(&dmaHandles[board.dmasActive[board.spis[board.flash[0].spiNumber].TXDma].dmaHandle]) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle]) == HAL_SPI_STATE_READY) {
		WriteEnableDataFlash();
		inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);
		flashInfo.status = DMA_DATA_WRITE_IN_PROGRESS;
		FlashChipReadWriteDataSpiDma(txBuffer, rxBuffer, FLASH_CHIP_BUFFER_SIZE);
	}

}


static int M25p16DmaReadPage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer)
{
	//address need to be aligned with the pages. We won't check since this is C!! Woohoo!
    //set up non blocking READ of data

  	bzero(txBuffer, FLASH_CHIP_BUFFER_SIZE);
  	bzero(rxBuffer, FLASH_CHIP_BUFFER_SIZE);
  	txBuffer[0] = M25P16_READ_BYTES;
  	txBuffer[1] = ((address >> 16) & 0xFF);
  	txBuffer[2] = ((address >> 8) & 0xFF);
  	txBuffer[3] = (address & 0xFF);

  	inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);
	flashInfo.status = DMA_DATA_READ_IN_PROGRESS;

	if (HAL_DMA_GetState(&dmaHandles[board.dmasActive[board.spis[board.flash[0].spiNumber].TXDma].dmaHandle]) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle]) == HAL_SPI_STATE_READY) {
		FlashChipReadWriteDataSpiDma(txBuffer, rxBuffer, FLASH_CHIP_BUFFER_SIZE);
		return(1);
	}
	return(0);

}


int M25p16ReadPage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer)
{
	//address need to be aligned with the pages. We won't check since this is C!! Woohoo!
    //set up non blocking READ of data

  	bzero(txBuffer, FLASH_CHIP_BUFFER_SIZE);
  	bzero(rxBuffer, FLASH_CHIP_BUFFER_SIZE);
  	txBuffer[0] = M25P16_READ_BYTES;
  	txBuffer[1] = ((address >> 16) & 0xFF);
  	txBuffer[2] = ((address >> 8) & 0xFF);
  	txBuffer[3] = (address & 0xFF);

  	inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);

	if (HAL_DMA_GetState(&dmaHandles[board.dmasActive[board.spis[board.flash[0].spiNumber].TXDma].dmaHandle]) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle]) == HAL_SPI_STATE_READY) {

		if (HAL_SPI_TransmitReceive(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], txBuffer, rxBuffer, FLASH_CHIP_BUFFER_SIZE, 1000) == HAL_OK) {
			inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);
			return 1;
		}

	}

	inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);
	return 0;

}


static int M25p16ReadIdSetFlashRecord(void)
{
    uint8_t command[] = { M25P16_RDID, 0, 0, 0};
    uint8_t reply[4];

    bzero(reply,sizeof(reply));

    flashInfo.currentWriteAddress = 0; //todo in future, read flash and determin where we need to begin.
    flashInfo.enabled = FLASH_DISABLED;
    flashInfo.chipId = 0;
    flashInfo.flashSectors = 0;
	flashInfo.pagesPerSector = 0;
	flashInfo.sectorSize = 0;
	flashInfo.totalSize = 0;
	flashInfo.pageSize = 0;
	flashInfo.status = 0;

	bzero(flashInfo.commandRxBuffer,sizeof(flashInfo.commandRxBuffer));
	bzero(flashInfo.commandTxBuffer,sizeof(flashInfo.commandTxBuffer));

	for (uint32_t x;x<2;x++)
	{
		flashInfo.buffer[x].txBufferPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
		flashInfo.buffer[x].rxBufferPtr = 0;

	}

	flashInfo.bufferNum = 0;


    flashInfo.pageSize     = M25P16_PAGESIZE;

    inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);
    HAL_SPI_TransmitReceive(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], command, reply, sizeof(command), 100);
	inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);

	flashInfo.chipId = (uint32_t)( (reply[1] << 16) | (reply[2] << 8) | (reply[3]) );

    switch (flashInfo.chipId) {
		case ID_MICRON_N25Q128:
		case ID_WINBOND_W25Q128:
			flashInfo.flashSectors = 256;
			flashInfo.pagesPerSector = 256;
			break;
		case ID_MICRON_N25Q064:
		case ID_WINBOND_W25Q64:
			flashInfo.flashSectors = 128;
			flashInfo.pagesPerSector = 256;
			break;
        case ID_MICRON_M25P16:
        	flashInfo.flashSectors = 32;
        	flashInfo.pagesPerSector = 256;
        	break;
        default:
        	flashInfo.chipId = 0;
        	flashInfo.flashSectors = 0;
        	flashInfo.pagesPerSector = 0;
        	flashInfo.sectorSize = 0;
        	flashInfo.totalSize = 0;
            return 0;
    }

    flashInfo.sectorSize = flashInfo.pagesPerSector * flashInfo.pageSize;
    flashInfo.totalSize = flashInfo.sectorSize * flashInfo.flashSectors;

    return flashInfo.chipId;
}

static unsigned int M25p16ReadIdSetFlashRecordDma(void)
{

    uint32_t x;

    //set up non blocking READ of status buffer
    flashInfo.status = DMA_READ_ID_IN_PROGRESS;
  	bzero(flashInfo.commandTxBuffer, sizeof(flashInfo.commandTxBuffer));
  	bzero(flashInfo.commandRxBuffer, sizeof(flashInfo.commandRxBuffer));
  	flashInfo.commandTxBuffer[0] = M25P16_RDID;
  	FlashChipReadWriteDataSpiDma(flashInfo.commandTxBuffer, flashInfo.commandRxBuffer, 4);

  	//give the DMA test 100 ms to complete. If it passes then we return 1.
  	for (x=0;x<100;x++) {
  		if (flashInfo.status != DMA_READ_COMPLETE)
  			DelayMs(1);
  		else
  			break;
  	}

  	if (x>98)
  		return 0;

  	return ( (uint32_t)( (flashInfo.commandRxBuffer[1] << 16) | (flashInfo.commandRxBuffer[2] << 8) | (flashInfo.commandRxBuffer[3]) ) );

}

static void SpiInit(uint32_t baudRatePrescaler)
{

	spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Instance               = spiInstance[board.flash[0].spiNumber];
    HAL_SPI_DeInit(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle]);

    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.Mode              = SPI_MODE_MASTER;
    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.Direction         = SPI_DIRECTION_2LINES;
    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.DataSize          = SPI_DATASIZE_8BIT;
    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.CLKPolarity       = SPI_POLARITY_HIGH;
    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.CLKPhase          = SPI_PHASE_2EDGE;
    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.NSS               = SPI_NSS_SOFT;
    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.BaudRatePrescaler = baudRatePrescaler;
    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.FirstBit          = SPI_FIRSTBIT_MSB;
    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.TIMode            = SPI_TIMODE_DISABLE;
    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    spiHandles[board.spis[board.flash[0].spiNumber].spiHandle].Init.CRCPolynomial     = 7;

    if (HAL_SPI_Init(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle]) != HAL_OK) {
        ErrorHandler(FLASH_SPI_INIT_FAILIURE);
    }

    inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);

}


int CheckIfFlashBusy(void)
{
	return(0);
}

int FindFirstEmptyPage(void)
{

	buffer_record *buffer = &flashInfo.buffer[flashInfo.bufferNum];

	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t allFFs;
	uint32_t allFFsTotal = 0;
	uint32_t firstEmptySector;

	//find first empty sector
	for (x = 0; x < flashInfo.totalSize; x = x + (flashInfo.pageSize * flashInfo.pagesPerSector) )
	{

		if ( M25p16ReadPage( x, buffer->txBuffer, buffer->rxBuffer) )
		{

			allFFs = 1;

			for (y=0;y<flashInfo.pageSize;y++) //check if page is empty, all 0xFF's
			{
				if (buffer->rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+y] != 0xFF)
					allFFs = 0; //any non FF's will set this to 0.
			}

			//firstEmptySector = x*(flashInfo.pageSize * flashInfo.pagesPerSector);

			if (allFFs && (x == 0) )
			{
				flashInfo.enabled = FLASH_ENABLED;
				flashInfo.currentWriteAddress = 0;
				return (1);
			}
			else if (allFFs)
			{

				firstEmptySector = x - (flashInfo.pageSize * flashInfo.pagesPerSector);
				//find first empty page in sector
				//for each sector we look for 4 FF pages in a row (FF being empty)
				for (z = (firstEmptySector);z < ( (firstEmptySector) + (flashInfo.pageSize * flashInfo.pagesPerSector) * 2);z = z + flashInfo.pageSize)
				{

					if ( M25p16ReadPage( z, buffer->txBuffer, buffer->rxBuffer) )
					{

						allFFs = 1;

						//check if page is empty, all 0xFF's
						for (y=0;y<flashInfo.pageSize;y++)
						{
							if (buffer->rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+y] != 0xFF)
							{
								allFFs = 0; //any non FF's will set this to 0.
								allFFsTotal = 0;
							}
						}

						//this page is empty since all FF's is an empty page
						if (allFFs)
						{
							allFFsTotal++;
						}

						if (z >= (uint32_t)( 0.95f * (float)flashInfo.totalSize ))
						{
							flashInfo.currentWriteAddress = z;
							flashInfo.enabled = FLASH_FULL;
							return (0);
						}

						if (allFFsTotal == 4)
						{
							flashInfo.enabled = FLASH_ENABLED;
							flashInfo.currentWriteAddress = (z - ( flashInfo.pageSize  * 3 ) );
							return (1);
						}

					}
					else
					{

						flashInfo.currentWriteAddress = z;
						flashInfo.enabled = FLASH_FULL;
						return (0);

					}

				}

				flashInfo.currentWriteAddress = z;
				flashInfo.enabled = FLASH_FULL;
				return (0);
			}

		}
		else
		{
			flashInfo.currentWriteAddress = x;
			flashInfo.enabled = FLASH_FULL;
			return (0);
		}

	}

}

void FlashDeinit(void)
{

	flashInfo.enabled = FLASH_DISABLED;

	//set CS high
    inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);

    //SPI DeInit will disable the GPIOs, DMAs, IRQs and SPIs attached to this SPI handle
    HAL_SPI_DeInit(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle]); //TODO: Remove all HAL and place these functions in the stm32.c file so we can support other MCU families.

}

int InitFlashChip(void)
{

	//TODO: Allow working with multiple flash chips

	//TODO: Check for DMA conflicts
	if (board.dmasSpi[board.spis[board.flash[0].spiNumber].RXDma].enabled)
	{
		memcpy( &board.dmasActive[board.spis[board.flash[0].spiNumber].RXDma], &board.dmasSpi[board.spis[board.flash[0].spiNumber].RXDma], sizeof(board_dma) );
	}
	if (board.dmasSpi[board.spis[board.flash[0].spiNumber].TXDma].enabled)
	{
		memcpy( &board.dmasActive[board.spis[board.flash[0].spiNumber].TXDma], &board.dmasSpi[board.spis[board.flash[0].spiNumber].TXDma], sizeof(board_dma) );
	}

	//FlashDeinit();

    SpiInit(board.flash[0].spiFastBaud);

    //check Read ID in blocking mode


    if (!M25p16ReadIdSetFlashRecord())
    {
    	DelayMs(70);
    	if (!M25p16ReadIdSetFlashRecord())
    	{
    		return 0;
    	}
    }

    return ( FindFirstEmptyPage() );
    //check Read ID in nonblocking mode, this function blocks, but it's used to test the non blocking functionality of the chip
    if (M25p16ReadIdSetFlashRecordDma() == flashInfo.chipId)
    {
    	//flash chip is good! Let's check if we can write to it and where we can write to
        if ( flashInfo.chipId )
        {
        	return ( FindFirstEmptyPage() );
        }

    }

    if (0)
    {
    	M25p16DmaReadPage(0, flashInfo.buffer[0].txBuffer, flashInfo.buffer[0].rxBuffer);
    }
    return (0);
}

void DataFlashProgramPage(uint32_t address, uint8_t *data, uint16_t length)
{
    uint8_t command[] = { M25P16_PAGE_PROGRAM, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    WriteEnableDataFlash();
    inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);
    HAL_SPI_Transmit(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], command, sizeof(command), 100);
    HAL_SPI_Transmit(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], data, length, 100);
	inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);
}

static uint8_t M25p16ReadStatus(void)
{
    uint8_t command[1] = {M25P16_READ_STATUS_REG};
    uint8_t in[2];

    bzero(in,sizeof(in));
	inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);
	HAL_SPI_Transmit(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], command, sizeof(command), 100);
	HAL_SPI_Receive(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], in, 2, 100);
	inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);

    return in[0];

}

inline void WriteEnableDataFlash(void)
{

	uint8_t c[1] = {M25P16_WRITE_ENABLE};

	//blocking transfer
	inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);
	HAL_SPI_Transmit(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], c, 1, 100);
	inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);
	simpleDelay_ASM(3); //give port time to change

}

int WriteEnableDataFlashDma(void)
{

	uint8_t c[1] = {M25P16_WRITE_ENABLE};

    if (HAL_DMA_GetState(&dmaHandles[board.dmasActive[board.spis[board.flash[0].spiNumber].TXDma].dmaHandle]) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle]) == HAL_SPI_STATE_READY)
    {

    	inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);
        //flashInfo.status = DMA_DATA_READ_IN_PROGRESS;
        HAL_SPI_Transmit_DMA(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], c, 1);

        return (1);

    }
    else
    {
        return (0);
    }

}

int MassEraseDataFlashByPage(int blocking)
{
	uint8_t command[4];
	uint32_t sectorsToErase = 0;
	uint32_t x;
	uint32_t addressToErase;

	(void)(blocking);

	sectorsToErase = ceil((float)flashInfo.currentWriteAddress / ((float)flashInfo.pageSize * (float)flashInfo.pagesPerSector));

	//for each sector
	for (x = 0;x<sectorsToErase;x++)
	{

		WriteEnableDataFlash();

		addressToErase = ( x * (flashInfo.pageSize * flashInfo.pagesPerSector) );
		command[0] = M25P16_SECTOR_ERASE;
		command[1] = ((addressToErase >> 16) & 0xFF);
		command[2] = ((addressToErase >> 8) & 0xFF);
		command[3] = (addressToErase & 0xFF);

		inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);
		HAL_SPI_Transmit(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], command, 4, 100);
		inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);

		blocking = 0;
		while ((M25p16ReadStatus() & M25P16_WRITE_IN_PROGRESS)) { //flash chip busy
			FeedTheDog();
			DelayMs(1);
			blocking++;
			if (blocking == 4000) //four seconds max time
				return 0;
		}
	}
	flashInfo.currentWriteAddress = 0;
	return 1;
}

int MassEraseDataFlash(int blocking)
{

	uint8_t c[1] = {M25P16_BULK_ERASE};

	//flash chip not busy
	if (!(M25p16ReadStatus() & M25P16_WRITE_IN_PROGRESS))
	{
		WriteEnableDataFlash();

		inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);
		HAL_SPI_Transmit(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], c, 1, 100);
		inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);

		if (blocking)
		{
			blocking = 0;
			//flash chip busy
			while ((M25p16ReadStatus() & M25P16_WRITE_IN_PROGRESS))
			{
				DelayMs(1);
				blocking++;
				if (blocking == 80000)
					return 0;
			}
			flashInfo.currentWriteAddress = 0;
			flashInfo.enabled = FLASH_ENABLED;
			return 1;
		}
		else
		{
			//flash chip busy
			if ((M25p16ReadStatus() & M25P16_WRITE_IN_PROGRESS))
			{
				flashInfo.currentWriteAddress = 0;
				flashInfo.enabled = FLASH_ENABLED;
				return 1;
			}
		}

	}

	return 0;

}

// TODO: get rid of this? only need read/write register and read/write data w/DMA or interrupt
int FlashChipWriteData(uint8_t *data, uint8_t length)
{
	(void)(data);
	(void)(length);

    return 1;
}


int FlashChipReadData(uint32_t address, uint8_t *buffer, int length)
{
    uint8_t command[] = { M25P16_READ_BYTES, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);

    HAL_SPI_Transmit(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], command, sizeof(command), 100);
    HAL_SPI_Receive(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], buffer, length, 100);

    inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);

    return length;
}

static int FlashChipReadWriteDataSpiDma(uint8_t *txData, uint8_t *rxData, uint16_t length)
{
    // ensure that both SPI and DMA resources are available, but don't block if they are not

    if (HAL_DMA_GetState(&dmaHandles[board.dmasActive[board.spis[board.flash[0].spiNumber].TXDma].dmaHandle]) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle]) == HAL_SPI_STATE_READY) {

    	inlineDigitalLo(ports[board.flash[0].csPort], board.flash[0].csPin);
        //flashInfo.status = DMA_DATA_READ_IN_PROGRESS;
        HAL_SPI_TransmitReceive_DMA(&spiHandles[board.spis[board.flash[0].spiNumber].spiHandle], txData, rxData, length);

        return (1);

    } else {
        return (0);
    }

}

void FlashDmaRxCallback(void)
{

	if (HAL_DMA_GetState(&dmaHandles[board.dmasActive[board.spis[board.flash[0].spiNumber].RXDma].dmaHandle]) == HAL_DMA_STATE_READY) {
        // reset chip select line
    	inlineDigitalHi(ports[board.flash[0].csPort], board.flash[0].csPin);
    	bzero(flashInfo.commandTxBuffer, sizeof(flashInfo.commandTxBuffer));
		flashInfo.status = DMA_READ_COMPLETE;
    }

}
