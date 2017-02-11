#pragma once

enum
{
    LEDS_OFF=0,
    LEDS_ON,
    LEDS_SLOW_BLINK,
    LEDS_MED_BLINK,
    LEDS_FAST_BLINK,
    LEDS_FASTER_BLINK,
    LEDS_FASTEST_BLINK,
    LEDS_ERROR,
};


#define MAX_LED_MODES 13
#define COLOR_CHART_SIZE 7



typedef struct ledStatus_t
{
    uint8_t status;
    uint8_t lastStatus;
    bool on;
    uint32_t timeStart;
    uint32_t timeStop;
} ledStatus_t;


extern uint8_t colorChart[COLOR_CHART_SIZE][3];
extern ledStatus_t ledStatus;

extern void DoLed(uint32_t number, uint32_t on);
extern void InitLeds (void);
extern void UpdateLeds(void);
extern void BlinkAllLeds(uint32_t timeNow, uint16_t time1, uint16_t time2);
