#include "includes.h"

//MAX Register defines
#define MAX7456ADD_VM0           0x00
#define MAX7456ADD_VM1           0x01
#define MAX7456ADD_HOS           0x02
#define MAX7456ADD_VOS           0x03
#define MAX7456ADD_DMM           0x04
#define MAX7456ADD_DMAH          0x05
#define MAX7456ADD_DMAL          0x06
#define MAX7456ADD_DMDI          0x07
#define MAX7456ADD_CMM           0x08
#define MAX7456ADD_CMAH          0x09
#define MAX7456ADD_CMAL          0x0a
#define MAX7456ADD_CMDI          0x0b
#define MAX7456ADD_OSDM          0x0c
#define MAX7456ADD_RB0           0x10
#define MAX7456ADD_RB1           0x11
#define MAX7456ADD_RB2           0x12
#define MAX7456ADD_RB3           0x13
#define MAX7456ADD_RB4           0x14
#define MAX7456ADD_RB5           0x15
#define MAX7456ADD_RB6           0x16
#define MAX7456ADD_RB7           0x17
#define MAX7456ADD_RB8           0x18
#define MAX7456ADD_RB9           0x19
#define MAX7456ADD_RB10          0x1a
#define MAX7456ADD_RB11          0x1b
#define MAX7456ADD_RB12          0x1c
#define MAX7456ADD_RB13          0x1d
#define MAX7456ADD_RB14          0x1e
#define MAX7456ADD_RB15          0x1f
#define MAX7456ADD_OSDBL         0x6c
#define MAX7456ADD_STAT          0xA0

//MAX Command defines

// video mode register 0 bits
#define VIDEO_BUFFER_DISABLE     0x01
#define MAX7456_RESET            0x02
#define VERTICAL_SYNC_NEXT_VSYNC 0x04
#define OSD_ENABLE               0x08
#define SYNC_MODE_AUTO           0x00
#define SYNC_MODE_INTERNAL       0x30
#define SYNC_MODE_EXTERNAL       0x20
#define VIDEO_MODE_PAL           0x40
#define VIDEO_MODE_NTSC          0x00

#define CLEAR_display 0x04
#define CLEAR_display_vert 0x06
#define END_string 0xff
#define WRITE_NVRAM 0xa0
// with NTSC
#define ENABLE_display 0x08
#define ENABLE_display_vert 0x0c
#define MAX7456_reset 0x02
#define DISABLE_display 0x00
#define STATUS_REG_NVR_BUSY 0x20


#define NVRAM_SIZE 54

//Signal defines
#define SIGNAL_NTSC              0x01
#define SIGNAL_PAL               0x02

#ifndef WHITEBRIGHTNESS
  #define WHITEBRIGHTNESS 0x01
#endif
#ifndef BLACKBRIGHTNESS
  #define BLACKBRIGHTNESS 0x00
#endif

#define BWBRIGHTNESS ((BLACKBRIGHTNESS << 2) | WHITEBRIGHTNESS)

max_osd_record maxOsdRecord;

uint8_t txBuffer[16];
uint8_t rxBuffer[16];


static int SpiInit(uint32_t baudRatePrescaler);
static int MaxSendReceiveCommand(uint8_t address, uint8_t data, int blocking);

static int SpiInit(uint32_t baudRatePrescaler)
{

	spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Instance               = spiInstance[board.maxOsd[0].spiNumber];
    HAL_SPI_DeInit(&spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle]);

    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.Mode              = SPI_MODE_MASTER;
    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.Direction         = SPI_DIRECTION_2LINES;
    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.DataSize          = SPI_DATASIZE_8BIT;
    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.CLKPolarity       = SPI_POLARITY_HIGH;
    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.CLKPhase          = SPI_PHASE_2EDGE;
    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.NSS               = SPI_NSS_SOFT;
    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.BaudRatePrescaler = baudRatePrescaler;
    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.FirstBit          = SPI_FIRSTBIT_MSB;
    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.TIMode            = SPI_TIMODE_DISABLE;
    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle].Init.CRCPolynomial     = 7;

    if (HAL_SPI_Init(&spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle]) != HAL_OK)
    {
        ErrorHandler(OSD_SPI_INIT_FAILIURE);
        return(0);
    }
    else
    {
        inlineDigitalHi(ports[board.maxOsd[0].csPort], board.maxOsd[0].csPin);
        return(1);
    }

}

int InitMaxOsd(void)
{

	if (board.maxOsd[0].enabled)
    {

        //TODO: Check for DMA conflicts
        if (board.dmasSpi[board.spis[board.maxOsd[0].spiNumber].RXDma].enabled)
        {
            memcpy( &board.dmasActive[board.spis[board.maxOsd[0].spiNumber].RXDma], &board.dmasSpi[board.spis[board.maxOsd[0].spiNumber].RXDma], sizeof(board_dma) );
        }
        if (board.dmasSpi[board.spis[board.maxOsd[0].spiNumber].TXDma].enabled)
        {
            memcpy( &board.dmasActive[board.spis[board.maxOsd[0].spiNumber].TXDma], &board.dmasSpi[board.spis[board.maxOsd[0].spiNumber].TXDma], sizeof(board_dma) );
        }

        if (SpiInit(board.maxOsd[0].spiFastBaud))
        {
            MaxSendReceiveCommand(MAX7456ADD_VM0, MAX7456_RESET, 1);
            DelayMs(100);
            uint8_t spiReturnData;
            //B00000011 = 3
            while ((3 & spiReturnData) == 0)
            {
                spiReturnData = MaxSendReceiveCommand(MAX7456ADD_STAT, 0xFF, 1);
                DelayMs(100);
            }

            //check if signal is PAL or NTSC
            //B00000001 = 1
            //B00000010 = 2
            if ((1 & spiReturnData) == 1) //PAL
            {
                maxOsdRecord.videoMode = VIDEO_MODE_PAL;
            }
            else if((2 & spiReturnData) == 2) //NTSC
            {
                maxOsdRecord.videoMode = VIDEO_MODE_NTSC;
            }
        }
        else
        {
            return(0);
        }

    }

    return(0);
}

static int MaxSendReceiveCommand(uint8_t address, uint8_t data, int blocking)
{
    txBuffer[0] = address;
    txBuffer[1] = data;
    uint8_t returnData;
    inlineDigitalLo(ports[board.maxOsd[0].csPort], board.maxOsd[0].csPin);
    if (blocking)
    {
        HAL_SPI_TransmitReceive(&spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle], txBuffer, rxBuffer, 2, 25);
        returnData = rxBuffer[1];
        inlineDigitalHi(ports[board.maxOsd[0].csPort], board.maxOsd[0].csPin);
        return(returnData);
    }
    else
    {
        HAL_SPI_TransmitReceive_DMA(&spiHandles[board.spis[board.maxOsd[0].spiNumber].spiHandle], txBuffer, rxBuffer, 2);
    }

    return(-1);
}

void MaxOsdDmaRxCallback(uint32_t callbackNumber)
{

	(void)(callbackNumber);
	if (HAL_DMA_GetState(&dmaHandles[board.dmasActive[board.spis[board.maxOsd[0].spiNumber].RXDma].dmaHandle]) == HAL_DMA_STATE_READY) {
        // reset chip select line
    	inlineDigitalHi(ports[board.maxOsd[0].csPort], board.maxOsd[0].csPin);
    	//bzero(flashInfo.commandTxBuffer, sizeof(flashInfo.commandTxBuffer));
		//flashInfo.status = DMA_READ_COMPLETE;
    }

}