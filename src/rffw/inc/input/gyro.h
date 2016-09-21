#pragma once

extern int16_t dpsGyroArray[3];

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

void updateGyro(int16_t *rawGyro, float scale);
