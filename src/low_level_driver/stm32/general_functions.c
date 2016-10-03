#include "includes.h"

static volatile uint32_t micros;
static volatile uint32_t millisClock;
uint32_t systemUsTicks;

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

inline void inlineDigitalHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
}

void inlineDigitalLo(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
}

inline bool inlineIsPinStatusHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) != (uint32_t)GPIO_PIN_RESET) {
		return false; //pin is set, so it is not reset, which means it is off, so the statement is false
	}
	return true; //pin is reset, so it is not set, which means it is on, so the statement is true
}

void VectorIrqInit(void) {
	SCB->VTOR = ADDRESS_FLASH_START; //set vector register to firmware start
	__enable_irq(); // enable interrupts

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
