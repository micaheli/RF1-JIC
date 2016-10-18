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

	    HAL_GPIO_DeInit(board.spis[0].NSSPort, board.spis[0].NSSPin);
	    HAL_GPIO_DeInit(board.spis[0].SCKPort, board.spis[0].SCKPin);
	    HAL_GPIO_DeInit(board.spis[0].MISOPort, board.spis[0].MISOPin);
	    HAL_GPIO_DeInit(board.spis[0].MOSIPort, board.spis[0].MOSIPin);

	    GPIO_InitStruct.Pin = board.spis[0].NSSPin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init(board.spis[0].NSSPort, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = board.spis[0].SCKPin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = board.spis[0].SCKAlternate;
	    HAL_GPIO_Init(board.spis[0].SCKPort, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = board.spis[0].MISOPin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = board.spis[0].MISOAlternate;
	    HAL_GPIO_Init(board.spis[0].MISOPort, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = board.spis[0].MOSIPin;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = board.spis[0].MOSIAlternate;
	    HAL_GPIO_Init(board.spis[0].MOSIPort, &GPIO_InitStruct);

        /* Peripheral DMA init*/

	    dma_gyro_rx.Instance = board.spis[0].RXDMAStream;
	    dma_gyro_rx.Init.Channel = board.spis[0].RXDMAChannel;
	    dma_gyro_rx.Init.Direction = board.spis[0].RXDMADirection;
	    dma_gyro_rx.Init.PeriphInc = board.spis[0].RXDMAPeriphInc;
	    dma_gyro_rx.Init.MemInc = board.spis[0].RXDMAMemInc;
	    dma_gyro_rx.Init.PeriphDataAlignment = board.spis[0].RXDMAPeriphDataAlignment;
	    dma_gyro_rx.Init.MemDataAlignment = board.spis[0].RXDMAMemDataAlignment;
	    dma_gyro_rx.Init.Mode = board.spis[0].RXDMAMode;
	    dma_gyro_rx.Init.Priority = board.spis[0].RXDMAPriority;
	    dma_gyro_rx.Init.FIFOMode = board.spis[0].RXDMAFIFOMode;
        if (HAL_DMA_Init(&dma_gyro_rx) != HAL_OK) {
            ErrorHandler();
        }

        __HAL_LINKDMA(hspi, hdmarx, dma_gyro_rx);

	    dma_gyro_tx.Instance = board.spis[0].TXDMAStream;
	    dma_gyro_tx.Init.Channel = board.spis[0].TXDMAChannel;
	    dma_gyro_tx.Init.Direction = board.spis[0].TXDMADirection;
	    dma_gyro_tx.Init.PeriphInc = board.spis[0].TXDMAPeriphInc;
	    dma_gyro_tx.Init.MemInc = board.spis[0].TXDMAMemInc;
	    dma_gyro_tx.Init.PeriphDataAlignment = board.spis[0].TXDMAPeriphDataAlignment;
	    dma_gyro_tx.Init.MemDataAlignment = board.spis[0].TXDMAMemDataAlignment;
	    dma_gyro_tx.Init.Mode = board.spis[0].TXDMAMode;
	    dma_gyro_tx.Init.Priority = board.spis[0].TXDMAPriority;
	    dma_gyro_tx.Init.FIFOMode = board.spis[0].TXDMAFIFOMode;
        if (HAL_DMA_Init(&dma_gyro_tx) != HAL_OK) {
            ErrorHandler();
        }

        __HAL_LINKDMA(hspi, hdmatx, dma_gyro_tx);

        /* Peripheral interrupt init */
	    HAL_NVIC_SetPriority(board.spis[0].SPI_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(board.spis[0].SPI_IRQn);
    }

	if (hspi->Instance == SPI2) {
        /* Peripheral clock enable */
		__HAL_RCC_SPI2_CLK_ENABLE();

		HAL_GPIO_DeInit(board.spis[1].NSSPort, board.spis[1].NSSPin);
		HAL_GPIO_DeInit(board.spis[1].SCKPort, board.spis[1].SCKPin);
		HAL_GPIO_DeInit(board.spis[1].MISOPort, board.spis[1].MISOPin);
		HAL_GPIO_DeInit(board.spis[1].MOSIPort, board.spis[1].MOSIPin);

		GPIO_InitStruct.Pin = board.spis[1].NSSPin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(board.spis[1].NSSPort, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = board.spis[1].SCKPin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = board.spis[1].SCKAlternate;
		HAL_GPIO_Init(board.spis[1].SCKPort, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = board.spis[1].MISOPin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = board.spis[1].MISOAlternate;
		HAL_GPIO_Init(board.spis[1].MISOPort, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = board.spis[1].MOSIPin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = board.spis[1].MOSIAlternate;
		HAL_GPIO_Init(board.spis[1].MOSIPort, &GPIO_InitStruct);

		        /* Peripheral DMA init*/

		dma_gyro_rx.Instance = board.spis[1].RXDMAStream;
		dma_gyro_rx.Init.Channel = board.spis[1].RXDMAChannel;
		dma_gyro_rx.Init.Direction = board.spis[1].RXDMADirection;
		dma_gyro_rx.Init.PeriphInc = board.spis[1].RXDMAPeriphInc;
		dma_gyro_rx.Init.MemInc = board.spis[1].RXDMAMemInc;
		dma_gyro_rx.Init.PeriphDataAlignment = board.spis[1].RXDMAPeriphDataAlignment;
		dma_gyro_rx.Init.MemDataAlignment = board.spis[1].RXDMAMemDataAlignment;
		dma_gyro_rx.Init.Mode = board.spis[1].RXDMAMode;
		dma_gyro_rx.Init.Priority = board.spis[1].RXDMAPriority;
		dma_gyro_rx.Init.FIFOMode = board.spis[1].RXDMAFIFOMode;
		if (HAL_DMA_Init(&dma_gyro_rx) != HAL_OK) {
			ErrorHandler();
		}

		__HAL_LINKDMA(hspi, hdmarx, dma_gyro_rx);

		dma_gyro_tx.Instance = board.spis[1].TXDMAStream;
		dma_gyro_tx.Init.Channel = board.spis[1].TXDMAChannel;
		dma_gyro_tx.Init.Direction = board.spis[1].TXDMADirection;
		dma_gyro_tx.Init.PeriphInc = board.spis[1].TXDMAPeriphInc;
		dma_gyro_tx.Init.MemInc = board.spis[1].TXDMAMemInc;
		dma_gyro_tx.Init.PeriphDataAlignment = board.spis[1].TXDMAPeriphDataAlignment;
		dma_gyro_tx.Init.MemDataAlignment = board.spis[1].TXDMAMemDataAlignment;
		dma_gyro_tx.Init.Mode = board.spis[1].TXDMAMode;
		dma_gyro_tx.Init.Priority = board.spis[1].TXDMAPriority;
		dma_gyro_tx.Init.FIFOMode = board.spis[1].TXDMAFIFOMode;
		if (HAL_DMA_Init(&dma_gyro_tx) != HAL_OK) {
			ErrorHandler();
		}

		__HAL_LINKDMA(hspi, hdmatx, dma_gyro_tx);

		        /* Peripheral interrupt init */
		HAL_NVIC_SetPriority(board.spis[1].SPI_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(board.spis[1].SPI_IRQn);
	}
    else if (hspi->Instance == SPI3)
    {
            /* Peripheral clock enable */
            __HAL_RCC_SPI3_CLK_ENABLE();

            /**SPI1 GPIO Configuration
            PC9      ------> SPI1_NSS   //for SPI3 we are using Soft CS B3
            PC10     ------> SPI1_SCK
            PC11     ------> SPI1_MISO
            PC12     ------> SPI1_MOSI
            */

            HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);
            HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);

            GPIO_InitStruct.Pin   = GPIO_PIN_3;
            GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull  = GPIO_PULLUP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

            GPIO_InitStruct.Pin   = GPIO_PIN_10;
            GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
            HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

            GPIO_InitStruct.Pin   = GPIO_PIN_11 | GPIO_PIN_12;
            GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull  = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
            HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

            /* Peripheral DMA init*/

            dma_flash_rx.Instance       = DMA1_Stream2;
            dma_flash_rx.Init.Channel   = DMA_CHANNEL_0;
            dma_flash_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
            dma_flash_rx.Init.PeriphInc = DMA_PINC_DISABLE;
            dma_flash_rx.Init.MemInc    = DMA_MINC_ENABLE;
            dma_flash_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            dma_flash_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
            dma_flash_rx.Init.Mode     = DMA_NORMAL;
            dma_flash_rx.Init.Priority = DMA_PRIORITY_HIGH;
            dma_flash_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            if (HAL_DMA_Init(&dma_flash_rx) != HAL_OK) {
                ErrorHandler();
            }

            __HAL_LINKDMA(hspi, hdmarx, dma_flash_rx);

            dma_flash_tx.Instance = DMA1_Stream5;
            dma_flash_tx.Init.Channel = DMA_CHANNEL_0;
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

	    HAL_GPIO_DeInit(board.spis[0].NSSPort, board.spis[0].NSSPin);
	    HAL_GPIO_DeInit(board.spis[0].SCKPort, board.spis[0].SCKPin);
	    HAL_GPIO_DeInit(board.spis[0].MISOPort, board.spis[0].MISOPin);
	    HAL_GPIO_DeInit(board.spis[0].MOSIPort, board.spis[0].MOSIPin);

        /* Peripheral DMA DeInit*/
        HAL_DMA_DeInit(hspi->hdmarx);
        HAL_DMA_DeInit(hspi->hdmatx);

        /* Peripheral interrupt DeInit*/
	    HAL_NVIC_DisableIRQ(board.spis[0].SPI_IRQn);
    }

	if (hspi->Instance == SPI2) {
        /* Peripheral clock disable */
		__HAL_RCC_SPI2_CLK_DISABLE();

		HAL_GPIO_DeInit(board.spis[1].NSSPort, board.spis[1].NSSPin);
		HAL_GPIO_DeInit(board.spis[1].SCKPort, board.spis[1].SCKPin);
		HAL_GPIO_DeInit(board.spis[1].MISOPort, board.spis[1].MISOPin);
		HAL_GPIO_DeInit(board.spis[1].MOSIPort, board.spis[1].MOSIPin);

		        /* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(hspi->hdmarx);
		HAL_DMA_DeInit(hspi->hdmatx);

		        /* Peripheral interrupt DeInit*/
		HAL_NVIC_DisableIRQ(board.spis[1].SPI_IRQn);
	}
    else if (hspi->Instance == SPI3)
    {
        /* Peripheral clock disable */
        __HAL_RCC_SPI3_CLK_DISABLE();

        /**SPI1 GPIO Configuration
        PB3      ------> SPI3_NSS   //for SPI3 we are using Soft CS B3
        PC10     ------> SPI3_SCK
        PC11     ------> SPI3_MISO
        PC12     ------> SPI3_MOSI
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);

        /* Peripheral DMA DeInit*/
        HAL_DMA_DeInit(hspi->hdmarx);
        HAL_DMA_DeInit(hspi->hdmatx);

        /* Peripheral interrupt DeInit*/
        HAL_NVIC_DisableIRQ(SPI3_IRQn);
    }
}
