#pragma once

extern uint32_t NewProcessSoftSerialBits(volatile uint32_t timerBuffer[], volatile uint32_t *timerBufferIndex, volatile uint8_t serialBuffer[], volatile uint32_t *serialBufferIndex, float bitWidthUs, uint32_t bitsInByte);
