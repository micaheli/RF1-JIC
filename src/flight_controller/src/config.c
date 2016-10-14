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

#define RF_BUFFER_SIZE 63

// use variable record but instead of storing address of variable, store offset based on address of field, that way it works with the record loaded from file

main_config mainConfig;
//airbot revolt is CW0, dji revolt and revo is CW270

const config_variables_rec valueTable[] = {

		{ "mixer_type", typeUINT,  &mainConfig.mixerConfig.mixerType, 0, MIXER_END, MIXER_X1234, "" },

		{ "gyro_rotation", typeUINT,  &mainConfig.gyroConfig.gyroRotation, 0, 12, CW0, "" },
		{ "board_rotation", typeUINT,  &mainConfig.gyroConfig.boardRotation, 0, 12, CW0, "" },
		{ "sml_board_rot_x", typeUINT,  &mainConfig.gyroConfig.minorBoardRotation[X], 0, 10, 0, "" },
		{ "sml_board_rot_y", typeUINT,  &mainConfig.gyroConfig.minorBoardRotation[Y], 0, 10, 0, "" },
		{ "sml_board_rot_z", typeUINT,  &mainConfig.gyroConfig.minorBoardRotation[Z], 0, 10, 0, "" },
		{ "rf_loop_ctrl", typeUINT,  &mainConfig.gyroConfig.loopCtrl, 0, LOOP_UH32, LOOP_H8, "" },

		{ "yaw_kp", typeFLOAT,  &mainConfig.pidConfig[YAW].kp, 0, 1, 0.00181111, "" },
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

		{ "pitch_rate", typeFLOAT,  &mainConfig.rcControlsConfig.rates[PITCH] , 0, 1400, 400, "" },
		{ "roll_rate", typeFLOAT,  &mainConfig.rcControlsConfig.rates[ROLL] , 0, 1400, 400, "" },
		{ "yaw_rate", typeFLOAT,  &mainConfig.rcControlsConfig.rates[YAW] , 0, 1400, 400, "" },

		{ "pitch_acrop", typeFLOAT,  &mainConfig.rcControlsConfig.acroPlus[PITCH] , 0, 4, 2, "" },
		{ "roll_acrop", typeFLOAT,  &mainConfig.rcControlsConfig.acroPlus[ROLL] , 0, 4, 2, "" },
		{ "yaw_acrop", typeFLOAT,  &mainConfig.rcControlsConfig.acroPlus[YAW] , 0, 4, 2, "" }

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
		case typeINT:
			*(uint32_t *)valueTable[x].ptr = (uint32_t)valueTable[x].Default;
			break;

		case typeUINT:
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
	mainConfig.czechsum = CalculateCzechsum((uint8_t *)(main_config *) &mainConfig, sizeof(main_config));

	InitWatchdog(WATCHDOG_TIMEOUT_16S);
	EraseFlash(addresConfigStart, addresConfigStart+sizeof(main_config));
	PrepareFlash();
	for (addressOffset = 0; addressOffset < sizeof(main_config); addressOffset += 4) {
		WriteFlash(*(uint32_t *) ((char *) &mainConfig + addressOffset), addresConfigStart+addressOffset );
	}
	FinishFlash();
	InitWatchdog(WATCHDOG_TIMEOUT_1S);
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

	for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
	{
		if (!strcmp(valueTable[x].name, inString))
		{
			SetValue(x, args);

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


void RfCustomReply(char *rf_custom_out_buffer) {

	uint32_t forCounter;

	unsigned char rfReplyBuffer[RF_BUFFER_SIZE];

	bzero(rfReplyBuffer, sizeof(rfReplyBuffer));

	rfReplyBuffer[0]=1;
	memcpy((char *)(rfReplyBuffer+1), rf_custom_out_buffer, RF_BUFFER_SIZE);
	//volatile uint32_t cat = &hUsbDeviceFS.dev_state;
	//volatile uint32_t dog = &hUsbDeviceFS.pClass->EP0_RxReady;

	for (forCounter = 0; forCounter < 40000; forCounter++) {
		if (hidToPcReady)
			break;
	}
	hidToPcReady = 0;
	USBD_HID_SendReport (&hUsbDeviceFS, rfReplyBuffer, HID_EPIN_SIZE);

}

void ProcessCommand(char *inString)
{
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
	else if (!strcmp("dump", inString))
	{
		bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
		memcpy(rf_custom_out_buffer, "dumpstarted", sizeof("dumpstarted"));
		RfCustomReply(rf_custom_out_buffer);
		for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
		{
			OutputVar(x);
		}
		bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
		memcpy(rf_custom_out_buffer, "dumpcomplete", sizeof("dumpcomplete"));
		RfCustomReply(rf_custom_out_buffer);

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





