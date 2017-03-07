#pragma once


enum
{
	TELEM_OFF       = 0,
	TELEM_USART1    = 1,
	TELEM_USART2    = 2,
	TELEM_USART3    = 3,
	TELEM_USART4    = 4,
	TELEM_USART5    = 5,
	TELEM_USART6    = 6,
	TELEM_ACTUATOR1 = 7,
	TELEM_ACTUATOR2 = 8,
	TELEM_ACTUATOR3 = 9,
	TELEM_ACTUATOR4 = 10,
	TELEM_ACTUATOR5 = 11,
	TELEM_ACTUATOR6 = 12,
	TELEM_ACTUATOR7 = 13,
	TELEM_ACTUATOR8 = 14,
	TELEM_NUM       = 15,
};

enum
{
	VTX_POWER_025MW = 0x00,
	VTX_POWER_200MW = 0x01,
	VTX_POWER_500MW = 0x02,
	VTX_POWER_800MW = 0x03,
};

enum
{
	VTX_BAND_A = 0,
	VTX_BAND_B = 1,
	VTX_BAND_E = 2,
	VTX_BAND_F = 3,
	VTX_BAND_R = 4,
};

enum
{
	VTX_DEVICE_NONE    = 0,
	VTX_DEVICE_SMARTV1 = 1,
	VTX_DEVICE_SMARTV2 = 2,
	VTX_DEVICE_TRAMP   = 3,
	VTX_DEVICE_RF      = 4,
};

enum
{
	VTX_MODE_PIT       = 0,
	VTX_MODE_ACTIVE    = 1,
};

enum
{
	VTX_CH_A1 = 0,
	VTX_CH_A2 = 1,
	VTX_CH_A3 = 2,
	VTX_CH_A4 = 3,
	VTX_CH_A5 = 4,
	VTX_CH_A6 = 5,
	VTX_CH_A7 = 6,
	VTX_CH_A8 = 7,
	VTX_CH_B1 = 8,
	VTX_CH_B2 = 9,
	VTX_CH_B3 = 10,
	VTX_CH_B4 = 11,
	VTX_CH_B5 = 12,
	VTX_CH_B6 = 13,
	VTX_CH_B7 = 14,
	VTX_CH_B8 = 15,
	VTX_CH_E1 = 16,
	VTX_CH_E2 = 17,
	VTX_CH_E3 = 18,
	VTX_CH_E4 = 19,
	VTX_CH_E5 = 20,
	VTX_CH_E6 = 21,
	VTX_CH_E7 = 22,
	VTX_CH_E8 = 23,
	VTX_CH_F1 = 24,
	VTX_CH_F2 = 25,
	VTX_CH_F3 = 26,
	VTX_CH_F4 = 27,
	VTX_CH_F5 = 28,
	VTX_CH_F6 = 29,
	VTX_CH_F7 = 30,
	VTX_CH_F8 = 31,
	VTX_CH_R1 = 32,
	VTX_CH_R2 = 33,
	VTX_CH_R3 = 34,
	VTX_CH_R4 = 35,
	VTX_CH_R5 = 36,
	VTX_CH_R6 = 37,
	VTX_CH_R7 = 38,
	VTX_CH_R8 = 39,
};

typedef struct
{
	uint32_t telemSmartAudio;
    uint32_t telemSport;
    uint32_t telemSpek;
    uint32_t telemMsp;
    uint32_t telemMav;
} telem_config;


typedef struct
{
	uint32_t vtxDevice;
	uint32_t vtxBand;
	uint32_t vtxChannel;
	uint32_t vtxBandChannel;
	uint32_t vtxPower;
	uint32_t vtxPit;
}	vtx_record;

extern volatile uint32_t vtxRecord;
extern volatile uint32_t sendSmartPortAt;
extern volatile uint32_t sendSmartPortLuaAt;
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
