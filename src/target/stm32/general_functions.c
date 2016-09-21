#include "includes.h"

uint32_t millis(void) {
	return HAL_GetTick();
}

void delayUs(uint32_t uSec)
{
    volatile uint32_t DWT_START = DWT->CYCCNT;

    // keep DWT_TOTAL from overflowing (max 59.652323s)
    if(uSec > 59652323) uSec = 59652323;

    volatile uint32_t DWT_TOTAL = ((HAL_RCC_GetHCLKFreq() / 1000000) * uSec);
    while ( (DWT->CYCCNT - DWT_START) < DWT_TOTAL);
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
