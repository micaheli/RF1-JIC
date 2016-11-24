#include "includes.h"

extern DMA_HandleTypeDef *dma_gyro_rx;
extern DMA_HandleTypeDef *dma_gyro_tx;
extern DMA_HandleTypeDef *dma_flash_rx;
extern DMA_HandleTypeDef *dma_flash_tx;

DMA_HandleTypeDef dma_spi1_rx;
DMA_HandleTypeDef dma_spi1_tx;
DMA_HandleTypeDef dma_spi2_rx;
DMA_HandleTypeDef dma_spi2_tx;
DMA_HandleTypeDef dma_spi3_rx;
DMA_HandleTypeDef dma_spi3_tx;


void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	if (GYRO_SPI == SPI1)
	{
		dma_gyro_rx = &dma_spi1_rx;
		dma_gyro_tx = &dma_spi1_tx;
	}
	else if (GYRO_SPI == SPI2)
	{
		dma_gyro_rx = &dma_spi2_rx;
		//dma_gyro_tx = &dma_spi2_tx;
	}
	else if (GYRO_SPI == SPI3)
	{
		dma_gyro_rx = &dma_spi3_rx;
		//dma_gyro_tx = &dma_spi3_tx;
	}

	if (FLASH_SPI == SPI1)
	{
		dma_flash_rx = &dma_spi1_rx;
		//dma_flash_tx = &dma_spi1_tx;
	}
	else if (FLASH_SPI == SPI2)
	{
		dma_flash_rx = &dma_spi2_rx;
		dma_flash_tx = &dma_spi2_tx;
	}
	else if (FLASH_SPI == SPI3)
	{
		dma_flash_rx = &dma_spi3_rx;
		dma_flash_tx = &dma_spi3_tx;
	}
	

    GPIO_InitTypeDef GPIO_InitStruct;
    if (hspi->Instance == SPI1) {
    	/* Peripheral clock enable */
    	__HAL_RCC_SPI1_CLK_ENABLE();

	    HAL_GPIO_DeInit(SPI1_NSS_GPIO_PORT, SPI1_NSS_PIN);
	    HAL_GPIO_DeInit(SPI1_SCK_GPIO_PORT, SPI1_SCK_PIN);
	    HAL_GPIO_DeInit(SPI1_MISO_GPIO_PORT, SPI1_MISO_PIN);
	    HAL_GPIO_DeInit(SPI1_MOSI_GPIO_PORT, SPI1_MOSI_PIN);

	    GPIO_InitStruct.Pin   = SPI1_NSS_PIN;
        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull  = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init(SPI1_NSS_GPIO_PORT, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin   = SPI1_SCK_PIN;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = SPI1_SCK_AF;
	    HAL_GPIO_Init(SPI1_SCK_GPIO_PORT, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin   = SPI1_MISO_PIN;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull  = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = SPI1_MISO_AF;
	    HAL_GPIO_Init(SPI1_MISO_GPIO_PORT, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin   = SPI1_MOSI_PIN;
	    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull  = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = SPI1_MOSI_AF;
	    HAL_GPIO_Init(SPI1_MOSI_GPIO_PORT, &GPIO_InitStruct);

        /* Peripheral DMA init*/

	    dma_spi1_rx.Instance                 = SPI1_RX_DMA_STREAM;
	    dma_spi1_rx.Init.Channel             = SPI1_RX_DMA_CHANNEL;
	    dma_spi1_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	    dma_spi1_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	    dma_spi1_rx.Init.MemInc              = DMA_MINC_ENABLE;
	    dma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	    dma_spi1_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	    dma_spi1_rx.Init.Mode                = DMA_NORMAL;
	    dma_spi1_rx.Init.Priority            = DMA_PRIORITY_HIGH;
	    dma_spi1_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&dma_spi1_rx) != HAL_OK) {
            ErrorHandler();
        }

        __HAL_LINKDMA(hspi, hdmarx, dma_spi1_rx);

        dma_spi1_tx.Instance                 = dmaStream[board.dmas[board.spis[0].TXDma].dmaStream];
        dma_spi1_tx.Init.Channel             = board.dmas[board.spis[0].TXDma].dmaChannel;
        dma_spi1_tx.Init.Direction           = board.dmas[board.spis[0].TXDma].dmaDirection;
        dma_spi1_tx.Init.PeriphInc           = board.dmas[board.spis[0].TXDma].dmaPeriphInc;
        dma_spi1_tx.Init.MemInc              = board.dmas[board.spis[0].TXDma].dmaMemInc;
        dma_spi1_tx.Init.PeriphDataAlignment = board.dmas[board.spis[0].TXDma].dmaPeriphAlignment;
        dma_spi1_tx.Init.MemDataAlignment    = board.dmas[board.spis[0].TXDma].dmaMemAlignment;
        dma_spi1_tx.Init.Mode                = board.dmas[board.spis[0].TXDma].dmaMode;
        dma_spi1_tx.Init.Priority            = board.dmas[board.spis[0].TXDma].dmaPriority;
        dma_spi1_tx.Init.FIFOMode            = board.dmas[board.spis[0].TXDma].fifoMode;
        if (HAL_DMA_Init(&dma_spi1_tx) != HAL_OK) {
            ErrorHandler();
        }

        __HAL_LINKDMA(hspi, hdmatx, dma_spi1_tx);

        /* Peripheral interrupt init */
	    HAL_NVIC_SetPriority(board.dmas[board.spis[0].TXDma].dmaIRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(board.dmas[board.spis[0].TXDma].dmaIRQn);

	    /*
	    dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle].Instance                 = dmaStream[board.dmas[board.spis[0].TXDma].dmaStream];
        dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle].Init.Channel             = board.dmas[board.spis[0].TXDma].dmaChannel;
        dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle].Init.Direction           = board.dmas[board.spis[0].TXDma].dmaDirection;
        dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle].Init.PeriphInc           = board.dmas[board.spis[0].TXDma].dmaPeriphInc;
        dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle].Init.MemInc              = board.dmas[board.spis[0].TXDma].dmaMemInc;
        dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle].Init.PeriphDataAlignment = board.dmas[board.spis[0].TXDma].dmaPeriphAlignment;
        dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle].Init.MemDataAlignment    = board.dmas[board.spis[0].TXDma].dmaMemAlignment;
        dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle].Init.Mode                = board.dmas[board.spis[0].TXDma].dmaMode;
        dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle].Init.Priority            = board.dmas[board.spis[0].TXDma].dmaPriority;
        dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle].Init.FIFOMode            = board.dmas[board.spis[0].TXDma].fifoMode;
        if (HAL_DMA_Init(&dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle]) != HAL_OK) {
            ErrorHandler();
        }

        __HAL_LINKDMA(hspi, hdmatx, dmaHandles[board.dmas[board.spis[0].TXDma].dmaHandle]);

        // Peripheral interrupt init
	    HAL_NVIC_SetPriority(board.dmas[board.spis[0].TXDma].dmaIRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(board.dmas[board.spis[0].TXDma].dmaIRQn);
	     */
    }

	if (hspi->Instance == SPI2) {
    	/* Peripheral clock enable */
		__HAL_RCC_SPI2_CLK_ENABLE();

		HAL_GPIO_DeInit(SPI2_NSS_GPIO_PORT, SPI2_NSS_PIN);
		HAL_GPIO_DeInit(SPI2_SCK_GPIO_PORT, SPI2_SCK_PIN);
		HAL_GPIO_DeInit(SPI2_MISO_GPIO_PORT, SPI2_MISO_PIN);
		HAL_GPIO_DeInit(SPI2_MOSI_GPIO_PORT, SPI2_MOSI_PIN);

		GPIO_InitStruct.Pin   = SPI2_NSS_PIN;
		GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull  = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(SPI2_NSS_GPIO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin   = SPI2_SCK_PIN;
		GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = SPI2_SCK_AF;
		HAL_GPIO_Init(SPI2_SCK_GPIO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin   = SPI2_MISO_PIN;
		GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull  = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = SPI2_MISO_AF;
		HAL_GPIO_Init(SPI2_MISO_GPIO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin   = SPI2_MOSI_PIN;
		GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull  = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = SPI2_MOSI_AF;
		HAL_GPIO_Init(SPI2_MOSI_GPIO_PORT, &GPIO_InitStruct);

		        /* Peripheral DMA init*/

		dma_spi2_rx.Instance                 = SPI2_RX_DMA_STREAM;
		dma_spi2_rx.Init.Channel             = SPI2_RX_DMA_CHANNEL;
		dma_spi2_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
		dma_spi2_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
		dma_spi2_rx.Init.MemInc              = DMA_MINC_ENABLE;
		dma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		dma_spi2_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
		dma_spi2_rx.Init.Mode                = DMA_NORMAL;
		dma_spi2_rx.Init.Priority            = DMA_PRIORITY_HIGH;
		dma_spi2_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&dma_spi2_rx) != HAL_OK) {
			ErrorHandler();
		}

		__HAL_LINKDMA(hspi, hdmarx, dma_spi2_rx);

		dma_spi2_tx.Instance                 = SPI2_TX_DMA_STREAM;
		dma_spi2_tx.Init.Channel             = SPI2_TX_DMA_CHANNEL;
		dma_spi2_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
		dma_spi2_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
		dma_spi2_tx.Init.MemInc              = DMA_MINC_ENABLE;
		dma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		dma_spi2_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
		dma_spi2_tx.Init.Mode                = DMA_NORMAL;
		dma_spi2_tx.Init.Priority            = DMA_PRIORITY_HIGH;
		dma_spi2_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&dma_spi2_tx) != HAL_OK) {
			ErrorHandler();
		}

		__HAL_LINKDMA(hspi, hdmatx, dma_spi2_tx);

		        /* Peripheral interrupt init */
		HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(SPI2_IRQn);
	}
    else if (hspi->Instance == SPI3)
    {
    	/* Peripheral clock enable */
    	__HAL_RCC_SPI3_CLK_ENABLE();

	    HAL_GPIO_DeInit(SPI3_NSS_GPIO_PORT, SPI3_NSS_PIN);
	    HAL_GPIO_DeInit(SPI3_SCK_GPIO_PORT, SPI3_SCK_PIN);
	    HAL_GPIO_DeInit(SPI3_MISO_GPIO_PORT, SPI3_MISO_PIN);
	    HAL_GPIO_DeInit(SPI3_MOSI_GPIO_PORT, SPI3_MOSI_PIN);

	    GPIO_InitStruct.Pin = SPI3_NSS_PIN;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_PULLUP;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init(SPI3_NSS_GPIO_PORT, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = SPI3_SCK_PIN;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = SPI3_SCK_AF;
	    HAL_GPIO_Init(SPI3_SCK_GPIO_PORT, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = SPI3_MISO_PIN;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = SPI3_MISO_AF;
	    HAL_GPIO_Init(SPI3_MISO_GPIO_PORT, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin       = SPI3_MOSI_PIN;
	    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull      = GPIO_NOPULL;
	    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = SPI3_MOSI_AF;
	    HAL_GPIO_Init(SPI3_MOSI_GPIO_PORT, &GPIO_InitStruct);

        /* Peripheral DMA init*/

	    dma_spi3_rx.Instance                 = SPI3_RX_DMA_STREAM;
	    dma_spi3_rx.Init.Channel             = SPI3_RX_DMA_CHANNEL;
	    dma_spi3_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	    dma_spi3_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	    dma_spi3_rx.Init.MemInc              = DMA_MINC_ENABLE;
	    dma_spi3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	    dma_spi3_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	    dma_spi3_rx.Init.Mode                = DMA_NORMAL;
	    dma_spi3_rx.Init.Priority            = DMA_PRIORITY_HIGH;
	    dma_spi3_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	    if (HAL_DMA_Init(&dma_spi3_rx) != HAL_OK) {
		    ErrorHandler();
	    }

	    __HAL_LINKDMA(hspi, hdmarx, dma_spi3_rx);

	    dma_spi3_tx.Instance                 = SPI3_TX_DMA_STREAM;
	    dma_spi3_tx.Init.Channel             = SPI3_TX_DMA_CHANNEL;
	    dma_spi3_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	    dma_spi3_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	    dma_spi3_tx.Init.MemInc              = DMA_MINC_ENABLE;
	    dma_spi3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	    dma_spi3_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	    dma_spi3_tx.Init.Mode                = DMA_NORMAL;
	    dma_spi3_tx.Init.Priority            = DMA_PRIORITY_HIGH;
	    dma_spi3_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	    if (HAL_DMA_Init(&dma_spi3_tx) != HAL_OK) {
		    ErrorHandler();
	    }

	    __HAL_LINKDMA(hspi, hdmatx, dma_spi3_tx);

	            /* Peripheral interrupt init */
	    HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(SPI3_IRQn);
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    if (hspi->Instance == SPI1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_SPI1_CLK_DISABLE();

	    HAL_GPIO_DeInit(SPI1_NSS_GPIO_PORT, SPI1_NSS_PIN);
	    HAL_GPIO_DeInit(SPI1_SCK_GPIO_PORT, SPI1_SCK_PIN);
	    HAL_GPIO_DeInit(SPI1_MISO_GPIO_PORT, SPI1_MISO_PIN);
	    HAL_GPIO_DeInit(SPI1_MOSI_GPIO_PORT, SPI1_MOSI_PIN);

        /* Peripheral DMA DeInit*/
        HAL_DMA_DeInit(hspi->hdmarx);
        HAL_DMA_DeInit(hspi->hdmatx);

        /* Peripheral interrupt DeInit*/
	    HAL_NVIC_DisableIRQ(SPI1_IRQn);
    }

	if (hspi->Instance == SPI2) {
        /* Peripheral clock disable */
		__HAL_RCC_SPI2_CLK_DISABLE();

		HAL_GPIO_DeInit(SPI2_NSS_GPIO_PORT, SPI2_NSS_PIN);
		HAL_GPIO_DeInit(SPI2_SCK_GPIO_PORT, SPI2_SCK_PIN);
		HAL_GPIO_DeInit(SPI2_MISO_GPIO_PORT, SPI2_MISO_PIN);
		HAL_GPIO_DeInit(SPI2_MOSI_GPIO_PORT, SPI2_MOSI_PIN);

		        /* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(hspi->hdmarx);
		HAL_DMA_DeInit(hspi->hdmatx);

		        /* Peripheral interrupt DeInit*/
		HAL_NVIC_DisableIRQ(SPI2_IRQn);
	}
    else if (hspi->Instance == SPI3)
    {
        /* Peripheral clock disable */
        __HAL_RCC_SPI3_CLK_DISABLE();

	    HAL_GPIO_DeInit(SPI3_NSS_GPIO_PORT, SPI3_NSS_PIN);
	    HAL_GPIO_DeInit(SPI3_SCK_GPIO_PORT, SPI3_SCK_PIN);
	    HAL_GPIO_DeInit(SPI3_MISO_GPIO_PORT, SPI3_MISO_PIN);
	    HAL_GPIO_DeInit(SPI3_MOSI_GPIO_PORT, SPI3_MOSI_PIN);

        /* Peripheral DMA DeInit*/
	    HAL_DMA_DeInit(hspi->hdmarx);
	    HAL_DMA_DeInit(hspi->hdmatx);

	            /* Peripheral interrupt DeInit*/
	    HAL_NVIC_DisableIRQ(SPI3_IRQn);
    }
}
