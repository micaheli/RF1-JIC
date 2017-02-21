#pragma once

enum {
	ADC_DISABLED = 0,
	ADC_CURRENT  = 1,
	ADC_VOLTAGE  = 2,
	ADC_RSSI     = 3,
};


extern ADC_HandleTypeDef adcHandleT;
extern float adcVoltage;


extern void PollAdc(void);
extern void InitAdc(void);
extern void CheckBatteryCellCount(void);
extern float lowVoltage;
extern float runningVoltage;
extern float fullVoltage;
extern float averageVoltage;
