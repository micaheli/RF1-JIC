#pragma once

#define MAXCHANNELS 8

#define MINRC 1000 //1000 to 2000 is old standard for RC channels. midRc needs to be adjustable.
#define MAXRC 2000 //1000 to 2000 is old standard for RC channels. midRc needs to be adjustable.

//config structure which is loaded by config
typedef struct rcControlsConfig_s {
    float    deadBand[MAXCHANNELS];
    uint16_t midRc[MAXCHANNELS];
    uint16_t minRc[MAXCHANNELS];
    uint16_t maxRc[MAXCHANNELS];
    uint8_t  useCurve[MAXCHANNELS];
    float    curveExpo[MAXCHANNELS];
} rcControlsConfig_t;

//Enumerate the different channels in code. The TX map is not affected by this. This is for internal code only.
enum {
	PITCH    = 0,
	ROLL     = 1,
	YAW      = 2,
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
};

extern float trueRcCommandF[4];     //4 sticks. range is -1 to 1, directly related to stick position
extern float curvedRcCommandF[4];   //4 sticks. range is -1 to 1, this is the rcCommand after the curve is applied
extern float smoothedRcCommandF[4]; //4 sticks. range is -1 to 1, this is the smoothed rcCommand

void inlineCollectRcCommand (uint16_t rcData[4], float *trueRcCommandF, float *curvedRcCommandF, rcControlsConfig_t rcControlsConfig);
float inlineApplyRcCommandCurve (float rcCommand, uint8_t curveToUse, float expo);
