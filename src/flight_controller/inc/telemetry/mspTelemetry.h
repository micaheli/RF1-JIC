#pragma once



extern volatile uint8_t mspRxBuffer[];
extern volatile uint8_t mspTxBuffer[];

extern void     InitMsp(uint32_t usartNumber);
