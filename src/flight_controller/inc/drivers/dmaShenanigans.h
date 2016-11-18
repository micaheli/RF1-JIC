#define WS2812_MAX_LEDS 32
#define MAX_LED_COLORS 7

typedef struct {
    uint8_t g;
    uint8_t r;
    uint8_t b;
} ws2812Led_t;

typedef struct {
    uint32_t ledColor;
    uint32_t ledCount;
} led_config;

extern ws2812Led_t WS2812_IO_colors[];
extern void Ws2812LedInit( void );
extern void ws2812_led_update(uint32_t nLeds);
extern void SetLEDColor(uint8_t newColor);
