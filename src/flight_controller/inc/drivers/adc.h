#pragma once

enum {
	ADC_DISABLED = 0,
	ADC_CURRENT  = 1,
	ADC_VOLTAGE  = 2,
	ADC_RSSI     = 3,
};


void InitAdc(void);
