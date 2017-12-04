#pragma once

enum
{
    LEDS_OFF           = 0,
    LEDS_ON            = 1,
    LEDS_SLOW_BLINK    = 2,
    LEDS_MED_BLINK     = 3,
    LEDS_FAST_BLINK    = 4,
    LEDS_FASTER_BLINK  = 5,
    LEDS_FASTEST_BLINK = 6,
    LEDS_ERROR         = 7,
};

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
extern void InitLeds (void);
extern void UpdateLeds(void);
extern void BlinkAllLeds(uint32_t timeNow, uint16_t time1, uint16_t time2);
