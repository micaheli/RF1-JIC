#include <stdbool.h>

#include "includes.h"

#include "accgyro/invensense_bus.h"
#include "accgyro/invensense_device.h"

#ifdef GYRO_EXTI_GPIO_Port
#include "exti.h"
#endif

SPI_HandleTypeDef gyro_spi;

static void SPI_Init(void)
{
    HAL_SPI_DeInit(&gyro_spi);

    gyro_spi.Instance = GYRO_SPI;
    gyro_spi.Init.Mode = SPI_MODE_MASTER;
    gyro_spi.Init.Direction = SPI_DIRECTION_2LINES;
    gyro_spi.Init.DataSize = SPI_DATASIZE_8BIT;
    gyro_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
    gyro_spi.Init.CLKPhase = SPI_PHASE_2EDGE;
    gyro_spi.Init.NSS = SPI_NSS_SOFT;
    gyro_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;  // TODO: this is slower than it needs to be for reading sensors, re-init later to 20 MHz
    gyro_spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    gyro_spi.Init.TIMode = SPI_TIMODE_DISABLE;
    gyro_spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    gyro_spi.Init.CRCPolynomial = 7;

    if (HAL_SPI_Init(&gyro_spi) != HAL_OK)
    {
        ErrorHandler();
    }

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

bool accgyroInit(void)
{
    SPI_Init();
    HAL_Delay(10);

    if (!accgyroDeviceDetect()) {
        return false;
    }

    HAL_Delay(5);

    if (!accgyroDeviceInit()) {
        return false;
    }

#ifdef GYRO_EXTI_GPIO_Port
    // after the gyro is started, start up the interrupt
    EXTI_Init(GYRO_EXTI_GPIO_Port, GYRO_EXTI_GPIO_Pin, GYRO_EXTI_IRQn, 0, 0);
#endif

    return true;
}

// for now all of these are blocking operations, which is fine for writes
// TODO: test DMA for non-blocking reads
// TODO: un-hardcode f4 CS pin

bool accgyroWriteRegister(uint8_t reg, uint8_t data)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_Delay(1);

    // TODO: what should these timeouts be?
    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Transmit(&gyro_spi, &data, 1, 100);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
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

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Receive(&gyro_spi, data, length, 100);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

    return true;
}

bool accgyroSlowReadRegister(uint8_t reg, uint8_t length, uint8_t *data)
{
    reg |= 0x80;

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_Delay(1);

    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Receive(&gyro_spi, data, length, 100);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_Delay(1);

    return true;
}
