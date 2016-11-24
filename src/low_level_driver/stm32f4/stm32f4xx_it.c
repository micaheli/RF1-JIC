#include "includes.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

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
	ZeroActuators(32000);
    while (1) {
		DoLed(0, 1);
		DoLed(1, 1);
		DoLed(2, 1);
    }
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
	ZeroActuators(32000);
    while (1);
}

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
	ZeroActuators(32000);
    while (1);
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
	ZeroActuators(32000);
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
    InlineUpdateMillisClock();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles USB On The Go FS global interrupt.
*/
void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}


void DMA2_Stream3_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream3_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_3]);
}


void DMA2_Stream2_IRQHandler(void) { //clear the IRQ and handle the DMA
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream2_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_2]);
}

void DMA2_Stream7_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream7_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_7]);
}
