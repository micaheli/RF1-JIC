#pragma once


typedef void (*softserial_function_pointer)(uint8_t *serialBuffer, uint32_t outputLength);

extern volatile softserial_function_pointer softserialCallbackFunctionArray[];



extern void     OneWireInit(void);
extern void     OneWireDeinit(void);
extern void     SoftSerialExtiCallback(void);
extern void     SoftSerialDmaCallback(void);
extern uint32_t HandleEscOneWire(uint8_t *serialBuffer, uint32_t outputLength);
