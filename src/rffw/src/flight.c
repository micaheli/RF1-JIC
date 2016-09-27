#include "includes.h"


void flightCode(float dpsGyroArray[]) {

	static float actuatorRange = 0;

	//gyro interrupts
	//gyro read using DMA
	//updateGyro is called after the read is complete.
	//updateGyro will call flightCode.
	//pid controller is run
	//mixer is applied
	//output to motors

	pidController(dpsGyroArray, curvedRcCommandF, pids, actuatorRange);
	actuatorRange = InlineApplyMotorMixer(pids, curvedRcCommandF);
	OutputActuators(motorOutput, servoOutput);

}
