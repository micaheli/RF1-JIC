#include "includes.h"

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    uint32_t spiInx;

    if (hspi->Instance == SPI1)
	{
		spiInx = ENUM_SPI1;
		__HAL_RCC_SPI1_CLK_ENABLE();
	}
	else if (hspi->Instance == SPI2)
	{
		spiInx = ENUM_SPI2;
		__HAL_RCC_SPI2_CLK_ENABLE();
	}
	else if (hspi->Instance == SPI3)
	{
		spiInx = ENUM_SPI3;
		__HAL_RCC_SPI3_CLK_ENABLE();
	}
	else
	{
		return;
	}


	HAL_GPIO_DeInit(ports[board.spis[spiInx].NSSPort],  board.spis[spiInx].NSSPin);
	HAL_GPIO_DeInit(ports[board.spis[spiInx].SCKPort],  board.spis[spiInx].SCKPin);
	HAL_GPIO_DeInit(ports[board.spis[spiInx].MISOPort], board.spis[spiInx].MISOPin);
	HAL_GPIO_DeInit(ports[board.spis[spiInx].MOSIPort], board.spis[spiInx].MOSIPin);

	GPIO_InitStruct.Pin   = board.spis[spiInx].NSSPin;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(ports[board.spis[spiInx].NSSPort], &GPIO_InitStruct);

	GPIO_InitStruct.Pin   = board.spis[spiInx].SCKPin;
	GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull  = board.spis[spiInx].SCKPull;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = board.spis[spiInx].SCKAlternate;
	HAL_GPIO_Init(ports[board.spis[spiInx].SCKPort], &GPIO_InitStruct);

	GPIO_InitStruct.Pin   = board.spis[spiInx].MISOPin;
	GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = board.spis[spiInx].MISOAlternate;
	HAL_GPIO_Init(ports[board.spis[spiInx].MISOPort], &GPIO_InitStruct);

	GPIO_InitStruct.Pin   = board.spis[spiInx].MOSIPin;
	GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = board.spis[spiInx].MOSIAlternate;
	HAL_GPIO_Init(ports[board.spis[spiInx].MOSIPort], &GPIO_InitStruct);


	/* Peripheral DMA init*/
	dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle].Instance                 = dmaStream[board.dmasActive[board.spis[spiInx].RXDma].dmaStream];
	dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle].Init.Channel             = board.dmasActive[board.spis[spiInx].RXDma].dmaChannel;
	dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle].Init.Direction           = board.dmasActive[board.spis[spiInx].RXDma].dmaDirection;
	dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle].Init.PeriphInc           = board.dmasActive[board.spis[spiInx].RXDma].dmaPeriphInc;
	dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle].Init.MemInc              = board.dmasActive[board.spis[spiInx].RXDma].dmaMemInc;
	dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle].Init.PeriphDataAlignment = board.dmasActive[board.spis[spiInx].RXDma].dmaPeriphAlignment;
	dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle].Init.MemDataAlignment    = board.dmasActive[board.spis[spiInx].RXDma].dmaMemAlignment;
	dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle].Init.Mode                = board.dmasActive[board.spis[spiInx].RXDma].dmaMode;
	dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle].Init.Priority            = board.dmasActive[board.spis[spiInx].RXDma].dmaPriority;
	dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle].Init.FIFOMode            = board.dmasActive[board.spis[spiInx].RXDma].fifoMode;

	HAL_DMA_UnRegisterCallback(&dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle], HAL_DMA_XFER_ALL_CB_ID);

	if (HAL_DMA_Init(&dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle]) != HAL_OK) {
		ErrorHandler(MSP_DMA_SPI_RX_INIT_FAILIURE);
	}

	__HAL_LINKDMA(hspi, hdmarx, dmaHandles[board.dmasActive[board.spis[spiInx].RXDma].dmaHandle]);


	dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle].Instance                 = dmaStream[board.dmasActive[board.spis[spiInx].TXDma].dmaStream];
	dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle].Init.Channel             = board.dmasActive[board.spis[spiInx].TXDma].dmaChannel;
	dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle].Init.Direction           = board.dmasActive[board.spis[spiInx].TXDma].dmaDirection;
	dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle].Init.PeriphInc           = board.dmasActive[board.spis[spiInx].TXDma].dmaPeriphInc;
	dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle].Init.MemInc              = board.dmasActive[board.spis[spiInx].TXDma].dmaMemInc;
	dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle].Init.PeriphDataAlignment = board.dmasActive[board.spis[spiInx].TXDma].dmaPeriphAlignment;
	dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle].Init.MemDataAlignment    = board.dmasActive[board.spis[spiInx].TXDma].dmaMemAlignment;
	dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle].Init.Mode                = board.dmasActive[board.spis[spiInx].TXDma].dmaMode;
	dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle].Init.Priority            = board.dmasActive[board.spis[spiInx].TXDma].dmaPriority;
	dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle].Init.FIFOMode            = board.dmasActive[board.spis[spiInx].TXDma].fifoMode;
	
	HAL_DMA_UnRegisterCallback(&dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle], HAL_DMA_XFER_ALL_CB_ID);
	
	if (HAL_DMA_Init(&dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle]) != HAL_OK) {
		ErrorHandler(MSP_DMA_SPI_TX_INIT_FAILIURE);
	}

	__HAL_LINKDMA(hspi, hdmatx, dmaHandles[board.dmasActive[board.spis[spiInx].TXDma].dmaHandle]);

	HAL_NVIC_SetPriority(board.dmasActive[board.spis[spiInx].TXDma].dmaIRQn, board.dmasActive[board.spis[spiInx].TXDma].priority, 0);
	HAL_NVIC_EnableIRQ(board.dmasActive[board.spis[spiInx].TXDma].dmaIRQn);

	HAL_NVIC_SetPriority(board.dmasActive[board.spis[spiInx].RXDma].dmaIRQn, board.dmasActive[board.spis[spiInx].RXDma].priority, 0);
	HAL_NVIC_EnableIRQ(board.dmasActive[board.spis[spiInx].RXDma].dmaIRQn);

	// Peripheral interrupt init
	HAL_NVIC_SetPriority(board.spis[spiInx].SPI_IRQn, board.spis[spiInx].priority, 0);
	HAL_NVIC_EnableIRQ(board.spis[spiInx].SPI_IRQn);

	board.dmasActive[board.spis[spiInx].TXDma].enabled = 1;
	board.dmasActive[board.spis[spiInx].RXDma].enabled = 1;

}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

	uint32_t spiInx;

	if (hspi->Instance == SPI1)
	{
		spiInx = ENUM_SPI1;
		__HAL_RCC_SPI1_CLK_DISABLE();
	}
	else if (hspi->Instance == SPI2)
	{
		spiInx = ENUM_SPI2;
		__HAL_RCC_SPI2_CLK_DISABLE();
	}
	else if (hspi->Instance == SPI3)
	{
		spiInx = ENUM_SPI3;
		__HAL_RCC_SPI3_CLK_DISABLE();
	}
	else
	{
		return;
	}


    HAL_GPIO_DeInit(ports[board.spis[spiInx].NSSPort],  board.spis[spiInx].NSSPin);
    HAL_GPIO_DeInit(ports[board.spis[spiInx].SCKPort],  board.spis[spiInx].SCKPin);
    HAL_GPIO_DeInit(ports[board.spis[spiInx].MISOPort], board.spis[spiInx].MISOPin);
    HAL_GPIO_DeInit(ports[board.spis[spiInx].MOSIPort], board.spis[spiInx].MOSIPin);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(hspi->hdmarx);
    HAL_DMA_DeInit(hspi->hdmatx);

    /* Peripheral interrupt DeInit*/
    HAL_NVIC_DisableIRQ(board.spis[spiInx].SPI_IRQn);
    HAL_NVIC_DisableIRQ(board.dmasActive[board.spis[spiInx].TXDma].dmaIRQn);
    HAL_NVIC_DisableIRQ(board.dmasActive[board.spis[spiInx].RXDma].dmaIRQn);
    board.dmasActive[board.spis[spiInx].TXDma].enabled = 0;
    board.dmasActive[board.spis[spiInx].RXDma].enabled = 0;

}






































/*






extern DMA_HandleTypeDef dma_gyro_rx;
extern DMA_HandleTypeDef dma_gyro_tx;

// unless remapped, define default SPI gpios

//#ifndef GYRO_SPI_CLK_GPIO_Port
//#define GYRO_SPI_CLK_GPIO_Port GPIOA
//#endif

//#ifndef GYRO_SPI_CLK_GPIO_Pin
//#define GYRO_SPI_CLK_GPIO_Pin GPIO_PIN_5
//#endif

//#ifndef GYRO_SPI_MISO_GPIO_Port
//#define GYRO_SPI_MISO_GPIO_Port GPIOA
//#endif

//#ifndef GYRO_SPI_MISO_GPIO_Pin
//#define GYRO_SPI_MISO_GPIO_Pin GPIO_PIN_6
//#endif

//#ifndef GYRO_SPI_MOSI_GPIO_Port
//#define GYRO_SPI_MOSI_GPIO_Port GPIOA
//#endif

//#ifndef GYRO_SPI_MOSI_GPIO_Pin
//#define GYRO_SPI_MOSI_GPIO_Pin GPIO_PIN_7
//#endif


void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if (hspi->Instance == SPI1) {
        // Peripheral clock enable
        __HAL_RCC_SPI1_CLK_ENABLE();

        HAL_GPIO_DeInit(GYRO_SPI_CLK_GPIO_Port, GYRO_SPI_CLK_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_MISO_GPIO_Port, GYRO_SPI_MISO_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_MOSI_GPIO_Port, GYRO_SPI_MOSI_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);

        // SPI1 GPIO Configuration
        //PA4     ------> SPI1_NSS
        //PA5     ------> SPI1_SCK
        //PA6     ------> SPI1_MISO
        //PA7     ------> SPI1_MOSI

        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Pin = GYRO_SPI_CS_GPIO_Pin;
        HAL_GPIO_Init(GYRO_SPI_CS_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        GPIO_InitStruct.Pin = GYRO_SPI_CLK_GPIO_Pin;
        HAL_GPIO_Init(GYRO_SPI_CLK_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GYRO_SPI_MISO_GPIO_Pin;
        HAL_GPIO_Init(GYRO_SPI_MISO_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GYRO_SPI_MOSI_GPIO_Pin;
        HAL_GPIO_Init(GYRO_SPI_MOSI_GPIO_Port, &GPIO_InitStruct);

        // Peripheral DMA init

        dma_gyro_rx.Instance = DMA1_Channel2;
        dma_gyro_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        dma_gyro_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        dma_gyro_rx.Init.MemInc = DMA_MINC_ENABLE;
        dma_gyro_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        dma_gyro_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        dma_gyro_rx.Init.Mode = DMA_NORMAL;
        dma_gyro_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        if (HAL_DMA_Init(&dma_gyro_rx) != HAL_OK) {
            ErrorHandler(MSP_DMA_GYRO_RX_INIT_FAILIURE);
        }

        __HAL_LINKDMA(hspi, hdmarx, dma_gyro_rx);

        dma_gyro_tx.Instance = DMA1_Channel3;
        dma_gyro_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        dma_gyro_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        dma_gyro_tx.Init.MemInc = DMA_MINC_ENABLE;
        dma_gyro_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        dma_gyro_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        dma_gyro_tx.Init.Mode = DMA_NORMAL;
        dma_gyro_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        if (HAL_DMA_Init(&dma_gyro_tx) != HAL_OK) {
            ErrorHandler(MSP_DMA_GYRO_TX_INIT_FAILIURE);
        }

        __HAL_LINKDMA(hspi, hdmatx, dma_gyro_tx);
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    if (hspi->Instance == SPI1) {
        // Peripheral clock disable
        __HAL_RCC_SPI1_CLK_DISABLE();

        // SPI1 GPIO Configuration
        HAL_GPIO_DeInit(GYRO_SPI_CLK_GPIO_Port, GYRO_SPI_CLK_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_MISO_GPIO_Port, GYRO_SPI_MISO_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_MOSI_GPIO_Port, GYRO_SPI_MOSI_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);

        // Peripheral DMA DeInit
        HAL_DMA_DeInit(hspi->hdmarx);
        HAL_DMA_DeInit(hspi->hdmatx);
    }
}
*/
