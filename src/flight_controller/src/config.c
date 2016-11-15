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

#define RF_BUFFER_SIZE HID_EPIN_SIZE-1

// use variable record but instead of storing address of variable, store offset based on address of field, that way it works with the record loaded from file

main_config mainConfig;
//airbot revolt is CW0, dji revolt and revo is CW270

static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void EncodeBlock( unsigned char *in, unsigned char *out, int len )
{
   out[0] = (unsigned char) cb64[ (int)(in[0] >> 2) ];
   out[1] = (unsigned char) cb64[ (int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) ];
   out[2] = (unsigned char) (len > 1 ? cb64[ (int)(((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)) ] : '=');
   out[3] = (unsigned char) (len > 2 ? cb64[ (int)(in[2] & 0x3f) ] : '=');
}


const config_variables_rec valueTable[] = {

		{ "mixer_type", 		typeUINT,  "mixr", &mainConfig.mixerConfig.mixerType,				0, MIXER_END, MIXER_X1234, "" },
		{ "esc_protocol", 		typeUINT,  "mixr", &mainConfig.mixerConfig.escProtcol,				0, ESC_PROTOCOL_END, ESC_MULTISHOT, "" },
		{ "idle_percent", 		typeUINT,  "mixr", &mainConfig.mixerConfig.escUpdateFrequency,		0, 32000, 32000, "" },
		{ "idle_percent", 		typeFLOAT, "mixr", &mainConfig.mixerConfig.idlePercent,				0, 15.0, 7.0, "" },

		{ "gyro_rotation", 		typeUINT,  "gyro", &mainConfig.gyroConfig.gyroRotation,				0, 12, CW0, "" },
		{ "board_calibrated", 	typeUINT,  "gyro", &mainConfig.gyroConfig.boardCalibrated,			0, 1, 0, "" },
		{ "sml_board_rot_x", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[X],	0, 10, 0, "" },
		{ "sml_board_rot_y", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[Y],	0, 10, 0, "" },
		{ "sml_board_rot_z", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[Z], 	0, 10, 0, "" },
		{ "rf_loop_ctrl", 		typeUINT,  "gyro", &mainConfig.gyroConfig.loopCtrl, 				0, LOOP_UH32, LOOP_UH32, "" },

		{ "yaw_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].kp, 					0, 300, 165.00, "" }, //1000 18
		{ "yaw_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].ki, 					0, 300, 180.00, "" }, //1000 14
		{ "yaw_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].kd, 					0, 300, 666.00, "" }, //1000000 114
		{ "yaw_wc", 			typeUINT,  "pids", &mainConfig.pidConfig[YAW].wc, 					0, 300, 4, "" },
		{ "roll_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].kp, 					0, 300, 180.00, "" },
		{ "roll_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].ki, 					0, 300, 220.00, "" },
		{ "roll_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].kd, 					0, 300, 777.00, "" },
		{ "roll_wc", 			typeUINT,  "pids", &mainConfig.pidConfig[ROLL].wc, 					0, 300, 4, "" },
		{ "pitch_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].kp, 				0, 300, 220.00, "" },
		{ "pitch_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].ki, 				0, 300, 220.00, "" },
		{ "pitch_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].kd, 				0, 300, 888.00, "" },
		{ "pitch_wc", 			typeUINT,  "pids", &mainConfig.pidConfig[PITCH].wc, 				0, 300, 4, "" },

		{ "yaw_quick", 			typeFLOAT, "filt", &mainConfig.filterConfig[YAW].gyro.q, 			0, 10, 0.00100, "" },
		{ "yaw_rap", 			typeFLOAT, "filt", &mainConfig.filterConfig[YAW].gyro.r, 			0, 10, 3.00000, "" },
		{ "yaw_press", 			typeFLOAT, "filt", &mainConfig.filterConfig[YAW].gyro.p, 			0, 10, 0.00150, "" },
		{ "roll_quick", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].gyro.q, 			0, 10, 0.00100, "" },
		{ "roll_rap", 			typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].gyro.r, 			0, 10, 3.00000, "" },
		{ "roll_press", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].gyro.p, 			0, 10, 0.00150, "" },
		{ "pitch_quick", 		typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].gyro.q, 			0, 10, 0.00100, "" },
		{ "pitch_rap", 			typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].gyro.r, 			0, 10, 3.00000, "" },
		{ "pitch_press", 		typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].gyro.p, 			0, 10, 0.00150, "" },

		{ "yaw_kd_quick", 		typeFLOAT, "filt", &mainConfig.filterConfig[YAW].kd.q, 				0, 10, 0.00100, "" },
		{ "yaw_kd_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[YAW].kd.r, 				0, 10, 2.00000, "" },
		{ "yaw_kd_press", 		typeFLOAT, "filt", &mainConfig.filterConfig[YAW].kd.p, 				0, 10, 0.00150, "" },
		{ "roll_kd_quick", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].kd.q, 			0, 10, 0.00100, "" },
		{ "roll_kd_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].kd.r, 			0, 10, 2.00000, "" },
		{ "roll_kd_press", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].kd.p, 			0, 10, 0.00150, "" },
		{ "pitch_kd_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].kd.q, 			0, 10, 0.00010, "" },
		{ "pitch_kd_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].kd.r, 			0, 10, 2.00000, "" },
		{ "pitch_kd_press", 	typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].kd.p, 			0, 10, 0.00150, "" },

		{ "yaw_kd_lpf", 		typeFLOAT, "filt", &mainConfig.filterConfig[YAW].kdBq.lpfHz, 		0, 10, 0.0000, "" },
		{ "roll_kd_lpf", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].kdBq.lpfHz, 		0, 10, 0.0000, "" },
		{ "pitch_kd_lpf", 		typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].kdBq.lpfHz, 		0, 10, 0.0000, "" },

		{ "x_vector_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCX].acc.q, 			0, 10, 0.00100, "" },
		{ "x_vector_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ACCX].acc.r, 			0, 10, 0.50000, "" },
		{ "x_vector_press", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCX].acc.p, 			0, 10, 0.00150, "" },
		{ "y_vector_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCY].acc.q, 			0, 10, 0.00100, "" },
		{ "y_vector_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ACCY].acc.r, 			0, 10, 0.50000, "" },
		{ "y_vector_press", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCY].acc.p, 			0, 10, 0.00150, "" },
		{ "z_vector_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCZ].acc.q, 			0, 10, 0.00100, "" },
		{ "z_vector_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ACCZ].acc.r, 			0, 10, 0.50000, "" },
		{ "z_vector_press", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCZ].acc.p, 			0, 10, 0.00150, "" },

		{ "pitch_deadband", 	typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[PITCH], 	0, 0.1, 0.01, "" },
		{ "roll_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[ROLL], 	0, 0.1, 0.01, "" },
		{ "yaw_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[YAW], 		0, 0.1, 0.01, "" },
		{ "throttle_deadband", 	typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[THROTTLE], 0, 0.1, 0, "" },
		{ "aux1_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX1], 	0, 0.1, 0, "" },
		{ "aux2_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX2], 	0, 0.1, 0, "" },
		{ "aux3_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX3], 	0, 0.1, 0, "" },
		{ "aux4_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX4], 	0, 0.1, 0, "" },

		{ "pitch_midRc",		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[PITCH], 		0, 2148, 1024, "" },
		{ "roll_midRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[ROLL], 		0, 2148, 1024, "" },
		{ "yaw_midRc", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[YAW], 		0, 2148, 1024, "" },
		{ "throttle_midRc", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[THROTTLE],	0, 2148, 1024, "" },
		{ "aux1_midRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX1], 		0, 2148, 1024, "" },
		{ "aux2_midRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX2], 		0, 2148, 1024, "" },
		{ "aux3_midRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX3], 		0, 2148, 1024, "" },
		{ "aux4_midRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX4], 		0, 2148, 1024, "" },

		{ "pitch_minRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[PITCH], 		0, 2148, 0, "" },
		{ "roll_minRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[ROLL], 		0, 2148, 0, "" },
		{ "yaw_minRc", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[YAW], 		0, 2148, 2048, "" },
		{ "throttle_minRc", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[THROTTLE], 	0, 2148, 0, "" },
		{ "aux1_minRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX1], 		0, 2148, 0, "" },
		{ "aux2_minRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX2], 		0, 2148, 0, "" },
		{ "aux3_minRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX3], 		0, 2148, 0, "" },
		{ "aux4_minRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX4], 		0, 2148, 0, "" },

		{ "pitch_maxRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[PITCH], 		0, 2148, 2048, "" },
		{ "roll_maxRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[ROLL], 		0, 2148, 2048, "" },
		{ "yaw_maxRc", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[YAW], 		0, 2148, 0, "" },
		{ "throttle_maxRc", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[THROTTLE], 	0, 2148, 2048, "" },
		{ "aux1_maxRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX1], 		0, 2148, 2048, "" },
		{ "aux2_maxRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX2], 		0, 2148, 2048, "" },
		{ "aux3_maxRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX3], 		0, 2148, 2048, "" },
		{ "aux4_maxRc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX4], 		0, 2148, 2048, "" },

		{ "pitch_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[PITCH], 	0, EXPO_CURVE_END, SKITZO_EXPO, "" },
		{ "roll_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[ROLL], 	0, EXPO_CURVE_END, SKITZO_EXPO, "" },
		{ "yaw_curve", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[YAW], 		0, EXPO_CURVE_END, SKITZO_EXPO, "" },
		{ "throttle_curve", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[THROTTLE], 0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux1_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX1], 	0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux2_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX2], 	0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux3_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX3], 	0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux4_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX4], 	0, EXPO_CURVE_END, NO_EXPO, "" },

		{ "pitch_expo", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[PITCH],	0, 100, 50, "" },
		{ "roll_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[ROLL],	0, 100, 50, "" },
		{ "yaw_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[YAW],		0, 100, 50, "" },
		{ "throttle_expo", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[THROTTLE],0, 100, 0, "" },
		{ "aux1_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[AUX1],	0, 100, 0, "" },
		{ "aux2_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[AUX2],	0, 100, 0, "" },
		{ "aux3_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[AUX3],	0, 100, 0, "" },
		{ "aux4_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[AUX4],	0, 100, 0, "" },

		{ "pitch_rate", 		typeFLOAT, "rate", &mainConfig.rcControlsConfig.rates[PITCH],		0, 1400, 400, "" },
		{ "roll_rate", 			typeFLOAT, "rate", &mainConfig.rcControlsConfig.rates[ROLL],		0, 1400, 400, "" },
		{ "yaw_rate", 			typeFLOAT, "rate", &mainConfig.rcControlsConfig.rates[YAW],			0, 1400, 400, "" },

		{ "pitch_acrop", 		typeFLOAT, "rate", &mainConfig.rcControlsConfig.acroPlus[PITCH],	0, 4, 1.4, "" },
		{ "roll_acrop", 		typeFLOAT, "rate", &mainConfig.rcControlsConfig.acroPlus[ROLL],		0, 4, 1.4, "" },
		{ "yaw_acrop", 			typeFLOAT, "rate", &mainConfig.rcControlsConfig.acroPlus[YAW],		0, 4, 1.4, "" }

};

char rf_custom_out_buffer[RF_BUFFER_SIZE];

//test


//Dynamic config is bad idea if we are supporting f1, so instead we will go to a static sized structure
// basically add padding at end of structure to make it a fixed size.   Then always ad variables to the end, then when loading
// the structure, if your new version have new variables they will be zeroied and then just check in this function for zeroed functions and set default


//TODO REwrite this better
char *ftoa(float x, char *floatString)
{
    int32_t value;
    char intString1[12];
    char intString2[12] = { 0, };
    char *decimalPoint = ".";
    uint8_t dpLocation;

    if (x > 0)
        x += 0.0005f;
    else
        x -= 0.0005f;

    value = (int32_t)(x * 1000.0f);

    itoa(ABS(value), intString1, 10);

    if (value >= 0)
        intString2[0] = ' ';
    else
        intString2[0] = '-';

    if (strlen(intString1) == 1) {
        intString2[1] = '0';
        intString2[2] = '0';
        intString2[3] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 2) {
        intString2[1] = '0';
        intString2[2] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 3) {
        intString2[1] = '0';
        strcat(intString2, intString1);
    } else {
        strcat(intString2, intString1);
    }

    dpLocation = strlen(intString2) - 3;

    strncpy(floatString, intString2, dpLocation);
    floatString[dpLocation] = '\0';
    strcat(floatString, decimalPoint);
    strcat(floatString, intString2 + dpLocation);

    return(floatString);
}


void GenerateConfig(void)
{
	uint32_t x;

	bzero(&mainConfig, sizeof(mainConfig));

	for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
	{
		switch(valueTable[x].type)
		{
		case typeUINT:
			*(uint32_t *)valueTable[x].ptr = (uint32_t)valueTable[x].Default;
			break;

		case typeINT:
			*(int32_t *)valueTable[x].ptr = (int32_t)valueTable[x].Default;
			break;

		case typeFLOAT:
			*(float *)valueTable[x].ptr = (float)valueTable[x].Default;
			break;
/*
		case typeSTRING:
			break;
*/

		}

	}


}

void SaveConfig (uint32_t addresConfigStart)
{

	uint32_t addressOffset;

	mainConfig.version  = CONFIG_VERSION;
	mainConfig.size     = sizeof(main_config);
	mainConfig.czechsum = CalculateCzechsum((const uint8_t *) &mainConfig, sizeof(main_config));

	InitWatchdog(WATCHDOG_TIMEOUT_16S);
	EraseFlash(addresConfigStart, addresConfigStart+sizeof(main_config));
	PrepareFlash();
	for (addressOffset = 0; addressOffset < sizeof(main_config); addressOffset += 4) {
		WriteFlash(*(uint32_t *) ((char *) &mainConfig + addressOffset), addresConfigStart+addressOffset );
	}
	FinishFlash();
	InitWatchdog(WATCHDOG_TIMEOUT_2S);
}

uint8_t CalculateCzechsum(const uint8_t *data, uint32_t length)
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

	czechsum = CalculateCzechsum((const uint8_t *) temp, sizeof(main_config));
	if (czechsum != 0)
		return (1);
	    //todo: fix stupid checksum return (0);

	return (1);

}

void LoadConfig (uint32_t addresConfigStart)
{
	if (ValidateConfig(addresConfigStart) ) {
		memcpy(&mainConfig, (char *) addresConfigStart, sizeof(main_config));
	} else {
		GenerateConfig();
		SaveConfig(addresConfigStart);
	}
}




//cleanup string // strip continuous spaces, first space, and non allowed characters
char *StripSpaces(char *inString)
{
	uint16_t head = 0;
	uint16_t position = 0;
	uint8_t inQuote = 0;
	uint16_t inStringLength = strlen(inString);

	for (position = 0; position < inStringLength; position++)
	{
		if (inString[position] == '"')
			inQuote = inQuote ^ 1;

		if ((inQuote) || (inString[position] != ' '))
			inString[head++] = inString[position];
	}


	inString[head] = 0;

	return (inString);
}

char *CleanupString(char *inString)
{
	char last_char = ' ';
	uint16_t head = 0;
	uint16_t position = 0;
	uint16_t inStringLength = strlen(inString);

	for (position = 0; position < inStringLength; position++)
	{
		if ((last_char == ' ') && (inString[position] == ' ')) // removes multiple spaces in a row
			continue;

		if (isalnum((unsigned char)inString[position]) || (inString[position] == ' ') || (inString[position] == '=') || (inString[position] == '"') || (inString[position] == '.') || (inString[position] == '-') || (inString[position] == '_'))
		{
			inString[head++] = inString[position];
			last_char = inString[position];
		}
	}


	inString[head] = 0;

	return (inString);
}





void SetValue(uint32_t position, char *value)
{
	switch (valueTable[position].type) {
	//TODO used something better then atoi
	case typeUINT:
	case typeINT:
		*(uint32_t *)valueTable[position].ptr = atoi(value);
		break;

	case typeFLOAT:
		*(float *)valueTable[position].ptr = atof(value);
		break;
	}


}

void SetVariable(char *inString) {
	uint32_t x;
	uint32_t inStringLength;
	char *args = NULL;
	StripSpaces(inString);

	inStringLength = strlen(inString);

	for (x = 0; x < inStringLength; x++) {
		if (inString[x] == '=')
			break;
	}

	if (inStringLength > x) {
		args = inString + x + 1;
	}

	inString[x] = 0;

	for (x = 0; x < strlen(inString); x++)
		inString[x] = tolower((unsigned char)inString[x]);

	if (!strcmp("board_calibrated", inString)) {
		memcpy(rf_custom_out_buffer, "board_calibrated cannot be set manually. Please run the board calibration routine.\0", sizeof("board_calibrated cannot be set manually. Please run the board calibration routine.\0"));
		RfCustomReply(rf_custom_out_buffer);
		return;
	}


	for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
	{
		if (!strcmp(valueTable[x].name, inString))
		{
			SetValue(x, args);
			autoSaveTimer = InlineMillis();
			if ( (!strcmp(valueTable[x].group, "mixr")) || (!strcmp(valueTable[x].group, "gyro")) || (!strcmp(valueTable[x].group, "filt"))  || (!strcmp(valueTable[x].group, "rccf")) ) {
				InitRcData();
			    InitMixer();
			    InitFlightCode();
			    InitPid();
			    InitActuators();
			}
		}
	}
}




/**********************************************************************************************************/
void OutputVar(uint32_t position)
{
	char fString[20];

	bzero(rf_custom_out_buffer, RF_BUFFER_SIZE);
	switch (valueTable[position].type) {

	case typeUINT:
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "%s=%d\n", valueTable[position].name, (int)*(uint32_t *)valueTable[position].ptr);
		break;


	case typeINT:
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "%s=%d\n", valueTable[position].name, (int)*(int32_t *)valueTable[position].ptr);
		break;


	case typeFLOAT:
		ftoa(*(float *)valueTable[position].ptr, fString);
		StripSpaces(fString);
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "%s=%s\n", valueTable[position].name, fString);
		break;
	}

	RfCustomReply(rf_custom_out_buffer);
}
/**********************************************************************************************************************/


int RfCustomReply(char *rf_custom_out_buffer) {

	uint32_t forCounter;

	unsigned char rfReplyBuffer[RF_BUFFER_SIZE];

	bzero((rfReplyBuffer+1), (sizeof(rfReplyBuffer)-1));

	rfReplyBuffer[0]=1;
	memcpy((char *)(rfReplyBuffer+1), rf_custom_out_buffer, RF_BUFFER_SIZE);

	for (forCounter = 0; forCounter < 40000; forCounter++) {
		if (hidToPcReady) {
			USBD_HID_SendReport (&hUsbDeviceFS, rfReplyBuffer, HID_EPIN_SIZE);
			hidToPcReady = 0;
			return(1);
		}
	}
	return(0);
}

void ProcessCommand(char *inString)
{
	//buffer_record *buffer = &flashInfo.buffer[flashInfo.bufferNum];
	uint32_t inStringLength;
	char *args = NULL;
	uint32_t x;

	inString = CleanupString(inString);

	inStringLength = strlen(inString);

	for (x = 0; x < inStringLength; x++) {
		if (inString[x] == ' ')
			break;
	}

	if (inStringLength > x) {
		args = inString + x + 1;
	}

	inString[x] = 0;

	for (x = 0; x < strlen(inString); x++)
		inString[x] = tolower((unsigned char)inString[x]);

	if (!strcmp("set", inString))
		{
			SetVariable(args);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "variableset", sizeof("variableset"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, args, inStringLength);
			RfCustomReply(rf_custom_out_buffer);

		}
	else if (!strcmp("rcdata", inString))
		{
			for (uint32_t xx = 0; xx < MAXCHANNELS;xx++) {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (volatile unsigned int)(rxData[xx]));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("calibratem1", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "calibratingmotors", sizeof("calibratingmotors"));
			RfCustomReply(rf_custom_out_buffer);

			calibrateMotors = 1;
			SKIP_GYRO=1;
			motorOutput[0] = 1;
			motorOutput[1] = 1;
			motorOutput[2] = 1;
			motorOutput[3] = 1;
			motorOutput[4] = 1;
			motorOutput[5] = 1;
			motorOutput[6] = 1;
			motorOutput[7] = 1;
			motorOutput[8] = 1;
			OutputActuators(motorOutput, servoOutput);
		}
	else if (!strcmp("calibratem2", inString))
		{
			motorOutput[0] = 0;
			motorOutput[1] = 0;
			motorOutput[2] = 0;
			motorOutput[3] = 0;
			motorOutput[4] = 0;
			motorOutput[5] = 0;
			motorOutput[6] = 0;
			motorOutput[7] = 0;
			motorOutput[8] = 0;
			OutputActuators(motorOutput, servoOutput);
			InitWatchdog(WATCHDOG_TIMEOUT_16S);
			FeedTheDog();
			DelayMs(5000);
			FeedTheDog();
			InitWatchdog(WATCHDOG_TIMEOUT_2S);
			SKIP_GYRO=0;
			calibrateMotors = 0;

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "motorscalibrated", sizeof("motorscalibrated"));
			RfCustomReply(rf_custom_out_buffer);

		}
	else if (!strcmp("calibratema", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "calibratingmotors", sizeof("calibratingmotors"));
			RfCustomReply(rf_custom_out_buffer);

			calibrateMotors = 1;
			SKIP_GYRO=1;
			boardArmed=0;
			motorOutput[0] = 1.0;
			motorOutput[1] = 1.0;
			motorOutput[2] = 1.0;
			motorOutput[3] = 1.0;
			motorOutput[4] = 1.0;
			motorOutput[5] = 1.0;
			motorOutput[6] = 1.0;
			motorOutput[7] = 1.0;
			motorOutput[8] = 1.0;
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (volatile unsigned int)(motorOutput[0] * 1000));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (volatile unsigned int)(motorOutput[1] * 1000));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (volatile unsigned int)(motorOutput[2] * 1000));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (volatile unsigned int)(motorOutput[3] * 1000));
			RfCustomReply(rf_custom_out_buffer);
			OutputActuators(motorOutput, servoOutput);
			InitWatchdog(WATCHDOG_TIMEOUT_16S);
			FeedTheDog();
			DelayMs(14000);
			FeedTheDog();
			motorOutput[0] = 0;
			motorOutput[1] = 0;
			motorOutput[2] = 0;
			motorOutput[3] = 0;
			motorOutput[4] = 0;
			motorOutput[5] = 0;
			motorOutput[6] = 0;
			motorOutput[7] = 0;
			motorOutput[8] = 0;
			OutputActuators(motorOutput, servoOutput);
			FeedTheDog();
			DelayMs(5000);
			FeedTheDog();
			InitWatchdog(WATCHDOG_TIMEOUT_2S);
			SKIP_GYRO=0;
			calibrateMotors = 0;

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "motorscalibrated", sizeof("motorscalibrated"));
			RfCustomReply(rf_custom_out_buffer);

		}
	else if (!strcmp("buzzer", inString))
		{
			args = StripSpaces(args);
			if (sizeof(atoi(args)) == sizeof(uint32_t)) {
				buzzerStatus.status = atoi(args);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "buzzer goes BUZZZZZZ!\0", sizeof("buzzer goes BUZZZZZZ!\0"));
				RfCustomReply(rf_custom_out_buffer);
			} else {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "buzzer needs a number to go BUZZZZZZ!\0", sizeof("buzzer needs a number to go BUZZZZZZ!\0"));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("dump", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "dumpstarted", sizeof("dumpstarted"));
			RfCustomReply(rf_custom_out_buffer);

			args = StripSpaces(args);
			if ( (!strcmp("", args)) || (!strcmp("all", args)) ) {
				for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
				{
					OutputVar(x);
				}
			} else {
				for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
				{
					if (!strcmp(valueTable[x].group, args))
						OutputVar(x);
				}
			}

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "dumpcomplete", sizeof("dumpcomplete"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("eraseflash", inString))
		{
			if (flashInfo.enabled) {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "erasingflash", sizeof("erasingflash"));
				RfCustomReply(rf_custom_out_buffer);

				if (((float)(flashInfo.currentWriteAddress * flashInfo.pageSize * flashInfo.pagesPerSector) / (float)flashInfo.totalSize) > 0.85) {
					if (MassEraseDataFlash(1)) {
						bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
						memcpy(rf_custom_out_buffer, "flasherasecomplete", sizeof("flasherasecomplete"));
						RfCustomReply(rf_custom_out_buffer);
					} else {
						bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
						memcpy(rf_custom_out_buffer, "flasherasefailed", sizeof("flasherasefailed"));
						RfCustomReply(rf_custom_out_buffer);
					}
				} else {
					if (MassEraseDataFlashByPage(1)) {
						bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
						memcpy(rf_custom_out_buffer, "flasherasecomplete", sizeof("flasherasecomplete"));
						RfCustomReply(rf_custom_out_buffer);
					}
				}

			} else {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected", sizeof("flashchipnotdetected"));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("startlog", inString))
		{
			if (flashInfo.enabled) {
				curvedRcCommandF[AUX2] = -0.6;
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "loggingstarted", sizeof("loggingstarted"));
				RfCustomReply(rf_custom_out_buffer);

			} else {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected", sizeof("flashchipnotdetected"));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("endlog", inString))
		{
			if (flashInfo.enabled) {
				curvedRcCommandF[AUX2] = 0.6;
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "loggingended", sizeof("loggingended"));
				RfCustomReply(rf_custom_out_buffer);

			} else {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected", sizeof("flashchipnotdetected"));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("dlflsize", inString))
		{


			if (flashInfo.enabled) {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (unsigned int)(flashInfo.currentWriteAddress));
				RfCustomReply(rf_custom_out_buffer);

			} else {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (unsigned int)0);
				RfCustomReply(rf_custom_out_buffer);

			}
		}
	else if ( (!strcmp("downloadflightlog", inString)) || (!strcmp("dlb", inString)) || (!strcmp("dlfl", inString)) )
		{
			int base64Encode = 0;

			args = StripSpaces(args);
			if (!strcmp("b64", args))
				base64Encode = 1;

			if (!strcmp("dlb", inString))
				base64Encode = 1;

			if (flashInfo.enabled) {

				if(flashInfo.currentWriteAddress < 256) {

					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					memcpy(rf_custom_out_buffer, "flightlogempty", sizeof("flightlogempty"));
					RfCustomReply(rf_custom_out_buffer);

				} else {

					uint8_t dataArray[45];
					uint32_t smallerPointer = 0;
					uint32_t pagesToSend = ((flashInfo.currentWriteAddress) / flashInfo.pageSize);

					for (uint32_t y = 0;y<pagesToSend;y++) {

						if ( M25p16ReadPage( (y * flashInfo.pageSize), flashInfo.buffer[0].txBuffer, flashInfo.buffer[0].rxBuffer) ) {

							for (uint32_t x=0;x<256;x++) {

								if (base64Encode) {
									dataArray[smallerPointer++] = flashInfo.buffer[0].rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+x];
									if (smallerPointer > 44) {

										//base64 encode
										for (smallerPointer = 0; smallerPointer < 15; smallerPointer++) {
											EncodeBlock( (unsigned char *)( dataArray + (smallerPointer*3) ), (unsigned char *)( rf_custom_out_buffer + (smallerPointer*4) ), 4);
										}

										RfCustomReply(rf_custom_out_buffer);
										smallerPointer = 0;
										bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
										bzero(dataArray,sizeof(dataArray));
									}

								} else {

									rf_custom_out_buffer[smallerPointer++] = flashInfo.buffer[0].rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+x];
									if (smallerPointer > 62) {
										RfCustomReply(rf_custom_out_buffer);
										smallerPointer = 0;
										bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
									}

								}

							}

						} else {

							bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
							memcpy(rf_custom_out_buffer, "flashreadfailed", sizeof("flashreadfailed"));
							RfCustomReply(rf_custom_out_buffer);

						}

					}

				}
			} else {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "downloadflightlogstarted", sizeof("downloadflightlogstarted"));
				RfCustomReply(rf_custom_out_buffer);

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (unsigned int)378);
				RfCustomReply(rf_custom_out_buffer);

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=45;
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=11;
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=22;
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=55;
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=33;
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=21;
				RfCustomReply(rf_custom_out_buffer);

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "downloadflightlogfinished", sizeof("downloadflightlogfinished"));
				RfCustomReply(rf_custom_out_buffer);

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected", sizeof("flashchipnotdetected"));
				RfCustomReply(rf_custom_out_buffer);

			}

		}
	else if (!strcmp("readflash", inString))
		{
			if (flashInfo.enabled) {

				args = StripSpaces(args);

				if ( M25p16ReadPage( atoi(args), flashInfo.buffer[0].txBuffer, flashInfo.buffer[0].rxBuffer) ) {

					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					memcpy(rf_custom_out_buffer, &flashInfo.buffer[0].rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+0], 32);
					RfCustomReply(rf_custom_out_buffer);

				} else {

					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					memcpy(rf_custom_out_buffer, "flashreadfailed", sizeof("flashreadfailed"));
					RfCustomReply(rf_custom_out_buffer);

				}

			} else {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected", sizeof("flashchipnotdetected"));
				RfCustomReply(rf_custom_out_buffer);

			}

		}
	else if (!strcmp("version", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, FULL_VERSION_STRING "\0", sizeof(FULL_VERSION_STRING "\0"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("calibratea", inString))
		{
			mainConfig.gyroConfig.boardCalibrated = 0;
			mainConfig.gyroConfig.gyroRotation = CW0;
			DelayMs(200); //need to reset calibration and give ACC data time to refresh
			if (SetCalibrate1()) {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "calibrate1finished", sizeof("calibrate1finished"));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("calibrateb", inString))
		{
			if (SetCalibrate2()) {
				SaveConfig(ADDRESS_CONFIG_START);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "calibrate2finished", sizeof("calibrate2finished"));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("save", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving", sizeof("saving"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete", sizeof("savecomplete"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("reboot", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "rebooting", sizeof("rebooting"));
			RfCustomReply(rf_custom_out_buffer);
			SystemReset();
		}
	else if (!strcmp("resetdfu", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "rebootingdfu", sizeof("rebootingdfu"));
			RfCustomReply(rf_custom_out_buffer);
			SystemResetToDfuBootloader();
		}
	else if (!strcmp("resetconfig", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "resettingconfig", sizeof("resettingconfig"));
			RfCustomReply(rf_custom_out_buffer);
			GenerateConfig();
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "configreset", sizeof("configreset"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("bind", inString))
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM9);
			rtc_write_backup_reg(FC_STATUS_REG,BOOT_TO_SPEKTRUM9);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "binding9", sizeof("binding9"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("bind9", inString))
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM9);
			rtc_write_backup_reg(FC_STATUS_REG,BOOT_TO_SPEKTRUM9);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "binding9", sizeof("binding9"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("bind5", inString))
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM5);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "binding5", sizeof("binding5"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("rebootrfbl", inString))
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_RFBL_COMMAND);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "rebootrfbl", sizeof("rebootrfbl"));
			RfCustomReply(rf_custom_out_buffer);
			SystemReset();
		}
	else if (!strcmp("rebootrecovery", inString))
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_RECOVERY_COMMAND);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "rebootrecovery", sizeof("rebootrecovery"));
			RfCustomReply(rf_custom_out_buffer);
			SystemReset();
		}
	else
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, inString, RF_BUFFER_SIZE);
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "unknowncommand", sizeof("unknowncommand"));
			RfCustomReply(rf_custom_out_buffer);
		}
/*
	else if (!strcmp("1wire", inString))
	{
		rfCustom1Wire(args);
	}
	else if (!strcmp("rfblbind", inString))
	{
		rfCustomRfblBind(args);
	}
*/


}





