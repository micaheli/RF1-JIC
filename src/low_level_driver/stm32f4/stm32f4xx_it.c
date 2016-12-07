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



void TIM1_CC_IRQHandler(void)
{
	//HAL_TIM_IRQHandler(&TimHandle);
}


void TIM2_IRQHandler(void)
{
	//HAL_TIM_IRQHandler(&TimHandle);
}

void TIM3_IRQHandler(void)
{
//	HAL_TIM_IRQHandler(&pwmTimers[board.motors[0].actuatorArrayNum]);
}

void TIM8_UP_TIM13_IRQHandler(void)
{
//	HAL_TIM_IRQHandler(&softSerialClockTimer);
//	if (inlineIsPinStatusHi(ports[board.motors[0].port], board.motors[0].pin))
//		inlineDigitalHi(ports[board.motors[0].port], board.motors[0].pin);
//	else
//		inlineDigitalLo(ports[board.motors[0].port], board.motors[0].pin);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	(void)(htim);
	//if(htim->Instance == TIM8)
	//{
		//HAL_TIM_IRQHandler(&softSerialClockTimer);
		//if (inlineIsPinStatusHi(ports[board.motors[0].port], board.motors[0].pin))
		//	inlineDigitalHi(ports[board.motors[0].port], board.motors[0].pin);
		//else
		//	inlineDigitalLo(ports[board.motors[0].port], board.motors[0].pin);
		//HAL_TIM_Base_Stop_IT(htim);
		//HAL_GPIO_WritePin(GPIOG,GPIO_PIN_2,GPIO_PIN_RESET);
    //}

}


void DMA1_Stream0_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream0_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_0]);
	if (callbackFunctionArray[FP_DMA1_S0])
		callbackFunctionArray[FP_DMA1_S0]();
}

void DMA1_Stream1_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream1_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_1]);
	if (callbackFunctionArray[FP_DMA1_S1])
		callbackFunctionArray[FP_DMA1_S1]();
}

void DMA1_Stream2_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream2_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_2]);
	if (callbackFunctionArray[FP_DMA1_S2])
		callbackFunctionArray[FP_DMA1_S2]();
}

void DMA1_Stream3_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream3_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_3]);
	if (callbackFunctionArray[FP_DMA1_S3])
		callbackFunctionArray[FP_DMA1_S3]();
}

void DMA1_Stream4_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream4_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_4]);
	if (callbackFunctionArray[FP_DMA1_S4])
		callbackFunctionArray[FP_DMA1_S4]();
}

void DMA1_Stream5_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream5_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_5]);
	if (callbackFunctionArray[FP_DMA1_S5])
		callbackFunctionArray[FP_DMA1_S5]();
}

void DMA1_Stream6_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream6_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_6]);
	if (callbackFunctionArray[FP_DMA1_S6])
		callbackFunctionArray[FP_DMA1_S6]();
}

void DMA1_Stream7_IRQHandler(void) {
	HAL_NVIC_ClearPendingIRQ(DMA1_Stream7_IRQn);
	HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA1_STREAM_7]);
	if (callbackFunctionArray[FP_DMA1_S7])
		callbackFunctionArray[FP_DMA1_S7]();
}

void DMA2_Stream0_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_0]);
    if (callbackFunctionArray[FP_DMA2_S0])
   		callbackFunctionArray[FP_DMA2_S0]();
}

void DMA2_Stream1_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream1_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_1]);
    if (callbackFunctionArray[FP_DMA2_S1])
		callbackFunctionArray[FP_DMA2_S1]();
}

void DMA2_Stream2_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream2_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_2]);
    if (callbackFunctionArray[FP_DMA2_S2])
		callbackFunctionArray[FP_DMA2_S2]();
}

void DMA2_Stream3_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream3_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_3]);
    if (callbackFunctionArray[FP_DMA2_S3])
		callbackFunctionArray[FP_DMA2_S3]();
}

void DMA2_Stream4_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream4_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_4]);
    if (callbackFunctionArray[FP_DMA2_S4])
		callbackFunctionArray[FP_DMA2_S4]();
}

void DMA2_Stream5_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream5_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_5]);
    if (callbackFunctionArray[FP_DMA2_S5])
		callbackFunctionArray[FP_DMA2_S5]();
}

void DMA2_Stream6_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream6_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_6]);
    if (callbackFunctionArray[FP_DMA2_S6])
		callbackFunctionArray[FP_DMA2_S6]();
}

void DMA2_Stream7_IRQHandler(void) {
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream7_IRQn);
    HAL_DMA_IRQHandler(&dmaHandles[ENUM_DMA2_STREAM_7]);
    if (callbackFunctionArray[FP_DMA2_S7])
		callbackFunctionArray[FP_DMA2_S7]();
}





void EXTI0_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI0])
		callbackFunctionArray[FP_EXTI0]();
}

void EXTI1_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI1])
		callbackFunctionArray[FP_EXTI1]();
}

void EXTI2_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI2])
		callbackFunctionArray[FP_EXTI2]();
}

void EXTI3_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI3])
		callbackFunctionArray[FP_EXTI3]();
}

void EXTI4_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI4])
		callbackFunctionArray[FP_EXTI4]();
}

void EXTI9_5_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI9_5])
		callbackFunctionArray[FP_EXTI9_5]();
}

void EXTI15_10_IRQHandler(void)
{
	if (callbackFunctionArray[FP_EXTI15_10])
		callbackFunctionArray[FP_EXTI15_10]();
}
