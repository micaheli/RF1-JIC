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
	SM_VERSION_1 = 0x01,
	SM_VERSION_2 = 0x09,
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
