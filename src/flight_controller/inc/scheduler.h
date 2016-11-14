#pragma once

extern uint32_t autoSaveTimer;

void scheduler(int32_t count);
void taskAutoSaveConfig(void);
void taskHandlePcComm(void);
void taskLed(void);
void taskBuzzer(void);
