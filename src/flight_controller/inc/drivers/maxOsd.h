#pragma once



typedef struct {
	volatile uint32_t enabled;
	volatile uint32_t chipId;
	volatile uint32_t videoMode;
} max_osd_record;

extern max_osd_record maxOsdRecord;

extern int  InitMaxOsd(void);
extern void MaxOsdDmaRxCallback(uint32_t callbackNumber);