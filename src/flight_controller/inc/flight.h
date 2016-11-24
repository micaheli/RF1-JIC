#pragma once

extern volatile uint32_t boardArmed, calibrateMotors, fullKiLatched;
extern float pitchAttitude, rollAttitude, yawAttitude;
extern volatile uint32_t SKIP_GYRO;

enum { CALIBRATE_BOARD_FAILED = 0, CALIBRATE_BOARD_UPRIGHT = 1, CALIBRATE_BOARD_INVERTED = 2, };

extern int SetCalibrate1(void);
extern int SetCalibrate2(void);
extern float AverageGyroADCbuffer(uint32_t axis, float currentData); //not really extern
void ComplementaryFilterUpdateAttitude(void);
void InitFlightCode(void);
void InlineInitGyroFilters(void);
void InlineInitAccFilters(void);
void InlineFlightCode(float dpsGyroArray[]);
void InlineUpdateAttitude(float geeForceAccArray[]);
float InlineGetSetPoint(float curvedRcCommandF, float rates, float acroPlus);
