#include "target.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
//extern DMA_HandleTypeDef hdma_spi1_rx;
//extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef gyro_spi;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
    while (1);
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
    while (1);
}

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
    while (1);
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
    while (1);
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/*
* This function handles gyro EXTI line interrupt.
*/
void GYRO_EXTI_IRQHandler(void)
{
    // TODO: read out the gyro
    // move this function to invensense_bus_spi? how much does this change between CPU families?
    HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_PIN);

    HAL_GPIO_EXTI_IRQHandler(GYRO_EXTI_GPIO_Pin);
}

/*
* This function handles gyro SPI global interrupt.
*/
void MPU_SPI_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&gyro_spi);
}

/**
* This function handles DMA2 stream0 global interrupt.
*/
/*
void DMA2_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi1_rx);
}
*/

/**
* This function handles DMA2 stream3 global interrupt.
*/
/*
void DMA2_Stream3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
}
*/

/**
* @brief This function handles USB On The Go FS global interrupt.
*/
void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}
