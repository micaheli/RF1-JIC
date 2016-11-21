#pragma once

extern void OutputActuators(volatile float motorOutputHere[], volatile float servoOutput[]);
extern void InitActuators(void);
void InitActuatorTimer(motor_type actuator, uint32_t pwmHz, uint32_t timerHz);
extern void ZeroActuators(void);
