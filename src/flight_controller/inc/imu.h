#pragma once

extern volatile float quat[];

extern void InitImu(void);
extern void ConvertToQuaternion(float gx, float gy, float gz, float ax, float ay, float az);
extern void CalculateQuaternions(void);
