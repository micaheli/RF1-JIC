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


main_config mainConfig;

const config_variables_rec valueTable[] = {

		{ "mixer_type", typeUINT,  &mainConfig.mixerConfig.mixerType, 0, MIXER_END, MIXER_X1234 },

		{ "yaw_kp", typeFLOAT,  &mainConfig.pidConfig[YAW].kp, 0, 1, 0.00180000, "" },
		{ "yaw_ki", typeFLOAT,  &mainConfig.pidConfig[YAW].ki, 0, 1, 0.01000000, "" },
		{ "yaw_kd", typeFLOAT,  &mainConfig.pidConfig[YAW].kd, 0, 1, 0.00011400, "" },
		{ "yaw_wc", typeUINT,  &mainConfig.pidConfig[YAW].wc, 0, 1, 16, "" },
		{ "roll_kp", typeFLOAT,  &mainConfig.pidConfig[ROLL].kp, 0, 1, 0.00180000, "" },
		{ "roll_ki", typeFLOAT,  &mainConfig.pidConfig[ROLL].ki, 0, 1, 0.01000000, "" },
		{ "roll_kd", typeFLOAT,  &mainConfig.pidConfig[ROLL].kd, 0, 1, 0.00011400, "" },
		{ "roll_wc", typeUINT,  &mainConfig.pidConfig[ROLL].wc, 0, 1, 8, "" },
		{ "pitch_kp", typeFLOAT,  &mainConfig.pidConfig[PITCH].kp, 0, 1, 0.00180000, "" },
		{ "pitch_ki", typeFLOAT,  &mainConfig.pidConfig[PITCH].ki, 0, 1, 0.01000000, "" },
		{ "pitch_kd", typeFLOAT,  &mainConfig.pidConfig[PITCH].kd, 0, 1, 0.00011400, "" },
		{ "pitch_wc", typeUINT,  &mainConfig.pidConfig[PITCH].wc, 0, 1, 8, "" },

		{ "yaw_q", typeFLOAT,  &mainConfig.filterConfig[YAW].gyro.q, 0, 10, 0.00100, "" },
		{ "yaw_r", typeFLOAT,  &mainConfig.filterConfig[YAW].gyro.r, 0, 10, 3.00000, "" },
		{ "yaw_p", typeFLOAT,  &mainConfig.filterConfig[YAW].gyro.p, 0, 10, 0.00150, "" },
		{ "roll_q", typeFLOAT,  &mainConfig.filterConfig[ROLL].gyro.q, 0, 10, 0.00010, "" },
		{ "roll_r", typeFLOAT,  &mainConfig.filterConfig[ROLL].gyro.r, 0, 10, 0.10000, "" },
		{ "roll_p", typeFLOAT,  &mainConfig.filterConfig[ROLL].gyro.p, 0, 10, 0.00015, "" },
		{ "pitch_q", typeFLOAT,  &mainConfig.filterConfig[PITCH].gyro.q, 0, 10, 0.00010, "" },
		{ "pitch_r", typeFLOAT,  &mainConfig.filterConfig[PITCH].gyro.r, 0, 10, 0.10000, "" },
		{ "pitch_p", typeFLOAT,  &mainConfig.filterConfig[PITCH].gyro.p, 0, 10, 0.00015, "" },

		{ "yaw_kd_q", typeFLOAT,  &mainConfig.filterConfig[YAW].kd.q, 0, 10, 0, "" },
		{ "yaw_kd_r", typeFLOAT,  &mainConfig.filterConfig[YAW].kd.r, 0, 10, 0, "" },
		{ "yaw_kd_p", typeFLOAT,  &mainConfig.filterConfig[YAW].kd.p, 0, 10, 0, "" },
		{ "roll_kd_q", typeFLOAT,  &mainConfig.filterConfig[ROLL].kd.q, 0, 10, 0, "" },
		{ "roll_kd_r", typeFLOAT,  &mainConfig.filterConfig[ROLL].kd.r, 0, 10, 0, "" },
		{ "roll_kd_p", typeFLOAT,  &mainConfig.filterConfig[ROLL].kd.p, 0, 10, 0, "" },
		{ "pitch_kd_q", typeFLOAT,  &mainConfig.filterConfig[PITCH].kd.q, 0, 10, 0, "" },
		{ "pitch_kd_r", typeFLOAT,  &mainConfig.filterConfig[PITCH].kd.r, 0, 10, 0, "" },
		{ "pitch_kd_p", typeFLOAT,  &mainConfig.filterConfig[PITCH].kd.p, 0, 10, 0, "" },

		{ "sml_board_rot_x", typeUINT,  &mainConfig.gyroConfig.minorBoardRotation[X], 0, 10, 0, "" },
		{ "sml_board_rot_y", typeUINT,  &mainConfig.gyroConfig.minorBoardRotation[Y], 0, 10, 0, "" },
		{ "sml_board_rot_z", typeUINT,  &mainConfig.gyroConfig.minorBoardRotation[Z], 0, 10, 0, "" },

		{ "rf_loop_ctrl", typeUINT,  &mainConfig.gyroConfig.loopCtrl, 0, LOOP_UH32, LOOP_H8, "" },

		{ "pitch_deadband", typeFLOAT,  &mainConfig.rcControlsConfig.deadBand[PITCH], 0, 0.1, 0.02, "" },
		{ "roll_deadband", typeFLOAT,  &mainConfig.rcControlsConfig.deadBand[ROLL], 0, 0.1, 0.02, "" },
		{ "yaw_deadband", typeFLOAT,  &mainConfig.rcControlsConfig.deadBand[YAW], 0, 0.1, 0.02, "" },
		{ "throttle_deadband", typeFLOAT,  &mainConfig.rcControlsConfig.deadBand[THROTTLE], 0, 0.1, 0, "" },
		{ "aux1_deadband", typeFLOAT,  &mainConfig.rcControlsConfig.deadBand[AUX1], 0, 0.1, 0, "" },
		{ "aux2_deadband", typeFLOAT,  &mainConfig.rcControlsConfig.deadBand[AUX2], 0, 0.1, 0, "" },
		{ "aux3_deadband", typeFLOAT,  &mainConfig.rcControlsConfig.deadBand[AUX3], 0, 0.1, 0, "" },
		{ "aux4_deadband", typeFLOAT,  &mainConfig.rcControlsConfig.deadBand[AUX4], 0, 0.1, 0, "" },

		{ "pitch_midRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[PITCH], 0, 2048, 1024, "" },
		{ "roll_midRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[ROLL], 0, 2048, 1024, "" },
		{ "yaw_midRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[YAW], 0, 2048, 1024, "" },
		{ "throttle_midRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[THROTTLE], 0, 2048, 1024, "" },
		{ "aux1_midRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX1], 0, 2048, 1024, "" },
		{ "aux2_midRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX2], 0, 2048, 1024, "" },
		{ "aux3_midRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX3], 0, 2048, 1024, "" },
		{ "aux4_midRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX4], 0, 2048, 1024, "" },

		{ "pitch_minRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[PITCH], 0, 2048, 0, "" },
		{ "roll_minRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[ROLL], 0, 2048, 0, "" },
		{ "yaw_minRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[YAW], 0, 2048, 0, "" },
		{ "throttle_minRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[THROTTLE], 0, 2048, 0, "" },
		{ "aux1_minRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX1], 0, 2048, 0, "" },
		{ "aux2_minRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX2], 0, 2048, 0, "" },
		{ "aux3_minRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX3], 0, 2048, 0, "" },
		{ "aux4_minRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX4], 0, 2048, 0, "" },

		{ "pitch_maxRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[PITCH], 0, 2048, 2048, "" },
		{ "roll_maxRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[ROLL], 0, 2048, 2048, "" },
		{ "yaw_maxRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[YAW], 0, 2048, 2048, "" },
		{ "throttle_maxRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[THROTTLE], 0, 2048, 2048, "" },
		{ "aux1_maxRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX1], 0, 2048, 2048, "" },
		{ "aux2_maxRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX2], 0, 2048, 2048, "" },
		{ "aux3_maxRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX3], 0, 2048, 2048, "" },
		{ "aux4_maxRc", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX4], 0, 2048, 2048, "" },

		{ "pitch_curve", typeUINT,  &mainConfig.rcControlsConfig.midRc[PITCH], 0, EXPO_CURVE_END, SKITZO_EXPO, "" },
		{ "roll_curve", typeUINT,  &mainConfig.rcControlsConfig.midRc[ROLL], 0, EXPO_CURVE_END, SKITZO_EXPO, "" },
		{ "yaw_curve", typeUINT,  &mainConfig.rcControlsConfig.midRc[YAW], 0, EXPO_CURVE_END, SKITZO_EXPO, "" },
		{ "throttle_curve", typeUINT,  &mainConfig.rcControlsConfig.midRc[THROTTLE], 0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux1_curve", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX1], 0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux2_curve", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX2], 0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux3_curve", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX3], 0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux4_curve", typeUINT,  &mainConfig.rcControlsConfig.midRc[AUX4], 0, EXPO_CURVE_END, NO_EXPO, "" },

		{ "pitch_expo", typeFLOAT,  &mainConfig.rcControlsConfig.midRc[PITCH], 0, 100, 40, "" },
		{ "roll_expo", typeFLOAT,  &mainConfig.rcControlsConfig.midRc[ROLL], 0, 100, 40, "" },
		{ "yaw_expo", typeFLOAT,  &mainConfig.rcControlsConfig.midRc[YAW], 0, 100, 40, "" },
		{ "throttle_expo", typeFLOAT,  &mainConfig.rcControlsConfig.midRc[THROTTLE], 0, 100, 0, "" },
		{ "aux1_expo", typeFLOAT,  &mainConfig.rcControlsConfig.midRc[AUX1], 0, 100, 0, "" },
		{ "aux2_expo", typeFLOAT,  &mainConfig.rcControlsConfig.midRc[AUX2], 0, 100, 0, "" },
		{ "aux3_expo", typeFLOAT,  &mainConfig.rcControlsConfig.midRc[AUX3], 0, 100, 0, "" },
		{ "aux4_expo", typeFLOAT,  &mainConfig.rcControlsConfig.midRc[AUX4], 0, 100, 0, "" },

		{ "aux4_expo", typeFLOAT,  &mainConfig.rcControlsConfig.rates[PITCH] , 0, 1400, 400, "" },
		{ "aux4_expo", typeFLOAT,  &mainConfig.rcControlsConfig.rates[ROLL] , 0, 1400, 400, "" },
		{ "aux4_expo", typeFLOAT,  &mainConfig.rcControlsConfig.rates[YAW] , 0, 1400, 400, "" },

		{ "aux4_expo", typeFLOAT,  &mainConfig.rcControlsConfig.acroPlus[PITCH] , 0, 4, 2, "" },
		{ "aux4_expo", typeFLOAT,  &mainConfig.rcControlsConfig.acroPlus[ROLL] , 0, 4, 2, "" },
		{ "aux4_expo", typeFLOAT,  &mainConfig.rcControlsConfig.acroPlus[YAW] , 0, 4, 2, "" },

};


//test


//Dynamic config is bad idea if we are supporting f1, so instead we will go to a static sized structure
// basically add padding at end of structure to make it a fixed size.   Then always ad variables to the end, then when loading
// the structure, if your new version have new variables they will be zeroied and then just check in this function for zeroed functions and set default

void SaveConfig (uint32_t addresConfigStart)
{

	uint32_t addressOffset;

	mainConfig.version  = CONFIG_VERSION;
	mainConfig.size     = sizeof(main_config);
	mainConfig.czechsum = CalculateCzechsum((uint8_t *)(main_config *) &mainConfig, sizeof(main_config));

	EraseFlash(addresConfigStart, addresConfigStart+sizeof(main_config));
	PrepareFlash();
	for (addressOffset = 0; addressOffset < sizeof(main_config); addressOffset += 4) {
		WriteFlash(*(uint32_t *) ((char *) &mainConfig + addressOffset), addresConfigStart+addressOffset );
	}
	FinishFlash();
}

uint8_t CalculateCzechsum(uint8_t *data, uint32_t length)
{

	uint8_t czechsum = 0;
    const uint8_t *byteOffset;
    for (byteOffset = data; byteOffset < (data + length); byteOffset++)
    	czechsum ^= *byteOffset;
    return(czechsum);

}

int ValidateConfig (uint32_t addresConfigStart)
{

	const main_config *temp = (main_config *) addresConfigStart; //ADDRESS_FLASH_START;
	uint8_t czechsum = 0;

	if (temp->size != sizeof(main_config))
	    return (0);

	if (CONFIG_VERSION != temp->version)
		return (0);

	czechsum = CalculateCzechsum((uint8_t *) temp, sizeof(main_config));
	if (czechsum != 0)
	    return (0);

	return (1);

}

void LoadConfig (uint32_t addresConfigStart)
{
	if (ValidateConfig(addresConfigStart) ) {
		ResetConfig(addresConfigStart);
		//memcpy(&mainConfig, (char *) addresConfigStart, sizeof(main_config));
	} else {
		ResetConfig(addresConfigStart);
	}
}

void ResetConfig (uint32_t addresConfigStart)
{

	//todo: make each config resettable seprately
	float deadband = 0.02; //2.5% deadband
	uint32_t minRc = 0; // middle of rc input, one for each channel. Default being 1500
	uint32_t midRc = 1024; // middle of rc input, one for each channel. Default being 1500
	uint32_t maxRc = 2048; // middle of rc input, one for each channel. Default being 1500


	mainConfig.mixerConfig.mixerType = MIXER_X1234_REVERSE_YAW;
	//mainConfig.mixerConfig.mixerType = MIXER_X1234;

	mainConfig.gyroConfig.gyroRotation  = CW0; //airbot revolt
	//mainConfig.gyroConfig.gyroRotation  = CW270; //dji revolt and revo
	mainConfig.gyroConfig.boardRotation = CW0;

	mainConfig.pidConfig[YAW].kp	= .00180000;
	mainConfig.pidConfig[YAW].ki	= .01000000;
	mainConfig.pidConfig[YAW].kd	= .00011400;
	mainConfig.pidConfig[YAW].wc	=  16;

	mainConfig.pidConfig[ROLL].kp	= .00130000;
	mainConfig.pidConfig[ROLL].ki	= .01000000;
	mainConfig.pidConfig[ROLL].kd	= .00011400;
	mainConfig.pidConfig[ROLL].wc	=  8;

	mainConfig.pidConfig[PITCH].kp	= .00140000;
	mainConfig.pidConfig[PITCH].ki	= .01000000;
	mainConfig.pidConfig[PITCH].kd	= .00011400;
	mainConfig.pidConfig[PITCH].wc	=  8;


	mainConfig.filterConfig[YAW].gyro.q   = 0.00100;
	//mainConfig.filterConfig[YAW].gyro.r  = 0.001000;
	mainConfig.filterConfig[YAW].gyro.r   = 3.000000;
	//mainConfig.filterConfig[YAW].gyro.r   = 5.630000;
	mainConfig.filterConfig[YAW].gyro.p   = 0.00150;

	mainConfig.filterConfig[ROLL].gyro.q  = 0.00010;
	//mainConfig.filterConfig[ROLL].gyro.r  = 0.001000;
	mainConfig.filterConfig[ROLL].gyro.r  = 0.100000;
	//mainConfig.filterConfig[ROLL].gyro.r  = 3.550000;
	mainConfig.filterConfig[ROLL].gyro.p  = 0.00015;

	mainConfig.filterConfig[PITCH].gyro.q = 0.00010;
	//mainConfig.filterConfig[PITCH].gyro.r = 0.0.001000;
	mainConfig.filterConfig[PITCH].gyro.r = 0.100000;
	//mainConfig.filterConfig[PITCH].gyro.r = 3.550000;
	mainConfig.filterConfig[PITCH].gyro.p = 0.00015;

	mainConfig.filterConfig[YAW].kd.q     = 0.00010;
	mainConfig.filterConfig[YAW].kd.r     = 0.00300;
	mainConfig.filterConfig[YAW].kd.p     = 0.00015;

	mainConfig.filterConfig[ROLL].kd.q    = 0.00010;
	mainConfig.filterConfig[ROLL].kd.r    = 0.00300;
	mainConfig.filterConfig[ROLL].kd.p    = 0.00015;

	mainConfig.filterConfig[PITCH].kd.q   = 0.00010;
	mainConfig.filterConfig[PITCH].kd.r   = 0.00300;
	mainConfig.filterConfig[PITCH].kd.p   = 0.00015;


	mainConfig.gyroConfig.minorBoardRotation[X] = 0;
	mainConfig.gyroConfig.minorBoardRotation[Y] = 0;
	mainConfig.gyroConfig.minorBoardRotation[Z] = 0;


	mainConfig.gyroConfig.loopCtrl = LOOP_UH32;

	mainConfig.rcControlsConfig.deadBand[PITCH]    = deadband;
	mainConfig.rcControlsConfig.deadBand[ROLL]     = deadband;
	mainConfig.rcControlsConfig.deadBand[YAW]      = deadband;
	mainConfig.rcControlsConfig.deadBand[THROTTLE] = 0;
	mainConfig.rcControlsConfig.deadBand[AUX1]     = deadband;
	mainConfig.rcControlsConfig.deadBand[AUX2]     = deadband;
	mainConfig.rcControlsConfig.deadBand[AUX3]     = deadband;
	mainConfig.rcControlsConfig.deadBand[AUX4]     = deadband;

	mainConfig.rcControlsConfig.midRc[PITCH]    = midRc;
	mainConfig.rcControlsConfig.midRc[ROLL]     = midRc;
	mainConfig.rcControlsConfig.midRc[YAW]      = midRc;
	mainConfig.rcControlsConfig.midRc[THROTTLE] = midRc;
	mainConfig.rcControlsConfig.midRc[AUX1]     = midRc;
	mainConfig.rcControlsConfig.midRc[AUX2]     = midRc;
	mainConfig.rcControlsConfig.midRc[AUX3]     = midRc;
	mainConfig.rcControlsConfig.midRc[AUX4]     = midRc;

	//radio calibration data is held here. It save cycles and RAM to just use this directly.
	mainConfig.rcControlsConfig.minRc[PITCH]    = minRc;
	mainConfig.rcControlsConfig.minRc[ROLL]     = minRc;
	mainConfig.rcControlsConfig.minRc[YAW]      = minRc;
	mainConfig.rcControlsConfig.minRc[THROTTLE] = minRc;
	mainConfig.rcControlsConfig.minRc[AUX1]     = minRc;
	mainConfig.rcControlsConfig.minRc[AUX2]     = minRc;
	mainConfig.rcControlsConfig.minRc[AUX3]     = minRc;
	mainConfig.rcControlsConfig.minRc[AUX4]     = minRc;

	mainConfig.rcControlsConfig.maxRc[PITCH]    = maxRc;
	mainConfig.rcControlsConfig.maxRc[ROLL]     = maxRc;
	mainConfig.rcControlsConfig.maxRc[YAW]      = maxRc;
	mainConfig.rcControlsConfig.maxRc[THROTTLE] = maxRc;
	mainConfig.rcControlsConfig.maxRc[AUX1]     = maxRc;
	mainConfig.rcControlsConfig.maxRc[AUX2]     = maxRc;
	mainConfig.rcControlsConfig.maxRc[AUX3]     = maxRc;
	mainConfig.rcControlsConfig.maxRc[AUX4]     = maxRc;

	mainConfig.rcControlsConfig.useCurve[PITCH]     = SKITZO_EXPO;
	mainConfig.rcControlsConfig.useCurve[ROLL]      = SKITZO_EXPO;
	mainConfig.rcControlsConfig.useCurve[YAW]       = SKITZO_EXPO;
	mainConfig.rcControlsConfig.useCurve[THROTTLE]  = NO_EXPO;
	mainConfig.rcControlsConfig.useCurve[AUX1]      = NO_EXPO;
	mainConfig.rcControlsConfig.useCurve[AUX2]      = NO_EXPO;
	mainConfig.rcControlsConfig.useCurve[AUX3]      = NO_EXPO;
	mainConfig.rcControlsConfig.useCurve[AUX4]      = NO_EXPO;

	mainConfig.rcControlsConfig.curveExpo[PITCH]    = 40.0f;
	mainConfig.rcControlsConfig.curveExpo[ROLL]     = 40.0f;
	mainConfig.rcControlsConfig.curveExpo[YAW]      = 40.0f;
	mainConfig.rcControlsConfig.curveExpo[THROTTLE] = 0;
	mainConfig.rcControlsConfig.curveExpo[AUX1]     = 0;
	mainConfig.rcControlsConfig.curveExpo[AUX2]     = 0;
	mainConfig.rcControlsConfig.curveExpo[AUX3]     = 0;
	mainConfig.rcControlsConfig.curveExpo[AUX4]     = 0;

	mainConfig.rcControlsConfig.rates[YAW]          = 400.0;
	mainConfig.rcControlsConfig.rates[ROLL]         = 400.0;
	mainConfig.rcControlsConfig.rates[PITCH]        = 400.0;

	mainConfig.rcControlsConfig.acroPlus[YAW]       = 2.000;
	mainConfig.rcControlsConfig.acroPlus[ROLL]      = 2.000;
	mainConfig.rcControlsConfig.acroPlus[PITCH]     = 2.000;

	SaveConfig(addresConfigStart);
}

