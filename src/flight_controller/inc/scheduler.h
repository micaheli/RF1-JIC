#pragma once

#define SOFT_SERIAL_BIT_TIME_ARRAY_SIZE	80

extern uint32_t autoSaveTimer;
extern volatile uint32_t lastTimeSPort;
extern volatile uint32_t okToSendSPort;
extern volatile uint32_t sPortExtiSet;
extern volatile uint32_t softSerialBuf[][SOFT_SERIAL_BIT_TIME_ARRAY_SIZE];
extern volatile uint32_t softSerialInd[];
extern volatile uint32_t softSerialCurBuf;
extern volatile uint32_t softSerialLastByteProcessedLocation;
extern volatile uint32_t softSerialSwitchBuffer;

extern void scheduler(int32_t count);
extern void SoftSerialCallback(void);
