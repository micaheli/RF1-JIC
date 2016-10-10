#pragma once

extern uint32_t boardArmed, calibrateMotors, fullKiLatched;

void InitFlightCode(void);
void InlineInitGyroFilters(void);
void InlineFlightCode(float dpsGyroArray[]);
float InlineGetSetPoint(float curvedRcCommandF, float rates, float acroPlus);
