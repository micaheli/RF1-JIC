#pragma once

extern float dpsGyroArrayUnrotated[3];
extern float dpsGyroArray[3];
extern int16_t rawGyroRotated[3];

//config structure which is loaded by config
typedef struct {
    int16_t minorBoardRotation[3]; //X, Y, Z
    unsigned char gyroRotation;
    unsigned char boardRotation;
    unsigned char loopCtrl;
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

void InlineUpdateGyro(int16_t rawGyro[], float scale);
void InlineApplyGyroRotation (int16_t rawGyro[], int16_t rawGyroRotated[]);
void InlineApplyBoardRotationAndScale ( int16_t rawGyroRotated[], float dpsGyroArray[], float scale );
