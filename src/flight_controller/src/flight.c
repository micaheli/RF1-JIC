#include "includes.h"

pid_output flightPids[3];
float filteredGyroData[3];
paf_state yawPafState;
paf_state rollPafState;
paf_state pitchPafState;
float actuatorRange;
float flightSetPoints[3];
uint32_t boardArmed, calibrateMotors;

void InitFlightCode(void) {

	bzero(filteredGyroData,sizeof(filteredGyroData));
	bzero(&flightPids,sizeof(flightPids));
	actuatorRange = 0;
	boardArmed = 0;
	calibrateMotors = 0;

	yawPafState   = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);
	rollPafState  = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);
	pitchPafState = InitPaf(filterConfig.gyroFilter.q, filterConfig.gyroFilter.r, filterConfig.gyroFilter.q, 0);

}

extern uint8_t tInBuffer[];
extern uint8_t tOutBuffer[];
uint32_t counterFish = 0;

volatile float rat[80];
volatile float cat[80];
inline void InlineFlightCode(float dpsGyroArray[]) {

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


	static uint32_t counterFish = 0;
	static uint32_t counterDog = 0;
	counterFish++;
	counterDog++;
	rat[counterDog] = rxData[THROTTLE];
	cat[counterDog] = smoothedRcCommandF[THROTTLE];
	(void)(rat);
	(void)(cat);
	if (counterDog == 79) {
		counterDog =0;
	}


	uint32_t catfish = Micros();

	PafUpdate(&yawPafState, dpsGyroArray[YAW]);
	PafUpdate(&rollPafState, dpsGyroArray[ROLL]);
	PafUpdate(&pitchPafState, dpsGyroArray[PITCH]);

	filteredGyroData[YAW]   = yawPafState.x;
	filteredGyroData[ROLL]  = rollPafState.x;
	filteredGyroData[PITCH] = pitchPafState.x;

	//smoothedRcCommandF[0]=curvedRcCommandF[0];
	//smoothedRcCommandF[1]=curvedRcCommandF[1];
	//smoothedRcCommandF[2]=curvedRcCommandF[2];
	//smoothedRcCommandF[3]=curvedRcCommandF[3];
	InlineRcSmoothing(curvedRcCommandF, smoothedRcCommandF);




	counterFish++;
	if (counterFish>=2000) {
		counterFish=0;
		tInBuffer[0] = 1;
		//tInBuffer[1]=(int8_t)dpsGyroArray[0];
		//tInBuffer[2]=(int8_t)dpsGyroArray[1];
		//tInBuffer[3]=(int8_t)dpsGyroArray[2];
		tInBuffer[1]=(uint8_t)(motorOutput[YAW]*100);
		tInBuffer[2]=(uint8_t)(motorOutput[ROLL]*100);
		tInBuffer[3]=(uint8_t)(motorOutput[PITCH]*100);
		tInBuffer[4]=(uint8_t)(motorOutput[THROTTLE]*100);
		tInBuffer[8]=(uint8_t)(smoothedRcCommandF[YAW]*100);
		tInBuffer[9]=(uint8_t)(smoothedRcCommandF[ROLL]*100);
		tInBuffer[10]=(uint8_t)(smoothedRcCommandF[PITCH]*100);
		tInBuffer[11]=(uint8_t)(smoothedRcCommandF[THROTTLE]*100);
		tInBuffer[12]=(uint8_t)debugU32[4];
		tInBuffer[14]=(uint8_t)debugU32[4];

		USBD_HID_SendReport (&hUsbDeviceFS, tInBuffer, HID_EPIN_SIZE);
	}



	flightSetPoints[YAW]   = InlineGetSetPoint(smoothedRcCommandF[YAW], rcControlsConfig.rates[YAW], rcControlsConfig.acroPlus[YAW]);
	flightSetPoints[ROLL]  = InlineGetSetPoint(smoothedRcCommandF[ROLL], rcControlsConfig.rates[ROLL], rcControlsConfig.acroPlus[ROLL]);
	flightSetPoints[PITCH] = InlineGetSetPoint(smoothedRcCommandF[PITCH], rcControlsConfig.rates[PITCH], rcControlsConfig.acroPlus[PITCH]);



	if (boardArmed) {
		InlinePidController(filteredGyroData, flightSetPoints, flightPids, actuatorRange, pidConfig);

		actuatorRange = InlineApplyMotorMixer(flightPids, smoothedRcCommandF, motorOutput); //put in PIDs and Throttle or passthru
	}

	OutputActuators(motorOutput, servoOutput);

	debugU32[0] = Micros() - catfish;

	debugU32[1] = flightPids[0].kp;
	debugU32[2] = flightPids[1].kp;
	debugU32[3] = flightPids[2].kp;

	debugU32[1] = filteredGyroData[0];
	debugU32[2] = filteredGyroData[1];
	debugU32[3] = filteredGyroData[2];

}

inline float InlineGetSetPoint(float curvedRcCommandF, float rates, float acroPlus) {
	return ((curvedRcCommandF * (rates + (rates*acroPlus))) * 0.10 ); //setpoint in DPS
}
