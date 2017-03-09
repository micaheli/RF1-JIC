#pragma once

extern ADC_HandleTypeDef adcHandleT;
extern float adcVoltage;

extern void PollAdc(void);
extern void InitAdc(void);
extern void CheckBatteryCellCount(void);
extern float lowVoltage;
extern float runningVoltage;
extern float fullVoltage;
extern float averageVoltage;
extern float cellCutoff;
