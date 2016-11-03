#pragma once

extern uint32_t LoggingEnabled;
extern uint32_t LogThisIteration;

extern int InitFlightLogger(void);
extern void EnableLogging(void);
extern void DisableLogging(void);
extern void UpdateBlackbox(pid_output *flightPids);
extern void InlineWrite16To8 (uint16_t data);
extern void DumbWriteToFlash (uint8_t data);
