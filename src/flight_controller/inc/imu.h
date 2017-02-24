#pragma once


typedef struct {
	volatile float x;
	volatile float y;
	volatile float z;
	volatile float w;
} quaternion_record;


extern volatile quaternion_record quat;

extern void InitImu(void);
extern void ConvertToQuaternion(float gx, float gy, float gz, float ax, float ay, float az);
extern void CalculateQuaternions(void);
