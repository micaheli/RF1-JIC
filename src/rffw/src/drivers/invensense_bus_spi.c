#include <stdbool.h>

#include "includes.h"

#include "drivers/invensense_bus.h"
#include "drivers/invensense_device.h"

#ifdef GYRO_EXTI
#include "exti.h"
#endif

SPI_HandleTypeDef gyro_spi;
DMA_HandleTypeDef dma_gyro_rx;
DMA_HandleTypeDef dma_gyro_tx;

static void SPI_Init(uint32_t baudRatePrescaler)
{
    gyro_spi.Instance = GYRO_SPI;
    HAL_SPI_DeInit(&gyro_spi);

    gyro_spi.Init.Mode = SPI_MODE_MASTER;
    gyro_spi.Init.Direction = SPI_DIRECTION_2LINES;
    gyro_spi.Init.DataSize = SPI_DATASIZE_8BIT;
    gyro_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
    gyro_spi.Init.CLKPhase = SPI_PHASE_2EDGE;
    gyro_spi.Init.NSS = SPI_NSS_SOFT;
    gyro_spi.Init.BaudRatePrescaler = baudRatePrescaler;
    gyro_spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    gyro_spi.Init.TIMode = SPI_TIMODE_DISABLE;
    gyro_spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    gyro_spi.Init.CRCPolynomial = 7;

    if (HAL_SPI_Init(&gyro_spi) != HAL_OK) {
        ErrorHandler();
    }

    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_SET);
}

void DMA_Init(void)
{
    /* DMA interrupt init */
    HAL_NVIC_SetPriority(GYRO_DMA_TX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GYRO_DMA_TX_IRQn);
    HAL_NVIC_SetPriority(GYRO_DMA_RX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GYRO_DMA_RX_IRQn);
}

bool accgyroInit(void)
{
#ifdef GYRO_EXTI
    // ensure the interrupt is not running
    HAL_NVIC_DisableIRQ(GYRO_EXTI_IRQn);
#endif

    DMA_Init();

    // read and write settings at slow speed (48 MHz / 64)
    SPI_Init(GYRO_SPI_SLOW_BAUD);
    HAL_Delay(5);

    if (!accgyroDeviceDetect()) {
        return false;
    }

    HAL_Delay(5);

    if (!accgyroDeviceInit()) {
        return false;
    }

    // reinitialize at full speed (48 MHz / 2)
    SPI_Init(GYRO_SPI_FAST_BAUD);

#ifdef GYRO_EXTI
    // after the gyro is started, start up the interrupt
    EXTI_Init(GYRO_EXTI_GPIO_Port, GYRO_EXTI_GPIO_Pin, GYRO_EXTI_IRQn, 0, 0);
#endif

    return true;
}

#ifdef GYRO_EXTI
void GYRO_EXTI_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GYRO_EXTI_GPIO_Pin);

    accgyroDeviceReadGyro();
}

void GYRO_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&dma_gyro_tx);
}

void GYRO_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&dma_gyro_rx);

    // reset chip select line
    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_SET);
    // run callback for completed gyro read
    accgyroDeviceReadGyroComplete();
}
#endif

// for now all of these are blocking operations, which is fine for writes

bool accgyroWriteRegister(uint8_t reg, uint8_t data)
{
    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);

    // TODO: what should these timeouts be?
    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Transmit(&gyro_spi, &data, 1, 100);

    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_SET);
    HAL_Delay(1);

    return true;
}

bool accgyroVerifyWriteRegister(uint8_t reg, uint8_t data)
{
    uint8_t attempt, data_verify;

    for (attempt = 0; attempt < 20; attempt++) {
        accgyroWriteRegister(reg, data);
        HAL_Delay(1);

        accgyroSlowReadRegister(reg, 1, &data_verify);
        if (data_verify == data) {
            return true;
        }
    }

    return false;
}

bool accgyroReadRegister(uint8_t reg, uint8_t length, uint8_t *data)
{
    reg |= 0x80;

    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Receive(&gyro_spi, data, length, 100);

    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_SET);

    return true;
}

bool accgyroSlowReadRegister(uint8_t reg, uint8_t length, uint8_t *data)
{
    reg |= 0x80;

    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);

    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Receive(&gyro_spi, data, length, 100);

    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_SET);
    HAL_Delay(1);

    return true;
}

bool accgyroDMAReadWriteRegister(uint8_t *txData, uint8_t *rxData, uint8_t length)
{
    // for whatever reason HAL_SPI_GetState(&gyro_spi) always returns HAL_SPI_STATE_BUSY_TX_RX
    if (HAL_DMA_GetState(&dma_gyro_rx) == HAL_DMA_STATE_READY) {
        HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_RESET);

        HAL_SPI_TransmitReceive_DMA(&gyro_spi, txData, rxData, length);

        return true;
    } else {
        return false;
    }
}
