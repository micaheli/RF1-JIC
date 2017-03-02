#pragma once

enum
{
	SM_GET_SETTINGS       = 0x01,
	SM_SET_POWER          = 0x02,
	SM_SET_CHANNEL        = 0x03,
	SM_SET_FREQUENCY      = 0x04,
	SM_SET_OPERATION_MODE = 0x05,
	SM_START_CODE1        = 0xAA,
	SM_START_CODE2        = 0x55,
};

enum
{
	SM_OPMODE_PMIR        = 1,
	SM_OPMODE_PMOR        = 2,
	SM_OPMODE_PM          = 4,
	SM_OPMODE_LOCKED      = 8,

};

enum
{
	SM_SET_OPMODE_PMIR     = 0x01,
	SM_SET_OPMODE_PMOR     = 0x02,
	SM_SET_OPMODE_PM       = 0x04,
	SM_SET_OPMODE_DIS_PMIR = 0x05,
	SM_SET_OPMODE_DIS_PMOR = 0x06,
};

enum
{
	VTX_POWER_025MW       = 0x00,
	VTX_POWER_200MW       = 0x01,
	VTX_POWER_500MW       = 0x02,
	VTX_POWER_800MW       = 0x03,
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
    uint32_t version;
    uint32_t channel;
    uint32_t powerLevel;
    uint32_t opMode;
    uint32_t frequency;
} smart_audio_vtx_structure;

extern uint32_t vtxEnabled;
extern smart_audio_vtx_structure smartAudioVtxStructure;
extern uint8_t smartAudioRxBuffer[];
extern uint8_t smartAudioTxBuffer[];

extern uint32_t TurnOnVtx(void);
extern uint32_t InitSmartAudio(uint32_t usartNumber);
extern void     DeInitSmartAudio(void);
extern uint32_t SmartAudioGetSettingsBlocking(uint32_t timeoutMs);
extern uint32_t SmartAudioSetPowerBlocking(uint32_t powerLevel);
extern uint32_t SmartAudioSetChannelBlocking(uint32_t channel);
extern uint32_t SmartAudioSetOpModeBlocking(uint32_t mask);
extern uint32_t SpektrumBandAndChannelToChannel(VTX_BAND vtxBand, uint8_t channel);

/*
extern volatile uint8_t rfVtxRxBuffer[];

extern void     InitRfVtx(uint32_t usartNumber);
extern uint32_t RfVtxOff(void);
extern uint32_t RfVtxBaud(void);
extern uint32_t RfVtxOn25(void);
extern uint32_t RfVtxOn200(void);
extern uint32_t RfVtxBand(uint32_t band);
extern uint32_t RfVtxChannel(uint32_t channel);
*/
