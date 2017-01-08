#pragma once

#define MAXCHANNELS 16

#define MINRC 1000 //1000 to 2000 is old standard for RC channels. midRc needs to be adjustable.
#define MAXRC 2000 //1000 to 2000 is old standard for RC channels. midRc needs to be adjustable.

//config structure which is loaded by config
typedef struct {
    float    deadBand[MAXCHANNELS];
    uint32_t midRc[MAXCHANNELS];
    uint32_t minRc[MAXCHANNELS];
    uint32_t maxRc[MAXCHANNELS];
    uint32_t useCurve[MAXCHANNELS];
    float    curveExpo[MAXCHANNELS];
    uint32_t channelMap[MAXCHANNELS];
    float    rates[3];
    float    acroPlus[3];
    uint32_t rcCalibrated;
    uint32_t rxProtcol;
    uint32_t rxUsart;
} rc_control_config;

typedef struct
{
	uint32_t dataValue;
	uint32_t timesOccurred;
} rx_calibraation_record;

#define RX_CHECK_AMOUNT 24

typedef struct
{
	rx_calibraation_record rxCalibrationRecord[RX_CHECK_AMOUNT];
	uint32_t highestDataValue;
} rx_calibration_records;

//Enumerate the different channels in code. The TX map is not affected by this. This is for internal code only.
enum {
	YAW      = 0,
	ROLL     = 1,
	PITCH    = 2,
	THROTTLE = 3,
	AUX1     = 4,
	AUX2     = 5,
	AUX3     = 6,
	AUX4     = 7,
	AUX5     = 8,
	AUX6     = 9,
	AUX7     = 10,
	AUX8     = 11,
	AUX9     = 12,
	AUX10    = 13,
	AUX11    = 14,
	AUX12    = 15,
};

enum {
	ACCX = 0,
	ACCY = 1,
	ACCZ = 2,
};

//We can use different styles of curves
//used in config.c string table
enum {
	NO_EXPO = 0,
	SKITZO_EXPO = 1,
	TARANIS_EXPO = 2,
	FAST_EXPO = 3,
	ACRO_PLUS = 33,
	EXPO_CURVE_END,
};

//used in config.c string table
#define USING_MANUAL           0
#define USING_SPEKTRUM_ONE_WAY 1
#define USING_SPEKTRUM_TWO_WAY 2
#define USING_SBUS             3
#define USING_SBUS_SPORT       4
#define USING_SUMD             5
#define USING_SUMD_TWO_WAY     6


extern volatile uint32_t rx_timeout;
extern float trueRcCommandF[MAXCHANNELS];     //4 sticks. range is -1 to 1, directly related to stick position
extern float curvedRcCommandF[MAXCHANNELS];   //4 sticks. range is -1 to 1, this is the rcCommand after the curve is applied
extern float smoothedRcCommandF[MAXCHANNELS]; //4 sticks. range is -1 to 1, this is the smoothed rcCommand
extern uint32_t rxData[MAXCHANNELS];
extern volatile unsigned char isRxDataNew;
extern uint32_t skipRxMap;
extern uint32_t activeFailsafe;

extern void SpektrumBind (uint32_t bindNumber);

extern void InitRcData(void);
extern void InlineCollectRcCommand (void);
extern float InlineApplyRcCommandCurve (float rcCommand, uint32_t curveToUse, float expo);
extern void InlineRcSmoothing(float curvedRcCommandF[], float smoothedRcCommandF[]);

extern void ProcessSpektrumPacket(uint32_t serialNumber);
extern void ProcessSbusPacket(uint32_t serialNumber);
extern void ProcessSumdPacket(uint8_t serialRxBuffer[], uint32_t frameSize);

extern void RxUpdate(void);
extern void CheckFailsafe(void);
extern uint32_t SpektrumChannelMap(uint32_t inChannel);
extern uint32_t ChannelMap(uint32_t inChannel);
