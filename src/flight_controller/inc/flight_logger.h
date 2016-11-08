#pragma once

extern uint32_t LoggingEnabled;
extern uint32_t LogThisIteration;

extern int InitFlightLogger(void);
extern void EnableLogging(void);
extern void DisableLogging(void);
extern void UpdateBlackbox(pid_output *flightPids, float flightSetPoints[] );
extern void InlineWrite16To8 (int16_t data);
extern void FinishPage(void);
extern void WriteByteToFlash (uint8_t data);
