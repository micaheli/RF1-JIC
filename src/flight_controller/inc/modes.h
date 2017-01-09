#pragma once

extern uint32_t activeModes;

typedef struct {
    const char *modeString;
    const int32_t modeNumber;
    const int32_t modeMask;
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
};

extern string_modes_rec stringModes[];

void InitModes(void);
void EnableMode(uint32_t modeMask);
void DisableMode(uint32_t modeMask);
uint32_t ModeActive(uint32_t modeMask);
void SetupModes(char *modeString);
