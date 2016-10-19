#include "includes.h"

#ifdef GYRO_EXTI
#include "exti.h"
#endif

bool skipGyro = true;

SPI_HandleTypeDef gyro_spi;
DMA_HandleTypeDef dma_gyro_rx;
DMA_HandleTypeDef dma_gyro_tx;

static void SPI_Init(uint32_t baudRatePrescaler)
{
	gyro_spi.Instance = board.gyro_pins.SPIInstance;
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

	HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_SET);
}

static void DMA_Init(void)
{
    /* DMA interrupt init */
	HAL_NVIC_SetPriority(board.spis[board.gyro_pins.SPINumber].TXDMA_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(board.spis[board.gyro_pins.SPINumber].TXDMA_IRQn);
	HAL_NVIC_SetPriority(board.spis[board.gyro_pins.SPINumber].RXDMA_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(board.spis[board.gyro_pins.SPINumber].RXDMA_IRQn);
}

bool accgyroInit(loopCtrl_e loopCtrl)
{
#ifdef GYRO_EXTI
    // ensure the interrupt is not running
	HAL_NVIC_DisableIRQ(board.gyro_pins.extiIRQn);
#endif
	HAL_NVIC_DisableIRQ(board.spis[board.gyro_pins.SPINumber].TXDMA_IRQn);
	HAL_NVIC_DisableIRQ(board.spis[board.gyro_pins.SPINumber].RXDMA_IRQn);

    // read and write settings at slow speed
	SPI_Init(board.gyro_pins.spiSlowBaud);
    HAL_Delay(5);

    if (!accgyroDeviceDetect()) {
        return false;
    }

    HAL_Delay(5);

    if (!accgyroDeviceInit(loopCtrl)) {
        return false;
    }

    // reinitialize at full speed
	SPI_Init(board.gyro_pins.spiFastBaud);
    DMA_Init();

#ifdef GYRO_EXTI
    // after the gyro is started, start up the interrupt
	EXTI_Init(board.gyro_pins.extiPort, board.gyro_pins.extiPin, board.gyro_pins.extiIRQn, 2, 0);
#endif

    skipGyro = false;

    return true;
}

#ifdef GYRO_EXTI
void GYRO_EXTI_IRQHandler(void)
{

	static uint32_t gyroLoopCounter = 0;
    HAL_GPIO_EXTI_IRQHandler(board.gyro_pins.extiPin);

    if (!skipGyro)
    {

    	//update ACC after the rest of the flight code upon the proper denom
    	//modulus works, &ing doesn't
//    	if ( (loopCounter-- % gyroConfig.accDenom) == 0 ) {
        //if (loopCounter-- & gyroConfig.accDenom) {
    	if (gyroLoopCounter--==0) {
    		gyroLoopCounter = gyroConfig.accDenom;
        	accgyroDeviceReadAccGyro();
        } else {
        	accgyroDeviceReadGyro();
        }
    }
}
#endif

void GYRO_TX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&dma_gyro_tx);
}

void GYRO_RX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&dma_gyro_rx);

    if (HAL_DMA_GetState(&dma_gyro_rx) == HAL_DMA_STATE_READY) {
        // reset chip select line
	    HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_SET);

        // run callback for completed gyro read
        accgyroDeviceReadComplete();
    }
}

// TODO: get rid of this? only need read/write register and read/write data w/DMA or interrupt
bool accgyroWriteData(uint8_t *data, uint8_t length)
{
    // poll until SPI is ready in case of ongoing DMA
    while (HAL_SPI_GetState(&gyro_spi) != HAL_SPI_STATE_READY);

    HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_SPI_Transmit(&gyro_spi, data, length, 100);
    HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_SET);
    HAL_Delay(2);

    return true;
}

bool accgyroWriteRegister(uint8_t reg, uint8_t data)
{
    // poll until SPI is ready in case of ongoing DMA
    while (HAL_SPI_GetState(&gyro_spi) != HAL_SPI_STATE_READY);

    HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_RESET);
    HAL_Delay(2);

    // TODO: what should these timeouts be?
    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Transmit(&gyro_spi, &data, 1, 100);

    HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_SET);
    HAL_Delay(2);

    return true;
}

bool accgyroVerifyWriteRegister(uint8_t reg, uint8_t data)
{
    uint8_t attempt, data_verify;

    for (attempt = 0; attempt < 20; attempt++) {
        accgyroWriteRegister(reg, data);
        HAL_Delay(2);

        accgyroSlowReadData(reg, &data_verify, 1);
        if (data_verify == data) {
            return true;
        }
    }

    ErrorHandler();

    return false;  // this is never reached
}

bool accgyroReadData(uint8_t reg, uint8_t *data, uint8_t length)
{
    reg |= 0x80;

    // poll until SPI is ready in case of ongoing DMA
    while (HAL_SPI_GetState(&gyro_spi) != HAL_SPI_STATE_READY);

    HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Receive(&gyro_spi, data, length, 100);

    HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_SET);

    return true;
}

bool accgyroSlowReadData(uint8_t reg, uint8_t *data, uint8_t length)
{
    reg |= 0x80;

    // poll until SPI is ready in case of ongoing DMA
    while (HAL_SPI_GetState(&gyro_spi) != HAL_SPI_STATE_READY);

    HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_RESET);
    HAL_Delay(1);

    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Receive(&gyro_spi, data, length, 100);

    HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_SET);
    HAL_Delay(1);

    return true;
}

bool accgyroDMAReadWriteData(uint8_t *txData, uint8_t *rxData, uint8_t length)
{
    // ensure that both SPI and DMA resources are available, but don't block if they are not
	//while (HAL_DMA_GetState(&dma_gyro_rx) != HAL_DMA_STATE_READY && HAL_SPI_GetState(&gyro_spi) != HAL_SPI_STATE_READY);
    if (HAL_DMA_GetState(&dma_gyro_rx) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&gyro_spi) == HAL_SPI_STATE_READY) {
        HAL_GPIO_WritePin(board.gyro_pins.csPort, board.gyro_pins.csPin, GPIO_PIN_RESET);

        HAL_SPI_TransmitReceive_DMA(&gyro_spi, txData, rxData, length);

        return true;
    } else {
        return false;
    }
}
