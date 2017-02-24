#include "includes.h"

volatile quaternion_record quat;
volatile quaternion_record commandQuat;
volatile quaternion_record accQuat;
volatile quaternion_record gyroQuat;
volatile quaternion_record groundQuat;
volatile quaternion_record attitudeFrameQuat;
volatile quaternion_record inertialFrameQuat;

volatile float currentSpinRate = 0.0f;
volatile float rotationalMatrix[3][3];

static void QuaternionZeroRotation(volatile quaternion_record *quaternion);
static void QuaternionNormalize (volatile quaternion_record *out);
//static void QuaternionMultiply(volatile quaternion_record *out, volatile quaternion_record *q1, volatile quaternion_record *q2);
//static void QuaternionConjugate (volatile quaternion_record *out);
//static quaternion_record QuaternionFromEuler (float halfBankRads, float halfAttitudeRads, float halfHeadingRads);
//static void QuaternionToEuler(volatile quaternion_record *inQuat, float *roll, float *pitch, float *yaw);
static void UpdateRotationMatrix(void);
static void UpdateAttitudeFrameQuat(float gyroRollDiffRads, float gyroPitchDiffRads, float gyroYawDiffRads);

static void QuaternionZeroRotation(volatile quaternion_record *quaternion)
{
	quaternion->w = 1.0f;
	quaternion->x = 0.0f;
	quaternion->y = 0.0f;
	quaternion->z = 0.0f;
}

static void QuaternionNormalize (volatile quaternion_record *out)
{
	float norm;
	arm_sqrt_f32( (out->w * out->w + out->x * out->x + out->y * out->y + out->z * out->z), &norm);
	norm = 1.0f/norm;
	out->w *= norm;
	out->x *= norm;
	out->y *= norm;
	out->z *= norm;
}

/*
static void QuaternionMultiply (volatile quaternion_record *out, volatile quaternion_record *q1, volatile quaternion_record *q2)
{
	float q1x = q1->x, q1y = q1->y, q1z = q1->z, q1w = q1->w;
	float q2x = q2->x, q2y = q2->y, q2z = q2->z, q2w = q2->w;

	out->x =  q1x * q2w + q1y * q2z - q1z * q2y + q1w * q2x;
	out->y = -q1x * q2z + q1y * q2w + q1z * q2x + q1w * q2y;
	out->z =  q1x * q2y - q1y * q2x + q1z * q2w + q1w * q2z;
	out->w = -q1x * q2x - q1y * q2y - q1z * q2z + q1w * q2w;
}


static void QuaternionConjugate (volatile quaternion_record *out)
{
	out->x = -out->x;
	out->y = -out->y;
	out->z = -out->z;
}

static float QuaternionDotProduct (volatile quaternion_record *a, volatile quaternion_record *b)
{
	return(a->w * b->w + a->x * b->x + a->y * b->y + a->z * b->z);
}


static quaternion_record QuaternionFromEuler (float halfBankRads, float halfAttitudeRads, float halfHeadingRads)
{
	quaternion_record tempQuaternion;
	float c2, c1, c3;
	float s2, s1, s3;

	c1 = arm_cos_f32(halfHeadingRads);
	c2 = arm_cos_f32(halfAttitudeRads);
	c3 = arm_cos_f32(halfBankRads);
	s1 = arm_sin_f32(halfHeadingRads);
	s2 = arm_sin_f32(halfAttitudeRads);
	s3 = arm_sin_f32(halfBankRads);

	tempQuaternion.w = (c1 * c2 * c3 - s1 * s2 * s3);
	tempQuaternion.x = (s1 * s2 * c3 + c1 * c2 * s3);
	tempQuaternion.y = (s1 * c2 * c3 + c1 * s2 * s3);
	tempQuaternion.z = (c1 * s2 * c3 - s1 * c2 * s3);

	return(tempQuaternion);
}

static void QuaternionToEuler(volatile quaternion_record *inQuat, float *roll, float *pitch, float *yaw)
{

	float test;
	float heading;
	float attitude;
	float bank;
	float sqx;
	float sqy;
	float sqz;

	test = (inQuat->x * inQuat->y + inQuat->z * inQuat->w);

	if (test > 0.499)
	{ // singularity at north pole
		heading  = 2.0f * atan2f(inQuat->x,inQuat->w);
		attitude = HALF_PI_F;
		bank     = 0.0f;
		return;
	}
	if (test < -0.499)
	{ // singularity at south pole
		heading  = -2.0f * atan2f(inQuat->x,inQuat->w);
		attitude = - HALF_PI_F;
		bank     = 0.0f;
		return;
	}

    sqx = inQuat->x * inQuat->x;
    sqy = inQuat->y * inQuat->y;
    sqz = inQuat->z * inQuat->z;

    heading  = atan2f(2.0f * inQuat->y * inQuat->w - 2.0f * inQuat->x * inQuat->z , 1.0f - 2.0f * sqy - 2.0f * sqz);
	attitude = asinf(2.0f * test);
	bank     = atan2f(2.0f * inQuat->x * inQuat->w - 2.0f * inQuat->y * inQuat->z , 1.0f - 2.0f * sqx - 2.0f * sqz);

	*yaw   = InlineRadiansToDegrees(heading);
	*pitch = InlineRadiansToDegrees(attitude);
	*roll  = InlineRadiansToDegrees(bank);
}
*/

static void UpdateRotationMatrix(void)
{
    float qxqx = (attitudeFrameQuat.x * attitudeFrameQuat.x);
    float qyqy = (attitudeFrameQuat.y * attitudeFrameQuat.y);
    float qzqz = (attitudeFrameQuat.z * attitudeFrameQuat.z);

    float qwqx = (attitudeFrameQuat.w * attitudeFrameQuat.x);
    float qwqy = (attitudeFrameQuat.w * attitudeFrameQuat.y);
    float qwqz = (attitudeFrameQuat.w * attitudeFrameQuat.z);
    float qxqy = (attitudeFrameQuat.x * attitudeFrameQuat.y);
    float qxqz = (attitudeFrameQuat.x * attitudeFrameQuat.z);
    float qyqz = (attitudeFrameQuat.y * attitudeFrameQuat.z);

    rotationalMatrix[0][0] = (1.0f - 2.0f * qyqy - 2.0f * qzqz);
    rotationalMatrix[0][1] = (2.0f * (qxqy - qwqz));
    rotationalMatrix[0][2] = (2.0f * (qxqz + qwqy));

    rotationalMatrix[1][0] = (2.0f * (qxqy + qwqz));
    rotationalMatrix[1][1] = (1.0f - 2.0f * qxqx - 2.0f * qzqz);
    rotationalMatrix[1][2] = (2.0f * (qyqz - qwqx));

    rotationalMatrix[2][0] = (2.0f * (qxqz - qwqy));
    rotationalMatrix[2][1] = (2.0f * (qyqz + qwqx));
    rotationalMatrix[2][2] = (1.0f - 2.0f * qxqx - 2.0f * qyqy);
}

void InitImu(void)
{
	uint32_t x, y;

	for (x = 0; x < 3; x++)
	{
		for (y = 0; y < 3; y++)
		{
			rotationalMatrix[x][y] = 0.0f;
		}
	}

	QuaternionZeroRotation(&attitudeFrameQuat);
	QuaternionZeroRotation(&inertialFrameQuat);
	QuaternionZeroRotation(&groundQuat);
	QuaternionZeroRotation(&gyroQuat);
	QuaternionZeroRotation(&quat);
	QuaternionZeroRotation(&commandQuat);
	QuaternionZeroRotation(&accQuat);

	UpdateRotationMatrix();
}

void UpdateAttitudeFrameQuat(float gyroRollDiffRads, float gyroPitchDiffRads, float gyroYawDiffRads)
{
	quaternion_record tempQuat;

	tempQuat.w = attitudeFrameQuat.w;
	tempQuat.x = attitudeFrameQuat.x;
	tempQuat.y = attitudeFrameQuat.y;
	tempQuat.z = attitudeFrameQuat.z;

	gyroRollDiffRads  = InlineDegreesToRadians( gyroRollDiffRads )  * loopSpeed.gyrodT;
	gyroPitchDiffRads = InlineDegreesToRadians( gyroPitchDiffRads ) * loopSpeed.gyrodT;
	gyroYawDiffRads   = InlineDegreesToRadians( gyroYawDiffRads )   * loopSpeed.gyrodT;

	attitudeFrameQuat.w += (-tempQuat.x * gyroRollDiffRads  - tempQuat.y * gyroPitchDiffRads - tempQuat.z * gyroYawDiffRads);
	attitudeFrameQuat.x += (tempQuat.w  * gyroRollDiffRads  + tempQuat.y * gyroYawDiffRads   - tempQuat.z * gyroPitchDiffRads);
	attitudeFrameQuat.y += (tempQuat.w  * gyroPitchDiffRads - tempQuat.x * gyroYawDiffRads   + tempQuat.z * gyroRollDiffRads);
	attitudeFrameQuat.z += (tempQuat.w  * gyroYawDiffRads   + tempQuat.x * gyroPitchDiffRads - tempQuat.y * gyroRollDiffRads);

	QuaternionNormalize(&attitudeFrameQuat);
}

void UpdateImu(float accX, float accY, float accZ, float gyroRoll, float gyroPitch, float gyroYaw)
{

	gyroPitch = -gyroPitch;
	gyroYaw   = -gyroYaw;

	float accTrust;
	float accTrustKi = 112.1000f;
	float norm;
	float accToGyroError[3];

	static float accTrustKiStorage[3] = {0.0f, 0.0f, 0.0f};
	static uint32_t gyroToAccDivisorCounter = 0;

	//calculate current spin rate in DPS
	arm_sqrt_f32( SQUARE(gyroRoll) + SQUARE(gyroPitch) + SQUARE(gyroYaw), &norm);
	currentSpinRate = norm;

	//use ACC to fix Gyro drift here. Only needs to be done every eigth iteration at 32 KHz.
	if( !( (accX == 0.0f) && (accY == 0.0f) && (accZ == 0.0f) ) )
	{

		gyroToAccDivisorCounter++;

		if (gyroToAccDivisorCounter == loopSpeed.gyroAccDiv)
		{

			gyroToAccDivisorCounter = 0;

			//normalize the acc readings
			arm_sqrt_f32( (accX * accX + accY * accY + accZ * accZ), &norm);
			norm = 1.0f/norm;
			accX *= norm;
			accY *= norm;
			accZ *= norm;

			accToGyroError[ACCX] += (accY * rotationalMatrix[2][2] - accZ * rotationalMatrix[2][1]);
			accToGyroError[ACCY] += (accZ * rotationalMatrix[2][0] - accX * rotationalMatrix[2][2]);
			accToGyroError[ACCZ] += (accX * rotationalMatrix[2][1] - accY * rotationalMatrix[2][0]);

			if (currentSpinRate < MAX_SPIN_RATE_RAD)
			{
				accTrustKiStorage[ACCX] += accTrustKi * accToGyroError[ACCX] * loopSpeed.gyrodT;
				accTrustKiStorage[ACCY] += accTrustKi * accToGyroError[ACCY] * loopSpeed.gyrodT;
				accTrustKiStorage[ACCZ] += accTrustKi * accToGyroError[ACCZ] * loopSpeed.gyrodT;
			}

			//trust ACCs more when the quad is disamred.
			if (boardArmed)
			{
				accTrust  = 11.13000f;
			}
			else
			{
				accTrust  = 4000.3000f;
			}

			gyroRoll  += accTrust * accToGyroError[ACCX] + accTrustKiStorage[ACCX];
			gyroPitch += accTrust * accToGyroError[ACCY] + accTrustKiStorage[ACCY];
			gyroYaw   += accTrust * accToGyroError[ACCZ] + accTrustKiStorage[ACCZ];

		}

	}

	UpdateAttitudeFrameQuat(gyroRoll, gyroPitch, gyroYaw);
	UpdateRotationMatrix();

	rollAttitude  = InlineRadiansToDegrees(atan2f(rotationalMatrix[2][1], rotationalMatrix[2][2]));
	pitchAttitude = -InlineRadiansToDegrees(HALF_PI_F - acosf(-rotationalMatrix[2][0]));
	yawAttitude   = -InlineRadiansToDegrees(atan2f(rotationalMatrix[1][0], rotationalMatrix[0][0]));

}
