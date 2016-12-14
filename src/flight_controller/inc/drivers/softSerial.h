#pragma once

typedef void (*softserial_function_pointer)(uint8_t *serialBuffer, uint32_t outputLength);

extern volatile softserial_function_pointer softserialCallbackFunctionArray[];

extern void     SoftSerialExtiCallback(void);
extern void     SoftSerialDmaCallback(void);
extern uint32_t SoftSerialSendReceiveBlocking(uint8_t serialOutBuffer[], uint32_t serialOutBufferLength, uint8_t inBuffer[], motor_type actuator, uint32_t timeoutMs);






