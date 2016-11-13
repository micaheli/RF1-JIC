#pragma once

#define LEDS_OFF			0
#define LEDS_ON				1
#define LEDS_SLOW_BLINK		2
#define LEDS_MED_BLINK		3
#define LEDS_FAST_BLINK		4
#define LEDS_FASTER_BLINK	5
#define LEDS_FASTEST_BLINK	6
#define LEDS_ERROR			7


typedef struct ledStatus_t
{
    uint8_t status;
    uint8_t lastStatus;
    bool on;
    uint32_t timeStart;
    uint32_t timeStop;
} ledStatus_t;

extern ledStatus_t ledStatus;

extern void DoLed(uint32_t number, uint32_t on);
extern void InitLeds(void);
