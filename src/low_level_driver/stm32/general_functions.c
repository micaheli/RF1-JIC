#include "includes.h"

static volatile uint32_t micros;
static volatile uint32_t millisClock;
volatile uint32_t systemUsTicks;

inline uint32_t InlineMillis(void) {
	return HAL_GetTick();
}

inline void InlineUpdateMillisClock (void) {
	millisClock = DWT->CYCCNT;
}

uint32_t Micros(void) {

	volatile uint32_t baseMillis;
	volatile uint32_t baseClock;

    int is = __get_PRIMASK();
    __disable_irq();

    baseMillis = InlineMillis();
    baseClock = millisClock;

    uint32_t elapsedSinceMillis = ( (DWT->CYCCNT-baseClock) / systemUsTicks );

    if ((is & 1) == 0) {
        __enable_irq();
    }

    return ((baseMillis * 1000) + elapsedSinceMillis);

}

inline void delayUs(uint32_t uSec)
{
	//simpleDelay_ASM(uSec);
	//return;
    volatile uint32_t DWT_START = DWT->CYCCNT;

    volatile uint32_t DWT_TOTAL = (systemUsTicks * uSec);
    while ( (DWT->CYCCNT - DWT_START) < DWT_TOTAL);
}

void DelayMs(uint32_t mSec) {
	HAL_Delay(mSec);
}

uint32_t rtc_read_backup_reg(uint32_t BackupRegister) {
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    return HAL_RTCEx_BKUPRead(&RtcHandle, BackupRegister);
}

void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data) {
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&RtcHandle, BackupRegister, data);
    HAL_PWR_DisableBkUpAccess();
}

void InitUsb(void) {
	USB_DEVICE_Init();
}

void SystemReset(void)
{
	__disable_irq();
	NVIC_SystemReset();
}

void SystemResetToDfuBootloader(void) {
	//todo make this work on all MCUs
	__disable_irq();
	*((uint32_t *)0x2001FFFC) = 0xDEADBEEF; // 128KB SRAM STM32F4XX
	NVIC_SystemReset();
}

inline void inlineDigitalHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
}

void inlineDigitalLo(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
}

inline int inlineIsPinStatusHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) != (uint32_t)GPIO_PIN_RESET) {
		return 0; //pin is set, so it is not reset, which means it is off, so the statement is false
	}
	return 1; //pin is reset, so it is not set, which means it is on, so the statement is true
}

void VectorIrqInit(uint32_t address) {
	SCB->VTOR = address; //set vector register to firmware start
	__enable_irq(); // enable interrupts

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void InitializeGpio(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t on)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(GPIOx, GPIO_Pin);

    GPIO_InitStructure.Pin   = GPIO_Pin;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);

    if (on) {
    	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
    } else {
    	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
    }
}

uint32_t GetExtinFromPin(uint16_t GPIO_Pin)
{

	switch (GPIO_Pin)
	{
		case GPIO_PIN_0:
			return (EXTI0_IRQn);
		case GPIO_PIN_1:
			return (EXTI1_IRQn);
		case GPIO_PIN_2:
			return (EXTI2_IRQn);
		case GPIO_PIN_3:
			return (EXTI3_IRQn);
		case GPIO_PIN_4:
			return (EXTI4_IRQn);
		case GPIO_PIN_5:
		case GPIO_PIN_6:
		case GPIO_PIN_7:
		case GPIO_PIN_8:
		case GPIO_PIN_9:
			return (EXTI9_5_IRQn);
		case GPIO_PIN_10:
		case GPIO_PIN_11:
		case GPIO_PIN_12:
		case GPIO_PIN_13:
		case GPIO_PIN_14:
		case GPIO_PIN_15:
			return (EXTI15_10_IRQn);
	}

	return (0);

}

uint32_t GetExtiCallbackFromPin(uint16_t GPIO_Pin)
{

	switch (GPIO_Pin)
	{
		case GPIO_PIN_0:
			return (FP_EXTI0);
		case GPIO_PIN_1:
			return (FP_EXTI1);
		case GPIO_PIN_2:
			return (FP_EXTI2);
		case GPIO_PIN_3:
			return (FP_EXTI3);
		case GPIO_PIN_4:
			return (FP_EXTI4);
		case GPIO_PIN_5:
		case GPIO_PIN_6:
		case GPIO_PIN_7:
		case GPIO_PIN_8:
		case GPIO_PIN_9:
			return (FP_EXTI9_5);
		case GPIO_PIN_10:
		case GPIO_PIN_11:
		case GPIO_PIN_12:
		case GPIO_PIN_13:
		case GPIO_PIN_14:
		case GPIO_PIN_15:
			return (FP_EXTI15_10);
	}

	return (0);

}
