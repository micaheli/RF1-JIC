#pragma once

extern int  InitTrampTelemetry(uint32_t usartNumber);
extern void DeInitTrampTelemetry(void);
extern void TrampSetPitMode(uint32_t on);
extern void TrampSetFreq(uint16_t freq);
extern void TrampSetRfPower(uint16_t power);
extern void TrampSetBandChannel(uint32_t bandChannel);
extern void ProcessTrampTelemetry(void);
extern int  TrampGetSettings(void);