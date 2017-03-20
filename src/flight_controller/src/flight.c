#include "includes.h"

#define GYRO_AVERAGE_MAX_SUM 33

pid_output   flightPids[AXIS_NUMBER];
biquad_state lpfFilterState[AXIS_NUMBER];
biquad_state lpfFilterStateKd[AXIS_NUMBER];
biquad_state lpfFilterStateAcc[AXIS_NUMBER];
//biquad_state lpfFilterStateNoise[6];
biquad_state hpfFilterStateAcc[6];

#define GYRO_STD_DEVIATION_SAMPLE_SIZE 100

float gyroStdDeviationSamples[AXIS_NUMBER][GYRO_STD_DEVIATION_SAMPLE_SIZE];
float gyroStdDeviationTotals[AXIS_NUMBER][GYRO_STD_DEVIATION_SAMPLE_SIZE];
uint32_t gyroStdDeviationPointer;
uint32_t gyroStdDeviationTotalsPointer;

float kdFiltUsed[AXIS_NUMBER];
float accNoise[6];
float averagedGyroData[AXIS_NUMBER][GYRO_AVERAGE_MAX_SUM];
uint32_t averagedGyroDataPointer[AXIS_NUMBER];
float averagedGyroDataPointerMultiplier[AXIS_NUMBER];
float filteredGyroData[AXIS_NUMBER];
float filteredAccData[VECTOR_NUMBER];
paf_state pafGyroStates[AXIS_NUMBER];
paf_state pafKdStates[AXIS_NUMBER];
paf_state pafAccStates[AXIS_NUMBER];
float actuatorRange;
float flightSetPoints[AXIS_NUMBER];
volatile uint32_t boardArmed, calibrateMotors, fullKiLatched;
float currentGyroFilterConfig[AXIS_NUMBER];
float currentKdFilterConfig[AXIS_NUMBER];
float currentAccFilterConfig[AXIS_NUMBER];
volatile uint32_t flightcodeTimeStart;
volatile float flightcodeTime;
float pitchAttitude = 0, rollAttitude = 0, yawAttitude = 0;
uint32_t boardOrientation1 = 0;
uint32_t RfblDisasterPreventionCheck = 1;
uint32_t timeSinceSelfLevelActivated;
float slpUsed;
float sliUsed;
float sldUsed;

float accCompAccTrust, accCompGyroTrust;
uint32_t khzLoopCounter = 0;
uint32_t gyroLoopCounter = 0;
volatile uint32_t SKIP_GYRO = 0;
volatile float rollAttitudeError = 0;
volatile float pitchAttitudeError = 0;
volatile float rollAttitudeErrorKi = 0;
volatile float pitchAttitudeErrorKi = 0;

volatile float rollAttitudeErrorKdelta = 0;
volatile float lastRollAttitudeError   = 0;
volatile float pitchAttitudeErrorKdelta = 0;
volatile float lastPitchAttitudeError   = 0;

uint32_t usedGa[AXIS_NUMBER];

//these numbers change based on loop_control
volatile loop_speed_record loopSpeed;


void ArmBoard(void)
{
	InitWatchdog(WATCHDOG_TIMEOUT_2S);
	boardArmed = 1;
	timeSinceSelfLevelActivated = 0;
}

void DisarmBoard(void)
{
	boardArmed = 0;
	InitWatchdog(WATCHDOG_TIMEOUT_32S);
}

int SetCalibrate1(void)
{

	if ( ABS(filteredAccData[ACCZ]) > (ABS(filteredAccData[ACCX]) + ABS(filteredAccData[ACCY])) )
	{
		// is king
		if (filteredAccData[ACCZ] < -0.8) { //ACCZ negative
			//board inverted
			return (boardOrientation1 = CALIBRATE_BOARD_INVERTED);
		} else if (filteredAccData[ACCZ] > 0.8) { //ACCZ positive
			//board upright
			return (boardOrientation1 = CALIBRATE_BOARD_UPRIGHT);
		}

	}

	return (CALIBRATE_BOARD_FAILED); //check sanity and return 0 if result not sane.

}

int SetCalibrate2(void)
{

	if (!boardOrientation1)
	{ //make sure step one completed successfully.
		return (0);
	}


	if (boardOrientation1 == CALIBRATE_BOARD_UPRIGHT)
	{
		if (filteredAccData[ACCX] < -0.9)
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW0;  //set proper rotation
		}
		else if (filteredAccData[ACCY] < -0.9)
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW90;  //set proper rotation
		}
		else if (filteredAccData[ACCX] > 0.9)
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW180;  //set proper rotation
		}
		else if (filteredAccData[ACCY] > 0.9)
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW270;  //set proper rotation
		}
		else if ((filteredAccData[ACCX] < -0.6) && (filteredAccData[ACCY] < -0.6))
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW45;  //set proper rotation
		}
		else if ((filteredAccData[ACCX] > 0.6) && (filteredAccData[ACCY] < -0.6))
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW135;  //set proper rotation
		}
		else if ((filteredAccData[ACCX] > 0.6) && (filteredAccData[ACCY] > 0.6))
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW225;  //set proper rotation
		}
		else if ((filteredAccData[ACCX] < -0.6) && (filteredAccData[ACCY] > 0.6))
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW315;  //set proper rotation
		}
		else
		{
			boardOrientation1 = 0;
			return (CALIBRATE_BOARD_FAILED);
		}
	}
	else
	{
		if (filteredAccData[ACCX] < -0.9)
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW180_INV;  //set proper rotation
		}
		else if (filteredAccData[ACCY] < -0.9)
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW90_INV;  //set proper rotation
		}
		else if (filteredAccData[ACCX] > 0.9)
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW0_INV;  //set proper rotation
		}
		else if (filteredAccData[ACCY] > 0.9)
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW270_INV;  //set proper rotation
		}
		else if ((filteredAccData[ACCX] < -0.6) && (filteredAccData[ACCY] < -0.6))
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW315_INV;  //set proper rotation
		}
		else if ((filteredAccData[ACCX] > 0.6) && (filteredAccData[ACCY] < -0.6))
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW225_INV;  //set proper rotation
		}
		else if ((filteredAccData[ACCX] > 0.6) && (filteredAccData[ACCY] > 0.6))
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW135_INV;  //set proper rotation
		}
		else if ((filteredAccData[ACCX] < -0.6) && (filteredAccData[ACCY] > 0.6))
		{
			boardOrientation1 = 0; //done calibrating
			mainConfig.gyroConfig.boardCalibrated = 1; //set board as calibrated
			mainConfig.gyroConfig.gyroRotation = CW45_INV;  //set proper rotation
		}
		else
		{
			boardOrientation1 = 0;
			return (CALIBRATE_BOARD_FAILED);
		}
	}

	return(1);

}

void InitFlightCode(void)
{

	uint32_t validLoopConfig = 0;

	kdFiltUsed[YAW]   = mainConfig.filterConfig[YAW].kd.r;
	kdFiltUsed[ROLL]  = mainConfig.filterConfig[ROLL].kd.r;
	kdFiltUsed[PITCH] = mainConfig.filterConfig[PITCH].kd.r;

	//bzero(lpfFilterStateNoise,sizeof(lpfFilterStateNoise));
	bzero(lpfFilterState,sizeof(lpfFilterState));
	bzero(lpfFilterStateKd,sizeof(lpfFilterStateKd));
	bzero(averagedGyroData,sizeof(averagedGyroData));
	bzero(filteredGyroData,sizeof(filteredGyroData));
	bzero(averagedGyroDataPointer,sizeof(averagedGyroDataPointer));
	bzero(&flightPids,sizeof(flightPids));
	bzero(gyroStdDeviationSamples, sizeof(gyroStdDeviationSamples));
	bzero(gyroStdDeviationTotals, sizeof(gyroStdDeviationTotals));
	gyroStdDeviationPointer = 0;
	gyroStdDeviationTotalsPointer = 0;
	timeSinceSelfLevelActivated = 0;
	slpUsed = 0.0f;
	sliUsed = 0.0f;
	sldUsed = 0.0f;

	averagedGyroDataPointerMultiplier[YAW]   = (1.0 / (float)mainConfig.pidConfig[YAW].ga);
	averagedGyroDataPointerMultiplier[ROLL]  = (1.0 / (float)mainConfig.pidConfig[ROLL].ga);
	averagedGyroDataPointerMultiplier[PITCH] = (1.0 / (float)mainConfig.pidConfig[PITCH].ga);

	usedGa[0] = mainConfig.pidConfig[0].ga;
	usedGa[1] = mainConfig.pidConfig[1].ga;
	usedGa[2] = mainConfig.pidConfig[2].ga;

	//Sanity Check!: make sure ESC Frequency, protocol and looptime gel:
	if ( (!validLoopConfig) && (mainConfig.mixerConfig.escUpdateFrequency >= 32000) )
	{
		mainConfig.mixerConfig.escUpdateFrequency = 32000;
		mainConfig.gyroConfig.loopCtrl = LOOP_UH32;
		if ( (mainConfig.mixerConfig.escProtocol == ESC_DSHOT1200) || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT25)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT125) )
		{
			validLoopConfig = 1;
		}
	}
	if ( (!validLoopConfig) && (mainConfig.mixerConfig.escUpdateFrequency >= 16000) )
	{
		mainConfig.mixerConfig.escUpdateFrequency = 16000;
		mainConfig.gyroConfig.loopCtrl = LOOP_UH16;
		if ( (mainConfig.mixerConfig.escProtocol == ESC_DSHOT600) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT1200) || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT25)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT125) )
		{
			validLoopConfig = 1;
		}
	}
	if ( (!validLoopConfig) && (mainConfig.mixerConfig.escUpdateFrequency >= 8000) )
	{
		mainConfig.mixerConfig.escUpdateFrequency = 8000;
		mainConfig.gyroConfig.loopCtrl = LOOP_UH8;
		if ( (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT42) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT300) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT600) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT1200) || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT25)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT125) )
		{
			validLoopConfig = 1;
		}
	}
	if ( (!validLoopConfig) && (mainConfig.mixerConfig.escUpdateFrequency >= 4000) )
	{
		mainConfig.mixerConfig.escUpdateFrequency = 4000;
		mainConfig.gyroConfig.loopCtrl = LOOP_UH4;
		if ( (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT) || (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT42) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT150) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT300) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT600) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT1200) || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT25)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT125) )
		{
			validLoopConfig = 1;
		}
	}
	if ( (!validLoopConfig) && (mainConfig.mixerConfig.escUpdateFrequency >= 2000) )
	{
		mainConfig.mixerConfig.escUpdateFrequency = 2000;
		mainConfig.gyroConfig.loopCtrl = LOOP_UH2;
		if ( (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT) || (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT42) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT150) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT300) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT600) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT1200) || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT25)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT125) )
		{
			validLoopConfig = 1;
		}
	}
	if ( (!validLoopConfig) && (mainConfig.mixerConfig.escUpdateFrequency >= 1000) )
	{
		mainConfig.mixerConfig.escUpdateFrequency = 1000;
		mainConfig.gyroConfig.loopCtrl = LOOP_UH1;
		if ( (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT) || (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT42) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT150) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT300) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT600) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT1200) || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT25)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT125) )
		{
			validLoopConfig = 1;
		}
	}
	if ( (!validLoopConfig) && (mainConfig.mixerConfig.escUpdateFrequency >= 500) )
	{
		mainConfig.mixerConfig.escUpdateFrequency = 500;
		mainConfig.gyroConfig.loopCtrl = LOOP_UH_500;
		if ( (mainConfig.mixerConfig.escProtocol == ESC_PWM) || (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT) || (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT42) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT150) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT300) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT600) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT1200) || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT25)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT125) )
		{
			validLoopConfig = 1;
		}
	}
	if ( (!validLoopConfig) && (mainConfig.mixerConfig.escUpdateFrequency >= 250) )
	{
		mainConfig.mixerConfig.escUpdateFrequency = 250;
		mainConfig.gyroConfig.loopCtrl = LOOP_UH_250;
		if ( (mainConfig.mixerConfig.escProtocol == ESC_PWM) || (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT) || (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT42) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT150) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT300) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT600) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT1200) || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT25)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT125) )
		{
			validLoopConfig = 1;
		}
	}
	if ( (!validLoopConfig) && (mainConfig.mixerConfig.escUpdateFrequency >= 62) )
	{
		mainConfig.mixerConfig.escUpdateFrequency = 62;
		mainConfig.gyroConfig.loopCtrl = LOOP_UH_062;
		if ( (mainConfig.mixerConfig.escProtocol == ESC_PWM) || (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT) || (mainConfig.mixerConfig.escProtocol == ESC_ONESHOT42) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT150) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT300) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT600) || (mainConfig.mixerConfig.escProtocol == ESC_DSHOT1200) || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT25)  || (mainConfig.mixerConfig.escProtocol == ESC_MULTISHOT125) )
		{
			validLoopConfig = 1;
		}
	}

	switch (mainConfig.gyroConfig.loopCtrl) {
		case LOOP_UH32:
		case LOOP_H32:
			loopSpeed.gyrodT      = 0.00003125;
			loopSpeed.dT          = 0.00003125;
			loopSpeed.uhohNumber  = 24000;
			loopSpeed.gyroDivider = 1;
			loopSpeed.khzDivider  = 32;
			loopSpeed.gyroAccDiv  = 8; //gyro and acc still run at full speed
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
			break;
		case LOOP_UH16:
		case LOOP_H16:
			loopSpeed.gyrodT      = 0.00003125;
			loopSpeed.dT          = 0.00006250;
			loopSpeed.uhohNumber  = 12000;
			loopSpeed.gyroDivider = 2;
			loopSpeed.khzDivider  = 16;
			loopSpeed.gyroAccDiv  = 8; //gyro and acc still run at full speed
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
			break;
		case LOOP_UH8:
			loopSpeed.gyrodT      = 0.00003125;
			loopSpeed.dT          = 0.00012500;
			loopSpeed.uhohNumber  = 6000;
			loopSpeed.gyroDivider = 4;
			loopSpeed.khzDivider  = 8;
			loopSpeed.gyroAccDiv  = 8; //gyro and acc still run at full speed
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
			break;
		case LOOP_H8:
		case LOOP_M8:
			loopSpeed.gyrodT      = 0.00012500;
			loopSpeed.dT          = 0.00012500;
			loopSpeed.uhohNumber  = 6000;
			loopSpeed.gyroDivider = 1;
			loopSpeed.khzDivider  = 8;
			loopSpeed.gyroAccDiv  = 2;
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
			break;
		case LOOP_UH4:
			loopSpeed.gyrodT      = 0.00003125;
			loopSpeed.dT          = 0.00025000;
			loopSpeed.uhohNumber  = 3000;
			loopSpeed.gyroDivider = 8;
			loopSpeed.khzDivider  = 4;
			loopSpeed.gyroAccDiv  = 8;
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
			break;
		case LOOP_H4:
		case LOOP_M4:
			loopSpeed.gyrodT      = 0.00012500;
			loopSpeed.dT          = 0.00025000;
			loopSpeed.uhohNumber  = 3000;
			loopSpeed.gyroDivider = 2;
			loopSpeed.khzDivider  = 4;
			loopSpeed.gyroAccDiv  = 2;
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
			break;
		case LOOP_UH2:
			loopSpeed.gyrodT      = 0.00003125;
			loopSpeed.dT          = 0.00050000;
			loopSpeed.uhohNumber  = 1500;
			loopSpeed.gyroDivider = 16;
			loopSpeed.khzDivider  = 2;
			loopSpeed.gyroAccDiv  = 8;
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
		case LOOP_H2:
		case LOOP_M2:
			loopSpeed.gyrodT      = 0.00012500;
			loopSpeed.dT          = 0.00050000;
			loopSpeed.uhohNumber  = 1500;
			loopSpeed.gyroDivider = 4;
			loopSpeed.khzDivider  = 2;
			loopSpeed.gyroAccDiv  = 2;
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
			break;
		case LOOP_UH1:
			loopSpeed.gyrodT      = 0.00003125;
			loopSpeed.dT          = 0.00100000;
			loopSpeed.uhohNumber  = 750;
			loopSpeed.gyroDivider = 32;
			loopSpeed.khzDivider  = 1;
			loopSpeed.gyroAccDiv  = 8;
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
			break;
		case LOOP_UH_500:
			loopSpeed.gyrodT      = 0.00003125;
			loopSpeed.dT          = 0.00200000;
			loopSpeed.uhohNumber  = 375;
			loopSpeed.gyroDivider = 64;
			loopSpeed.khzDivider  = 1;
			loopSpeed.gyroAccDiv  = 8;
			loopSpeed.fsCount     = 250; //failsafe count for khzdivider
			break;
		case LOOP_UH_250:
			loopSpeed.gyrodT      = 0.00003125;
			loopSpeed.dT          = 0.00400000;
			loopSpeed.uhohNumber  = 187;
			loopSpeed.gyroDivider = 128;
			loopSpeed.khzDivider  = 1;
			loopSpeed.gyroAccDiv  = 8;
			loopSpeed.fsCount     = 125; //failsafe count for khzdivider
			break;
		case LOOP_UH_062:
			loopSpeed.gyrodT      = 0.00003125;
			loopSpeed.dT          = 0.01600000;
			loopSpeed.uhohNumber  = 94;
			loopSpeed.gyroDivider = 512;
			loopSpeed.khzDivider  = 1;
			loopSpeed.gyroAccDiv  = 8;
			loopSpeed.fsCount     = 62; //failsafe count for khzdivider
			break;
		case LOOP_H1:
		case LOOP_M1:
			loopSpeed.gyrodT      = 0.00012500;
			loopSpeed.dT          = 0.00100000;
			loopSpeed.uhohNumber  = 750;
			loopSpeed.gyroDivider = 8;
			loopSpeed.khzDivider  = 1;
			loopSpeed.gyroAccDiv  = 2;
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
			break;
		case LOOP_L1:
		default:
			loopSpeed.gyrodT      = 0.00100000;
			loopSpeed.dT          = 0.00100000;
			loopSpeed.uhohNumber  = 750;
			loopSpeed.gyroDivider = 1;
			loopSpeed.khzDivider  = 1;
			loopSpeed.gyroAccDiv  = 1;
			loopSpeed.fsCount     = 500; //failsafe count for khzdivider
			break;
	}

	//TODO: gyroConfig.accDenom is not set until after gyro is running.
	//loopSpeed.accdT     = loopSpeed.gyrodT * gyroConfig.accDenom;
	loopSpeed.halfGyrodT = loopSpeed.gyrodT * 0.5f;
	loopSpeed.accdT      = loopSpeed.gyrodT * (float)loopSpeed.gyroAccDiv;
	loopSpeed.InversedT  = (1/loopSpeed.dT);



	actuatorRange   = 0;
	boardArmed      = 0;
	calibrateMotors = 0;
	fullKiLatched   = 0;
	flightcodeTime  = 0.0f;
	flightcodeTimeStart = 0;

	InlineInitGyroFilters();
	InlineInitKdFilters();
	InlineInitSpectrumNoiseFilter();
	InlineInitAccFilters();
	InitImu();
}

inline void InlineInitGyroFilters(void)
{

	int32_t axis;

	for (axis = 2; axis >= 0; --axis) {

		if (mainConfig.filterConfig[axis].gyro.r == 1)
			pafGyroStates[axis]   = InitPaf( mainConfig.filterConfig[axis].gyro.q, 88.0f, 0.0f, filteredGyroData[axis]);
		else
			pafGyroStates[axis]   = InitPaf( mainConfig.filterConfig[axis].gyro.q, mainConfig.filterConfig[axis].gyro.r, 0.1500, filteredGyroData[axis]);

		currentGyroFilterConfig[axis] = mainConfig.filterConfig[axis].gyro.r;

	}

}

inline void InlineInitKdFilters(void)
{

	int32_t axis;

	for (axis = 2; axis >= 0; --axis) {

		InitBiquad(kdFiltUsed[axis], &lpfFilterStateKd[axis], loopSpeed.gyrodT, FILTER_TYPE_LOWPASS, &lpfFilterStateKd[axis], 1.99f);

		currentKdFilterConfig[axis] = kdFiltUsed[axis];

	}

}

inline void InlineInitSpectrumNoiseFilter(void)
{

	//InitBiquad(075, &lpfFilterStateNoise[0], loopSpeed.accdT, FILTER_TYPE_PEEK, &lpfFilterStateNoise[0], 0.33333333333f);
	//InitBiquad(125, &lpfFilterStateNoise[1], loopSpeed.accdT, FILTER_TYPE_PEEK, &lpfFilterStateNoise[1], 0.20000000000f);
	//InitBiquad(175, &lpfFilterStateNoise[2], loopSpeed.accdT, FILTER_TYPE_PEEK, &lpfFilterStateNoise[2], 0.14285714292f);
	//InitBiquad(225, &lpfFilterStateNoise[3], loopSpeed.accdT, FILTER_TYPE_PEEK, &lpfFilterStateNoise[3], 0.11111111111f);
	//InitBiquad(275, &lpfFilterStateNoise[4], loopSpeed.accdT, FILTER_TYPE_PEEK, &lpfFilterStateNoise[4], 0.09090909091f);
	//InitBiquad(325, &lpfFilterStateNoise[5], loopSpeed.accdT, FILTER_TYPE_PEEK, &lpfFilterStateNoise[5], 0.07692307692f);

}

inline void InlineInitAccFilters(void)
{

	int32_t vector;

	for (vector = 2; vector >= 0; --vector) {

		InitBiquad(mainConfig.filterConfig[vector].acc.r, &lpfFilterStateAcc[vector], loopSpeed.accdT, FILTER_TYPE_LOWPASS, &lpfFilterStateAcc[vector], mainConfig.filterConfig[vector].acc.q);
		//pafAccStates[vector]   = InitPaf( mainConfig.filterConfig[vector].acc.q, mainConfig.filterConfig[vector].acc.r, mainConfig.filterConfig[vector].acc.p, filteredAccData[vector]);

		currentAccFilterConfig[vector] = mainConfig.filterConfig[vector].acc.r;

	}

}

inline void InlineUpdateAttitude(float geeForceAccArray[])
{

	//update gyro filter
	//PafUpdate(&pafAccStates[ACCX], geeForceAccArray[ACCX]);
	//PafUpdate(&pafAccStates[ACCY], geeForceAccArray[ACCY]);
	//PafUpdate(&pafAccStates[ACCZ], geeForceAccArray[ACCZ]);

	//filteredAccData[ACCX] = pafAccStates[ACCX].output;
	//filteredAccData[ACCY] = pafAccStates[ACCY].output;
	//filteredAccData[ACCZ] = pafAccStates[ACCZ].output;

	filteredAccData[ACCX] = BiquadUpdate(geeForceAccArray[ACCX], &lpfFilterStateAcc[ACCX]);
	filteredAccData[ACCY] = BiquadUpdate(geeForceAccArray[ACCY], &lpfFilterStateAcc[ACCY]);
	filteredAccData[ACCZ] = BiquadUpdate(geeForceAccArray[ACCZ], &lpfFilterStateAcc[ACCZ]);

	//accNoise[0] = BiquadUpdate(geeForceAccArray[ACCZ], &lpfFilterStateNoise[0]);
	//accNoise[1] = BiquadUpdate(geeForceAccArray[ACCZ], &lpfFilterStateNoise[1]);
	//accNoise[2] = BiquadUpdate(geeForceAccArray[ACCZ], &lpfFilterStateNoise[2]);
	//accNoise[3] = BiquadUpdate(geeForceAccArray[ACCZ], &lpfFilterStateNoise[3]);
	//accNoise[4] = BiquadUpdate(geeForceAccArray[ACCZ], &lpfFilterStateNoise[4]);
	//accNoise[5] = BiquadUpdate(geeForceAccArray[ACCZ], &lpfFilterStateNoise[5]);

}


inline void ComplementaryFilterUpdateAttitude(void)
{
	UpdateImu(filteredAccData[ACCX], filteredAccData[ACCY], filteredAccData[ACCZ], filteredGyroData[ROLL], filteredGyroData[PITCH], filteredGyroData[YAW]);
}

inline float AverageGyroADCbuffer(uint32_t axis, volatile float currentData)
{

	float returnData;
	if (usedGa[axis] > 1)
	{

		averagedGyroData[axis][averagedGyroDataPointer[axis]++] = currentData;
		averagedGyroData[axis][GYRO_AVERAGE_MAX_SUM-1] += currentData;

		if (averagedGyroDataPointer[axis] == usedGa[axis])
			averagedGyroDataPointer[axis] = 0;

		returnData = (averagedGyroData[axis][GYRO_AVERAGE_MAX_SUM-1] * averagedGyroDataPointerMultiplier[axis]);
		averagedGyroData[axis][GYRO_AVERAGE_MAX_SUM-1] -= averagedGyroData[axis][averagedGyroDataPointer[axis]];

		return(returnData);

	}

	return currentData;

}

inline void InlineFlightCode(float dpsGyroArray[])
{

	static float yy[3] = {0,0,0};
	static uint32_t gyroStdDeviationLatch = 0;
	int32_t axis;
	volatile float averagedGyro;

	//cycle time
	//flightcodeTimeStart = Micros();

	//Gyro routine:
	//gyro interrupts
	//gyro read using DMA
	//updateGyro is called after the read is complete.
	//gyro and board rotation is applied to gyro data in updateGyro
	//updateGyro will call InlineFlightCode.

	//InlineFlightCode:
	//gyro filter applied
	//rc smoothing and acro+ is applied to arrive at setPoint. actuatorRange is used to limit setPoint.
	//pid controller is run using setpoint and smoothed gyro data
	//mixer is applied and outputs it's status as actuatorRange
	//output to motors

	if (SKIP_GYRO)
	{
		FeedTheDog();
		ledStatus.status = LEDS_FAST_BLINK;
		return;
	}

	//update gyro filter, every time there's an interrupt
	for (axis = 2; axis >= 0; --axis)
	{

		averagedGyro = AverageGyroADCbuffer(axis, dpsGyroArray[axis]);

		PafUpdate(&pafGyroStates[axis], averagedGyro );
		filteredGyroData[axis] = pafGyroStates[axis].output;


	}

	//update PIDs, mixer, outputs at gyro divider speed
	if (gyroLoopCounter-- == 0)
	{

		gyroLoopCounter=loopSpeed.gyroDivider;

		//smooth the rx data between rx signals
		InlineRcSmoothing(curvedRcCommandF, smoothedRcCommandF);

		//1st, find request rates regardless of modes
		if (ModeActive(M_DIRECT))
		{
			flightSetPoints[YAW]   = InlineGetSetPoint(curvedRcCommandF[YAW], mainConfig.rcControlsConfig.useCurve[PITCH], mainConfig.rcControlsConfig.rates[YAW], mainConfig.rcControlsConfig.acroPlus[YAW] * 0.01, YAW); //yaw is backwards for some reason
			flightSetPoints[ROLL]  = InlineGetSetPoint(curvedRcCommandF[ROLL], mainConfig.rcControlsConfig.useCurve[PITCH], mainConfig.rcControlsConfig.rates[ROLL], mainConfig.rcControlsConfig.acroPlus[ROLL] * 0.01, ROLL);
			flightSetPoints[PITCH] = -InlineGetSetPoint(curvedRcCommandF[PITCH], mainConfig.rcControlsConfig.useCurve[PITCH], mainConfig.rcControlsConfig.rates[PITCH], mainConfig.rcControlsConfig.acroPlus[PITCH] * 0.01, PITCH);
		}
		else
		{
			flightSetPoints[YAW]   = InlineGetSetPoint(smoothedRcCommandF[YAW], mainConfig.rcControlsConfig.useCurve[PITCH], mainConfig.rcControlsConfig.rates[YAW], mainConfig.rcControlsConfig.acroPlus[YAW] * 0.01, YAW); //yaw is backwards for some reason
			flightSetPoints[ROLL]  = InlineGetSetPoint(smoothedRcCommandF[ROLL], mainConfig.rcControlsConfig.useCurve[PITCH], mainConfig.rcControlsConfig.rates[ROLL], mainConfig.rcControlsConfig.acroPlus[ROLL] * 0.01, ROLL);
			flightSetPoints[PITCH] = -InlineGetSetPoint(smoothedRcCommandF[PITCH], mainConfig.rcControlsConfig.useCurve[PITCH], mainConfig.rcControlsConfig.rates[PITCH], mainConfig.rcControlsConfig.acroPlus[PITCH] * 0.01, PITCH);
		}

		//get setpoint for PIDC
		if (ModeActive(M_ATTITUDE) || ModeActive(M_HORIZON)) //no auto level modes active, we're in rate mode
		{
			if (!timeSinceSelfLevelActivated)
				timeSinceSelfLevelActivated = InlineMillis();

			//todo move this check to the 1khz section
			//slowly ramp up self leveling over 100 milliseconds
			if ((InlineMillis() - timeSinceSelfLevelActivated) < 300)
			{
				slpUsed = mainConfig.pidConfig[PITCH].slp * (float)(InlineMillis() - timeSinceSelfLevelActivated) * 0.003333f;
				sliUsed = mainConfig.pidConfig[PITCH].sli * (float)(InlineMillis() - timeSinceSelfLevelActivated) * 0.003333f;
				sldUsed = mainConfig.pidConfig[PITCH].sld * (float)(InlineMillis() - timeSinceSelfLevelActivated) * 0.003333f;
			}
			else
			{
				slpUsed = mainConfig.pidConfig[PITCH].slp;
				sliUsed = mainConfig.pidConfig[PITCH].sli;
				sldUsed = mainConfig.pidConfig[PITCH].sld;
			}

			rollAttitudeError        = ( (trueRcCommandF[ROLL]  *  mainConfig.pidConfig[PITCH].sla ) - rollAttitude  );
			pitchAttitudeError       = ( (trueRcCommandF[PITCH] * -mainConfig.pidConfig[PITCH].sla ) - pitchAttitude );

			rollAttitudeErrorKi      = (rollAttitudeErrorKi  + rollAttitudeError  * sliUsed * loopSpeed.dT);
			pitchAttitudeErrorKi     = (pitchAttitudeErrorKi + pitchAttitudeError * sliUsed * loopSpeed.dT);

			rollAttitudeErrorKdelta  = -(rollAttitudeError  - lastRollAttitudeError);
			lastRollAttitudeError    = rollAttitudeError;
			pitchAttitudeErrorKdelta = -(pitchAttitudeError - lastPitchAttitudeError);
			lastPitchAttitudeError   = pitchAttitudeError;

			if (ModeActive(M_ATTITUDE)) //if M_ATTITUDE mode
			{
				//roll and pitch are set directly from self level mode
				flightSetPoints[ROLL]    = InlineConstrainf( (rollAttitudeError * slpUsed) + rollAttitudeErrorKi + (rollAttitudeErrorKdelta / loopSpeed.dT * sldUsed), -300.0, 300.0);
				flightSetPoints[PITCH]   = InlineConstrainf( (pitchAttitudeError * slpUsed) + pitchAttitudeErrorKi + (pitchAttitudeErrorKdelta / loopSpeed.dT * sldUsed), -300.0, 300.0);

			}
			else if (ModeActive(M_HORIZON)) //if M_HORIZON mode
			{
				//roll and pitch and modified by stick angle proportionally to stick angle
				if ( (ABS(trueRcCommandF[PITCH]) < 0.75) && (ABS(trueRcCommandF[ROLL]) < 0.75) && ABS(pitchAttitude) < 75 ) //prevent gimbal lock since PIDc uses euler angles
				{
					flightSetPoints[ROLL]    += InlineConstrainf( (rollAttitudeError * slpUsed) + rollAttitudeErrorKi + (rollAttitudeErrorKdelta / loopSpeed.dT * sldUsed), -300.0, 300.0) * (1.0f - ABS(trueRcCommandF[ROLL]) );
				}
				if ( (ABS(trueRcCommandF[PITCH]) < 0.75)  && (ABS(trueRcCommandF[ROLL]) < 0.75) )
					flightSetPoints[PITCH]   += InlineConstrainf( (pitchAttitudeError * slpUsed) + pitchAttitudeErrorKi + (pitchAttitudeErrorKdelta / loopSpeed.dT * sldUsed), -300.0, 300.0) * (1.0f - ABS(trueRcCommandF[PITCH]) );
			}
		}
		else //we're in a self level mode, let's find the set point based on angle of sticks and angle of craft
		{
			if (timeSinceSelfLevelActivated)
				timeSinceSelfLevelActivated = 0;
		}

		//Run PIDC
		InlinePidController(filteredGyroData, flightSetPoints, flightPids, actuatorRange, mainConfig.pidConfig);

		if ( boardArmed || PreArmFilterCheck )
		{
			if (gyroCalibrationCycles != 0)
			{
			   return;
			}
			else if (gyroStdDeviationPointer < GYRO_STD_DEVIATION_SAMPLE_SIZE)
			{
				//gather cycles during board calibration
				gyroStdDeviationSamples[YAW][gyroStdDeviationPointer] = dpsGyroArray[YAW];
				gyroStdDeviationSamples[ROLL][gyroStdDeviationPointer] = dpsGyroArray[ROLL];
				gyroStdDeviationSamples[PITCH][gyroStdDeviationPointer++] = dpsGyroArray[PITCH];

				if (gyroStdDeviationPointer == GYRO_STD_DEVIATION_SAMPLE_SIZE)
				{
					gyroStdDeviationPointer = 0;
					if (gyroStdDeviationTotalsPointer < GYRO_STD_DEVIATION_SAMPLE_SIZE)
					{
						gyroStdDeviationTotals[YAW][gyroStdDeviationTotalsPointer] = CalculateSDSize(gyroStdDeviationSamples[YAW], GYRO_STD_DEVIATION_SAMPLE_SIZE);
						gyroStdDeviationTotals[ROLL][gyroStdDeviationTotalsPointer] = CalculateSDSize(gyroStdDeviationSamples[ROLL], GYRO_STD_DEVIATION_SAMPLE_SIZE);
						gyroStdDeviationTotals[PITCH][gyroStdDeviationTotalsPointer++] = CalculateSDSize(gyroStdDeviationSamples[PITCH], GYRO_STD_DEVIATION_SAMPLE_SIZE);
					}
					else
					{
						gyroStdDeviationPointer = GYRO_STD_DEVIATION_SAMPLE_SIZE;
						gyroStdDeviationTotalsPointer = GYRO_STD_DEVIATION_SAMPLE_SIZE;
					}

				}

				if (gyroStdDeviationTotalsPointer == GYRO_STD_DEVIATION_SAMPLE_SIZE)
				{
					for (uint32_t xx = 0; xx < GYRO_STD_DEVIATION_SAMPLE_SIZE; xx++)
					{
						yy[YAW] += gyroStdDeviationTotals[YAW][xx];
						yy[ROLL] += gyroStdDeviationTotals[ROLL][xx];
						yy[PITCH] += gyroStdDeviationTotals[PITCH][xx];
					}
					yy[YAW] /= GYRO_STD_DEVIATION_SAMPLE_SIZE;
					yy[ROLL] /= GYRO_STD_DEVIATION_SAMPLE_SIZE;
					yy[PITCH] /= GYRO_STD_DEVIATION_SAMPLE_SIZE;
				}

			}
			else if (!gyroStdDeviationLatch)
			{
				//DelayMs(50);
				//gyroStdDeviationPointer = 0;
				//gyroStdDeviationTotalsPointer = 0;
				//uint32_t cat = (uint32_t)(yy[YAW] * 100);
				//uint32_t dog = (uint32_t)(yy[ROLL] * 100);
				//uint32_t rat = (uint32_t)(yy[PITCH] * 100);
				//uint8_t  reportOut[50];
				//sprintf((char *)reportOut, "%lu,%lu,%lu\n", cat, dog, rat);
				//SendStatusReport((char *)reportOut);
				//PreArmFilterCheck

				gyroStdDeviationLatch = 1;
				PreArmFilterCheck = 0;

				if (mainConfig.filterConfig[YAW].gyro.r == 1)
					pafGyroStates[YAW]   = InitPaf( mainConfig.filterConfig[YAW].gyro.q, yy[YAW] * 100, 0.0f, filteredGyroData[YAW]);
				if (mainConfig.filterConfig[ROLL].gyro.r == 1)
					pafGyroStates[ROLL]   = InitPaf( mainConfig.filterConfig[ROLL].gyro.q, yy[ROLL] * 100, 0.0f, filteredGyroData[ROLL]);
				if (mainConfig.filterConfig[PITCH].gyro.r == 1)
					pafGyroStates[PITCH]   = InitPaf( mainConfig.filterConfig[PITCH].gyro.q, yy[PITCH] * 100, 0.0f, filteredGyroData[PITCH]);

		//		gyroStdDeviationLatch = 1;
		//		mainConfig.filterConfig[YAW].gyro.r = 86;//CalculateSDSize(gyroStdDeviationSamples[YAW], GYRO_STD_DEVIATION_SAMPLE_SIZE)     * 100;
		//		mainConfig.filterConfig[ROLL].gyro.r = 86;//CalculateSDSize(gyroStdDeviationSamples[ROLL], GYRO_STD_DEVIATION_SAMPLE_SIZE)   * 100;
		//		mainConfig.filterConfig[PITCH].gyro.r = 86;//CalculateSDSize(gyroStdDeviationSamples[PITCH], GYRO_STD_DEVIATION_SAMPLE_SIZE) * 100;
		//		pafGyroStates[YAW]    = InitPaf( mainConfig.filterConfig[YAW].gyro.q, mainConfig.filterConfig[YAW].gyro.r    , 0, 0);
		//		pafGyroStates[ROLL]   = InitPaf( mainConfig.filterConfig[ROLL].gyro.q, mainConfig.filterConfig[ROLL].gyro.r  , 0, 0);
		//		pafGyroStates[PITCH]  = InitPaf( mainConfig.filterConfig[PITCH].gyro.q, mainConfig.filterConfig[PITCH].gyro.r, 0, 0);
				//pafGyroStates[YAW]    = InitPaf( mainConfig.filterConfig[YAW].gyro.q * 0.01, CalculateSDSize(gyroStdDeviationSamples[YAW], GYRO_STD_DEVIATION_SAMPLE_SIZE) * 100, 0, 0);
				//pafGyroStates[ROLL]   = InitPaf( mainConfig.filterConfig[ROLL].gyro.q * 0.01, CalculateSDSize(gyroStdDeviationSamples[ROLL], GYRO_STD_DEVIATION_SAMPLE_SIZE) * 100, 0, 0);
				//pafGyroStates[PITCH]  = InitPaf( mainConfig.filterConfig[PITCH].gyro.q * 0.01, CalculateSDSize(gyroStdDeviationSamples[PITCH], GYRO_STD_DEVIATION_SAMPLE_SIZE) * 100, 0, 0);
				//set filters
			}

			if (threeDeeMode)
			{
				static uint32_t actuatorToUse = 0;
				if (smoothedRcCommandF[THROTTLE] >= 0.1) //throttle above neutral latch
				{
					actuatorToUse = 1;

				}
				else if (smoothedRcCommandF[THROTTLE] <= -0.1) //throttle below neutral latch
				{
					actuatorToUse = 2;

				}

				if (actuatorToUse == 2)
				{
					smoothedRcCommandF[THROTTLE] = CONSTRAIN(smoothedRcCommandF[THROTTLE], -1.0f, -0.1f);
					actuatorRange = InlineApplyMotorMixer3dInverted(flightPids, smoothedRcCommandF[THROTTLE]); //put in PIDs and Throttle or passthru
				}
				else
				{
					smoothedRcCommandF[THROTTLE] = CONSTRAIN(smoothedRcCommandF[THROTTLE], 0.1f, 1.0f);
					actuatorRange = InlineApplyMotorMixer3dUpright(flightPids, smoothedRcCommandF[THROTTLE]); //put in PIDs and Throttle or passthru
				}

			}
			else
			{
				if (mainConfig.mixerConfig.mixerStyle == 1) //race mixer
					actuatorRange = InlineApplyMotorMixer1(flightPids, smoothedRcCommandF[THROTTLE]); //put in PIDs and Throttle or passthru
				else //freestyle mixer
					actuatorRange = InlineApplyMotorMixer(flightPids, smoothedRcCommandF[THROTTLE]); //put in PIDs and Throttle or passthru

			}

		}
		else
		{
			//otherwise we keep Ki zeroed.

			flightPids[YAW].ki   = 0;
			flightPids[ROLL].ki  = 0;
			flightPids[PITCH].ki = 0;
			rollAttitudeErrorKi  = 0;
			pitchAttitudeErrorKi = 0;
			fullKiLatched        = 1;
		}

		//output to actuators. This is the end of the flight code for this iteration.
		OutputActuators(motorOutput, servoOutput);


		//this code is less important that stabilization, so it happens AFTER stabilization.
		//Anything changed here can happen after the next iteration without any drawbacks. Stabilization above all else!

		ComplementaryFilterUpdateAttitude(); //stabilization above all else. This update happens after gyro stabilization

		if (khzLoopCounter-- == 0)
		{
			//runs at 1KHz or loopspeed, whatever is slower
			khzLoopCounter=loopSpeed.khzDivider;

			CheckFailsafe();

			/*
			//every 32 cycles we check if the filter needs an update.
			if (
					(currentGyroFilterConfig[YAW]   != mainConfig.filterConfig[YAW].gyro.r) ||
					(currentGyroFilterConfig[ROLL]  != mainConfig.filterConfig[ROLL].gyro.r) ||
					(currentGyroFilterConfig[PITCH] != mainConfig.filterConfig[PITCH].gyro.r)
			) {
				//InlineInitGyroFilters();
			}

			if (
					(currentKdFilterConfig[YAW]   != kdFiltUsed[YAW]) ||
					(currentKdFilterConfig[ROLL]  != kdFiltUsed[ROLL]) ||
					(currentKdFilterConfig[PITCH] != kdFiltUsed[PITCH])
			) {
				InlineInitKdFilters();
			}

			if (
					(currentAccFilterConfig[ACCX] != mainConfig.filterConfig[ACCX].acc.r) ||
					(currentAccFilterConfig[ACCY] != mainConfig.filterConfig[ACCY].acc.r) ||
					(currentAccFilterConfig[ACCZ] != mainConfig.filterConfig[ACCZ].acc.r)
			) {
				InlineInitAccFilters();
			}
			 */

			//update blackbox here   //void UpdateBlackbox(pid_output *flightPids)
			UpdateBlackbox(flightPids, flightSetPoints, dpsGyroArray, filteredGyroData, filteredAccData);

			//check for fullKiLatched here
			if ( (boardArmed) && (smoothedRcCommandF[THROTTLE] > -0.65) )
			{
				fullKiLatched = 1;
			}


			if (RfblDisasterPreventionCheck)
			{
				if (InlineMillis() > 5000)
				{
					RfblDisasterPreventionCheck = 0;
					HandleRfblDisasterPrevention();
				}
			}

		}

	}
	else
	{
		ComplementaryFilterUpdateAttitude(); //we update this in the main loop after stabilization but before logging. Outside of the divider we only update this
	}

}

//return setpoint in degrees per second, this is after stick smoothing
inline float InlineGetSetPoint(float curvedRcCommandF, uint32_t curveToUse, float rates, float acroPlus, uint32_t axis)
{
	float returnValue;

	//betaflop and kiss set points are calculated in rx.c during curve calculation
	switch(curveToUse)
	{
		case ACRO_PLUS:
			returnValue = (curvedRcCommandF * ( rates + ( ABS(curvedRcCommandF) * rates * acroPlus ) ) );
			break;
		case BETAFLOP_EXPO:
			returnValue = (curvedRcCommandF * maxFlopRate[axis]);
			break;
		case KISS_EXPO:
		case KISS_EXPO2:
			returnValue = (curvedRcCommandF * maxKissRate[axis]);
			break;
		case TARANIS_EXPO:
		case SKITZO_EXPO:
		case FAST_EXPO:
		case NO_EXPO:
		default:
			returnValue = (curvedRcCommandF * (rates + (rates * acroPlus) ) );
			break;

	}

	returnValue = InlineConstrainf(returnValue,-1400,1400);

	return (returnValue);
}




//these functions really don't belong here



static void InitVbusSensing(void);
static uint32_t IsUsbConnected(void);

static void InitVbusSensing(void)
{
#ifdef VBUS_SENSING
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(ports[VBUS_SENSING_GPIO], VBUS_SENSING_PIN);

    GPIO_InitStructure.Pin   = VBUS_SENSING_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    HAL_GPIO_Init(ports[VBUS_SENSING_GPIO], &GPIO_InitStructure);
#endif
}

static uint32_t IsUsbConnected(void)
{
#ifdef VBUS_SENSING
	return(!inlineIsPinStatusHi(ports[VBUS_SENSING_GPIO], VBUS_SENSING_PIN));
#else
	return(0);
#endif

}

void DeinitFlight(void)
{
	DisarmBoard();              //sets WD to 32S
	AccGyroDeinit();            //takes about 200ms maybe to run, this will also stop the flight code from running so no reason to stop that.
	DeInitBoardUsarts();        //deinit all the USARTs.
	DeInitActuators();          //deinit all the Actuators.
	DeInitAllowedSoftOutputs(); //deinit all the soft outputs
}

//init the board
void InitFlight(void) {

    //TODO: move the check into the init functions.

	DeInitAllowedSoftOutputs();

    if (board.flash[0].enabled)
    {
    	InitFlashChip();
    	InitFlightLogger();
    }

    InitVbusSensing();
    InitRcData();
    InitMixer();          //init mixders
    InitFlightCode();     //flight code before PID code is a must since flight.c contains loop time settings the pid.c uses.
    InitPid();            //Relies on InitFlightCode for proper activations.
    DeInitActuators();    //Deinit before Init is a shotgun startup
    InitActuators();      //Actuator init should happen after soft serial init.
    ZeroActuators(5000);  //output actuators to idle after timers are stable;

	InitAdc();            //init ADC functions
    InitModes();          //set flight modes mask to zero.
    InitBoardUsarts();    //most important thing is activated last, the ability to control the craft.

	if (!AccGyroInit(mainConfig.gyroConfig.loopCtrl))
	{
		ErrorHandler(GYRO_INIT_FAILIURE);
	}

	//make sure gyro is interrupting and init scheduler
	InitScheduler();

	//init telemtry, if there's a gyro EXTI and soft serial collision then the fake EXTI will be used in place of the actual gyro EXTI
	InitTelemtry();

#ifndef SPMFC400
	if (!IsUsbConnected())
	{
		InitWs2812();
	}
#else
	InitWs2812();
#endif

	//InitTransponderTimer();
	DelayMs(2);

}
