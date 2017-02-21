#include "includes.h"


#define QUAT_INDEX 4

volatile float quat[QUAT_INDEX];

void InitImu(void)
{
	int32_t x;

	quat[0] = 1.0f;
	quat[1] = 0.0f;
	quat[2] = 0.0f;
	quat[3] = 0.0f;

}

void ConvertToQuaternion(float gx, float gy, float gz, float ax, float ay, float az)
{
	(void)(gx);
	(void)(gy);
	(void)(gz);
	(void)(ax);
	(void)(ay);
	(void)(az);
}

void CalculateQuaternions(void)
{
	//yawAttitude   = atan2(2.0f * (quat[1] * quat[2] + quat[0] * quat[3]), quat[0] * quat[0] + quat[1] * quat[1] - quat[2] * quat[2] - quat[3] * quat[3]);
	//pitchAttitude = asin(2.0f * (quat[1] * quat[3] - quat[0] * quat[2]));
	//rollAttitude  = atan2(2.0f * (quat[0] * quat[1] + quat[2] * quat[3]), quat[0] * quat[0] - quat[1] * quat[1] - quat[2] * quat[2] + quat[3] * quat[3]);
	//pitchAttitude *= 180.0f * IPIf;
	//yawAttitude   *= 180.0f * IPIf;
	//rollAttitude  *= 180.0f * IPIf;
}
//function [q] = convertToQuaternion(axisangle)
//% Convert a axisangle rotation to a quaternion
//q = [axisangle(1:3).*sin(axisangle(4)/2); cos(axisangle(4)/2)];
//end

//function [axisangle] = convertToAxisAngle(q)
//t = wrapToPi(2*acos(q(4)));
//k = q(1:3)./sin(t/2);
//axisangle = [NormalizeV(k);t];
//end
