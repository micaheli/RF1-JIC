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
    uint32_t filterTypeGyro;
    uint32_t filterTypeKd;
} gyro_config;

enum {X=0,Y,Z};

enum {CW0=0,CW90,CW180,CW270,CW0_INV,CW90_INV,CW180_INV,CW270_INV};

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
} loopCtrl_e;

void ResetGyroCalibration(void);
void InlineUpdateAcc(int32_t rawAcc[], float scale);
void InlineUpdateGyro(int32_t rawGyro[], float scale);
void InlineApplyGyroAccRotationAndScale (int32_t rawData[], float dataArray[], float scale );
