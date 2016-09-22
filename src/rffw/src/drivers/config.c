#include "includes.h"

rcControlsConfig_t rcControlsConfig;

void loadConfig (void)
{

	//todo: Read this from storage. These are defaults for now.
	float deadBand = 0.025; //2.5% deadband
	uint16_t midRc = 1500; // middle of rc input, one for each channel. Default being 1500
	uint16_t minRc = 1000; // middle of rc input, one for each channel. Default being 1500
	uint16_t maxRc = 2000; // middle of rc input, one for each channel. Default being 1500

	rcControlsConfig->deadBand[PITCH]    = deadband;
	rcControlsConfig->deadBand[ROLL]     = deadband;
	rcControlsConfig->deadBand[YAW]      = deadband;
	rcControlsConfig->deadBand[THROTTLE] = deadband;
	rcControlsConfig->deadBand[AUX1]     = deadband;
	rcControlsConfig->deadBand[AUX2]     = deadband;
	rcControlsConfig->deadBand[AUX3]     = deadband;
	rcControlsConfig->deadBand[AUX4]     = deadband;

	rcControlsConfig->midRc[PITCH]    = midRc;
	rcControlsConfig->midRc[ROLL]     = midRc;
	rcControlsConfig->midRc[YAW]      = midRc;
	rcControlsConfig->midRc[THROTTLE] = midRc;
	rcControlsConfig->midRc[AUX1]     = midRc;
	rcControlsConfig->midRc[AUX2]     = midRc;
	rcControlsConfig->midRc[AUX3]     = midRc;
	rcControlsConfig->midRc[AUX4]     = midRc;

	//radio calibration data is held here. It save cycles and RAM to just use this directly.
	rcControlsConfig->minRc[PITCH]    = minRc;
	rcControlsConfig->minRc[ROLL]     = minRc;
	rcControlsConfig->minRc[YAW]      = minRc;
	rcControlsConfig->minRc[THROTTLE] = minRc;
	rcControlsConfig->minRc[AUX1]     = minRc;
	rcControlsConfig->minRc[AUX2]     = minRc;
	rcControlsConfig->minRc[AUX3]     = minRc;
	rcControlsConfig->minRc[AUX4]     = minRc;

	rcControlsConfig->maxRc[PITCH]    = maxRc;
	rcControlsConfig->maxRc[ROLL]     = maxRc;
	rcControlsConfig->maxRc[YAW]      = maxRc;
	rcControlsConfig->maxRc[THROTTLE] = maxRc;
	rcControlsConfig->maxRc[AUX1]     = maxRc;
	rcControlsConfig->maxRc[AUX2]     = maxRc;
	rcControlsConfig->maxRc[AUX3]     = maxRc;
	rcControlsConfig->maxRc[AUX4]     = maxRc;

	rcControlsConfig->useCurve[PITCH]    = CLEANFLIGHT_EXPO;
	rcControlsConfig->useCurve[ROLL]     = CLEANFLIGHT_EXPO;
	rcControlsConfig->useCurve[YAW]      = CLEANFLIGHT_EXPO;
	rcControlsConfig->useCurve[THROTTLE] = NO_EXPO;
	rcControlsConfig->useCurve[AUX1]     = NO_EXPO;
	rcControlsConfig->useCurve[AUX2]     = NO_EXPO;
	rcControlsConfig->useCurve[AUX3]     = NO_EXPO;
	rcControlsConfig->useCurve[AUX4]     = NO_EXPO;

	rcControlsConfig->curveExpo[PITCH]    = 60.0f;
	rcControlsConfig->curveExpo[ROLL]     = 60.0f;
	rcControlsConfig->curveExpo[YAW]      = 60.0f;
	rcControlsConfig->curveExpo[THROTTLE] = 0;
	rcControlsConfig->curveExpo[AUX1]     = 0;
	rcControlsConfig->curveExpo[AUX2]     = 0;
	rcControlsConfig->curveExpo[AUX3]     = 0;
	rcControlsConfig->curveExpo[AUX4]     = 0;

}

