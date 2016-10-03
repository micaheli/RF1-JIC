#pragma once

extern uint32_t systemUsTicks;

uint32_t InlineMillis(void);
uint32_t Micros(void);

void DelayMs(uint32_t mSec);
void delayUs(uint32_t uSec);
uint32_t rtc_read_backup_reg(uint32_t BackupRegister);
void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data);

void InitUsb(void);

void inlineDigitalHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void inlineDigitalLo(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
bool inlineIsPinStatusHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void InlineUpdateMillisClock (void);

void VectorIrqInit(void);
