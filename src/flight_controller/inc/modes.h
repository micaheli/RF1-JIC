#pragma once

extern uint32_t activeModes;

typedef struct {
    const char *modeString;
    const uint32_t modeNumber;
    const uint32_t modeMask;
} string_modes_rec;

//also modify stringModes in modes.c
enum {
	M_ARMED    = (1 << 0),
	M_TAKEOFF  = (1 << 1),
	M_HORIZON  = (1 << 2),
	M_ATTITUDE = (1 << 3),
	M_LANDING  = (1 << 4),
	M_FAILSAFE = (1 << 5),
	M_LOGGING  = (1 << 6),
	M_BUZZER   = (1 << 7),
	M_LEDMODE  = (1 << 8),
	M_LEDCOLOR = (1 << 9),
};

extern string_modes_rec stringModes[];

extern void PrintModes(uint32_t backupText);
extern void InitModes(void);
extern void EnableMode(uint32_t modeMask);
extern void DisableMode(uint32_t modeMask);
extern uint32_t ModeActive(uint32_t modeMask);
extern void SetMode(uint32_t modeMask, uint16_t channel, int16_t minRc, int16_t maxRc);
extern void SetupModes(char *modString);
extern void CheckRxToModes(void);
