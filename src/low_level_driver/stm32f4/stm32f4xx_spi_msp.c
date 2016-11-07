#include "includes.h"

extern DMA_HandleTypeDef dma_gyro_rx;
extern DMA_HandleTypeDef dma_gyro_tx;
extern DMA_HandleTypeDef dma_flash_rx;
extern DMA_HandleTypeDef dma_flash_tx;

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if (hspi->Instance == SPI1) {
        /* Peripheral clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();

	    HAL_GPIO_DeInit(SPI1_NSS_GPIO_PORT, SPI1_NSS_PIN);
	    HAL_GPIO_DeInit(SPI1_SCK_GPIO_PORT, SPI1_SCK_PIN);
	    HAL_GPIO_DeInit(SPI1_MISO_GPIO_PORT, SPI1_MISO_PIN);
	    HAL_GPIO_DeInit(SPI1_MOSI_GPIO_PORT, SPI1_MOSI_PIN);

	    GPIO_InitStruct.Pin = SPI1_NSS_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init(SPI1_NSS_GPIO_PORT, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = SPI1_SCK_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = SPI1_SCK_AF;
	    HAL_GPIO_Init(SPI1_SCK_GPIO_PORT, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = SPI1_MISO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = SPI1_MISO_AF;
	    HAL_GPIO_Init(SPI1_MISO_GPIO_PORT, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = SPI1_MOSI_PIN;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = SPI1_MOSI_AF;
	    HAL_GPIO_Init(SPI1_MOSI_GPIO_PORT, &GPIO_InitStruct);

        /* Peripheral DMA init*/

	    dma_gyro_rx.Instance = SPI1_RX_DMA_STREAM;
	    dma_gyro_rx.Init.Channel = SPI1_RX_DMA_CHANNEL;
	    dma_gyro_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	    dma_gyro_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	    dma_gyro_rx.Init.MemInc = DMA_MINC_ENABLE;
	    dma_gyro_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	    dma_gyro_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	    dma_gyro_rx.Init.Mode = DMA_NORMAL;
	    dma_gyro_rx.Init.Priority = DMA_PRIORITY_HIGH;
	    dma_gyro_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&dma_gyro_rx) != HAL_OK) {
            ErrorHandler();
        }

        __HAL_LINKDMA(hspi, hdmarx, dma_gyro_rx);

	    dma_gyro_tx.Instance = SPI1_TX_DMA_STREAM;
	    dma_gyro_tx.Init.Channel = SPI1_TX_DMA_CHANNEL;
	    dma_gyro_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	    dma_gyro_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	    dma_gyro_tx.Init.MemInc = DMA_MINC_ENABLE;
	    dma_gyro_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	    dma_gyro_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	    dma_gyro_tx.Init.Mode = DMA_NORMAL;
	    dma_gyro_tx.Init.Priority = DMA_PRIORITY_HIGH;
	    dma_gyro_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&dma_gyro_tx) != HAL_OK) {
            ErrorHandler();
        }

        __HAL_LINKDMA(hspi, hdmatx, dma_gyro_tx);

        /* Peripheral interrupt init */
	    HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(SPI1_IRQn);
    }

	if (hspi->Instance == SPI2) {
        /* Peripheral clock enable */
		__HAL_RCC_SPI2_CLK_ENABLE();

		HAL_GPIO_DeInit(SPI2_NSS_GPIO_PORT, SPI2_NSS_PIN);
		HAL_GPIO_DeInit(SPI2_SCK_GPIO_PORT, SPI2_SCK_PIN);
		HAL_GPIO_DeInit(SPI2_MISO_GPIO_PORT, SPI2_MISO_PIN);
		HAL_GPIO_DeInit(SPI2_MOSI_GPIO_PORT, SPI2_MOSI_PIN);

		GPIO_InitStruct.Pin = SPI2_NSS_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(SPI2_NSS_GPIO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = SPI2_SCK_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = SPI2_SCK_AF;
		HAL_GPIO_Init(SPI2_SCK_GPIO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = SPI2_MISO_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = SPI2_MISO_AF;
		HAL_GPIO_Init(SPI2_MISO_GPIO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = SPI2_MOSI_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = SPI2_MOSI_AF;
		HAL_GPIO_Init(SPI2_MOSI_GPIO_PORT, &GPIO_InitStruct);

		        /* Peripheral DMA init*/

		dma_gyro_rx.Instance = SPI2_RX_DMA_STREAM;
		dma_gyro_rx.Init.Channel = SPI2_RX_DMA_CHANNEL;
		dma_gyro_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		dma_gyro_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		dma_gyro_rx.Init.MemInc = DMA_MINC_ENABLE;
		dma_gyro_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		dma_gyro_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		dma_gyro_rx.Init.Mode = DMA_NORMAL;
		dma_gyro_rx.Init.Priority = DMA_PRIORITY_HIGH;
		dma_gyro_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&dma_gyro_rx) != HAL_OK) {
			ErrorHandler();
		}

		__HAL_LINKDMA(hspi, hdmarx, dma_gyro_rx);

		dma_gyro_tx.Instance = SPI2_TX_DMA_STREAM;
		dma_gyro_tx.Init.Channel = SPI2_TX_DMA_CHANNEL;
		dma_gyro_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		dma_gyro_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		dma_gyro_tx.Init.MemInc = DMA_MINC_ENABLE;
		dma_gyro_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		dma_gyro_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		dma_gyro_tx.Init.Mode = DMA_NORMAL;
		dma_gyro_tx.Init.Priority = DMA_PRIORITY_HIGH;
		dma_gyro_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&dma_gyro_tx) != HAL_OK) {
			ErrorHandler();
		}

		__HAL_LINKDMA(hspi, hdmatx, dma_gyro_tx);

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

	    GPIO_InitStruct.Pin = SPI3_MOSI_PIN;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = SPI3_MOSI_AF;
	    HAL_GPIO_Init(SPI3_MOSI_GPIO_PORT, &GPIO_InitStruct);

        /* Peripheral DMA init*/

	    dma_flash_rx.Instance = SPI3_RX_DMA_STREAM;
	    dma_flash_rx.Init.Channel = SPI3_RX_DMA_CHANNEL;
	    dma_flash_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	    dma_flash_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	    dma_flash_rx.Init.MemInc = DMA_MINC_ENABLE;
	    dma_flash_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	    dma_flash_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	    dma_flash_rx.Init.Mode = DMA_NORMAL;
	    dma_flash_rx.Init.Priority = DMA_PRIORITY_HIGH;
	    dma_flash_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	    if (HAL_DMA_Init(&dma_flash_rx) != HAL_OK) {
		    ErrorHandler();
	    }

	    __HAL_LINKDMA(hspi, hdmarx, dma_flash_rx);

	    dma_flash_tx.Instance = SPI3_TX_DMA_STREAM;
	    dma_flash_tx.Init.Channel = SPI3_TX_DMA_CHANNEL;
	    dma_flash_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	    dma_flash_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	    dma_flash_tx.Init.MemInc = DMA_MINC_ENABLE;
	    dma_flash_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	    dma_flash_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	    dma_flash_tx.Init.Mode = DMA_NORMAL;
	    dma_flash_tx.Init.Priority = DMA_PRIORITY_HIGH;
	    dma_flash_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	    if (HAL_DMA_Init(&dma_flash_tx) != HAL_OK) {
		    ErrorHandler();
	    }

	    __HAL_LINKDMA(hspi, hdmatx, dma_flash_tx);

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
