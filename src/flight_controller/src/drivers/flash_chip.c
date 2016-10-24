#include "includes.h"

SPI_HandleTypeDef flash_spi;
DMA_HandleTypeDef dma_flash_rx;
DMA_HandleTypeDef dma_flash_tx;

static void SpiInit(uint32_t baudRatePrescaler);
static void DmaInit(void);

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

static bool m25p16_readIdentification()
{
    uint8_t out[] = { M25P16_INSTRUCTION_RDID, 0, 0, 0};
    uint8_t in[4];
    uint32_t chipID;

    DelayMs(50);

    // Just in case transfer fails and writes nothing, so we don't try to verify the ID against random garbage
    // * from the stack:
    //
    bzero(in,sizeof(in));

    inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
    HAL_SPI_TransmitReceive(&flash_spi, out, in, sizeof(out), 100);
	//HAL_SPI_Transmit(&flash_spi, data, sizeof(data), 100);
	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

    chipID = (in[1] << 16) | (in[2] << 8) | (in[3]);

/*

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
*/
    return true;
}

static void SpiInit(uint32_t baudRatePrescaler)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* SPI SCK GPIO pin configuration  */
	GPIO_InitStruct.Pin       = FLASH_SPI_SCK_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = FLASH_SPI_SCK_AF;
	HAL_GPIO_Init(FLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MISO GPIO pin configuration  */
	GPIO_InitStruct.Pin = FLASH_SPI_MISO_PIN;
	GPIO_InitStruct.Alternate = FLASH_SPI_MISO_AF;
	HAL_GPIO_Init(FLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MOSI GPIO pin configuration  */
	GPIO_InitStruct.Pin = FLASH_SPI_MOSI_PIN;
	GPIO_InitStruct.Alternate = FLASH_SPI_MOSI_AF;
	HAL_GPIO_Init(FLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

	InitializeGpio(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin, 1); //soft CS

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
    HAL_NVIC_SetPriority(FLASH_DMA_TX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(FLASH_DMA_TX_IRQn);
    HAL_NVIC_SetPriority(FLASH_DMA_RX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(FLASH_DMA_RX_IRQn);
}

int InitFlashChip(void)
{

    HAL_NVIC_DisableIRQ(FLASH_DMA_TX_IRQn);
    HAL_NVIC_DisableIRQ(FLASH_DMA_RX_IRQn);

    // read and write settings at slow speed
    SpiInit(FLASH_SPI_BAUD);
    DelayMs(5);

    //if (!accgyroDeviceDetect()) {
    //    return 0;
    //}

    DmaInit();
    m25p16_readIdentification();
    //uint8_t command[] = { M25P16_INSTRUCTION_READ_BYTES, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    volatile uint32_t fish = InlineMillis();
    MassEraseDataFlash();
    while (m25p16_readStatus() & M25P16_STATUS_FLAG_WRITE_IN_PROGRESS) {
    	DelayMs(1);
    }


    volatile uint32_t fish1 = InlineMillis();
    volatile uint32_t fish3 = fish1 - fish;

    //DelayMs(17000);
    //volatile uint8_t cat2 = m25p16_readStatus();

    uint32_t address = 0x00;
    uint8_t buffer[10];
    bzero(buffer,sizeof(buffer));

    int length = 10;

    FlashChipReadData(address, buffer, length);
    DelayMs(20);
    volatile uint8_t cat3 = m25p16_readStatus();

    volatile uint8_t cat = buffer[3];

    WriteEnableDataFlash();

    uint8_t writeData[256];

    uint32_t x;

    for (x=4;x<260;x++)
    	writeData[x-4]=x+4;

    DataFlashProgramPage(address, writeData, 256);

    DelayMs(5);
    while (m25p16_readStatus() & M25P16_STATUS_FLAG_WRITE_IN_PROGRESS) {
    	DelayMs(1);
    }
    volatile uint8_t dog = buffer[3];

    FlashChipReadData(address, buffer, length);
    DelayMs(20);
    volatile uint8_t rat = buffer[3];

    return 1;
}

void DataFlashProgramPage(uint32_t address, uint8_t *data, uint16_t length)
{
	uint32_t x;
    uint8_t command[] = { M25P16_INSTRUCTION_PAGE_PROGRAM, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    WriteEnableDataFlash();
    inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
    HAL_SPI_Transmit(&flash_spi, command, sizeof(command), 100);
    HAL_SPI_Transmit(&flash_spi, data, length, 100);
	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

}

uint8_t m25p16_readStatus(void)
{
    uint8_t command[1] = {M25P16_INSTRUCTION_READ_STATUS_REG};
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

void WriteEnableDataFlash(void) {

	uint8_t c[1] = {M25P16_INSTRUCTION_WRITE_ENABLE};

	//blocking transfer
	inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
	HAL_SPI_Transmit(&flash_spi, &c, 1, 100);
	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

}

void MassEraseDataFlash(void) {

	uint8_t c[1] = {M25P16_INSTRUCTION_BULK_ERASE};

	WriteEnableDataFlash();

	inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);
	HAL_SPI_Transmit(&flash_spi, &c, 1, 100);
	inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

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

        // run callback for completed gyro read
        accgyroDeviceReadComplete();
    }

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
    uint8_t command[] = { M25P16_INSTRUCTION_READ_BYTES, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

    HAL_SPI_Transmit(&flash_spi, command, sizeof(command), 100);
    HAL_SPI_Receive(&flash_spi, buffer, length, 100);

    inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

    return length;
}

int FlashChipReadWriteData(uint8_t *txData, uint8_t *rxData, uint8_t length)
{
    // ensure that both SPI and DMA resources are available, but don't block if they are not
    if (HAL_DMA_GetState(&dma_flash_tx) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&flash_spi) == HAL_SPI_STATE_READY) {
//        inlineDigitalLo(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

//        HAL_SPI_TransmitReceive_DMA(&flash_spi, txData, rxData, length);
//        inlineDigitalHi(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_GPIO_Pin);

        return 1;
    } else {
        return 0;
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
