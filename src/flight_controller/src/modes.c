#include "includes.h"



uint32_t activeModes;
uint32_t setModes;



//also modify the enumeration in modes.h
string_modes_rec stringModes[] = {
		{"ARMED",    0,  M_ARMED },
		{"TAKEOFF",  1,  M_TAKEOFF },
		{"HORIZON",  2,  M_HORIZON },
		{"ATTITUDE", 3,  M_ATTITUDE },
		{"LANDING",  4,  M_LANDING },
		{"FAILSAFE", 5,  M_FAILSAFE },
		{"LOGGING",  6,  M_LOGGING },
		{"BUZZER",   7,  M_BUZZER },
		{"LEDMODE",  8,  M_LEDMODE },
		{"LEDCOLOR", 9,  M_LEDCOLOR },
		{"DIRECT",   10, M_DIRECT },
};



void InitModes(void)
{
	setModes = 0;
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

inline uint32_t ModeSet(uint32_t modeMask)
{
	return (setModes & modeMask);
}

void CheckRxToModes(void)
{
	uint32_t mode = 1;
	uint16_t x;
	volatile uint16_t channel;
	float rcMin;
	float rcMax;

	for (x=0;x<FLIGHT_MODE_ARRAY_SIZE;x=x+3)
	{

		channel = (uint16_t)mainConfig.flightModeArray[x];
		rcMin   = (float)mainConfig.flightModeArray[x+1] * 0.01;
		rcMax   = (float)mainConfig.flightModeArray[x+2] * 0.01;
		if ((channel > 3) && (channel < MAXCHANNELS)) //first four channels are not to be used for flight modes
		{
			setModes |= (mode); //this mode is currently assigned.
			if ( (trueRcCommandF[channel] >= rcMin) && (trueRcCommandF[channel] <= rcMax) )
			{
				EnableMode(mode);
			}
			else
			{
				DisableMode(mode);
			}
		}
		else
		{
			setModes &= ~(mode); //this mode is not currently assigned.
		}
		mode *= 2;
	}

}

void PrintModes(void)
{
	uint32_t x;
	uint32_t channel;

	for (x=0;x<(sizeof(stringModes)/sizeof(string_modes_rec));x++)
	{
		//display corrected output using the channel variable
		channel = mainConfig.flightModeArray[x*3+0];

		if (channel < 4)
			channel = 0;
		else
			channel += 1;

		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "modes %s=%d=%d=%d", stringModes[x].modeString, channel, mainConfig.flightModeArray[x*3+1], mainConfig.flightModeArray[x*3+2] );
		RfCustomReply(rf_custom_out_buffer);
	}

}

/*
void SplitString(char *inString, char *inString2, char token)
{
	uint32_t x;
	uint32_t stringLength;

	stringLength = strlen(inString);

	for (x = 0; x < stringLength; x++)
	{
		if (inString[x] == token)
			break;
	}

	if ( stringLength > x )
	{
		inString2 = inString + x + 1; //put everything after the token into inString2
	}

	inString[x] = 0; //set modstrging to modeSting
}
*/

void SetMode(uint32_t modeMask, uint16_t channel, int16_t minRc, int16_t maxRc)
{

	uint32_t x;

	for (x=0;x<(sizeof(stringModes)/sizeof(string_modes_rec));x++)
	{
		if ( stringModes[x].modeMask == modeMask)
		{
			mainConfig.flightModeArray[x*3+0] = channel;
			mainConfig.flightModeArray[x*3+1] = minRc;
			mainConfig.flightModeArray[x*3+2] = maxRc;
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);

			//display corrected output using the channel variable
			if (channel < 4)
				channel = 0;
			else
				channel += 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%s set to channel %d and range %d to %d", stringModes[x].modeString, channel, mainConfig.flightModeArray[x*3+1], mainConfig.flightModeArray[x*3+2] );
			RfCustomReply(rf_custom_out_buffer);
		}
	}
}

void SetupModes(char *modString)
{

	uint32_t x;
	char *channelString = NULL;
	char *minRcString   = NULL;
	char *maxRcString   = NULL;
	int16_t channel;
	int16_t minRc;
	int16_t maxRc;
	uint32_t stringLength;

	if (!strcmp("list", modString))
	{
		PrintModes();
	}
	else
	{
		//look for "MODE=CHANEL=MINRX=MAXRC"
		StripSpaces(modString);

		stringLength = strlen(modString);

		for (x = 0; x < stringLength; x++)
		{
			if (modString[x] == '=')
				break;
		}

		if ( stringLength > x )
		{
			channelString = modString + x + 1; //put everything after the token into inString2
		}

		modString[x] = 0; //set modstrging to modeSting

		stringLength = strlen(channelString);

		for (x = 0; x < stringLength; x++)
		{
			if (channelString[x] == '=')
				break;
		}

		if ( stringLength > x )
		{
			minRcString = channelString + x + 1; //put everything after the token into inString2
		}

		channelString[x] = 0; //set modstrging to modeSting




		stringLength = strlen(minRcString);

		for (x = 0; x < stringLength; x++)
		{
			if (minRcString[x] == '=')
				break;
		}

		if ( stringLength > x )
		{
			maxRcString = minRcString + x + 1; //put everything after the token into inString2
		}

		minRcString[x] = 0; //set modstring to modeSting

		//SplitString(modeString,    channelString, '=');
		//SplitString(channelString, minRcString,   '=');
		//SplitString(minRcString,   maxRcString,   '=');

		channel = atoi(channelString) - 1;
		minRc   = atoi(minRcString);
		maxRc   = atoi(maxRcString);

		//add logic since people can set this manually.
		if (minRc < -100)
			minRc = -100;
		if (minRc > 100)
			minRc = 100;

		if (maxRc < -100)
			maxRc = -100;
		if (maxRc > 100)
			maxRc = 100;

		if (minRc > maxRc)
			minRc = maxRc;

		if (maxRc < minRc)
			maxRc = minRc;
		//
		if (channel < 4)
			channel = 0;

		if (channel >= MAXCHANNELS)
			channel = 0;

		for (x=0;x<(sizeof(stringModes)/sizeof(string_modes_rec));x++)
		{
			if (!strcmp(stringModes[x].modeString, modString))
			{
				mainConfig.flightModeArray[x*3+0] = channel;
				mainConfig.flightModeArray[x*3+1] = minRc;
				mainConfig.flightModeArray[x*3+2] = maxRc;

				//display corrected output using the channel variable
				if (channel < 5)
					channel = 0;
				else
					channel += 1;

				bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%s set to channel %d and range %d to %d", stringModes[x].modeString, channel, mainConfig.flightModeArray[x*3+1], mainConfig.flightModeArray[x*3+2] );
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	}
}
