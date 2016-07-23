#include "includes.h"

extern DMA_HandleTypeDef dma_gyro_rx;
extern DMA_HandleTypeDef dma_gyro_tx;

// unless remapped, define default SPI gpios
/*
#ifndef GYRO_SPI_CLK_GPIO_Port
#define GYRO_SPI_CLK_GPIO_Port GPIOA
#endif

#ifndef GYRO_SPI_CLK_GPIO_Pin
#define GYRO_SPI_CLK_GPIO_Pin GPIO_PIN_5
#endif

#ifndef GYRO_SPI_MISO_GPIO_Port
#define GYRO_SPI_MISO_GPIO_Port GPIOA
#endif

#ifndef GYRO_SPI_MISO_GPIO_Pin
#define GYRO_SPI_MISO_GPIO_Pin GPIO_PIN_6
#endif

#ifndef GYRO_SPI_MOSI_GPIO_Port
#define GYRO_SPI_MOSI_GPIO_Port GPIOA
#endif

#ifndef GYRO_SPI_MOSI_GPIO_Pin
#define GYRO_SPI_MOSI_GPIO_Pin GPIO_PIN_7
#endif
*/

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if (hspi->Instance == SPI1) {
        /* Peripheral clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();

        HAL_GPIO_DeInit(GYRO_SPI_CLK_GPIO_Port, GYRO_SPI_CLK_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_MISO_GPIO_Port, GYRO_SPI_MISO_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_MOSI_GPIO_Port, GYRO_SPI_MOSI_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);

        /* SPI1 GPIO Configuration
        PA4     ------> SPI1_NSS
        PA5     ------> SPI1_SCK
        PA6     ------> SPI1_MISO
        PA7     ------> SPI1_MOSI
        */

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

        /* Peripheral DMA init */

        dma_gyro_rx.Instance = DMA1_Channel2;
        dma_gyro_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        dma_gyro_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        dma_gyro_rx.Init.MemInc = DMA_MINC_ENABLE;
        dma_gyro_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        dma_gyro_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        dma_gyro_rx.Init.Mode = DMA_NORMAL;
        dma_gyro_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        if (HAL_DMA_Init(&dma_gyro_rx) != HAL_OK) {
            ErrorHandler();
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
            ErrorHandler();
        }

        __HAL_LINKDMA(hspi, hdmatx, dma_gyro_tx);
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    if (hspi->Instance == SPI1) {
        /* Peripheral clock disable */
        __HAL_RCC_SPI1_CLK_DISABLE();

        /* SPI1 GPIO Configuration */
        HAL_GPIO_DeInit(GYRO_SPI_CLK_GPIO_Port, GYRO_SPI_CLK_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_MISO_GPIO_Port, GYRO_SPI_MISO_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_MOSI_GPIO_Port, GYRO_SPI_MOSI_GPIO_Pin);
        HAL_GPIO_DeInit(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);

        /* Peripheral DMA DeInit*/
        HAL_DMA_DeInit(hspi->hdmarx);
        HAL_DMA_DeInit(hspi->hdmatx);
    }
}
