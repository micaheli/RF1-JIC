#pragma once

#define GYRO_CALIBRATION_CYCLES 1000

extern volatile uint32_t gyroCalibrationCycles;
extern float geeForceAccArray[3];

//config structure which is loaded by config
typedef struct {
    int32_t minorBoardRotation[3]; //X, Y, Z
    uint32_t gyroRotation;
    uint32_t boardCalibrated; //board calibration complete?
    uint32_t loopCtrl;
} gyro_config;

enum {X=0,Y,Z};

enum {
	CW0=0,
	CW90=1,
	CW180=2,
	CW270=3,
	CW0_INV=4,
	CW90_INV=5,
	CW180_INV=6,
	CW270_INV=7,
	CW45=8,
	CW135=9,
	CW225=10,
	CW315=11,
	CW45_INV=12,
	CW135_INV=13,
	CW225_INV=14,
	CW315_INV=15,
};

typedef enum {
    LOOP_L1,
    LOOP_M1,
    LOOP_M2,
    LOOP_M4,
    LOOP_M8,
    LOOP_H1,
    LOOP_H2,
    LOOP_H4,
    LOOP_H8,
    LOOP_H16,
    LOOP_H32,
    LOOP_UH1,
    LOOP_UH2,
    LOOP_UH4,
    LOOP_UH8,
    LOOP_UH16,
    LOOP_UH32,
	LOOP_UH_500,
	LOOP_UH_250,
	LOOP_UH_062,
} loopCtrl_e;

void ResetGyroCalibration(void);
void InlineUpdateAcc(int32_t rawAcc[], float scale);
void InlineUpdateGyro(int32_t rawGyro[], float scale);
void InlineApplyGyroAccRotationAndScale (int32_t rawData[], float dataArray[], float scale );
