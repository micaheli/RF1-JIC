#include "includes.h"


uint32_t checkRxData[MAXCHANNELS];


static int32_t  FindRxCenter(void);
static int32_t  FindRxMinMax(void);
static void     ResetChannelCheck(void);
static int32_t  WhichInChannelChange(void);
static int32_t  SetChannelMapAndDirection(uint32_t inChannel, uint32_t outChannel);
static uint32_t CheckSafeMotors(uint32_t time, uint32_t deviationAllowed);
static int32_t  CheckAndSetChannel(uint32_t outChannel);


static int32_t FindRxCenter(void) {

	volatile int32_t successRxStuff = 0;
	uint32_t time = 5 * 400; //two seconds
	rc_control_config tempRc;
	bzero(&tempRc,sizeof(tempRc));

	for (uint32_t timeCounter = 0;timeCounter < time;timeCounter+=5) {

		calibrateMotors = 0;
		ZeroActuators(0);

		for (uint32_t x = 0;x<MAXCHANNELS;x++) {
			tempRc.midRc[x] = rxData[x];
		}

		if (rx_timeout > 1000)
		{
			return -1;
		}

	}

	for (uint32_t x = 0;x<MAXCHANNELS;x++) {
		if ( tempRc.maxRc[x] != 1000000 ) {
			mainConfig.rcControlsConfig.midRc[x] = tempRc.midRc[x];
			if ( ( ABS((int32_t)mainConfig.rcControlsConfig.midRc[x] - (int32_t)mainConfig.rcControlsConfig.minRc[x]) < 10 ) ) { //looks like switch
				mainConfig.rcControlsConfig.midRc[x] = mainConfig.rcControlsConfig.minRc[x]; //set center to min RC
			} else
			if ( ( ABS((int32_t)mainConfig.rcControlsConfig.midRc[x] - (int32_t)mainConfig.rcControlsConfig.maxRc[x]) < 10 ) ) { //looks like switch
				mainConfig.rcControlsConfig.midRc[x] = mainConfig.rcControlsConfig.minRc[x]; //set center to min RC
			}
			successRxStuff++;
		}
	}

	calibrateMotors = 0;
	ZeroActuators(32000);

	return (successRxStuff);
}

static int32_t FindRxMinMax(void) {

	volatile int32_t successMinMaxStuff = 0;
	uint32_t time = 20 * 750; //fifteen seconds
	rc_control_config tempRc;

	mainConfig.rcControlsConfig.rcCalibrated = 0;
	bzero(&tempRc,sizeof(tempRc));

	for (uint32_t x = 0;x<MAXCHANNELS;x++) {
		tempRc.maxRc[x] = 0;
		tempRc.minRc[x] = 1000000;
		tempRc.channelMap[x] = 1000; //disable the channel
	}

	for (uint32_t timeCounter = 0;timeCounter < time;timeCounter+=20) {

		calibrateMotors = 0;
		ZeroActuators(0);
		DelayMs(20);

		bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (volatile unsigned int)(rxData[0]));
		RfCustomReply(rf_custom_out_buffer);

		for (uint32_t x = 0;x<MAXCHANNELS;x++) {
			if (rxData[x] < tempRc.minRc[x]) {
				tempRc.minRc[x] = (volatile unsigned int)rxData[x];
				tempRc.channelMap[x] = x;
			}

			if (rxData[x] > tempRc.maxRc[x]) {
				tempRc.maxRc[x] = (volatile unsigned int)rxData[x];
				tempRc.channelMap[x] = x;
			}
		}

		if (rx_timeout > 1000)
		{
			return -1;
		}

	}

	for (uint32_t xx = 0;xx<MAXCHANNELS;xx++) {

		if ( (tempRc.minRc[xx] != 1000000) && ( ABS( (int32_t)tempRc.maxRc[xx] - (int32_t)tempRc.minRc[xx] ) > 200 ) ) { //we have RC data and the difference is greater than 200
			mainConfig.rcControlsConfig.maxRc[xx]      = tempRc.maxRc[xx];
			mainConfig.rcControlsConfig.minRc[xx]      = tempRc.minRc[xx];
			mainConfig.rcControlsConfig.channelMap[xx] = 50; //set to 50 means waiting to be assigned
			successMinMaxStuff++;
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (volatile unsigned int)(xx));
			RfCustomReply(rf_custom_out_buffer);
		} else {
			mainConfig.rcControlsConfig.maxRc[xx]      = 1000000;
			mainConfig.rcControlsConfig.minRc[xx]      = 0;
			mainConfig.rcControlsConfig.channelMap[xx] = 100; //set to 100 disables the channel
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u, %u, %u, %u", (volatile unsigned int)xx, (volatile unsigned int)(tempRc.maxRc[xx]), (volatile unsigned int)(tempRc.minRc[xx]), (volatile unsigned int)ABS( (int32_t)tempRc.maxRc[xx] - (int32_t)tempRc.minRc[xx] ));
			RfCustomReply(rf_custom_out_buffer);
		}

	}

	calibrateMotors = 0;
	ZeroActuators(32000);


	return (successMinMaxStuff);
}

static void ResetChannelCheck(void)
{
	memcpy(checkRxData, rxData, sizeof(checkRxData));
}

//only returns unassigned channels
//returns channel with largest change that's unassigned
static int32_t WhichInChannelChange(void)
{

	int32_t inChannelChanged  = -1;
	uint32_t changeValue      = 0;
	float currentChannelRange = 0;
	float percentFromMax      = 0;
	float percentFromMin      = 0;
	volatile float diffFloat  = 0;
	float closestToEndPoint   = .5;

	for (uint32_t x = 0;x<MAXCHANNELS;x++) {

		changeValue = ABS((int32_t)rxData[x] - (int32_t)checkRxData[x]);

		if ( changeValue > 200 ) {
			if (mainConfig.rcControlsConfig.channelMap[x] == 50) {
				currentChannelRange = ABS((float)mainConfig.rcControlsConfig.maxRc[x] - (float)mainConfig.rcControlsConfig.minRc[x]); //1000    //0  //1
				diffFloat      = (float)rxData[x] -  (float)mainConfig.rcControlsConfig.maxRc[x];
				percentFromMax = (float)( ABS(diffFloat) / (float)currentChannelRange);
				diffFloat      = (float)rxData[x] -  (float)mainConfig.rcControlsConfig.minRc[x];
				percentFromMin = (float)( ABS(diffFloat) / (float)currentChannelRange);
				if (percentFromMax > percentFromMin) { //we're near min
					if (percentFromMax > closestToEndPoint ) { //if current channel is closer to endpoint and it's changed at least 200 points, this is our channel
						closestToEndPoint = percentFromMin;
						//inChannelChanged = x;
					}
				} else { //we're closer to max or we're in the middle
					if (percentFromMin > closestToEndPoint ) { //if current channel is closer to endpoint and it's changed at least 200 points, this is our channel
						closestToEndPoint = percentFromMax;
						//inChannelChanged = x;
					}
				}
				if (closestToEndPoint < 0.05) { //at least within 5% of endpoint
					inChannelChanged = x;
				}

			}

		}

	}

	return inChannelChanged;

}

static int32_t SetChannelMapAndDirection(uint32_t inChannel, uint32_t outChannel)
{

	int32_t channelCheck = ( rxData[inChannel] < (mainConfig.rcControlsConfig.maxRc[inChannel] - 300) ); //channel is reversed

	if (mainConfig.rcControlsConfig.channelMap[inChannel] == 50) { //if channelMap for the inChannel is 50 than it's waiting to be assigned.

		mainConfig.rcControlsConfig.channelMap[inChannel] = outChannel; //set channel map

		if ( channelCheck ) { //min is higher so channel is reversed, reverse if needed
			channelCheck = (int32_t)mainConfig.rcControlsConfig.maxRc[inChannel];
			mainConfig.rcControlsConfig.maxRc[inChannel] = mainConfig.rcControlsConfig.minRc[inChannel];
			mainConfig.rcControlsConfig.minRc[inChannel] = (uint32_t)channelCheck;
		}
		return 1;

	}
	return -1;

}

//Using standard deviation of ACC to make sure motors aren't running.
static uint32_t CheckSafeMotors(uint32_t time, uint32_t deviationAllowed)
{

	float stdDeviation[10];
	bzero(stdDeviation, sizeof(stdDeviation));
	uint32_t simpleCouter = 0;

	float strdDeviationCheck;

	time = time / 10;

	for (uint32_t timeCounter = 0;timeCounter < time;timeCounter++) {

		for (simpleCouter=0;simpleCouter < 10;simpleCouter++) {
			stdDeviation[simpleCouter]   = ABS(geeForceAccArray[ACCZ]);
			DelayMs(1);
		}
		FeedTheDog();
		simpleCouter = 0;
		strdDeviationCheck = CalculateSD(stdDeviation) * 100000;

		if (strdDeviationCheck > deviationAllowed) {
			calibrateMotors = 0;
			DisarmBoard();
			ZeroActuators(32000);
			return 0;
		}

	}
	return 1;
}

static int32_t CheckAndSetChannel(uint32_t outChannel) {
	int32_t changedInChannel = -1;
	int32_t throttleFix = 0;
	for (uint32_t x = 0;x<1000;x++) {
		changedInChannel = WhichInChannelChange();
		if ((changedInChannel > -1) && (x > 100))
			break;
		DelayMs(10);
	}
	if (changedInChannel > -1) {
		if (SetChannelMapAndDirection(changedInChannel, outChannel)) {
			if ( (outChannel == THROTTLE) || (outChannel == AUX1) || (outChannel == AUX2) || (outChannel == AUX3) || (outChannel == AUX4) ) { //set mid point at center between extremes
				throttleFix = (int32_t)(((int32_t)mainConfig.rcControlsConfig.maxRc[changedInChannel] - (int32_t)mainConfig.rcControlsConfig.minRc[changedInChannel]) / 2);
				mainConfig.rcControlsConfig.midRc[changedInChannel] = (uint32_t)( throttleFix + (int32_t)mainConfig.rcControlsConfig.minRc[changedInChannel]);
			}
			return 1;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

void MixerWizard(char *inString)
{

	if (!strcmp("mixera", inString))
	{
		DisarmBoard();
		SKIP_GYRO=1;
		memcpy(rf_custom_out_buffer, "spinningmotor1\n", sizeof("spinningmotor1\n"));
		RfCustomReply(rf_custom_out_buffer);
		DelayMs(10);
		IdleActuator(0);
		DelayMs(10);
	}
	else if (!strcmp("mixerb", inString))
	{
		DisarmBoard();
		SKIP_GYRO=0;
	}
	else if (!strcmp("mixerc", inString))
	{
		DisarmBoard();
		SKIP_GYRO=0;
	}
}

void SetupWizard(char *inString)
{

	int32_t returnedValue = 0;

	if (!strcmp("mixera", inString))
	{
		MixerWizard(inString);
	}
	else if (!strcmp("mixerb", inString))
	{
		MixerWizard(inString);
	}
	else if (!strcmp("mixerc", inString))
	{
		MixerWizard(inString);
	}
	else if (!strcmp("cala", inString))
	{
		mainConfig.gyroConfig.boardCalibrated = 0;
		mainConfig.gyroConfig.gyroRotation = CW0;
		DelayMs(200); //need to reset calibration and give ACC data time to refresh
		if (SetCalibrate1()) {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "calibrate1finished\n", sizeof("calibrate1finished\n"));
			RfCustomReply(rf_custom_out_buffer);
		} else {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "calibrationfailed\n", sizeof("calibrationfailed\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	}
	else if (!strcmp("calb", inString))
	{

		if (SetCalibrate2()) {

			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "calibrate2finished\n", sizeof("calibrate2finished\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);

		} else {

			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "calibrationfailed\n", sizeof("calibrationfailed\n"));
			RfCustomReply(rf_custom_out_buffer);

		}

	}
	else if (!strcmp("rca", inString))
	{

		//reset rcCalibration
		mainConfig.rcControlsConfig.rcCalibrated = 0;
		skipRxMap = 1;

		//reset channelMap
		for (uint32_t x = 0;x<MAXCHANNELS;x++) {
			mainConfig.rcControlsConfig.channelMap[x] = x;
		}

		bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
		memcpy(rf_custom_out_buffer, "\n", sizeof("\n"));
		RfCustomReply(rf_custom_out_buffer);
		bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
		memcpy(rf_custom_out_buffer, "movercfor15seconds", sizeof("movercfor15seconds"));
		RfCustomReply(rf_custom_out_buffer);

		returnedValue = FindRxMinMax();
		DisarmBoard();
		if (returnedValue == -1) {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "calibratercfailedradionotdetected", sizeof("calibratercfailedradionotdetected"));
			RfCustomReply(rf_custom_out_buffer);
		} else if (returnedValue) {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "endpointsset", sizeof("endpointsset"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "centersticksandswitches", sizeof("centersticksandswitches"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "runwizrcb", sizeof("runwizrcb"));
			RfCustomReply(rf_custom_out_buffer);
		} else {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "calibratercfailed", sizeof("calibratercfailed"));
			RfCustomReply(rf_custom_out_buffer);
		}
	}
	else if (!strcmp("rcb", inString))
	{

		bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
		memcpy(rf_custom_out_buffer, "\n", sizeof("\n"));
		RfCustomReply(rf_custom_out_buffer);
		bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
		memcpy(rf_custom_out_buffer, "calibratestickcenters", sizeof("calibratestickcenters"));
		RfCustomReply(rf_custom_out_buffer);

		returnedValue = FindRxCenter();
		DisarmBoard();
		if (returnedValue == -1) {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "calibratercfailedradionotdetected", sizeof("calibratercfailedradionotdetected"));
			RfCustomReply(rf_custom_out_buffer);
		} else if (returnedValue) {
			ResetChannelCheck(); //reset stick checker
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "centersset", sizeof("centersset"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "\n", sizeof("\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "setARMSWITCHdisarmed", sizeof("setARMSWITCHdisarmed"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "setLOGSWITCHoff", sizeof("setLOGSWITCHoff"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "setTHROTTLEbottom", sizeof("setTHROTTLEbottom"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "runwizrcc", sizeof("runwizrcc"));
			RfCustomReply(rf_custom_out_buffer);
		} else {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "calibratercfailed", sizeof("calibratercfailed"));
			RfCustomReply(rf_custom_out_buffer);
		}
	}
	else if (!strcmp("rcc", inString))
	{

		ResetChannelCheck();
		bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
		memcpy(rf_custom_out_buffer, "setTHROTTLEtop", sizeof("setTHROTTLEtop"));
		RfCustomReply(rf_custom_out_buffer);

		if (CheckAndSetChannel(THROTTLE)) {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "setYAWright", sizeof("setYAWright"));
			RfCustomReply(rf_custom_out_buffer);
			if (CheckAndSetChannel(YAW)) {
				bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
				memcpy(rf_custom_out_buffer, "setPITCHtop", sizeof("setPITCHtop"));
				RfCustomReply(rf_custom_out_buffer);
				if (CheckAndSetChannel(PITCH)) {
					bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
					memcpy(rf_custom_out_buffer, "setROLLright", sizeof("setROLLright"));
					RfCustomReply(rf_custom_out_buffer);
					if (CheckAndSetChannel(ROLL)) {
						bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
						memcpy(rf_custom_out_buffer, "setARMSWITCHarmed", sizeof("setARMSWITCHarmed"));
						RfCustomReply(rf_custom_out_buffer);
						if (CheckAndSetChannel(AUX1)) {
							bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
							memcpy(rf_custom_out_buffer, "setLOGarmed", sizeof("setLOGarmed"));
							RfCustomReply(rf_custom_out_buffer);
							if (CheckAndSetChannel(AUX2)) {
								bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
								memcpy(rf_custom_out_buffer, "loggingswitchenabled", sizeof("loggingswitchenabled"));
								RfCustomReply(rf_custom_out_buffer);
							} else {
								bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
								memcpy(rf_custom_out_buffer, "loggingswitchdisabled", sizeof("loggingswitchdisabled"));
								RfCustomReply(rf_custom_out_buffer);
							}
							bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
							memcpy(rf_custom_out_buffer, "calibrationcomplete", sizeof("calibrationcomplete"));
							RfCustomReply(rf_custom_out_buffer);
							mainConfig.rcControlsConfig.rcCalibrated = 1;
							skipRxMap = 0;
							bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
							memcpy(rf_custom_out_buffer, "saving", sizeof("saving"));
							RfCustomReply(rf_custom_out_buffer);
							SaveConfig(ADDRESS_CONFIG_START);
							bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
							memcpy(rf_custom_out_buffer, "savecomplete", sizeof("savecomplete"));
							RfCustomReply(rf_custom_out_buffer);
						} else {
							bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
							memcpy(rf_custom_out_buffer, "calibrationfailed", sizeof("calibrationfailed"));
							RfCustomReply(rf_custom_out_buffer);
						}
					} else {
						bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
						memcpy(rf_custom_out_buffer, "calibrationfailed", sizeof("calibrationfailed"));
						RfCustomReply(rf_custom_out_buffer);
					}
				} else {
					bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
					memcpy(rf_custom_out_buffer, "calibrationfailed", sizeof("calibrationfailed"));
					RfCustomReply(rf_custom_out_buffer);
				}
			} else {
				bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
				memcpy(rf_custom_out_buffer, "calibrationfailed", sizeof("calibrationfailed"));
				RfCustomReply(rf_custom_out_buffer);
			}
		} else {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "calibrationfailed", sizeof("calibrationfailed"));
			RfCustomReply(rf_custom_out_buffer);
		}

	}
	else if (!strcmp("mot", inString))
	{

		calibrateMotors = 1;
		SKIP_GYRO=1;
		DisarmBoard();
		motorOutput[0] = 1.0;
		OutputActuators(motorOutput, servoOutput);
		if (CheckSafeMotors(1000, 10000)) { //check for safe motors for 3 seconds, 10000 standard deviation allowed
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "calibratingmotorspluginbattery", sizeof("calibratingmotorspluginbattery"));
			RfCustomReply(rf_custom_out_buffer);
		} else {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "motorcalibrationfailed", sizeof("motorcalibrationfailed"));
			RfCustomReply(rf_custom_out_buffer);
			InitWatchdog(WATCHDOG_TIMEOUT_2S);
			return;
		}

		if (CheckSafeMotors(15000,200000)) { //calibrate for 15 seconds safe motors for 3 seconds

		}
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
		InitWatchdog(WATCHDOG_TIMEOUT_2S);
		SKIP_GYRO=0;
		calibrateMotors = 0;

		bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
		memcpy(rf_custom_out_buffer, "calibratingmotorscomplete", sizeof("calibratingmotorscomplete"));
		RfCustomReply(rf_custom_out_buffer);

	}
	else
	{
		bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "noargumentsfoundforargument:%s", inString);
		RfCustomReply(rf_custom_out_buffer);
	}
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////ONE WIRE/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OneWire(char *inString) {

	esc_hex_location escHexLocation;
	const oneWireParameter_t *parameter;
	int16_t value, bytesWritten = 0;
	uint32_t idx;
	uint32_t x;
	char *modString = NULL;
	char *args = NULL;
	uint32_t motorNumber;
	uint32_t modStringLength;
	uint32_t outputNumber;

	if (!strcmp("start", inString) || !strcmp("read", inString))
	{

		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Reading ESCs...");
		RfCustomReply(rf_custom_out_buffer);
		DelayMs(5);
		if (OneWireInit() == 0)
		{

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "No ESCs detected. Is your battery connected?");
			RfCustomReply(rf_custom_out_buffer);
			DelayMs(5);

		}
		else
		{
			for (x = 0; x < MAX_MOTOR_NUMBER; x++)
			{
				outputNumber = mainConfig.mixerConfig.motorOutput[x];
				if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
				{
					if (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].enabled)
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu: %u.%u, %s, %s, %s", x, (uint8_t)( (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].version >> 8) & 0xFF), (uint8_t)(escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].version & 0xFF), escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].nameStr, escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].fwStr, escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].versionStr);
						RfCustomReply(rf_custom_out_buffer);
						DelayMs(1);
						if (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].escHexLocation.version > escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].version) {
							snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu: Upgrade to version %u.%u is available", x, (uint8_t)( (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].escHexLocation.version >> 8) & 0xFF), (uint8_t)( escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].escHexLocation.version & 0xFF) );
							RfCustomReply(rf_custom_out_buffer);
							DelayMs(5);
						}
					}
					else
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu Unreadable", x);
						RfCustomReply(rf_custom_out_buffer);
						DelayMs(5);
					}
				}
				else
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu Disabled", x);
					RfCustomReply(rf_custom_out_buffer);
					DelayMs(5);
				}

			}

			snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "1wiredumpstart\n");
			RfCustomReply(rf_custom_out_buffer);
			DelayMs(5);
			bytesWritten = 0;

			for (x = 0; x < MAX_MOTOR_NUMBER; x++)
			{

				outputNumber = mainConfig.mixerConfig.motorOutput[x];
				if ( (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) && (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].enabled) )
				{

					for (idx = 0; oneWireParameters[idx] != NULL; idx++)
					{

						parameter = oneWireParameters[idx];
						bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "m%lu=%s=", x, parameter->name);
						value = Esc1WireParameterFromDump(board.motors[outputNumber], parameter, escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].config);
						// make sure the value is valid

						if (value == 0xFF)
						{
							bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "NONE\n");
							RfCustomReply(rf_custom_out_buffer);
							DelayMs(5);
							bytesWritten = 0;
							continue;
						}

						// add the readable form of the parameter value to the buffer
						if (parameter->parameterNamed)
						{
							bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "%s\n", OneWireParameterValueToName(parameter->parameterNamed, value));
							RfCustomReply(rf_custom_out_buffer);
							DelayMs(5);
							bytesWritten = 0;
						}
						else
						{
							bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "%d\n", OneWireParameterValueToNumber(parameter->parameterNumerical, value));
							RfCustomReply(rf_custom_out_buffer);
							DelayMs(5);
							bytesWritten = 0;
						}

					}

				}

			}

			snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "1wiredumpcomplete\n");
			RfCustomReply(rf_custom_out_buffer);
			DelayMs(5);
			bytesWritten = 0;

		}

	}
	else if (!strcmp("list", inString))
	{
		if (!ListAllEscHexesInFlash())
		{
			snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "No ESC Hexes found in flash\n");
			RfCustomReply(rf_custom_out_buffer);
		}
	}
	else if (!strcmp("config", inString))
	{

		snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "1wiredumpstart\n");
		RfCustomReply(rf_custom_out_buffer);
		bytesWritten = 0;

		for (x = 0; x < MAX_MOTOR_NUMBER; x++)
		{

			outputNumber = mainConfig.mixerConfig.motorOutput[x];
			if ( (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) && (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].enabled) )
			{

				for (idx = 0; oneWireParameters[idx] != NULL; idx++)
				{

					parameter = oneWireParameters[idx];
					bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "m%lu=%s=\n", x, parameter->name);
					value = Esc1WireParameterFromDump(board.motors[outputNumber], parameter, escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].config);
					// make sure the value is valid

					if (value == 0xFF)
					{
						bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "NONE\n");
						RfCustomReply(rf_custom_out_buffer);
						DelayMs(2);
						bytesWritten = 0;
						continue;
					}

					// add the readable form of the parameter value to the buffer
					if (parameter->parameterNamed)
					{
						bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "%s\n", OneWireParameterValueToName(parameter->parameterNamed, value));
						RfCustomReply(rf_custom_out_buffer);
						DelayMs(2);
						bytesWritten = 0;
					}
					else
					{
						bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "%d\n", OneWireParameterValueToNumber(parameter->parameterNumerical, value));
						RfCustomReply(rf_custom_out_buffer);
						DelayMs(2);
						bytesWritten = 0;
					}

				}

			}

		}

		snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "1wiredumpcomplete\n");
		RfCustomReply(rf_custom_out_buffer);
		bytesWritten = 0;

	}
	else if (!strcmp("save", inString))
	{

		for (x = 0; x < MAX_MOTOR_NUMBER; x++)
		{
			outputNumber = mainConfig.mixerConfig.motorOutput[x];
			if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Saving Motor %lu Config.\n", x);
				RfCustomReply(rf_custom_out_buffer);

				if (OneWireSaveConfig(board.motors[outputNumber]))
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu Config Saved.\n", x);
					RfCustomReply(rf_custom_out_buffer);
				}
				else
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Error Saving Motor %lu Config.\n", x);
					RfCustomReply(rf_custom_out_buffer);
				}

			}

		}

	}
	else if (!strcmp("stop", inString))
	{
		OneWireDeinit();
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "1Wire Session Ended.");
		RfCustomReply(rf_custom_out_buffer);
	}
	else
	{
		if ( (inString[0] == 'm') && (inString[2] == '=') ) {
			motorNumber=atoi(&inString[1]);
			outputNumber = mainConfig.mixerConfig.motorOutput[motorNumber];
			modString = inString+3;

			StripSpaces(modString);

			modStringLength = strlen(modString);

			for (x = 0; x < modStringLength; x++) {
				if (modString[x] == '=')
					break;
			}

			if (modStringLength > x) {
				args = modString + x + 1;
			}

			modString[x] = 0;

			for (x = 0; x < strlen(modString); x++)
				modString[x] = tolower((unsigned char)modString[x]);

			for (x = 0; x < strlen(args); x++)
				args[x] = tolower((unsigned char)args[x]);


			if (!strcmp("upgrade", modString) || !strcmp("forceupgrade", modString))
			{
				if (!strcmp("forceupgrade", modString))
				{
					if (atoi(args) < 50)
					{
						memcpy(&escHexLocation, &escHexByPosition[atoi(args)], sizeof(esc_hex_location));
					}
					else
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ESC %lu upgrade will not progress. ESC Hex not found.\n", motorNumber);
						RfCustomReply(rf_custom_out_buffer);
						return;
					}
				}
				else
				{
					memcpy(&escHexLocation, &escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].escHexLocation, sizeof(esc_hex_location));
				}

				if ( (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) && (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].enabled) )
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Upgrading %lu...\n", motorNumber);
					RfCustomReply(rf_custom_out_buffer);
					DelayMs(5);
					if ( BuiltInUpgradeSiLabsBLHeli(board.motors[outputNumber], escHexLocation) )
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ESC %lu upgrade complete\n", motorNumber);
						RfCustomReply(rf_custom_out_buffer);
						DelayMs(5);
					}
					else
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ESC %lu upgrade failed\n", motorNumber);
						RfCustomReply(rf_custom_out_buffer);
						DelayMs(5);
					}
				}
				else
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "No upgrade available for ESC %lu\n", motorNumber);
					RfCustomReply(rf_custom_out_buffer);
					DelayMs(5);
				}
			}
			else
			{
				if ( (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) && (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].enabled) )
				{
					for (idx = 0; oneWireParameters[idx] != NULL; idx++)
					{
						parameter = oneWireParameters[idx];
						if (!strcmp(parameter->name, modString)) //found the proper parameter, now let's get the proper value based on the string args
						{
							if (parameter->parameterNamed) //is the value a string?
							{
								value = OneWireParameterNameToValue(parameter->parameterNamed, args);
							}
							else //then it's an int.
							{
								value = OneWireParameterNumberToValue(parameter->parameterNumerical, (int16_t)atoi(args));
							}
							if (value < 0)
							{
								snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ERROR=Unknown parameter value\n");
								RfCustomReply(rf_custom_out_buffer);
								return;
							}
							else
							{
								if ( Esc1WireSetParameter(board.motors[outputNumber], parameter, escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].config, value) )
								{
									snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ESC %lu Value Set! Please Save Changes!\n", motorNumber);
									RfCustomReply(rf_custom_out_buffer);
									return;
								}
								else
								{
									snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ERROR=Unknown ESC Layout\n");
									RfCustomReply(rf_custom_out_buffer);
									return;
								}

							}

						}

					}

				}

			}

		}

	}

}
