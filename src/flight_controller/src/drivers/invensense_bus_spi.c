#include "includes.h"

#ifdef GYRO_EXTI
#include "exti.h"
#endif

uint32_t skipGyro = 1;

SPI_HandleTypeDef gyro_spi;
DMA_HandleTypeDef *dma_gyro_rx;
DMA_HandleTypeDef *dma_gyro_tx;

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
        ErrorHandler(GYRO_SPI_INIT_FAILIURE);
    }

    inlineDigitalHi(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);
}

void AccGyroDeinit(void) {

	//reset the gyro if it's connected and talking to us.
	//will timeout if gyro isn't initialized, but this is okay.
	AccGyroWriteRegister(INVENS_RM_PWR_MGMT_1, INVENS_CONST_H_RESET);
    DelayMs(125);

	//TODO: get rid of these defines
#ifdef GYRO_EXTI
    // ensure the interrupt is not running
	HAL_NVIC_DisableIRQ(GYRO_EXTI_IRQn);
#endif
	HAL_NVIC_DisableIRQ(GYRO_TX_DMA_IRQn);
	HAL_NVIC_DisableIRQ(GYRO_RX_DMA_IRQn);

    HAL_SPI_DeInit(&gyro_spi); //TODO: Remove all HAL and place these functions in the stm32.c file so we can support other MCU families.
    inlineDigitalHi(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);
	HAL_GPIO_DeInit(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);

	EXTI_Deinit(GYRO_EXTI_GPIO_Port, GYRO_EXTI_GPIO_Pin, GYRO_EXTI_IRQn);

}

uint32_t AccGyroInit(loopCtrl_e loopCtrl)
{

	AccGyroDeinit();

    // read and write settings at slow speed
	//TODO: All this needs to go into the board record.
	SPI_Init(GYRO_SPI_SLOW_BAUD);
    DelayMs(5);

    if (!AccGyroDeviceDetect()) {
        return 0;
    }

    DelayMs(5);

    if (!AccGyroDeviceInit(loopCtrl)) {
        return 0;
    }

    // reinitialize at full speed
	SPI_Init(GYRO_SPI_FAST_BAUD);

#ifdef GYRO_EXTI
    // after the gyro is started, start up the interrupt
	EXTI_Init(GYRO_EXTI_GPIO_Port, GYRO_EXTI_GPIO_Pin, GYRO_EXTI_IRQn, 2, 0);
#endif

    skipGyro = 0;

    return 1;
}

#ifdef GYRO_EXTI
void GYRO_EXTI_IRQHandler(void)
{

	static uint32_t gyroLoopCounter = 0;
	HAL_GPIO_EXTI_IRQHandler(GYRO_EXTI_GPIO_Pin);

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


void GYRO_RX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(dma_gyro_rx);

    if (HAL_DMA_GetState(dma_gyro_rx) == HAL_DMA_STATE_READY) {
        // reset chip select line
	    HAL_GPIO_WritePin(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin, GPIO_PIN_SET);

        // run callback for completed gyro read
        accgyroDeviceReadComplete();
    }
}

// TODO: get rid of this? only need read/write register and read/write data w/DMA or interrupt
uint32_t AccGyroWriteData(uint8_t *data, uint8_t length)
{
    // poll until SPI is ready in case of ongoing DMA
    while (HAL_SPI_GetState(&gyro_spi) != HAL_SPI_STATE_READY);

    inlineDigitalLo(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);
    DelayMs(2);
    HAL_SPI_Transmit(&gyro_spi, data, length, 100);
    inlineDigitalHi(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);
    DelayMs(2);

    return 1;
}

uint32_t AccGyroWriteRegister(uint8_t reg, uint8_t data)
{
	uint32_t timeout;

    // poll until SPI is ready in case of ongoing DMA
	for (timeout = 0;timeout<10;timeout++) {
		if (HAL_SPI_GetState(&gyro_spi) == HAL_SPI_STATE_READY)
			break;
		DelayMs(1);
	}
	if (timeout == 10) {
		return 0;
	}

    inlineDigitalLo(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);
    DelayMs(2);

    // TODO: what should these timeouts be?
    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 25);
    HAL_SPI_Transmit(&gyro_spi, &data, 1, 25);

    inlineDigitalHi(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);
    DelayMs(2);

    return 1;
}

uint32_t AccGyroVerifyWriteRegister(uint8_t reg, uint8_t data)
{
    uint8_t attempt, data_verify;

    for (attempt = 0; attempt < 20; attempt++) {
    	AccGyroWriteRegister(reg, data);
        DelayMs(2);

        AccGyroSlowReadData(reg, &data_verify, 1);
        if (data_verify == data) {
            return 1;
        }
    }

    ErrorHandler(GYRO_SETUP_COMMUNICATION_FAILIURE);

    return 0;  // this is never reached
}

uint32_t AccGyroReadData(uint8_t reg, uint8_t *data, uint8_t length)
{
    reg |= 0x80;

    // poll until SPI is ready in case of ongoing DMA
    while (HAL_SPI_GetState(&gyro_spi) != HAL_SPI_STATE_READY);

    inlineDigitalLo(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);

    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Receive(&gyro_spi, data, length, 100);

    inlineDigitalHi(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);

    return 1;
}

uint32_t AccGyroSlowReadData(uint8_t reg, uint8_t *data, uint8_t length)
{
    reg |= 0x80;

    // poll until SPI is ready in case of ongoing DMA
    while (HAL_SPI_GetState(&gyro_spi) != HAL_SPI_STATE_READY);

    inlineDigitalLo(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);
    DelayMs(1);

    HAL_SPI_Transmit(&gyro_spi, &reg, 1, 100);
    HAL_SPI_Receive(&gyro_spi, data, length, 100);

    inlineDigitalHi(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);
    DelayMs(1);

    return 1;
}

uint32_t AccGyroDMAReadWriteData(uint8_t *txData, uint8_t *rxData, uint8_t length)
{
    // ensure that both SPI and DMA resources are available, but don't block if they are not
	//while (HAL_DMA_GetState(dma_gyro_rx) != HAL_DMA_STATE_READY && HAL_SPI_GetState(&gyro_spi) != HAL_SPI_STATE_READY);
    if (HAL_DMA_GetState(dma_gyro_rx) == HAL_DMA_STATE_READY && HAL_SPI_GetState(&gyro_spi) == HAL_SPI_STATE_READY) {
    	inlineDigitalLo(GYRO_SPI_CS_GPIO_Port, GYRO_SPI_CS_GPIO_Pin);

        HAL_SPI_TransmitReceive_DMA(&gyro_spi, txData, rxData, length);

        return 1;
    } else {
        return 0;
    }
}
