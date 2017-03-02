#pragma once


enum
{
	TELEM_OFF=0,
	TELEM_USART1=1,
	TELEM_USART2=2,
	TELEM_USART3=3,
	TELEM_USART4=4,
	TELEM_USART5=5,
	TELEM_USART6=6,
	TELEM_ACTUATOR1=7,
	TELEM_ACTUATOR2=8,
	TELEM_ACTUATOR3=9,
	TELEM_ACTUATOR4=10,
	TELEM_ACTUATOR5=11,
	TELEM_ACTUATOR6=12,
	TELEM_ACTUATOR7=13,
	TELEM_ACTUATOR8=14,
	TELEM_NUM=15,
};

typedef struct
{
	uint32_t telemSmartAudio;
    uint32_t telemSport;
    uint32_t telemSpek;
    uint32_t telemMsp;
    uint32_t telemMav;
} telem_config;


extern volatile uint32_t sendSmartPortAt;
extern volatile uint32_t sendSpektrumTelemtryAt;

extern volatile uint32_t telemEnabled;
extern volatile uint32_t lastTimeSPort;
extern volatile uint32_t okToSendSPort;
extern volatile uint32_t sPortExtiSet;

extern void InitTelemtry(void);
extern void ProcessTelemtry(void);

extern void TelemtryRxCallback(uint8_t serialBuffer[], uint32_t outputLength);
extern void TelemtryTxCallback(uint8_t serialBuffer[], uint32_t outputLength);

extern void SportSoftSerialExtiCallback(uint32_t callbackNumber);
extern void SportSoftSerialDmaCallback(uint32_t callbackNumber);
