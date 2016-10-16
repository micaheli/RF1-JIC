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

        /**SPI1 GPIO Configuration
        PA4     ------> SPI1_NSS
        PA5     ------> SPI1_SCK
        PA6     ------> SPI1_MISO
        PA7     ------> SPI1_MOSI
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Peripheral DMA init*/

        dma_gyro_rx.Instance = DMA2_Stream0;
        dma_gyro_rx.Init.Channel = DMA_CHANNEL_3;
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

        dma_gyro_tx.Instance = DMA2_Stream3;
        dma_gyro_tx.Init.Channel = DMA_CHANNEL_3;
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

		        /**SPI2 GPIO Configuration
		        PB12     ------> SPI2_NSS
		        PB13     ------> SPI2_SCK
		        PB14     ------> SPI2_MISO
		        PB15     ------> SPI2_MOSI
		        */
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

		GPIO_InitStruct.Pin = GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_13;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		        /* Peripheral DMA init*/

		dma_gyro_rx.Instance = DMA1_Stream3;
		dma_gyro_rx.Init.Channel = DMA_CHANNEL_0;
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

		dma_gyro_tx.Instance = DMA1_Stream4;
		dma_gyro_tx.Init.Channel = DMA_CHANNEL_0;
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

        /**SPI1 GPIO Configuration
        PA4     ------> SPI1_NSS
        PA5     ------> SPI1_SCK
        PA6     ------> SPI1_MISO
        PA7     ------> SPI1_MOSI
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

        /* Peripheral DMA DeInit*/
        HAL_DMA_DeInit(hspi->hdmarx);
        HAL_DMA_DeInit(hspi->hdmatx);

        /* Peripheral interrupt DeInit*/
        HAL_NVIC_DisableIRQ(SPI1_IRQn);
    }

	if (hspi->Instance == SPI2) {
        /* Peripheral clock disable */
		__HAL_RCC_SPI2_CLK_DISABLE();

		        /**SPI2 GPIO Configuration
		        PB12     ------> SPI2_NSS
		        PB13     ------> SPI2_SCK
		        PB14     ------> SPI2_MISO
		        PB15     ------> SPI2_MOSI
		        */
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

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
