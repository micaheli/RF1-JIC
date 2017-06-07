#include "includes.h"

static void SpiInit(uint32_t baudRatePrescaler);

static void SpiInit(uint32_t baudRatePrescaler)
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
    }

    inlineDigitalHi(ports[board.maxOsd[0].csPort], board.maxOsd[0].csPin);

}

int InitMaxOsd(void)
{

	//TODO: Allow working with multiple flash chips

	//TODO: Check for DMA conflicts
	if (board.dmasSpi[board.spis[board.maxOsd[0].spiNumber].RXDma].enabled)
	{
		memcpy( &board.dmasActive[board.spis[board.maxOsd[0].spiNumber].RXDma], &board.dmasSpi[board.spis[board.maxOsd[0].spiNumber].RXDma], sizeof(board_dma) );
	}
	if (board.dmasSpi[board.spis[board.maxOsd[0].spiNumber].TXDma].enabled)
	{
		memcpy( &board.dmasActive[board.spis[board.maxOsd[0].spiNumber].TXDma], &board.dmasSpi[board.spis[board.maxOsd[0].spiNumber].TXDma], sizeof(board_dma) );
	}

    SpiInit(board.maxOsd[0].spiFastBaud);

    return (0);
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