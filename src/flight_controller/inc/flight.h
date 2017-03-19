#pragma once


typedef struct {
	volatile float dT;
	volatile uint32_t gyroAccDiv;
	volatile float gyrodT;
	volatile float halfGyrodT;
	volatile float accdT;
	volatile float InversedT;
	volatile uint32_t uhohNumber;
	volatile uint32_t khzDivider;
	volatile uint32_t gyroDivider;
	volatile uint32_t fsCount;
} loop_speed_record;

extern volatile loop_speed_record loopSpeed;
extern float filteredGyroData[];
extern volatile uint32_t boardArmed, calibrateMotors, fullKiLatched;
extern float pitchAttitude, rollAttitude, yawAttitude;
extern volatile uint32_t SKIP_GYRO;
extern float accNoise[];
extern float filteredAccData[];


enum { CALIBRATE_BOARD_FAILED = 0, CALIBRATE_BOARD_UPRIGHT = 1, CALIBRATE_BOARD_INVERTED = 2, };

extern void DeinitFlight(void);
extern void InitFlight(void);
extern void ArmBoard(void);
extern void DisarmBoard(void);
extern int SetCalibrate1(void);
extern int SetCalibrate2(void);
extern float AverageGyroADCbuffer(uint32_t axis, volatile float currentData); //not really extern
void ComplementaryFilterUpdateAttitude(void);
void InitFlightCode(void);
void InlineInitGyroFilters(void);
void InlineInitKdFilters(void);
void InlineInitSpectrumNoiseFilter(void);
void InlineInitAccFilters(void);
void InlineFlightCode(float dpsGyroArray[]);
void InlineUpdateAttitude(float geeForceAccArray[]);
float InlineGetSetPoint(float curvedRcCommandF, uint32_t curveToUse, float rates, float acroPlus, uint32_t axis);
