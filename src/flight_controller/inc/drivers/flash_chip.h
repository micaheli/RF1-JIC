#pragma once

extern int InitFlashChip(void);
extern void MassEraseDataFlash(void);
extern void FLASH_DMA_RX_IRQHandler(void);
extern void DataFlashBeginProgram(uint32_t address);
extern int FlashChipWriteData(uint8_t *data, uint8_t length);
extern int FlashChipReadWriteData(uint8_t *txData, uint8_t *rxData, uint8_t length);
extern int FlashChipReadData(uint32_t address, uint8_t *buffer, int length);
extern void MassEraseDataFlash(void);
extern void DataFlashProgramPage(uint32_t address, uint8_t *data, uint16_t length);
extern void WriteEnableDataFlash(void);
extern uint8_t m25p16_readStatus(void);
