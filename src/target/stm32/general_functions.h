#pragma once

uint32_t millis(void);
void delayUs(uint32_t uSec);
uint32_t rtc_read_backup_reg(uint32_t BackupRegister);
void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data);

void inlineDigitalHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void inlineDigitalLo(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
bool inlineIsPinStatusHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
