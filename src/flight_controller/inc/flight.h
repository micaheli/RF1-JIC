#pragma once

void InitFlightCode(void);
void InlineFlightCode(float dpsGyroArray[]);
float InlineGetSetPoint(float curvedRcCommandF, float rates, float acroPlus);
