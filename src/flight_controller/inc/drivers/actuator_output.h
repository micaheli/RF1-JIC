#pragma once

extern void DeinitActuators(void);
extern void InitActuators(void);
extern void OutputActuators(volatile float motorOutputHere[], volatile float servoOutput[]);
extern void ZeroActuators(uint32_t delayUs);
