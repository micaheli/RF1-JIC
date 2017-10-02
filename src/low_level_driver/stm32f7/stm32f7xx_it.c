/**
  ******************************************************************************
  * @file    USB_Device/HID_Standalone/Src/stm32f7xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.3
  * @date    22-April-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

#include "stm32f7xx_hal.h"
#include "stm32f7xx.h"
#include "stm32f7xx_it.h"

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
	ErrorHandler(HARD_FAULT);
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
	ErrorHandler(MEM_FAULT);
}

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
	ErrorHandler(BUS_FAULT);
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
	ErrorHandler(USAGE_FAULT);
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  InlineUpdateMillisClock();
}

/******************************************************************************/
/*                 STM32F7xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f7xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles USB-On-The-Go FS/HS global interrupt request.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
//  HAL_PCD_IRQHandler(&hpcd);
}

/**
  * @brief  This function handles USB OTG FS/HS Wakeup IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_FS  
void OTG_FS_WKUP_IRQHandler(void)
#else
void OTG_HS_WKUP_IRQHandler(void)
#endif
{
//  if((&hpcd)->Init.low_power_enable)
//  {
//    /* Reset SLEEPDEEP bit of Cortex System Control Register */
//    SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
//    
//    SystemClockConfig_STOP();
//    
//    /* Ungate PHY clock */
//     __HAL_PCD_UNGATE_PHYCLOCK((&hpcd));
//  }
#ifdef USE_USB_FS
  /* Clear EXTI pending Bit*/
//  __HAL_USB_OTG_FS_WAKEUP_EXTI_CLEAR_FLAG();
#else
  /* Clear EXTI pending Bit*/
//  __HAL_USB_OTG_HS_WAKEUP_EXTI_CLEAR_FLAG();
#endif
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
* @brief This function handles USB On The Go FS global interrupt.
*/
void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}



void TIM6_DAC_IRQHandler(void)
{
	if (callbackFunctionArray[FP_TIM6])
		callbackFunctionArray[FP_TIM6](FP_TIM6);

}

void TIM7_IRQHandler(void)
{
	if (callbackFunctionArray[FP_TIM7])
		callbackFunctionArray[FP_TIM7](FP_TIM7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if(htim->Instance == TIM1)
	{
		if (callbackFunctionArray[FP_TIM1])
			callbackFunctionArray[FP_TIM1](FP_TIM1);
	}
	else if(htim->Instance == TIM2)
	{
		if (callbackFunctionArray[FP_TIM2])
			callbackFunctionArray[FP_TIM2](FP_TIM2);
	}
	else if(htim->Instance == TIM3)
	{
		if (callbackFunctionArray[FP_TIM3])
			callbackFunctionArray[FP_TIM3](FP_TIM3);
	}
	else if(htim->Instance == TIM4)
	{
		if (callbackFunctionArray[FP_TIM4])
			callbackFunctionArray[FP_TIM4](FP_TIM4);
	}
	else if(htim->Instance == TIM5)
	{
		if (callbackFunctionArray[FP_TIM5])
			callbackFunctionArray[FP_TIM5](FP_TIM5);
	}
	else if(htim->Instance == TIM6)
	{
		if (callbackFunctionArray[FP_TIM6])
			callbackFunctionArray[FP_TIM6](FP_TIM6);
	}
	else if(htim->Instance == TIM7)
	{
		if (callbackFunctionArray[FP_TIM7])
			callbackFunctionArray[FP_TIM7](FP_TIM7);
	}
	else if(htim->Instance == TIM8)
	{
		if (callbackFunctionArray[FP_TIM8])
			callbackFunctionArray[FP_TIM8](FP_TIM8);
	}
	else if(htim->Instance == TIM9)
	{
		if (callbackFunctionArray[FP_TIM9])
			callbackFunctionArray[FP_TIM9](FP_TIM9);
	}
	else if(htim->Instance == TIM10)
	{
		if (callbackFunctionArray[FP_TIM10])
			callbackFunctionArray[FP_TIM10](FP_TIM10);
	}
	else if(htim->Instance == TIM11)
	{
		if (callbackFunctionArray[FP_TIM11])
			callbackFunctionArray[FP_TIM11](FP_TIM11);
	}
	else if(htim->Instance == TIM12)
	{
		if (callbackFunctionArray[FP_TIM12])
			callbackFunctionArray[FP_TIM12](FP_TIM12);
	}
	else if(htim->Instance == TIM13)
	{
		if (callbackFunctionArray[FP_TIM13])
			callbackFunctionArray[FP_TIM13](FP_TIM13);
	}
	else if(htim->Instance == TIM14)
	{
		if (callbackFunctionArray[FP_TIM14])
			callbackFunctionArray[FP_TIM14](FP_TIM14);
	}

}



void DMA1_Stream0_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream0_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_0]);
	if (callbackFunctionArray[FP_DMA1_S0])
		callbackFunctionArray[FP_DMA1_S0](FP_DMA1_S0);
}

void DMA1_Stream1_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream1_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_1]);
	if (callbackFunctionArray[FP_DMA1_S1])
		callbackFunctionArray[FP_DMA1_S1](FP_DMA1_S1);
}

void DMA1_Stream2_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream2_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_2]);
	if (callbackFunctionArray[FP_DMA1_S2])
		callbackFunctionArray[FP_DMA1_S2](FP_DMA1_S2);
}

void DMA1_Stream3_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream3_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_3]);
	if (callbackFunctionArray[FP_DMA1_S3])
		callbackFunctionArray[FP_DMA1_S3](FP_DMA1_S3);
}

void DMA1_Stream4_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream4_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_4]);
	if (callbackFunctionArray[FP_DMA1_S4])
		callbackFunctionArray[FP_DMA1_S4](FP_DMA1_S4);
}

void DMA1_Stream5_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream5_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_5]);
	if (callbackFunctionArray[FP_DMA1_S5])
		callbackFunctionArray[FP_DMA1_S5](FP_DMA1_S5);
}

void DMA1_Stream6_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream6_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_6]);
	if (callbackFunctionArray[FP_DMA1_S6])
		callbackFunctionArray[FP_DMA1_S6](FP_DMA1_S6);
}

void DMA1_Stream7_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream7_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_7]);
	if (callbackFunctionArray[FP_DMA1_S7])
		callbackFunctionArray[FP_DMA1_S7](FP_DMA1_S7);
}

void DMA2_Stream0_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_0]);
    if (callbackFunctionArray[FP_DMA2_S0])
   		callbackFunctionArray[FP_DMA2_S0](FP_DMA2_S0);
}

void DMA2_Stream1_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream1_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_1]);
    if (callbackFunctionArray[FP_DMA2_S1])
		callbackFunctionArray[FP_DMA2_S1](FP_DMA2_S1);
}

void DMA2_Stream2_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream2_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_2]);
    if (callbackFunctionArray[FP_DMA2_S2])
		callbackFunctionArray[FP_DMA2_S2](FP_DMA2_S2);
}

void DMA2_Stream3_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream3_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_3]);
    if (callbackFunctionArray[FP_DMA2_S3])
		callbackFunctionArray[FP_DMA2_S3](FP_DMA2_S3);
}

void DMA2_Stream4_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream4_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_4]);
    if (callbackFunctionArray[FP_DMA2_S4])
		callbackFunctionArray[FP_DMA2_S4](FP_DMA2_S4);
}

void DMA2_Stream5_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream5_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_5]);
    if (callbackFunctionArray[FP_DMA2_S5])
		callbackFunctionArray[FP_DMA2_S5](FP_DMA2_S5);
}

void DMA2_Stream6_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream6_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_6]);
    if (callbackFunctionArray[FP_DMA2_S6])
		callbackFunctionArray[FP_DMA2_S6](FP_DMA2_S6);
}

void DMA2_Stream7_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream7_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_7]);
    if (callbackFunctionArray[FP_DMA2_S7])
		callbackFunctionArray[FP_DMA2_S7](FP_DMA2_S7);
}





void EXTI0_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI0])
		callbackFunctionArray[FP_EXTI0](FP_EXTI0);
}

void EXTI1_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI1])
		callbackFunctionArray[FP_EXTI1](FP_EXTI1);
}

void EXTI2_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI2])
		callbackFunctionArray[FP_EXTI2](FP_EXTI2);
}

void EXTI3_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI3])
		callbackFunctionArray[FP_EXTI3](FP_EXTI3);
}

void EXTI4_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI4])
		callbackFunctionArray[FP_EXTI4](FP_EXTI4);
}

void EXTI9_5_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI9_5])
		callbackFunctionArray[FP_EXTI9_5](FP_EXTI9_5);
}

void EXTI15_10_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI15_10])
		callbackFunctionArray[FP_EXTI15_10](FP_EXTI15_10);
}

void ADC_IRQHandler(void)
{
  //HAL_ADC_IRQHandler(&adcHandleT);
}


void SPI1_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&spiHandles[ENUM_SPI1]);
}

void SPI2_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&spiHandles[ENUM_SPI2]);
}

void SPI3_IRQHandler(void)
{
	//todo fix hacky with callback function
	flashInfo.status = DMA_READ_COMPLETE;
  	HAL_SPI_IRQHandler(&spiHandles[ENUM_SPI3]);
}