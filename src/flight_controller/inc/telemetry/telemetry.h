#pragma once

extern volatile uint32_t telemEnabled;
extern volatile uint32_t lastTimeSPort;
extern volatile uint32_t okToSendSPort;
extern volatile uint32_t sPortExtiSet;

extern void InitTelemtry(void);

extern void TelemtryRxCallback(uint8_t serialBuffer[], uint32_t outputLength);
extern void TelemtryTxCallback(uint8_t serialBuffer[], uint32_t outputLength);

extern void SportSoftSerialExtiCallback(void);
extern void SportSoftSerialDmaCallback(void);
