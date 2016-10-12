#include "includes.h"

// put this in a config.h
/*
#define UINT32 1;
#define FLOAT 2;
#define STRING 3;  // max size 32

typedef struct {
    void *location;
    char name[8];  // max variable name 12
} variableList;

variableList variables[100];


void AddVariable(char *variableName, void *VariableLocation, uint32_t variableType)
{


}
*/


// use variable record but instead of storing address of variable, store offset based on address of field, that way it works with the record loaded from file



//Dynamic config is bad idea if we are supporting f1, so instead we will go to a static sized structure
// basically add padding at end of structure to make it a fixed size.   Then always ad variables to the end, then when loading
// the structure, if your new version have new variables they will be zeroied and then just check in this function for zeroed functions and set default

void LoadConfig (void)
{

	//todo: Read this from storage. These are defaults for now.
	float deadband = 0.025; //2.5% deadband
	uint32_t minRc = 0; // bottom of rc input, one for each channel. Default being 1500
	uint32_t midRc = 1024; // middle of rc input, one for each channel. Default being 1500
	uint32_t maxRc = 2048; // middle of rc input, one for each channel. Default being 1500

	pidConfig[YAW].kp	= .00140000;
	pidConfig[YAW].ki	= .00380000;
	pidConfig[YAW].kd	= .00000320;
	pidConfig[YAW].wc	= 16;

	pidConfig[ROLL].kp	= .00140000;
	pidConfig[ROLL].ki	= .00380000;
	pidConfig[ROLL].kd	= .00000320;
	pidConfig[ROLL].wc	= 16;

	pidConfig[PITCH].kp	= .00140000;
	pidConfig[PITCH].ki	= .00380000;
	pidConfig[PITCH].kd	= .00000320;
	pidConfig[PITCH].wc	= 16;


	filterConfig[YAW].gyro.q   = 0.00010;
	filterConfig[YAW].gyro.r   = 0.01800;
	filterConfig[YAW].gyro.p   = 0.00015;

	filterConfig[ROLL].gyro.q  = 0.00010;
	filterConfig[ROLL].gyro.r  = 0.01800;
	filterConfig[ROLL].gyro.p  = 0.00015;

	filterConfig[PITCH].gyro.q = 0.00010;
	filterConfig[PITCH].gyro.r = 0.01800;
	filterConfig[PITCH].gyro.p = 0.00015;

	filterConfig[YAW].kd.q     = 0.00010;
	filterConfig[YAW].kd.r     = 0.00200;
	filterConfig[YAW].kd.p     = 0.00015;

	filterConfig[ROLL].kd.q    = 0.00010;
	filterConfig[ROLL].kd.r    = 0.00200;
	filterConfig[ROLL].kd.p    = 0.00015;

	filterConfig[PITCH].kd.q   = 0.00010;
	filterConfig[PITCH].kd.r   = 0.00200;
	filterConfig[PITCH].kd.p   = 0.00015;


	gyroConfig.minorBoardRotation[X] = 0;
	gyroConfig.minorBoardRotation[Y] = 0;
	gyroConfig.minorBoardRotation[Z] = 0;

	gyroConfig.gyroRotation  = CW270;
	gyroConfig.boardRotation = CW0;
	gyroConfig.loopCtrl = LOOP_H8;

	rcControlsConfig.deadBand[PITCH]    = deadband;
	rcControlsConfig.deadBand[ROLL]     = deadband;
	rcControlsConfig.deadBand[YAW]      = deadband;
	rcControlsConfig.deadBand[THROTTLE] = 0;
	rcControlsConfig.deadBand[AUX1]     = deadband;
	rcControlsConfig.deadBand[AUX2]     = deadband;
	rcControlsConfig.deadBand[AUX3]     = deadband;
	rcControlsConfig.deadBand[AUX4]     = deadband;

	rcControlsConfig.midRc[PITCH]    = midRc;
	rcControlsConfig.midRc[ROLL]     = midRc;
	rcControlsConfig.midRc[YAW]      = midRc;
	rcControlsConfig.midRc[THROTTLE] = midRc;
	rcControlsConfig.midRc[AUX1]     = midRc;
	rcControlsConfig.midRc[AUX2]     = midRc;
	rcControlsConfig.midRc[AUX3]     = midRc;
	rcControlsConfig.midRc[AUX4]     = midRc;

	//radio calibration data is held here. It save cycles and RAM to just use this directly.
	rcControlsConfig.minRc[PITCH]    = minRc;
	rcControlsConfig.minRc[ROLL]     = minRc;
	rcControlsConfig.minRc[YAW]      = minRc;
	rcControlsConfig.minRc[THROTTLE] = minRc;
	rcControlsConfig.minRc[AUX1]     = minRc;
	rcControlsConfig.minRc[AUX2]     = minRc;
	rcControlsConfig.minRc[AUX3]     = minRc;
	rcControlsConfig.minRc[AUX4]     = minRc;

	rcControlsConfig.maxRc[PITCH]    = maxRc;
	rcControlsConfig.maxRc[ROLL]     = maxRc;
	rcControlsConfig.maxRc[YAW]      = maxRc;
	rcControlsConfig.maxRc[THROTTLE] = maxRc;
	rcControlsConfig.maxRc[AUX1]     = maxRc;
	rcControlsConfig.maxRc[AUX2]     = maxRc;
	rcControlsConfig.maxRc[AUX3]     = maxRc;
	rcControlsConfig.maxRc[AUX4]     = maxRc;

	rcControlsConfig.useCurve[PITCH]     = SKITZO_EXPO;
	rcControlsConfig.useCurve[ROLL]      = SKITZO_EXPO;
	rcControlsConfig.useCurve[YAW]       = SKITZO_EXPO;
	rcControlsConfig.useCurve[THROTTLE]  = NO_EXPO;
	rcControlsConfig.useCurve[AUX1]      = NO_EXPO;
	rcControlsConfig.useCurve[AUX2]      = NO_EXPO;
	rcControlsConfig.useCurve[AUX3]      = NO_EXPO;
	rcControlsConfig.useCurve[AUX4]      = NO_EXPO;

	rcControlsConfig.curveExpo[PITCH]    = 60.0f;
	rcControlsConfig.curveExpo[ROLL]     = 60.0f;
	rcControlsConfig.curveExpo[YAW]      = 60.0f;
	rcControlsConfig.curveExpo[THROTTLE] = 0;
	rcControlsConfig.curveExpo[AUX1]     = 0;
	rcControlsConfig.curveExpo[AUX2]     = 0;
	rcControlsConfig.curveExpo[AUX3]     = 0;
	rcControlsConfig.curveExpo[AUX4]     = 0;

	rcControlsConfig.rates[YAW]          = 400.0;
	rcControlsConfig.rates[ROLL]         = 400.0;
	rcControlsConfig.rates[PITCH]        = 400.0;

	rcControlsConfig.acroPlus[YAW]       = 14.00;
	rcControlsConfig.acroPlus[ROLL]      = 14.00;
	rcControlsConfig.acroPlus[PITCH]     = 14.00;
}

