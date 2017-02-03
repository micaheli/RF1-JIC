#include "includes.h"


uint32_t checkRxData[MAXCHANNELS];
rc_control_config tempRc;
wizard_record wizardStatus;

//static int32_t  FindRxCenter(void);
//static int32_t  FindRxMinMax(void);
static void     ResetChannelCheck(void);
static uint32_t CheckSafeMotors(uint32_t time, uint32_t deviationAllowed);
//static int32_t  CheckAndSetChannel(uint32_t outChannel);

static void     PrepareRcWizard(void);
static void     WizRcCheckMinMax(void);
static void     WizRcCheckCenter(void);
static void     WizRcCheckAndSendDirection(void);
static int32_t  WizRcWhichInChannelChange(void);
static int32_t  WizRcSetChannelMapAndDirection(uint32_t inChannel, uint32_t outChannel);
static void     WizRcSetRestOfMap(void);

static void     HandleWizRx(void);
static uint32_t WizRxCheckRxDataLooksValid(void);

static void ResetChannelCheck(void)
{
	memcpy(checkRxData, rxData, sizeof(checkRxData));
}


//Using standard deviation of ACC to make sure motors aren't running.
static uint32_t CheckSafeMotors(uint32_t time, uint32_t deviationAllowed)
{

	float stdDeviation[10];
	bzero(stdDeviation, sizeof(stdDeviation));
	uint32_t simpleCouter = 0;

	float strdDeviationCheck;

	time = time / 10;

	for (uint32_t timeCounter = 0;timeCounter < time;timeCounter++)
	{

		for (simpleCouter=0;simpleCouter < 10;simpleCouter++)
		{
			stdDeviation[simpleCouter]   = ABS(geeForceAccArray[ACCZ]);
		}
		FeedTheDog();
		simpleCouter = 0;
		strdDeviationCheck = CalculateSD(stdDeviation) * 100000;

		if (strdDeviationCheck > deviationAllowed)
		{
			calibrateMotors = 0;
			DisarmBoard();
			ZeroActuators(32000);
			return 0;
		}

	}
	return 1;
}

void MixerWizard(char *inString)
{

	if (!strcmp("mixera", inString))
	{
		DisarmBoard();
		SKIP_GYRO=1;
		memcpy(rf_custom_out_buffer, "spinningmotor1", sizeof("spinningmotor1"));
		RfCustomReplyBuffer(rf_custom_out_buffer);
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

void WizRcSetRestOfMap(void)
{

	uint32_t x;
	uint32_t y;

	uint32_t alreadyMapped[MAXCHANNELS];

	bzero(alreadyMapped, sizeof(alreadyMapped));

	//channels that are already mapped get set to 1 in this array
	for (x = 0;x<MAXCHANNELS;x++)
	{
		if (mainConfig.rcControlsConfig.channelMap[x] < MAXCHANNELS)
			alreadyMapped[mainConfig.rcControlsConfig.channelMap[x]]=1;
	}

	for (x = 0;x<MAXCHANNELS;x++)
	{
		if (alreadyMapped[x] == 0) //x is not assigned to a channel so we assign it to the first channel mapped to 1000
		{
			for (y = 0;y<MAXCHANNELS;y++)
			{
				if (mainConfig.rcControlsConfig.channelMap[y] == 1000) //this channel is not assigned a map, so we assign it X and set min max to the mapped channel 0
				{
					mainConfig.rcControlsConfig.channelMap[y] = x;
					mainConfig.rcControlsConfig.minRc[x] = mainConfig.rcControlsConfig.minRc[0];
					mainConfig.rcControlsConfig.midRc[x] = mainConfig.rcControlsConfig.midRc[0];
					mainConfig.rcControlsConfig.maxRc[x] = mainConfig.rcControlsConfig.maxRc[0];
					break; //break out of inner loop
				}
			}
		}
	}
}

uint32_t WizRxCheckRxDataLooksValid(void)
{
	uint32_t correct = 0;

	//if processed data is within 5% of center on 3 axis it's safe to assume it's valid data
	if ( (trueRcCommandF[YAW] < 0.05) && (trueRcCommandF[YAW] > - 0.05) )
		correct ++;
	if ( (trueRcCommandF[ROLL] < 0.05) && (trueRcCommandF[ROLL] > - 0.05) )
		correct ++;
	if ( (trueRcCommandF[PITCH] < 0.05) && (trueRcCommandF[PITCH] > - 0.05) )
		correct ++;
	if ( (trueRcCommandF[THROTTLE] < 0.05) && (trueRcCommandF[THROTTLE] > - 0.05) )
		correct ++;

	if (correct > 2)
	{
		return(1);
	}
	return(0);
}


void PrepareRcWizard(void)
{
	//set wizard structure
	wizardStatus.currentWizard = WIZ_RC;
	wizardStatus.currentStep   = 1;
	//reset rcCalibration config
	mainConfig.rcControlsConfig.rcCalibrated = 0;
	//don't use RX map until wizard completes
	skipRxMap = 1;

	bzero(&tempRc, sizeof(rc_control_config));

	//reset channelMap config and prepare tempRc
	for (uint32_t x = 0;x<MAXCHANNELS;x++)
	{
		mainConfig.rcControlsConfig.channelMap[x] = 1000;
		tempRc.maxRc[x] = 0; //high will never be this low so we set it to high
		tempRc.minRc[x] = 1000000; //nothing will go this high so we set it to min
		tempRc.channelMap[x] = 1000; //disable the channel
	}

}

//only returns unassigned channels
//returns channel with largest change that's unassigned
static int32_t WizRcWhichInChannelChange(void)
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

		if ( changeValue > 200 )
		{
			if (mainConfig.rcControlsConfig.channelMap[x] == 1000)
			{
				currentChannelRange = ABS((float)mainConfig.rcControlsConfig.maxRc[x] - (float)mainConfig.rcControlsConfig.minRc[x]); //1000    //0  //1
				diffFloat      = (float)rxData[x] -  (float)mainConfig.rcControlsConfig.maxRc[x];
				percentFromMax = (float)( ABS(diffFloat) / (float)currentChannelRange);
				diffFloat      = (float)rxData[x] -  (float)mainConfig.rcControlsConfig.minRc[x];
				percentFromMin = (float)( ABS(diffFloat) / (float)currentChannelRange);
				if (percentFromMax > percentFromMin)
				{ //we're near min
					if (percentFromMax > closestToEndPoint )
					{ //if current channel is closer to endpoint and it's changed at least 200 points, this is our channel
						closestToEndPoint = percentFromMin;
						//inChannelChanged = x;
					}
				}
				else
				{ //we're closer to max or we're in the middle
					if (percentFromMin > closestToEndPoint )
					{ //if current channel is closer to endpoint and it's changed at least 200 points, this is our channel
						closestToEndPoint = percentFromMax;
						//inChannelChanged = x;
					}
				}
				if (closestToEndPoint < 0.05)
				{ //at least within 5% of endpoint
					inChannelChanged = x;
				}

			}

		}

	}

	return(inChannelChanged);

}

void WizRcCheckMinMax(void)
{
	uint32_t x;

	for (x = 0;x<MAXCHANNELS;x++)
	{
		//for each channel we check if the current value is smaller than the set min value
		if (rxData[x] < tempRc.minRc[x])
		{
			tempRc.minRc[x] = (volatile unsigned int)rxData[x];
		}

		//for each channel we check if the current value is larger than the set max value
		if (rxData[x] > tempRc.maxRc[x])
		{
			tempRc.maxRc[x] = (volatile unsigned int)rxData[x];
		}

		//we just brute force set these for later use.
		tempRc.midRc[x] = (volatile unsigned int)rxData[x];
		tempRc.channelMap[x] = 1000;
	}
}

void WizRcCheckCenter(void)
{
	uint32_t x;

	for (x = 0;x<MAXCHANNELS;x++)
	{

		if ( tempRc.maxRc[x] != 1000000 )
		{
			mainConfig.rcControlsConfig.midRc[x] = tempRc.midRc[x];
			mainConfig.rcControlsConfig.minRc[x] = tempRc.minRc[x];
			mainConfig.rcControlsConfig.maxRc[x] = tempRc.maxRc[x];

			if ( ( ABS((int32_t)mainConfig.rcControlsConfig.midRc[x] - (int32_t)mainConfig.rcControlsConfig.minRc[x]) < 10 ) )
			{ //looks like switch
				mainConfig.rcControlsConfig.midRc[x] = mainConfig.rcControlsConfig.minRc[x]; //set center to min RC
			}
			else if ( ( ABS((int32_t)mainConfig.rcControlsConfig.midRc[x] - (int32_t)mainConfig.rcControlsConfig.maxRc[x]) < 10 ) )
			{ //looks like switch
				mainConfig.rcControlsConfig.midRc[x] = mainConfig.rcControlsConfig.minRc[x]; //set center to min RC
			}

		}

	}

}

static int32_t WizRcSetChannelMapAndDirection(uint32_t inChannel, uint32_t outChannel)
{

	int32_t channelCheck = ( rxData[inChannel] < (mainConfig.rcControlsConfig.maxRc[inChannel] - 300) ); //channel is reversed

	if (mainConfig.rcControlsConfig.channelMap[inChannel] == 1000)
	{ //if channelMap for the inChannel is 50 than it's waiting to be assigned.

		mainConfig.rcControlsConfig.channelMap[inChannel] = outChannel; //set channel map

		if ( channelCheck )
		{ //min is higher so channel is reversed, reverse if needed
			channelCheck = (int32_t)mainConfig.rcControlsConfig.maxRc[inChannel];
			mainConfig.rcControlsConfig.maxRc[inChannel] = mainConfig.rcControlsConfig.minRc[inChannel];
			mainConfig.rcControlsConfig.minRc[inChannel] = (uint32_t)channelCheck;
		}
		return(1);

	}
	return(-1);

}

int32_t WizRcCheckAndSetChannel(uint32_t outChannel)
{
	int32_t changedInChannel = -1;
	int32_t throttleFix = 0;

	changedInChannel = WizRcWhichInChannelChange();

	if (changedInChannel > -1)
	{
		if (WizRcSetChannelMapAndDirection(changedInChannel, outChannel))
		{
			if ( (outChannel == THROTTLE) || (outChannel == AUX1) || (outChannel == AUX2) || (outChannel == AUX3) || (outChannel == AUX4) )
			{ //set mid point at center between extremes
				throttleFix = (int32_t)(((int32_t)mainConfig.rcControlsConfig.maxRc[changedInChannel] - (int32_t)mainConfig.rcControlsConfig.minRc[changedInChannel]) / 2);
				mainConfig.rcControlsConfig.midRc[changedInChannel] = (uint32_t)( throttleFix + (int32_t)mainConfig.rcControlsConfig.minRc[changedInChannel]);
			}
			return(1);
		}
		else
		{
			return(-1);
		}
	}
	else
	{
		return(-1);
	}
}

void WizRcCheckAndSendDirection(void)
{
	switch(wizardStatus.wicRcCheckDirection)
	{
		case WIZ_RC_THROTTLE_UP:
			if (WizRcCheckAndSetChannel(THROTTLE) > -1)
			{
				RfCustomReplyBuffer("#wiz Throttle Set");
				RfCustomReplyBuffer("#wiz Set Yaw To Right");
				wizardStatus.wicRcCheckDirection = WIZ_RC_YAW_RIGHT;
			}
			else
			{
				RfCustomReplyBuffer("#wiz Set Throttle To Top");
			}
			break;
		case WIZ_RC_YAW_RIGHT:
			if (WizRcCheckAndSetChannel(YAW) > -1)
			{
				RfCustomReplyBuffer("#wiz Yaw Set");
				RfCustomReplyBuffer("#wiz Set Pitch To Top");
				wizardStatus.wicRcCheckDirection = WIZ_RC_PITCH_UP;
			}
			else
			{
				RfCustomReplyBuffer("#wiz Set Yaw To Right");
			}
			break;
		case WIZ_RC_PITCH_UP:
			if (WizRcCheckAndSetChannel(PITCH) > -1)
			{
				RfCustomReplyBuffer("#wiz Pitch Set");
				RfCustomReplyBuffer("#wiz Set Roll To Right");
				wizardStatus.wicRcCheckDirection = WIZ_RC_ROLL_RIGHT;
			}
			else
			{
				RfCustomReplyBuffer("#wiz Set Pitch To Top");
			}
			break;
		case WIZ_RC_ROLL_RIGHT:
			if (WizRcCheckAndSetChannel(ROLL) > -1)
			{
				WizRcSetRestOfMap();
				skipRxMap = 0;
				RfCustomReplyBuffer("#wiz Roll Set");
				RfCustomReplyBuffer("#wiz RC Setup Complete");
				mainConfig.rcControlsConfig.rcCalibrated = 1;
				bzero(&wizardStatus, sizeof(wizardStatus)); //all done
				SaveAndSend();
			}
			else
			{
				RfCustomReplyBuffer("#wiz Set Roll To Right");
			}
			break;

	}
}

void HandleWizRc(void)
{

	DisarmBoard();
	switch(wizardStatus.currentStep)
	{
		case 0:
			PrepareRcWizard(); //set's current step to 1 after setting things up.
			//send reply that wizard has started
			RfCustomReplyBuffer("#wiz Move Sticks to extremes, center sticks, place throttle idle, then run wiz rc2");
			break;
		case 1:
			//Step 1 is running, record min max values every time this is run by the scheduler
			WizRcCheckMinMax();
			//todo: a report message here might be good
			break;
		case 2:
			//min max values are set, let's save them and now check centers
			WizRcCheckCenter();
			RfCustomReplyBuffer("#wiz Stick centers set, throttle idle set, run wiz rc3");
			ResetChannelCheck();
			break;
		case 3:
			//min max values are set, let's save them and now check centers
			WizRcCheckAndSendDirection();
			break;
		default:
			RfCustomReplyBuffer("#wiz Unknown Step, Wiz RC");
			break;

	}
	DisarmBoard();
}

uint32_t WizRxCheckProtocol(uint32_t rxProtocol, uint32_t usart)
{
	FeedTheDog();
	DisarmBoard();
	DeInitBoardUsarts();
	DelayMs(2);
	SetRxDefaults(rxProtocol,usart);
	DelayMs(2);
	InitFlight();
	bzero(rxData, sizeof(rxData));
	trueRcCommandF[0] = -1.1;
	trueRcCommandF[1] = -1.1;
	trueRcCommandF[2] = -1.1;
	trueRcCommandF[3] = -1.1;
	DisarmBoard();
	DelayMs(20);
	DisarmBoard();
	if (WizRxCheckRxDataLooksValid())
	{
		bzero(rxData, sizeof(rxData));
		trueRcCommandF[0] = -1.1;
		trueRcCommandF[1] = -1.1;
		trueRcCommandF[2] = -1.1;
		trueRcCommandF[3] = -1.1;
		DelayMs(40);
		//check twice after a reset of data checks
		if (WizRxCheckRxDataLooksValid())
		{
			bzero(rxData, sizeof(rxData));
			trueRcCommandF[0] = -1.1;
			trueRcCommandF[1] = -1.1;
			trueRcCommandF[2] = -1.1;
			trueRcCommandF[3] = -1.1;
			DelayMs(60);
			//check thrice after a reset of data checks
			if (WizRxCheckRxDataLooksValid())
			{
				//three times it looks good, most likely good data then
				return(1);
			}
		}
	}
	DelayMs(2);
	DisarmBoard();
	return(0);
}

void HandleWizRx(void)
{
	/*
	#define USING_MANUAL           0
	#define USING_SPEK_R           1
	#define USING_SPEK_T           2
	#define USING_SBUS_R           3
	#define USING_SBUS_T           4
	#define USING_SUMD_R           5
	#define USING_SUMD_T           6
	#define USING_IBUS_R           7
	#define USING_IBUS_T           8
	#define USING_CPPM_R           9
	#define USING_CPPM_T           10
	#define USING_DSM2_R           11
	#define USING_DSM2_T           12
	#define USING_RX_END           13
	 */
	uint32_t x, y, z = 0;

	for (x=1;x<USING_RX_END;x++)
	{
		for (y=0;y<MAX_USARTS;y++)
		{
			if (z == wizardStatus.currentStep)
			{
				wizardStatus.currentStep++;
				if (WizRxCheckProtocol(x,y))
				{
					snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me RX Protocol %lu Found on Usart %lu",x, y+1 );
					RfCustomReplyBuffer(rf_custom_out_buffer);
					wizardStatus.currentWizard = 0;
					wizardStatus.currentStep = 0;
					SetMode(M_ARMED, 4, 50, 100);
					resetBoard = 1;
					mainConfig.rcControlsConfig.rcCalibrated = 1;
					SaveAndSend();
					return;
				}
				else
				{
					snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Scanning for RX... %lu",z+1 );
					RfCustomReplyBuffer(rf_custom_out_buffer);
					return;
				}
			}
			z++;
		}
	}
	snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me RX Not Found" );
	RfCustomReplyBuffer(rf_custom_out_buffer);
	wizardStatus.currentWizard = 0;
	wizardStatus.currentStep = 0;
}

void SetupWizard(char *inString)
{

	if (!strcmp("rx", inString))
	{
		if (wizardStatus.currentWizard != WIZ_RX)
		{
			wizardStatus.currentWizard = WIZ_RX;
			wizardStatus.currentStep = 0;
		}
		HandleWizRx();
		return;
	}
	else if (!strcmp("mixera", inString))
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
		MassEraseDataFlash(0);
		mainConfig.gyroConfig.boardCalibrated = 0;
		mainConfig.gyroConfig.gyroRotation = CW0;
		DelayMs(200); //need to reset calibration and give ACC data time to refresh
		if (SetCalibrate1()) {
			RfCustomReplyBuffer("calibrate1finished");
		} else {
			RfCustomReplyBuffer("calibrationfailed");
		}
		return;
	}
	else if (!strcmp("calb", inString))
	{

		if (SetCalibrate2())
		{
			RfCustomReplyBuffer("calibrate2finished");
			SaveAndSend();
			return;
		} else {
			RfCustomReplyBuffer("calibrationfailed");
			return;
		}

	}
	else if (!strcmp("rc1", inString))
	{
		bzero(&wizardStatus, sizeof(wizardStatus));
		HandleWizRc();
		return;
	}
	else if (!strcmp("rc2", inString))
	{
		wizardStatus.currentStep = 2;
		HandleWizRc();
		return;
	}
	else if (!strcmp("rc3", inString))
	{
		wizardStatus.currentStep = 3;
		HandleWizRc();
		return;
	}
	else if (!strcmp("mot1", inString))
	{

		calibrateMotors = 1;
		SKIP_GYRO=1;
		DisarmBoard();
		motorOutput[0] = 1.0;
		OutputActuators(motorOutput, servoOutput);
		if (CheckSafeMotors(1000, 10000)) { //check for safe motors for 3 seconds, 10000 standard deviation allowed
			RfCustomReplyBuffer("#me Plug in battery, run wiz mot2 when tones finish");
			return;
		} else {
			bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
			memcpy(rf_custom_out_buffer, "motorcalibrationfailed", sizeof("motorcalibrationfailed"));
			RfCustomReplyBuffer(rf_custom_out_buffer);
			InitWatchdog(WATCHDOG_TIMEOUT_2S);
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
		}

	}
	else if (!strcmp("mot2", inString))
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
		InitWatchdog(WATCHDOG_TIMEOUT_2S);
		SKIP_GYRO=0;
		calibrateMotors = 0;

		RfCustomReplyBuffer("#me Motor Calibration Success");

	}
	else
	{
		bzero(rf_custom_out_buffer,RF_BUFFER_SIZE);
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Unknown Argument:%s", inString);
		RfCustomReplyBuffer(rf_custom_out_buffer);
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
	uint32_t y;
	char *modString = NULL;
	char *args = NULL;
	uint32_t motorNumber;
	uint32_t maxMotors;
	uint32_t modStringLength;
	uint32_t outputNumber;
	uint32_t somethingHappened = 0;
	uint32_t verbose = 1;
	uint32_t doingAuto = 0;
	uint32_t doingSettings = 0;

	if (!strcmp("start", inString) || !strcmp("read", inString) || !strcmp("check", inString) || !strcmp("settings", inString) || !strcmp("auto", inString))
	{

		if (!strcmp("check", inString))
		{
			verbose = 0;
		}

		if (!strcmp("auto", inString))
		{
			doingAuto = 1;
		}
		if (!strcmp("settings", inString))
		{
			doingSettings = 1;
		}

		oneWireActive = 1;
		if (verbose)
		{
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Reading ESCs...");
			RfCustomReplyBuffer(rf_custom_out_buffer);
			DelayMs(5);
		}
		if (OneWireInit() == 0)
		{
			if (verbose)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "No ESCs detected. Is your battery connected?");
				RfCustomReplyBuffer(rf_custom_out_buffer);
				DelayMs(5);
			}

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
						if (verbose)
						{
							snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu: %u.%u, %s, %s, %s", x, (uint8_t)( (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].version >> 8) & 0xFF), (uint8_t)(escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].version & 0xFF), escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].nameStr, escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].fwStr, escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].versionStr);
							RfCustomReplyBuffer(rf_custom_out_buffer);
						}
						if (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].escHexLocation.version > escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].version) {
							if (verbose)
							{
								snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu: Upgrade to version %u.%u is available", x, (uint8_t)( (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].escHexLocation.version >> 8) & 0xFF), (uint8_t)( escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].escHexLocation.version & 0xFF) );
								RfCustomReplyBuffer(rf_custom_out_buffer);
							}
							else
							{
								snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "upgrade available" );
								RfCustomReplyBuffer(rf_custom_out_buffer);
								return;
							}
						}
					}
					else
					{
						if (verbose)
						{
							snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu Unreadable", x);
							RfCustomReplyBuffer(rf_custom_out_buffer);
						}
					}
				}
				else
				{
					if (verbose)
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu Disabled", x);
						RfCustomReplyBuffer(rf_custom_out_buffer);
					}
				}

			}

			if (!verbose)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "no upgrade available");
				RfCustomReplyBuffer(rf_custom_out_buffer);
				return;
			}
			if ( doingSettings || doingAuto )
			{
				for (x = 0; x < MAX_MOTOR_NUMBER; x++)
				{
					outputNumber = mainConfig.mixerConfig.motorOutput[x];
					if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
					{
						//snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Saving Motor %lu Config.", x);
						//RfCustomReplyBuffer(rf_custom_out_buffer);

						if (OneWireSaveConfig(board.motors[outputNumber]))
						{
							snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu Config Read.", x);
							RfCustomReplyBuffer(rf_custom_out_buffer);
						}
						else
						{
							snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Error Motor %lu Config.", x);
							RfCustomReplyBuffer(rf_custom_out_buffer);
						}

					}

				}
			}
			if (doingAuto)
			{
				inString = "ma=upgrade";
				OneWire(inString);
			}

			snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "1wiredumpstart");
			RfCustomReplyBuffer(rf_custom_out_buffer);
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
							bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "NONE");
							RfCustomReplyBuffer(rf_custom_out_buffer);
							bytesWritten = 0;
							continue;
						}

						// add the readable form of the parameter value to the buffer
						if (parameter->parameterNamed)
						{
							bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "%s", OneWireParameterValueToName(parameter->parameterNamed, value));
							RfCustomReplyBuffer(rf_custom_out_buffer);
							bytesWritten = 0;
						}
						else
						{
							bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "%d", OneWireParameterValueToNumber(parameter->parameterNumerical, value));
							RfCustomReplyBuffer(rf_custom_out_buffer);
							bytesWritten = 0;
						}

					}

				}

			}

			snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "1wiredumpcomplete");
			RfCustomReplyBuffer(rf_custom_out_buffer);
			bytesWritten = 0;

		}

	}
	else if (!strcmp("list", inString))
	{
		if (!ListAllEscHexesInFlash())
		{
			snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "No ESC Hexes found in flash");
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	}
	else if (!strcmp("config", inString))
	{

		snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "1wiredumpstart");
		RfCustomReplyBuffer(rf_custom_out_buffer);
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
						bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "NONE");
						RfCustomReplyBuffer(rf_custom_out_buffer);
						bytesWritten = 0;
						continue;
					}

					// add the readable form of the parameter value to the buffer
					if (parameter->parameterNamed)
					{
						bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "%s", OneWireParameterValueToName(parameter->parameterNamed, value));
						RfCustomReplyBuffer(rf_custom_out_buffer);
						bytesWritten = 0;
					}
					else
					{
						bytesWritten += snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "%d", OneWireParameterValueToNumber(parameter->parameterNumerical, value));
						RfCustomReplyBuffer(rf_custom_out_buffer);
						bytesWritten = 0;
					}

				}

			}

		}

		snprintf(rf_custom_out_buffer+bytesWritten, RF_BUFFER_SIZE-bytesWritten, "1wiredumpcomplete");
		RfCustomReplyBuffer(rf_custom_out_buffer);
		bytesWritten = 0;

	}
	else if (!strcmp("save", inString))
	{

		for (x = 0; x < MAX_MOTOR_NUMBER; x++)
		{
			outputNumber = mainConfig.mixerConfig.motorOutput[x];
			if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Saving Motor %lu Config.", x);
				RfCustomReplyBuffer(rf_custom_out_buffer);

				if (OneWireSaveConfig(board.motors[outputNumber]))
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Motor %lu Config Saved.", x);
					RfCustomReplyBuffer(rf_custom_out_buffer);
				}
				else
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Error Saving Motor %lu Config.", x);
					RfCustomReplyBuffer(rf_custom_out_buffer);
				}

			}

		}

	}
	else if (!strcmp("stop", inString))
	{
		oneWireActive = 0;
		OneWireDeinit();
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "1Wire Session Ended.");
		RfCustomReplyBuffer(rf_custom_out_buffer);
	}
	else
	{
		uint32_t forceUpgrade = 0;
		uint32_t normalUpgrade = 0;

		if ( (inString[0] == 'm') && (inString[2] == '=') ) {

			if (inString[1] == 'a' )
			{
				y=0;
				maxMotors=4;
			}
			else
			{
				y=atoi(&inString[1]);
				maxMotors=y+1;
			}

			modString = inString+3;

			StripSpaces(modString);

			modStringLength = strlen(modString);

			for (x = 0; x < modStringLength; x++)
			{
				if (modString[x] == '=')
					break;
			}

			if (modStringLength > x)
			{
				args = modString + x + 1;
			}

			modString[x] = 0;

			for (x = 0; x < strlen(modString); x++)
				modString[x] = tolower((unsigned char)modString[x]);

			for (x = 0; x < strlen(args); x++)
				args[x] = tolower((unsigned char)args[x]);

			if ( (inString[1] == 'a' ) && !strcmp("forceupgrade", modString) )
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Automatic Force Upgrading Not Allowed");
				RfCustomReplyBuffer(rf_custom_out_buffer);
				return;
			}
			else
			{
				if (!strcmp("upgrade", modString))
				{
					normalUpgrade = 1;
				}
				else if (!strcmp("forceupgrade", modString))
				{
					forceUpgrade = 1;
				}
			}

			for (motorNumber=y;motorNumber<maxMotors;motorNumber++)
			{
				outputNumber = mainConfig.mixerConfig.motorOutput[motorNumber];


				if ( normalUpgrade || forceUpgrade)
				{
					if (forceUpgrade)
					{
						if (atoi(args) < 50)
						{
							memcpy(&escHexLocation, &escHexByPosition[atoi(args)], sizeof(esc_hex_location));
						}
						else
						{
							snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ESC %lu upgrade will not progress. ESC Hex not found.", motorNumber);
							RfCustomReplyBuffer(rf_custom_out_buffer);
							somethingHappened=1;
							continue;
						}
					}
					else
					{
						memcpy(&escHexLocation, &escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].escHexLocation, sizeof(esc_hex_location));
					}

					if ( (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR) && (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].enabled) )
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "Upgrading %lu...", motorNumber);
						RfCustomReplyBuffer(rf_custom_out_buffer);
						somethingHappened=1;
						if ( BuiltInUpgradeSiLabsBLHeli(board.motors[outputNumber], escHexLocation) )
						{
							snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ESC %lu upgrade complete.", motorNumber);
							RfCustomReplyBuffer(rf_custom_out_buffer);
						}
						else
						{
							snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ESC %lu not upgraded.", motorNumber);
							RfCustomReplyBuffer(rf_custom_out_buffer);
						}
					}
					else
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "No upgrade available for ESC %lu", motorNumber);
						RfCustomReplyBuffer(rf_custom_out_buffer);
						somethingHappened=1;
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
									snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ERROR=Unknown parameter value");
									RfCustomReplyBuffer(rf_custom_out_buffer);
									somethingHappened=1;
									return;
								}
								else
								{
									if ( Esc1WireSetParameter(board.motors[outputNumber], parameter, escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].config, value) )
									{
										snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ESC %lu Value Set! Please Save Changes!", motorNumber);
										RfCustomReplyBuffer(rf_custom_out_buffer);
										somethingHappened=1;
										continue;
									}
									else
									{
										snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ERROR=Unknown ESC Layout");
										RfCustomReplyBuffer(rf_custom_out_buffer);
										somethingHappened=1;
										continue;
									}

								}

							}

						}

					}

				}

			}

		}
		if (!somethingHappened)
		{
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ERROR=Unknown Command");
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}

	}

}
