#include "includes.h"



SPI_HandleTypeDef flash_spi;
DMA_HandleTypeDef dma_flash_rx;
DMA_HandleTypeDef dma_flash_tx;
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
static void DmaInit(void);
static int M25p16ReadIdSetFlashRecord(void);
static unsigned int M25p16ReadIdSetFlashRecordDma(void);
static uint8_t M25p16ReadStatus(void);
static int FlashChipReadWriteDataSpiDma(uint8_t *txData, uint8_t *rxData, uint16_t length);
static int M25p16DmaReadPage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer);
extern void M25p16DmaWritePage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer);


void M25p16DmaWritePage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer) {

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

	//while (HAL_DMA_GetState(&dma_flash_tx) != HAL_DMA_STATE_READY || HAL_SPI_GetState(&flash_spi) != HAL_SPI_STATE_READY);
	if (HAL_DMA_GetState(&dma_flash_tx) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&flash_spi) == HAL_SPI_STATE_READY) {
		WriteEnableDataFlash();
		inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
		flashInfo.status = DMA_DATA_WRITE_IN_PROGRESS;
		FlashChipReadWriteDataSpiDma(txBuffer, rxBuffer, FLASH_CHIP_BUFFER_SIZE);
	}

}

static int M25p16DmaReadPage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer) {
	//address need to be aligned with the pages. We won't check since this is C!! Woohoo!
    //set up non blocking READ of data

  	bzero(txBuffer, FLASH_CHIP_BUFFER_SIZE);
  	bzero(rxBuffer, FLASH_CHIP_BUFFER_SIZE);
  	txBuffer[0] = M25P16_READ_BYTES;
  	txBuffer[1] = ((address >> 16) & 0xFF);
  	txBuffer[2] = ((address >> 8) & 0xFF);
  	txBuffer[3] = (address & 0xFF);

  	inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
	flashInfo.status = DMA_DATA_READ_IN_PROGRESS;

	if (HAL_DMA_GetState(&dma_flash_tx) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&flash_spi) == HAL_SPI_STATE_READY) {
		FlashChipReadWriteDataSpiDma(txBuffer, rxBuffer, FLASH_CHIP_BUFFER_SIZE);
		return(1);
	}
	return(0);

}

int M25p16ReadPage(uint32_t address, uint8_t *txBuffer, uint8_t *rxBuffer) {
	//address need to be aligned with the pages. We won't check since this is C!! Woohoo!
    //set up non blocking READ of data

  	bzero(txBuffer, FLASH_CHIP_BUFFER_SIZE);
  	bzero(rxBuffer, FLASH_CHIP_BUFFER_SIZE);
  	txBuffer[0] = M25P16_READ_BYTES;
  	txBuffer[1] = ((address >> 16) & 0xFF);
  	txBuffer[2] = ((address >> 8) & 0xFF);
  	txBuffer[3] = (address & 0xFF);

  	inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
	flashInfo.status = DMA_DATA_READ_IN_PROGRESS;

	if (HAL_DMA_GetState(&dma_flash_tx) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&flash_spi) == HAL_SPI_STATE_READY) {

		if (HAL_SPI_TransmitReceive(&flash_spi, txBuffer, rxBuffer, FLASH_CHIP_BUFFER_SIZE, 1000) == HAL_OK) {
			inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
			return 1;
		}

	}

	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
	return 0;

}

static int M25p16ReadIdSetFlashRecord(void)
{
    uint8_t command[] = { M25P16_RDID, 0, 0, 0};
    uint8_t reply[4];

    bzero(reply,sizeof(reply));

    flashInfo.currentWriteAddress = 0; //todo in future, read flash and determin where we need to begin.
    flashInfo.enabled = 0;
    flashInfo.chipId = 0;
    flashInfo.flashSectors = 0;
	flashInfo.pagesPerSector = 0;
	flashInfo.sectorSize = 0;
	flashInfo.totalSize = 0;
	flashInfo.pageSize = 0;
	flashInfo.status = 0;

	bzero(flashInfo.commandRxBuffer,sizeof(flashInfo.commandRxBuffer));
	bzero(flashInfo.commandTxBuffer,sizeof(flashInfo.commandTxBuffer));
/*
	bzero(flashInfo.txBufferA,sizeof(flashInfo.txBufferA));
	bzero(flashInfo.rxBufferA,sizeof(flashInfo.rxBufferA));
	bzero(flashInfo.txBufferB,sizeof(flashInfo.txBufferB));
	bzero(flashInfo.rxBufferB,sizeof(flashInfo.rxBufferB));
*/

	/*
	bzero(flashInfo.txBuffer,sizeof(flashInfo.txBuffer));
	bzero(flashInfo.rxBuffer,sizeof(flashInfo.rxBuffer));
*/

	/*
	flashInfo.bufferStatus = BUFFER_STATUS_FILLING_A;

	flashInfo.txBufferAPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
	flashInfo.txBufferBPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
	flashInfo.rxBufferAPtr = 0;
	flashInfo.rxBufferBPtr = 0;
*/
	for (uint32_t x;x<2;x++)
	{
		flashInfo.buffer[x].txBufferPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
		flashInfo.buffer[x].rxBufferPtr = 0;

	}

	flashInfo.bufferNum = 0;
	for (int x=0;x<2;x++)
	{
		flashInfo.buffer[x].txBufferPtr = FLASH_CHIP_BUFFER_WRITE_DATA_START;
		flashInfo.buffer[x].rxBufferPtr = 0;
	}


    flashInfo.pageSize     = M25P16_PAGESIZE;

    inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
    HAL_SPI_TransmitReceive(&flash_spi, command, reply, sizeof(command), 100);
	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

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
	//GPIO_InitTypeDef  GPIO_InitStruct;

	//this is all handled in stm32f4xx_spi_msp.c
	///*##-2- Configure peripheral GPIO ##########################################*/
	///* SPI SCK GPIO pin configuration  */
	//GPIO_InitStruct.Pin       = FLASH_SPI_SCK_PIN;
	//GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	//GPIO_InitStruct.Pull      = GPIO_NOPULL;
	//GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	//GPIO_InitStruct.Alternate = FLASH_SPI_SCK_AF;
	//HAL_GPIO_Init(FLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

	///* SPI MISO GPIO pin configuration  */
	//GPIO_InitStruct.Pin = FLASH_SPI_MISO_PIN;
	//GPIO_InitStruct.Alternate = FLASH_SPI_MISO_AF;
	//HAL_GPIO_Init(FLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

	///* SPI MOSI GPIO pin configuration  */
	//GPIO_InitStruct.Pin = FLASH_SPI_MOSI_PIN;
	//GPIO_InitStruct.Alternate = FLASH_SPI_MOSI_AF;
	//HAL_GPIO_Init(FLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

	//InitializeGpio(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin, 1); //soft CS

	flash_spi.Instance = FLASH_SPI;
    HAL_SPI_DeInit(&flash_spi);

    flash_spi.Init.Mode = SPI_MODE_MASTER;
    flash_spi.Init.Direction = SPI_DIRECTION_2LINES;
    flash_spi.Init.DataSize = SPI_DATASIZE_8BIT;
    flash_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
    flash_spi.Init.CLKPhase = SPI_PHASE_2EDGE;
    flash_spi.Init.NSS = SPI_NSS_SOFT;
    flash_spi.Init.BaudRatePrescaler = baudRatePrescaler;
    flash_spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    flash_spi.Init.TIMode = SPI_TIMODE_DISABLE;
    flash_spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    flash_spi.Init.CRCPolynomial = 7;

    if (HAL_SPI_Init(&flash_spi) != HAL_OK) {
        ErrorHandler();
    }

    HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin, GPIO_PIN_SET);


}

static void DmaInit(void)
{
    /* DMA interrupt init */
    HAL_NVIC_SetPriority(FLASH_DMA_TX_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(FLASH_DMA_TX_IRQn);
    HAL_NVIC_SetPriority(FLASH_DMA_RX_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(FLASH_DMA_RX_IRQn);
}

int CheckIfFlashBusy(void) {
	return(0);
}

int FindFirstEmptyPage(void) {
	buffer_record *buffer = &flashInfo.buffer[flashInfo.bufferNum];

	uint32_t x;
	uint32_t y;
	uint32_t allFFs;

	for (x = 0;x < flashInfo.totalSize;x = x + flashInfo.pageSize) {

		if ( M25p16ReadPage( x, buffer->txBuffer, buffer->rxBuffer) ) {

			allFFs = 1;

			for (y=0;y<flashInfo.pageSize;y++) { //check if page is empty, all 0xFF's
				if (buffer->rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+y] != 0xFF)
					allFFs = 0; //any non FF's will set this to 0.
			}

			if (allFFs) { //this page is empty since
				flashInfo.enabled = 1;
				flashInfo.currentWriteAddress = x;
				return 1;
			}

		} else {

			flashInfo.enabled = 1;
			return 0;

		}

	}

	flashInfo.enabled = 0;
	return 0;

}

int InitFlashChip(void)
{

	//uint32_t x;

    HAL_NVIC_DisableIRQ(FLASH_DMA_TX_IRQn);
    HAL_NVIC_DisableIRQ(FLASH_DMA_RX_IRQn);

    SpiInit(FLASH_SPI_BAUD);

    //check Read ID in blocking mode


    if (!M25p16ReadIdSetFlashRecord()) {
    	DelayMs(70);
    	if (!M25p16ReadIdSetFlashRecord()) {
    		return 0;
    	}
    }

    DmaInit();
    flashInfo.enabled = 1;
    return ( FindFirstEmptyPage() );
    //check Read ID in nonblocking mode, this function blocks, but it's used to test the non blocking functionality of the chip
    if (M25p16ReadIdSetFlashRecordDma() == flashInfo.chipId) {
        if ( flashInfo.chipId ) {
        	flashInfo.enabled = 1;
        	//flash chip is good! Let's check if we can write to it and where we can write to
        	return ( FindFirstEmptyPage() );
        }

    }

    return 0;
}

void DoStuff(void) {

	/*
    MassEraseDataFlash();

    while (m25p16_readStatus() & M25P16_WRITE_IN_PROGRESS) {
    	DelayMs(1);
    }


    uint32_t address = 0x00;
    uint8_t buffer[10];
    bzero(buffer,sizeof(buffer));

    int length = 10;

    FlashChipReadData(address, buffer, length);
    DelayMs(20);

    WriteEnableDataFlash();

    uint8_t writeData[256];

    uint32_t x;

    for (x=4;x<260;x++)
    	writeData[x-4]=x+4;

    DataFlashProgramPage(address, writeData, 256);

    DelayMs(5);
    while (m25p16_readStatus() & M25P16_WRITE_IN_PROGRESS) {
    	DelayMs(1);
    }
    volatile uint8_t dog = buffer[3];

    FlashChipReadData(address, buffer, length);
    DelayMs(20);
    volatile uint8_t rat = buffer[3];
*/
}
void DataFlashProgramPage(uint32_t address, uint8_t *data, uint16_t length)
{
    uint8_t command[] = { M25P16_PAGE_PROGRAM, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    WriteEnableDataFlash();
    inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
    HAL_SPI_Transmit(&flash_spi, command, sizeof(command), 100);
    HAL_SPI_Transmit(&flash_spi, data, length, 100);
	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

}

static uint8_t M25p16ReadStatus(void)
{
    uint8_t command[1] = {M25P16_READ_STATUS_REG};
    uint8_t in[2];

    bzero(in,sizeof(in));
	inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
	HAL_SPI_Transmit(&flash_spi, command, sizeof(command), 100);
	HAL_SPI_Receive(&flash_spi, in, 2, 100);
	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

    return in[0];
    /*
    volatile uint8_t command[2] = {M25P16_INSTRUCTION_READ_STATUS_REG, 0};
    volatile uint8_t in[2];

	inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
	HAL_SPI_TransmitReceive(&flash_spi, command, in, sizeof(command), 100);
	HAL_SPI_Receive(&flash_spi, in, 1, 100);
	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

    return in[1];
     */
}

inline void WriteEnableDataFlash(void) {

	uint8_t c[1] = {M25P16_WRITE_ENABLE};

	//blocking transfer
	inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
	HAL_SPI_Transmit(&flash_spi, c, 1, 100);
	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
	simpleDelay_ASM(1);

}

int WriteEnableDataFlashDma(void) {

	uint8_t c[1] = {M25P16_WRITE_ENABLE};

    if (HAL_DMA_GetState(&dma_flash_tx) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&flash_spi) == HAL_SPI_STATE_READY) {

    	inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
        //flashInfo.status = DMA_DATA_READ_IN_PROGRESS;
        HAL_SPI_Transmit_DMA(&flash_spi, c, 1);

        return (1);

    } else {
        return (0);
    }

}

int MassEraseDataFlash(int blocking) {

	uint8_t c[1] = {M25P16_BULK_ERASE};

	if (!(M25p16ReadStatus() & M25P16_WRITE_IN_PROGRESS)) { //flash chip not busy
		WriteEnableDataFlash();

		inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
		HAL_SPI_Transmit(&flash_spi, c, 1, 100);
		inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

		flashInfo.currentWriteAddress = 0;
		if (blocking) {
			blocking = 0;
			while ((M25p16ReadStatus() & M25P16_WRITE_IN_PROGRESS)) { //flash chip busy
				DelayMs(1);
				blocking++;
				if (blocking == 80000)
					return 0;
			}
			flashInfo.currentWriteAddress = 0;
			return 1;
		} else {
			if ((M25p16ReadStatus() & M25P16_WRITE_IN_PROGRESS)) { //flash chip busy
				flashInfo.currentWriteAddress = 0;
				return 1;
			}
		}

	}

	return 0;

}

/*
uint8_t command[] = { M25P16_INSTRUCTION_READ_BYTES, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

if (!m25p16_waitForReady(DEFAULT_TIMEOUT_MILLIS)) {
    return 0;
}

ENABLE_M25P16;

spiTransfer(M25P16_SPI_INSTANCE, NULL, command, sizeof(command));
spiTransfer(M25P16_SPI_INSTANCE, buffer, NULL, length);

DISABLE_M25P16;

return length;
*/

// TODO: get rid of this? only need read/write register and read/write data w/DMA or interrupt
int FlashChipWriteData(uint8_t *data, uint8_t length)
{
	(void)(data);
	(void)(length);
    // poll until SPI is ready in case of ongoing DMA
//    while (HAL_SPI_GetState(&gyro_spi) != HAL_SPI_STATE_READY);

//    inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

//    inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
//    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_RESET);

//    HAL_SPI_Transmit(&gyro_spi, data, length, 100);
//    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_SET);

    return 1;
}


int FlashChipReadData(uint32_t address, uint8_t *buffer, int length)
{
    uint8_t command[] = { M25P16_READ_BYTES, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

    HAL_SPI_Transmit(&flash_spi, command, sizeof(command), 100);
    HAL_SPI_Receive(&flash_spi, buffer, length, 100);

    inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

    return length;
}

static int FlashChipReadWriteDataSpiDma(uint8_t *txData, uint8_t *rxData, uint16_t length)
{
    // ensure that both SPI and DMA resources are available, but don't block if they are not

    if (HAL_DMA_GetState(&dma_flash_tx) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&flash_spi) == HAL_SPI_STATE_READY) {

    	inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
        //flashInfo.status = DMA_DATA_READ_IN_PROGRESS;
        HAL_SPI_TransmitReceive_DMA(&flash_spi, txData, rxData, length);

        return (1);

    } else {
        return (0);
    }

}

void FLASH_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&dma_flash_tx);
}

void FLASH_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&dma_flash_rx);

    if (HAL_DMA_GetState(&dma_flash_rx) == HAL_DMA_STATE_READY) {
        // reset chip select line
    	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
    	bzero(flashInfo.commandTxBuffer, sizeof(flashInfo.commandTxBuffer));
		flashInfo.status = DMA_READ_COMPLETE;

    	//if (flashInfo.status == DMA_READ_ID_IN_PROGRESS)

        // run callback for completed gyro read
       // accgyroDeviceReadComplete();
    }

}
/*
#define M25P16_INSTRUCTION_RDID              0x9F
#define M25P16_INSTRUCTION_READ_BYTES        0x03
#define M25P16_INSTRUCTION_READ_STATUS_REG   0x05
#define M25P16_INSTRUCTION_WRITE_STATUS_REG  0x01
#define M25P16_INSTRUCTION_WRITE_ENABLE      0x06
#define M25P16_INSTRUCTION_WRITE_DISABLE     0x04
#define M25P16_INSTRUCTION_PAGE_PROGRAM      0x02
#define M25P16_INSTRUCTION_SECTOR_ERASE      0xD8
#define M25P16_INSTRUCTION_BULK_ERASE        0xC7

#define M25P16_STATUS_FLAG_WRITE_IN_PROGRESS 0x01
#define M25P16_STATUS_FLAG_WRITE_ENABLED     0x02


#define JEDEC_ID_MICRON_M25P16         0x202015
#define JEDEC_ID_MICRON_N25Q064        0x20BA17
#define JEDEC_ID_WINBOND_W25Q64        0xEF4017
#define JEDEC_ID_MICRON_N25Q128        0x20ba18
#define JEDEC_ID_WINBOND_W25Q128       0xEF4018

#define DISABLE_M25P16       IOHi(flashSpim25p16CsPin)
#define ENABLE_M25P16        IOLo(flashSpim25p16CsPin)


#define DEFAULT_TIMEOUT_MILLIS       6


#define SECTOR_ERASE_TIMEOUT_MILLIS  5000
#define BULK_ERASE_TIMEOUT_MILLIS    21000

static flashGeometry_t geometry = {.pageSize = M25P16_PAGESIZE};
static IO_t flashSpim25p16CsPin = IO_NONE;
//
// * Whether we've performed an action that could have made the device busy for writes.
// *
// * This allows us to avoid polling for writable status when it is definitely ready already.
//
static bool couldBeBusy = false;

//
// * Send the given command byte to the device.
//
static void m25p16_performOneByteCommand(uint8_t command)
{
    ENABLE_M25P16;

    spiTransferByte(M25P16_SPI_INSTANCE, command);

    DISABLE_M25P16;
}

//
// * The flash requires this write enable command to be sent before commands that would cause
// * a write like program and erase.
//
static void m25p16_writeEnable()
{
    m25p16_performOneByteCommand(M25P16_INSTRUCTION_WRITE_ENABLE);


    couldBeBusy = true;
}

static uint8_t m25p16_readStatus()
{
    uint8_t command[2] = {M25P16_INSTRUCTION_READ_STATUS_REG, 0};
    uint8_t in[2];

    ENABLE_M25P16;

    spiTransfer(M25P16_SPI_INSTANCE, in, command, sizeof(command));

    DISABLE_M25P16;

    return in[1];
}

bool m25p16_isReady()
{

    couldBeBusy = couldBeBusy && ((m25p16_readStatus() & M25P16_STATUS_FLAG_WRITE_IN_PROGRESS) != 0);

    return !couldBeBusy;
}

bool m25p16_waitForReady(uint32_t timeoutMillis)
{
    uint32_t time = millis();
    while (!m25p16_isReady()) {
        if (millis() - time > timeoutMillis) {
            return false;
        }
    }

    return true;
}

//
// * Read chip identification and geometry information (into global `geometry`).
// *
// * Returns true if we get valid ident, false if something bad happened like there is no M25P16.
//
static bool m25p16_readIdentification()
{
    uint8_t out[] = { M25P16_INSTRUCTION_RDID, 0, 0, 0};
    uint8_t in[4];
    uint32_t chipID;

    delay(50);

    // Just in case transfer fails and writes nothing, so we don't try to verify the ID against random garbage
    // * from the stack:
    //
    in[1] = 0;

    ENABLE_M25P16;

    spiTransfer(M25P16_SPI_INSTANCE, in, out, sizeof(out));


    DISABLE_M25P16;


    chipID = (in[1] << 16) | (in[2] << 8) | (in[3]);



    switch (chipID) {
        case JEDEC_ID_MICRON_M25P16:
            geometry.sectors = 32;
            geometry.pagesPerSector = 256;
        break;
        case JEDEC_ID_MICRON_N25Q064:
        case JEDEC_ID_WINBOND_W25Q64:
            geometry.sectors = 128;
            geometry.pagesPerSector = 256;
        break;
        case JEDEC_ID_MICRON_N25Q128:
        case JEDEC_ID_WINBOND_W25Q128:
            geometry.sectors = 256;
            geometry.pagesPerSector = 256;
        break;
        default:

            geometry.sectors = 0;
            geometry.pagesPerSector = 0;

            geometry.sectorSize = 0;
            geometry.totalSize = 0;
            return false;
    }

    geometry.sectorSize = geometry.pagesPerSector * geometry.pageSize;
    geometry.totalSize = geometry.sectorSize * geometry.sectors;

    couldBeBusy = true;

    return true;
}

//
// * Initialize the driver, must be called before any other routines.
// *
// * Attempts to detect a connected m25p16. If found, true is returned and device capacity can be fetched with
// * m25p16_getGeometry().
//
bool m25p16_init()
{
#ifdef M25P16_CS_PIN
	flashSpim25p16CsPin = IOGetByTag(IO_TAG(M25P16_CS_PIN));
#endif
    IOInit(flashSpim25p16CsPin, OWNER_SYSTEM, RESOURCE_SPI);
	IOConfigGPIO(flashSpim25p16CsPin, SPI_IO_CS_CFG);


    spiSetDivisor(M25P16_SPI_INSTANCE, SPI_ULTRAFAST_CLOCK);

    return m25p16_readIdentification();
}

//
// * Erase a sector full of bytes to all 1's at the given byte offset in the flash chip.
//
void m25p16_eraseSector(uint32_t address)
{
    uint8_t out[] = { M25P16_INSTRUCTION_SECTOR_ERASE, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    m25p16_waitForReady(SECTOR_ERASE_TIMEOUT_MILLIS);

    m25p16_writeEnable();

    ENABLE_M25P16;

    spiTransfer(M25P16_SPI_INSTANCE, NULL, out, sizeof(out));

    DISABLE_M25P16;
}

void m25p16_eraseCompletely()
{
    m25p16_waitForReady(BULK_ERASE_TIMEOUT_MILLIS);

    m25p16_writeEnable();

    m25p16_performOneByteCommand(M25P16_INSTRUCTION_BULK_ERASE);
}

void m25p16_pageProgramBegin(uint32_t address)
{
    uint8_t command[] = { M25P16_INSTRUCTION_PAGE_PROGRAM, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    m25p16_waitForReady(DEFAULT_TIMEOUT_MILLIS);

    m25p16_writeEnable();

    ENABLE_M25P16;

    spiTransfer(M25P16_SPI_INSTANCE, NULL, command, sizeof(command));
}

void m25p16_pageProgramContinue(const uint8_t *data, int length)
{
    spiTransfer(M25P16_SPI_INSTANCE, NULL, data, length);
}

void m25p16_pageProgramFinish()
{
    DISABLE_M25P16;
}

// * Write bytes to a flash page. Address must not cross a page boundary.
// *
// * Bits can only be set to zero, not from zero back to one again. In order to set bits to 1, use the erase command.
// *
// * Length must be smaller than the page size.
// *
// * This will wait for the flash to become ready before writing begins.
// *
// * Datasheet indicates typical programming time is 0.8ms for 256 bytes, 0.2ms for 64 bytes, 0.05ms for 16 bytes.
// * (Although the maximum possible write time is noted as 5ms).
// *
// * If you want to write multiple buffers (whose sum of sizes is still not more than the page size) then you can
// * break this operation up into one beginProgram call, one or more continueProgram calls, and one finishProgram call.

void m25p16_pageProgram(uint32_t address, const uint8_t *data, int length)
{
    m25p16_pageProgramBegin(address);

    m25p16_pageProgramContinue(data, length);

    m25p16_pageProgramFinish();
}

// * Read `length` bytes into the provided `buffer` from the flash starting from the given `address` (which need not lie
// * on a page boundary).
// *
// * Waits up to DEFAULT_TIMEOUT_MILLIS milliseconds for the flash to become ready before reading.
// *
// * The number of bytes actually read is returned, which can be zero if an error or timeout occurred.

int m25p16_readBytes(uint32_t address, uint8_t *buffer, int length)
{
    uint8_t command[] = { M25P16_INSTRUCTION_READ_BYTES, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    if (!m25p16_waitForReady(DEFAULT_TIMEOUT_MILLIS)) {
        return 0;
    }

    ENABLE_M25P16;

    spiTransfer(M25P16_SPI_INSTANCE, NULL, command, sizeof(command));
    spiTransfer(M25P16_SPI_INSTANCE, buffer, NULL, length);

    DISABLE_M25P16;

    return length;
}

// * Fetch information about the detected flash chip layout.
// *
// * Can be called before calling m25p16_init() (the result would have totalSize = 0).

const flashGeometry_t* m25p16_getGeometry()
{
    return &geometry;
}
*/
