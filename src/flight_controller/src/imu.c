#include "includes.h"

volatile quaternion_record quat;
volatile quaternion_record commandQuat;
volatile quaternion_record accQuat;
volatile quaternion_record gyroQuat;
volatile quaternion_record groundQuat;
volatile quaternion_record attitudeFrameQuat;
volatile quaternion_record inertialFrameQuat;

volatile float rotationalMatrix[3][3];

static void QuaternionZeroRotation(volatile quaternion_record *quaternion);
static void QuaternionMultiply(volatile quaternion_record *out, volatile quaternion_record *q1, volatile quaternion_record *q2);
static void QuaternionConjugate (volatile quaternion_record *out);
static void QuaternionNormalize (volatile quaternion_record *out);
static quaternion_record QuaternionFromEuler (float halfBankRads, float halfAttitudeRads, float halfHeadingRads);
static void QuaternionToEuler(volatile quaternion_record *inQuat, float *roll, float *pitch, float *yaw);
static void UpdateRotationMatrix(void);

static void QuaternionZeroRotation(volatile quaternion_record *quaternion)
{
	quaternion->w = 1.0f;
	quaternion->x = 0.0f;
	quaternion->y = 0.0f;
	quaternion->z = 0.0f;
}

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

	tempQuaternion.w = c1 * c2 * c3 - s1 * s2 * s3;
	tempQuaternion.x = s1 * s2 * c3 + c1 * c2 * s3;
	tempQuaternion.y = s1 * c2 * c3 + c1 * s2 * s3;
	tempQuaternion.z = c1 * s2 * c3 - s1 * c2 * s3;

	return(tempQuaternion);
}

static void UpdateRotationMatrix(void)
{
    float qxqx = gyroQuat.x * gyroQuat.x;
    float qyqy = gyroQuat.y * gyroQuat.y;
    float qzqz = gyroQuat.z * gyroQuat.z;

    float qwqx = gyroQuat.w * gyroQuat.x;
    float qwqy = gyroQuat.w * gyroQuat.y;
    float qwqz = gyroQuat.w * gyroQuat.z;
    float qxqy = gyroQuat.x * gyroQuat.y;
    float qxqz = gyroQuat.x * gyroQuat.z;
    float qyqz = gyroQuat.y * gyroQuat.z;

    rotationalMatrix[0][0] = 1.0f - 2.0f * qyqy - 2.0f * qzqz;
    rotationalMatrix[0][1] = 2.0f * qxqy - 2.0f * qwqz;
    rotationalMatrix[0][2] = 2.0f * qxqz + 2.0f * qwqy;
    //good above

    //2*qx*qy + 2*qz*qw
    //1 - 2*qx2 - 2*qz2
    //2*qy*qz - 2*qx*qw
    //change below to above
    rotationalMatrix[1][0] = 2.0f * (qxqy - -qwqz);
    rotationalMatrix[1][1] = 1.0f - 2.0f * qxqx - 2.0f * qzqz;
    rotationalMatrix[1][2] = 2.0f * (qyqz + -qwqx);

    //2*qx*qz - 2*qy*qw
    //2*qy*qz + 2*qx*qw
    //1 - 2*qx2 - 2*qy2
    //change below to above
    rotationalMatrix[2][0] = 2.0f * (qxqz + -qwqy);
    rotationalMatrix[2][1] = 2.0f * (qyqz - -qwqx);
    rotationalMatrix[2][2] = 1.0f - 2.0f * qxqx - 2.0f * qyqy;
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

	test = inQuat->x * inQuat->y + inQuat->z * inQuat->w;
	if (test > 0.499) { // singularity at north pole
		heading  = 2.0f * atan2f(inQuat->x,inQuat->w);
		attitude = HALF_PI_F;
		bank     = 0.0f;
		return;
	}
	if (test < -0.499) { // singularity at south pole
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

static void QuaternionRotate (volatile quaternion_record *quatA, volatile quaternion_record *quatB)
{
	//b.multiply(this)
	//QuaternionMultiply(&quatA,&quatA,&quatB);
	//QuaternionConjugate(&quatB);
	//QuaternionMultiply(&quatA,&quatA,&quatB);

	//return (b.multiply(this)).multiply(b.conjugate());
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

	imuComputeRotationMatrix();
}


void ConvertToQuaternion(float ax, float ay, float az, float gyroRoll, float gyroPitch, float gyroYaw)
{
	volatile quaternion_record differenceQuat;
	/*
    float q1q1 = sq(q1);
    float q2q2 = sq(q2);
    float q3q3 = sq(q3);

    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
    float q0q3 = q0 * q3;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q2q3 = q2 * q3;

    rMat[0][0] = 1.0f - 2.0f * q2q2 - 2.0f * q3q3;
    rMat[0][1] = 2.0f * (q1q2 + -q0q3);
    rMat[0][2] = 2.0f * (q1q3 - -q0q2);

    rMat[1][0] = 2.0f * (q1q2 - -q0q3);
    rMat[1][1] = 1.0f - 2.0f * q1q1 - 2.0f * q3q3;
    rMat[1][2] = 2.0f * (q2q3 + -q0q1);

    rMat[2][0] = 2.0f * (q1q3 + -q0q2);
    rMat[2][1] = 2.0f * (q2q3 - -q0q1);
    rMat[2][2] = 1.0f - 2.0f * q1q1 - 2.0f * q2q2;
	 */

	rotationalMatrix[0]
	float accTrust   = 1.13000f;
	float accTrustKi = 0.00100f;
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;
	volatile float c1, c2, c3;
	volatile float s1, s2, s3;

	//	float forceMagnitudeApprox;
	static float integralFBx = 0.0f, integralFBy = 0.0f, integralFBz = 0.0f;
	static uint32_t counterism = 0;



	//use ACC to fix Gyro drift here. Only needs to be done every eigth iteration at 32 KHz.
	if( !( (ax == 0.0f) && (ay == 0.0f) && (az == 0.0f) ) )
	{
		//only look at ACC data if it's within .45 and 2.1 Gees
		//forceMagnitudeApprox = ABS(ax) + ABS(ay) + ABS(az);
		//if (forceMagnitudeApprox > .45 && forceMagnitudeApprox < 2.1)
		//{
			counterism++;

			if (counterism == 8)
			{

				counterism = 0;

				//normalize the acc readings
				arm_sqrt_f32( (ax * ax + ay * ay + az * az), &norm);
				norm = 1.0f/norm;
				ax *= norm;
				ay *= norm;
				az *= norm;

				//radians
				volatile float headingAcc = 0.0f;
				volatile float pitchAcc   = atan2f( filteredAccData[ACCY], filteredAccData[ACCZ] );
				volatile float rollAcc    = atan2f( filteredAccData[ACCX], filteredAccData[ACCZ] );

				//radians
				volatile float reqeustedHeading = 0.0f;
				volatile float reqeustedPitch   = 0.0f;
				volatile float reqeustedRoll    = InlineDegreesToRadians(0.0f);

				c1 = arm_cos_f32(headingAcc * 0.5f);
				c2 = arm_cos_f32(pitchAcc   * 0.5f);
				c3 = arm_cos_f32(rollAcc    * 0.5f);
				s1 = arm_sin_f32(headingAcc * 0.5f);
				s2 = arm_sin_f32(pitchAcc   * 0.5f);
				s3 = arm_sin_f32(rollAcc    * 0.5f);

				accQuat.w = ( (c1 * c2 * c3) - (s1 * s2 * s3) );
				accQuat.x = ( (s1 * s2 * c3) + (c1 * c2 * s3) );
				accQuat.y = ( (s1 * c2 * c3) + (c1 * s2 * s3) );
				accQuat.z = ( (c1 * s2 * c3) - (s1 * c2 * s3) );

				c1 = arm_cos_f32(reqeustedHeading * 0.5f);
				c2 = arm_cos_f32(reqeustedPitch   * 0.5f);
				c3 = arm_cos_f32(reqeustedRoll    * 0.5f);
				s1 = arm_sin_f32(reqeustedHeading * 0.5f);
				s2 = arm_sin_f32(reqeustedPitch   * 0.5f);
				s3 = arm_sin_f32(reqeustedRoll    * 0.5f);

				commandQuat.w = c1 * c2 * c3 - s1 * s2 * s3;
				commandQuat.x = s1 * s2 * c3 + c1 * c2 * s3;
				commandQuat.y = s1 * c2 * c3 + c1 * s2 * s3;
				commandQuat.z = c1 * s2 * c3 - s1 * c2 * s3;

				quat.w = commandQuat.w - accQuat.w;
				quat.x = commandQuat.x - accQuat.x;
				quat.y = commandQuat.y - accQuat.y;
				quat.z = commandQuat.z - accQuat.z;
				//put command into quat



				//arm_cos_f32();
/*
heading = 0 degrees
bank = 90 degrees
attitude = 0 degrees

so substituting this in the above formula gives:

c1 = cos(heading / 2) = 1
c2 = cos(attitude / 2) = 1
c3 = cos(bank / 2) = 0.7071
s1 = sin(heading / 2) = 0
s2 = sin(attitude / 2) = 0
s3 = sin(bank / 2) = 0.7071
w = c1 c2 c3 - s1 s2 s3 = 0.7071
x = s1 s2 c3 +c1 c2 s3 = 0.7071
y = s1 c2 c3 + c1 s2 s3 = 0
z = c1 s2 c3 - s1 c2 s3 = 0

which gives the quaternion 0.7071 + i 0.7071
*/

//		    	pitchAcc = (atan2f( (float)filteredAccData[ACCX], (float)filteredAccData[ACCZ]) + PIf) * (180.0 * IPIf) - 180.0;
//		    	pitchAttitude = pitchAttitude * accCompGyroTrust + pitchAcc * accCompAccTrust;

		    	// Turning around the Y axis results in a vector on the X-axis
//		        rollAcc = (atan2f((float)filteredAccData[ACCY], (float)filteredAccData[ACCZ]) + PIf) * (180.0 * IPIf) - 180.0;
//		        rollAttitude = rollAttitude * accCompGyroTrust + rollAcc * accCompAccTrust;

//				c1 = 1; //no heading info from ACC. This is always 1 since cos(0) = 1
//				c2 = arm_cos_f32(attitude/2);
//				vx = 2.0f * (quat.x * quat.z - quat.w * quat.y);
//				vy = 2.0f * (quat.w * quat.x + quat.y * quat.z);
//				vz = quat.w * quat.w - quat.x * quat.x - quat.y * quat.y + quat.z * quat.z;

//				ex = (ay * vz - az * vy);
//				ey = (az * vx - ax * vz);
//				ez = (ax * vy - ay * vx);

				//float qw = q[0], qx = q[1], qy = q[2], qz = q[3];
				//halfvx = q1 * q3 - q0 * q2;
				//halfvy = q0 * q1 + q2 * q3;
				//halfvz = q0 * q0 - 0.5f + q3 * q3;
				//vx = 2.0f * (q2q4 - q1q3);
				//vy = 2.0f * (q1q2 + q3q4);
				//vz = q1q1 - q2q2 - q3q3 + q4q4;



				//find gravity vector
				//halfVx = quat[1] * quat[3] - quat[0] * quat[2];
				//halfVy = quat[0] * quat[1] + quat[2] * quat[3];
				//halfVz = quat[0] * quat[0] - 0.5f + quat[3] * quat[3];

				//error
				//halfEx = (ay * halfVz - az * halfVy);
				//halfEy = (az * halfVx - ax * halfVz);
				//halfEz = (ax * halfVy - ay * halfVx);

//				integralFBx += accTrustKi * ex * loopSpeed.accdT;	// integral error scaled by Ki
//				integralFBy += accTrustKi * ey * loopSpeed.accdT;
//				integralFBz += accTrustKi * ez * loopSpeed.accdT;

//				gx += integralFBx;	// apply integral feedback
//				gy += integralFBy;
//				gz += integralFBz;

				//correct gyro drift here
//				gx += accTrust * ex;
//				gy += accTrust * ey;
//				gz += accTrust * ez;
			}


		//}
	}

//	+X to the right
//	+Y straight up
//	+Z axis toward viewer
//	Heading = rotation about y axis
//	Attitude = rotation about z axis
//	Bank = rotation about x axis

	//find difference quaternion
	differenceQuat = QuaternionFromEuler ( -InlineDegreesToRadians( gyroRoll ) * loopSpeed.gyrodT, -InlineDegreesToRadians( gyroPitch ) * loopSpeed.gyrodT, -InlineDegreesToRadians( gyroYaw ) * loopSpeed.gyrodT);
	QuaternionNormalize(&differenceQuat);

	QuaternionMultiply(&attitudeFrameQuat,&attitudeFrameQuat,&differenceQuat);
	QuaternionNormalize(&attitudeFrameQuat);

	float squareAnswer;
	arm_sqrt_f32( (1.0f-attitudeFrameQuat.w*attitudeFrameQuat.w) , &squareAnswer);
	float planeangle = 2.0f * acosf(attitudeFrameQuat.w);
	float planex = attitudeFrameQuat.x / squareAnswer;
	float planey = attitudeFrameQuat.y / squareAnswer;
	float planez = attitudeFrameQuat.z / squareAnswer;

	quat.w = attitudeFrameQuat.w;
	quat.x = attitudeFrameQuat.x;
	quat.y = attitudeFrameQuat.y;
	quat.z = attitudeFrameQuat.z;

    /* Compute pitch/roll angles */
	rollAttitude  = InlineRadiansToDegrees(atan2f(rMat[2][1], rMat[2][2]));
	pitchAttitude = InlineRadiansToDegrees(HALF_PI_F - acosf(-rMat[2][0]));
	yawAttitude   = InlineRadiansToDegrees(-atan2f(rMat[1][0], rMat[0][0]));

    //if (yawAttitude < 0)
    //	yawAttitude += 360;


	QuaternionToEuler(&attitudeFrameQuat, &rollAttitude, &pitchAttitude, &yawAttitude);

	return;
	//volatile float avx1 = 2*(differenceQuat.y*differenceQuat.x - differenceQuat.w*differenceQuat.z);
	//volatile float avy1 = 2*(differenceQuat.w*differenceQuat.y + differenceQuat.z*differenceQuat.x);
	//volatile float avz1 = differenceQuat.w*differenceQuat.w - differenceQuat.y*differenceQuat.y - differenceQuat.z*differenceQuat.z + differenceQuat.x*differenceQuat.x;

//	gyroQuat.w=0;
//	gyroQuat.x=0;
//	gyroQuat.y=1;
//	gyroQuat.z=0;

//	QuaternionMultiply(&attitudeFrameQuat,&gyroQuat,&attitudeFrameQuat);
//	QuaternionNormalize(&attitudeFrameQuat);

	//gyroQuat.w=attitudeFrameQuat.w;
	//gyroQuat.x=-attitudeFrameQuat.x;
	//gyroQuat.y=-attitudeFrameQuat.y;
	//gyroQuat.z=-attitudeFrameQuat.z;

	//QuaternionMultiply(&attitudeFrameQuat,&attitudeFrameQuat,&gyroQuat);
	//QuaternionNormalize(&attitudeFrameQuat);


	QuaternionMultiply(&attitudeFrameQuat,&differenceQuat,&attitudeFrameQuat);
	QuaternionNormalize(&attitudeFrameQuat);

	QuaternionToEuler(&inertialFrameQuat, &rollAttitude, &pitchAttitude, &yawAttitude);

	return;
	//multiply it by last gyro quaternion and place product back into differenceQuat quaternion
//	QuaternionMultiply(&inertialFrameQuat,&inertialFrameQuat,&differenceQuat);
//	QuaternionNormalize(&inertialFrameQuat);

	//QuaternionToEuler(&inertialFrameQuat, &rollAttitude, &pitchAttitude, &yawAttitude);


//	QuaternionMultiply(&attitudeFrameQuat,&differenceQuat,&attitudeFrameQuat);
//	QuaternionNormalize(&attitudeFrameQuat);

	QuaternionMultiply(&inertialFrameQuat,&inertialFrameQuat,&differenceQuat);
	QuaternionNormalize(&inertialFrameQuat);

//	QuaternionToEuler(&inertialFrameQuat, &rollAttitude, &pitchAttitude, &yawAttitude);


//    float qdot[4];
//    qdot[0] = (-gyroQuat.x * xxxx - gyroQuat.y * yyyy - gyroQuat.z * zzzz) * loopSpeed.gyrodT;
//    qdot[1] = (gyroQuat.w * xxxx - gyroQuat.z * yyyy + gyroQuat.y * zzzz) * loopSpeed.gyrodT;
//    qdot[2] = (gyroQuat.z * xxxx + gyroQuat.w * yyyy - gyroQuat.x * zzzz) * loopSpeed.gyrodT;
//    qdot[3] = (-gyroQuat.y * xxxx + gyroQuat.x * yyyy + gyroQuat.w * zzzz) * loopSpeed.gyrodT;

    // Take a time step
//    gyroQuat.w = gyroQuat.w + qdot[0];
//    gyroQuat.x = gyroQuat.x + qdot[1];
//    gyroQuat.y = gyroQuat.y + qdot[2];
//    gyroQuat.z = gyroQuat.z + qdot[3];

//    QuaternionNormalize(&gyroQuat);
//	QuaternionToEuler(&gyroQuat, &rollAttitude, &pitchAttitude, &yawAttitude);

//	imuComputeRotationMatrix();

//	QuaternionMultiply(&attitudeFrameQuat,&differenceQuat,&inertialFrameQuat);
//	QuaternionNormalize(&attitudeFrameQuat);

//	QuaternionToEuler(&quat, &rollAttitude, &pitchAttitude, &yawAttitude);




	return;
	float yaw = atan2f( 2.0f * inertialFrameQuat.y * inertialFrameQuat.w - 2.0f * inertialFrameQuat.x * inertialFrameQuat.z , 1.0f - 2.0f * (inertialFrameQuat.y * inertialFrameQuat.y) - 2.0f * (inertialFrameQuat.z * inertialFrameQuat.z) );

	differenceQuat = QuaternionFromEuler(0, yaw, 0);
	QuaternionMultiply(&attitudeFrameQuat,&differenceQuat,&inertialFrameQuat);

	QuaternionNormalize(&attitudeFrameQuat);
	QuaternionToEuler(&attitudeFrameQuat, &rollAttitude, &pitchAttitude, &yawAttitude);

    float x = rMat[0][0] * inertialFrameQuat.x + rMat[0][1] * inertialFrameQuat.y + rMat[0][2] * inertialFrameQuat.z;
    float y = rMat[1][0] * inertialFrameQuat.x + rMat[1][1] * inertialFrameQuat.y + rMat[1][2] * inertialFrameQuat.z;
    float z = rMat[2][0] * inertialFrameQuat.x + rMat[2][1] * inertialFrameQuat.y + rMat[2][2] * inertialFrameQuat.z;;

	volatile quaternion_record quat1;
	quat1.w=0.9238795325112867;
	quat1.x=0.3826834323650898;
	quat1.y=0.0;
	quat1.z=0.0;

	volatile quaternion_record quat2;
	quat2.w=0.9238795325112867;
	quat2.x=0.0;
	quat2.y=0.3826834323650898;
	quat2.z=0.0;

	volatile quaternion_record quat3;
	QuaternionMultiply(&quat3,&quat2,&quat1);
	QuaternionToEuler(&quat3, &rollAttitude, &pitchAttitude, &yawAttitude);
	QuaternionToEuler(&quat3, &rollAttitude, &pitchAttitude, &yawAttitude);
	//gyroQuat.w=cosf(inertialFrameQuat.w * 0.5);
	//gyroQuat.x=inertialFrameQuat.x * sinf(inertialFrameQuat.w * 0.5);
	//gyroQuat.y=inertialFrameQuat.y * sinf(inertialFrameQuat.w * 0.5);
	//gyroQuat.z=inertialFrameQuat.z * sinf(inertialFrameQuat.w * 0.5);
	//differenceQuat.w=0;
	//differenceQuat.x=0;
	//differenceQuat.y=0;
	//differenceQuat.z=1;
	//QuaternionConjugate(&differenceQuat);

	//QuaternionToEuler(&gyroQuat, &rollAttitude, &pitchAttitude, &yawAttitude);

	//QuaternionMultiply(&attitudeFrameQuat,&inertialFrameQuat,&differenceQuat);

	//QuaternionNormalize(&attitudeFrameQuat);

	//differenceQuat.w = 1;
	//differenceQuat.x = 0;
	//differenceQuat.y = 0;
	//differenceQuat.z = 0;
	//QuaternionMultiply(&differenceQuat,&differenceQuat,&gyroQuat);

	//normalize differenceQuat quaternion

	//gyroQuat.w = differenceQuat.w;
	//gyroQuat.x = differenceQuat.x;
	//gyroQuat.y = differenceQuat.y;
	//gyroQuat.z = differenceQuat.z;



	volatile float vx1 = 2*(gyroQuat.y*gyroQuat.x - gyroQuat.w*gyroQuat.z);
	volatile float vy1 = 2*(gyroQuat.w*gyroQuat.y + gyroQuat.z*gyroQuat.x);
	volatile float vz1 = gyroQuat.w*gyroQuat.w - gyroQuat.y*gyroQuat.y - gyroQuat.z*gyroQuat.z + gyroQuat.x*gyroQuat.x;

    float xxx,xxy,xxz;

    /* From body frame to earth frame */
    xxx = rMat[0][0] * vx1 + rMat[0][1] * vy1 + rMat[0][2] * vz1;
    xxy = rMat[1][0] * vx1 + rMat[1][1] * vy1 + rMat[1][2] * vz1;
    xxz = rMat[2][0] * vx1 + rMat[2][1] * vy1 + rMat[2][2] * vz1;

	volatile float xxxxx = ax;
	volatile float yyyyy = ay;
	volatile float zzzzz = az;

	volatile float xxxxx111 = x;
	volatile float yyyyy111 = -y;
	volatile float zzzzz111 = z;
	return;

//	QuaternionMultiply(&gyroQuat,&differenceQuat,&gyroQuat);
//	QuaternionMultiply(&differenceQuat,&groundQuat,&gyroQuat);

	//volatile float dotProduct = QuaternionDotProduct(&gyroQuat, &groundQuat);
//	QuaternionToEuler(&differenceQuat, &rollAttitude, &pitchAttitude, &yawAttitude);


	//roll (phi)    is rotation about the x axis, positive is a roll to the right
	//pitch (theta) is rotation about the y axis, positive is a pitch up
	//yaw is (psi)  is rotation about the z axism positive is a yaw right
	//w is the real componant,
	//i is x,
	//j is y,
	//k is z,



	//DPS to half angles in RPS multiplied by half DeltaT to provide movement since last itteration in half angles


	//roll/bank      (phi)   is rotation about the x axis, positive is a roll to the right
	//pitch/attitude (theta) is rotation about the y axis, positive is a pitch up
	//yaw/headgin    (psi)   is rotation about the z axism positive is a yaw right
//	c1 = arm_cos_f32(gz);
//	c2 = arm_cos_f32(gy);
//	c3 = arm_cos_f32(gx);
//	s1 = arm_sin_f32(gz);
//	s2 = arm_sin_f32(gy);
//	s3 = arm_sin_f32(gx);

//	differenceQuat.w = ( (c1 * c2 * c3) - (s1 * s2 * s3) );
//	differenceQuat.x = ( (s1 * s2 * c3) + (c1 * c2 * s3) );
//	differenceQuat.y = ( (s1 * c2 * c3) + (c1 * s2 * s3) );
//	differenceQuat.z = ( (c1 * s2 * c3) - (s1 * c2 * s3) );

	//differenceQuat.w = arm_cos_f32(gx) * arm_cos_f32(gy) * arm_cos_f32(gz) + arm_sin_f32(gx) * arm_sin_f32(gy) * arm_sin_f32(gz);
	//differenceQuat.x = arm_sin_f32(gx) * arm_cos_f32(gy) * arm_cos_f32(gz) - arm_cos_f32(gx) * arm_sin_f32(gy) * arm_sin_f32(gz);
	//differenceQuat.y = arm_cos_f32(gx) * arm_sin_f32(gy) * arm_cos_f32(gz) + arm_sin_f32(gx) * arm_cos_f32(gy) * arm_sin_f32(gz);
	//differenceQuat.z = arm_cos_f32(gx) * arm_cos_f32(gy) * arm_sin_f32(gz) - arm_sin_f32(gx) * arm_sin_f32(gy) * arm_cos_f32(gz);

	//QuaternionMultiply(&gyroQuat,&gyroQuat,&differenceQuat);

	//arm_sqrt_f32( (gyroQuat.w * gyroQuat.w + gyroQuat.x * gyroQuat.x + gyroQuat.y * gyroQuat.y + gyroQuat.z * gyroQuat.z), &norm);
	//norm = 1.0f/norm;
	//gyroQuat.w *= norm;
	//gyroQuat.x *= norm;
	//gyroQuat.y *= norm;
	//gyroQuat.z *= norm;

	//gyroQuat.w -= differenceQuat.w;
	//gyroQuat.x -= differenceQuat.x;
	//gyroQuat.y -= differenceQuat.y;
	//gyroQuat.z -= differenceQuat.z;


//	arm_sqrt_f32( (gyroQuat.w * gyroQuat.w + gyroQuat.x * gyroQuat.x + gyroQuat.y * gyroQuat.y + gyroQuat.z * gyroQuat.z), &norm);
//	norm = 1.0f/norm;
//	gyroQuat.w *= norm;
//	gyroQuat.x *= norm;
//	gyroQuat.y *= norm;
//	gyroQuat.z *= norm;

	//	qDotW = 0.5f * -qx*gx - qy*gy - qz*gz;
	//	  qDotX = 0.5f *  qw*gx + qy*gz - qz*gy;
		//  qDotY = 0.5f *  qw*gy - qx*gz + qz*gx;
		  //qDotZ = 0.5f *  qw*gz + qx*gy - qy*gx;

//	previousQuat.w = 0.5f * -quat.x*gx - quat.y*gy - quat.z*gz;
//	previousQuat.x = 0.5f *  quat.w*gx + quat.y*gz - quat.z*gy;
//	previousQuat.y = 0.5f *  quat.w*gy - quat.x*gz + quat.z*gx;
//	previousQuat.z = 0.5f *  quat.w*gz + quat.x*gy - quat.y*gx;

//	quat.w += previousQuat.w * loopSpeed.gyrodT;
//	quat.x += previousQuat.x * loopSpeed.gyrodT;
//	quat.y += previousQuat.y * loopSpeed.gyrodT;
//	quat.z += previousQuat.z * loopSpeed.gyrodT;

	//normalize quaternion
//	arm_sqrt_f32( (quat.w * quat.w + quat.x * quat.x + quat.y * quat.y + quat.z * quat.z), &norm);
//	norm = 1.0f/norm;
//	quat.w *= norm;
//	quat.x *= norm;
//	quat.y *= norm;
//	quat.z *= norm;

}

void CalculateQuaternions(void)
{


	//yawAttitude   = atan2f( 2.0f * (quat.x * quat.y + quat.w * quat.z), quat.w * quat.w + quat.x * quat.x - quat.y * quat.y - quat.z * quat.z) * 180 * IPIf;
	//pitchAttitude = -asinf( 2.0f * (quat.x * quat.z - quat.w * quat.y)) * 180 * IPIf;
	//rollAttitude  = 180.0f + atan2f( 2.0f * (quat.w * quat.x + quat.y * quat.z), quat.w * quat.w - quat.x * quat.x - quat.y * quat.y + quat.z * quat.z) * 180 * IPIf;

	//yawAttitude   = atan2f( 2.0 * (quat.y * quat.z + quat.w * quat.x), quat.w * quat.w - quat.x * quat.x - quat.y * quat.y + quat.z * quat.z) * 180 * IPIf;
	//pitchAttitude = asinf( -2.0 * (quat.x * quat.z - quat.w * quat.y)) * 180 * IPIf;
	//rollAttitude  = atan2f( 2.0 * (quat.x * quat.y + quat.w * quat.z), quat.w * quat.w + quat.x * quat.x - quat.y * quat.y - quat.z * quat.z) * 180 * IPIf;

//    float gx    = 2 * (quat[1] * quat[3] - quat[0] * quat[2]);
//    float gy    = 2 * (quat[0] * quat[1] + quat[2] * quat[3]);
//    float gz    = quat[0] * quat[0] - quat[1] * quat[1] - quat[2] * quat[2] + quat[3] * quat[3];

//    float roll  = atanf(gy / sqrtf(gx * gx + gz * gz));
//    float pitch = atanf(gx / sqrtf(gy * gy + gz * gz));
//    float yaw   = atan2f(2 * quat[1] * quat[2] - 2 * quat[0] * quat[3], 2 * quat[0] * quat[0] + 2 * quat[1] * quat[1] - 1);

//    rollAttitude  =  roll  * 180 * IPIf;
//    pitchAttitude =  pitch * 180 * IPIf;
//    yawAttitude   =  -yaw  * 180 * IPIf;


//	roll  = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z);
//	pitch = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z);
//	yaw   = asin(2*x*y + 2*z*w);

//	yawAttitude   = atan2(2.0f * (quat[1] * quat[2] + quat[0] * quat[3]), quat[0] * quat[0] + quat[1] * quat[1] - quat[2] * quat[2] - quat[3] * quat[3]);
//	pitchAttitude = asin(2.0f * (quat[1] * quat[3] - quat[0] * quat[2]));
//	rollAttitude  = atan2(2.0f * (quat[0] * quat[1] + quat[2] * quat[3]), quat[0] * quat[0] - quat[1] * quat[1] - quat[2] * quat[2] + quat[3] * quat[3]);

//	yawAttitude   *= 180.0f * IPIf;
//	pitchAttitude *= 180.0f * IPIf;
//	rollAttitude  *= 180.0f * IPIf;
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
