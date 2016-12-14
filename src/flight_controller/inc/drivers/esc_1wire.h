#pragma once


typedef void (*softserial_function_pointer)(uint8_t *serialBuffer, uint32_t outputLength);

extern volatile softserial_function_pointer softserialCallbackFunctionArray[];


extern void     OneWireInit(void);
extern void     OneWireDeinit(void);
