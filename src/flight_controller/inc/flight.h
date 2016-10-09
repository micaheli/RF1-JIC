#pragma once

extern uint32_t boardArmed, calibrateMotors;

void InitFlightCode(void);
void InlineFlightCode(float dpsGyroArray[]);
float InlineGetSetPoint(float curvedRcCommandF, float rates, float acroPlus);
