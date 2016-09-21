#pragma once

#if LEDn >= 1
#define LED1_ON     HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_GPIO_Pin, GPIO_PIN_RESET)
#define LED1_OFF    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_GPIO_Pin, GPIO_PIN_SET)
#define LED1_TOGGLE HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_GPIO_Pin)
#else
#define LED1_ON
#define LED1_OFF
#define LED1_TOGGLE
#endif

#if LEDn >= 2
#define LED2_ON     HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_GPIO_Pin, GPIO_PIN_RESET)
#define LED2_OFF    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_GPIO_Pin, GPIO_PIN_SET)
#define LED2_TOGGLE HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_GPIO_Pin)
#else
#define LED2_ON
#define LED2_OFF
#define LED2_TOGGLE
#endif

#if LEDn >= 3
#define LED3_ON     HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_GPIO_Pin, GPIO_PIN_RESET)
#define LED3_OFF    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_GPIO_Pin, GPIO_PIN_SET)
#define LED3_TOGGLE HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_GPIO_Pin)
#else
#define LED3_ON
#define LED3_OFF
#define LED3_TOGGLE
#endif


enum {
	LEDS_OFF,
	LEDS_ON,
	LEDS_SLOW_BLINK,
	LEDS_MED_BLINK,
	LEDS_FAST_BLINK,
	LEDS_ERROR,
};

typedef struct ledStatus_t {
	uint8_t status;
	uint8_t lastStatus;
	bool on;
	uint32_t timeStart;
	uint32_t timeStop;
} ledStatus_t;

extern ledStatus_t ledStatus;

void LedInit (void);
void InitializeLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void updateLeds(void);
void blinkAllLeds(uint32_t timeNow, uint16_t time1, uint16_t time2);
