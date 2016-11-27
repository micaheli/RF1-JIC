#define WS2812_MAX_LEDS 32
#define MAX_LED_COLORS 7

#define DMA_OUTPUT_WS2812_LEDS 0
#define DMA_OUTPUT_ESC_1WIRE   1
#define DMA_OUTPUT_DSHOT       2
#define DMA_OUTPUT_SPORT       3

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


extern void InitDmaInputOnMotors(motor_type actuator);
extern void InitDmaOutputOnMotors(uint32_t usedFor);
extern void Ws2812LedInit(void);
extern void ws2812_led_update(uint32_t nLeds);
extern void SetLEDColor(uint8_t newColor);
extern void OutputSerialDmaByte(uint8_t *serialOutBuffer, uint32_t outputLength, motor_type actuator);
