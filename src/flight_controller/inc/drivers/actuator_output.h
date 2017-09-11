#pragma once

extern volatile int escFrequency;
extern volatile float boostIdle;

extern void DeInitActuators(void);
extern void InitActuators(void);
extern void OutputActuators(volatile float motorOutputHere[], volatile float servoOutput[]);
extern void ZeroActuators(uint32_t delayUs);
extern void IdleActuator(uint32_t motorNum);
extern void DirectActuator(uint32_t motorNum, float throttle);