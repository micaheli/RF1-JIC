#pragma once
/*
#define BUZZER_ON     HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_GPIO_Pin, GPIO_PIN_RESET)
#define BUZZER_OFF    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_GPIO_Pin, GPIO_PIN_SET)
#define BUZZER_TOGGLE HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_GPIO_Pin)
*/


enum
{
    STATE_BUZZER_OFF = 0,
    STATE_BUZZER_ON,
    STATE_BUZZER_ERROR,
	STATE_BUZZER_LOST,
	STATE_BUZZER_SIMPLE,
};

typedef struct buzzerStatus_t
{
    uint8_t status;
    uint8_t lastStatus;
    bool on;
    uint32_t timeStart;
    uint32_t timeStop;
} buzzerStatus_t;

extern buzzerStatus_t buzzerStatus;

//void BuzzerInit (void);
void InitBuzzer(void);
void InitializeBuzzerPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Buzz(uint32_t timeNow, uint16_t time1, uint16_t time2);
void UpdateBuzzer(void);
