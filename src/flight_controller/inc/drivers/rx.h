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
    float    rates[3];
    float    acroPlus[3];
} rc_control_config;

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
};


//We can use different styles of curves
enum {
	NO_EXPO = 0,
	SKITZO_EXPO = 1,
	TARANIS_EXPO = 2,
	ACRO_PLUS = 33,
};


extern uint32_t rx_timeout;
extern float trueRcCommandF[MAXCHANNELS];     //4 sticks. range is -1 to 1, directly related to stick position
extern float curvedRcCommandF[MAXCHANNELS];   //4 sticks. range is -1 to 1, this is the rcCommand after the curve is applied
extern float smoothedRcCommandF[MAXCHANNELS]; //4 sticks. range is -1 to 1, this is the smoothed rcCommand
extern uint32_t rxData[MAXCHANNELS];
extern unsigned char isRxDataNew;

extern void InitRcData(void);
extern void InlineCollectRcCommand (void);
extern float InlineApplyRcCommandCurve (float rcCommand, uint32_t curveToUse, float expo);
extern void InlineRcSmoothing(float curvedRcCommandF[], float smoothedRcCommandF[]);
extern void ProcessSpektrumPacket(void);
