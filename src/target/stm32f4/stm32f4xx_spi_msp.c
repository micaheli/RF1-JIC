#include "includes.h"
#include "stm32f4xx_hal.h"

extern DMA_HandleTypeDef dma_gyro_rx;
extern DMA_HandleTypeDef dma_gyro_tx;

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
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    if (hspi->Instance == SPI1) {
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
}
