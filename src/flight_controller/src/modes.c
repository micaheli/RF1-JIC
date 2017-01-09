#include "includes.h"



uint32_t activeModes;


//also modify the enumeration in modes.h
string_modes_rec stringModes[] = {
		{"ARMED",    0, M_ARMED },
		{"TAKEOFF",  1, M_TAKEOFF },
		{"HORIZON",  2, M_HORIZON },
		{"ATTITUDE", 3, M_ATTITUDE },
		{"LANDING",  4, M_LANDING },
		{"FAILSAFE", 5, M_FAILSAFE },
		{"LOGGING",  6, M_LOGGING },
		{"BUZZER",   7, M_BUZZER },
};



void InitModes(void)
{
	activeModes = 0;
}

inline void EnableMode(uint32_t modeMask)
{
	activeModes |= (modeMask);
}

inline void DisableMode(uint32_t modeMask)
{
	activeModes &= ~(modeMask);
}

inline uint32_t ModeActive(uint32_t modeMask)
{
	return (activeModes & modeMask);
}

void CheckRxToModes(void)
{
	uint32_t mode = 0;
	uint16_t x;
	uint16_t channel;
	float rcMin;
	float rcMax;

	for (x=0;x<FLIGHT_MODE_ARRAY_SIZE;x=x+3)
	{
		channel = (uint16_t)mainConfig.flightModeArray[x];
		rcMin   = (float)mainConfig.flightModeArray[x+1];
		rcMax   = (float)mainConfig.flightModeArray[x+2];
		if ((channel > 3) && (channel < MAXCHANNELS)) //first four channels are not to be used for flight modes
		{
			if ( (trueRcCommandF[channel] > rcMin) && (trueRcCommandF[channel] < rcMax) )
			{
				EnableMode(mode);
			}
			else
			{
				DisableMode(mode);
			}
		}
		mode++;
	}

}

void PrintModes(void)
{
	uint32_t x;
	for (x=0;x<(sizeof(stringModes)/sizeof(string_modes_rec));x++)
	{
		//mainConfig.flightModeArray[x];   //flight mode
		//mainConfig.flightModeArray[x+1]; //flight mode min Rc
		//mainConfig.flightModeArray[x+2]; //flight mode max Rc

		bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%s=%d=%d=%d", stringModes[x].modeString, mainConfig.flightModeArray[x+0], mainConfig.flightModeArray[x+1], mainConfig.flightModeArray[x+2] );
		RfCustomReply(rf_custom_out_buffer);
		DelayMs(3);
	}
}

void SplitString(char *inString, char *inString2, char token)
{
	uint32_t x;
	uint32_t stringLength;

	(void)(inString2);

	stringLength = strlen(inString);

	for (x = 0; x < stringLength; x++)
	{
		if (inString[x] == token)
			break;
	}

	if (stringLength > x)
	{
		inString2 = inString + x + 1; //put everything after the token into inString2
	}

	inString[x] = 0; //set modstrging to modeSting
}

void SetupModes(char *modeString)
{
	uint32_t x;
	char *channelString = NULL;
	char *minRcString   = NULL;
	char *maxRcString   = NULL;
	uint16_t channel;
	uint16_t minRc;
	uint16_t maxRc;

	if (!strcmp("list", modeString))
	{
		PrintModes();
	}
	else
	{
		//look for "MODE=CHANEL=MINRX=MAXRC"
		StripSpaces(modeString);

		SplitString(modeString,    channelString, '=');
		SplitString(channelString, minRcString,   '=');
		SplitString(minRcString,   maxRcString,   '=');

		channel = atoi(channelString);
		minRc   = atoi(minRcString);
		maxRc   = atoi(maxRcString);

		for (x=0;x<(sizeof(stringModes)/sizeof(string_modes_rec));x++)
		{
			if (!strcmp(stringModes[x].modeString, modeString))
			{
				mainConfig.flightModeArray[x+0] = channel;
				mainConfig.flightModeArray[x+1] = minRc;
				mainConfig.flightModeArray[x+2] = maxRc;
				bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%s set to channel %d and range %d to %d", stringModes[x].modeString, mainConfig.flightModeArray[x+0], mainConfig.flightModeArray[x+1], mainConfig.flightModeArray[x+2] );
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	}
}
