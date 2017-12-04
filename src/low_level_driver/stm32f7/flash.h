#pragma once

#include <stdbool.h>

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 32 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08008000) /* Base @ of Sector 1, 32 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08010000) /* Base @ of Sector 2, 32 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08018000) /* Base @ of Sector 3, 32 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08020000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08040000) /* Base @ of Sector 5, 256 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08080000) /* Base @ of Sector 6, 256 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x080C0000) /* Base @ of Sector 7, 256 Kbytes */

void PrepareFlash(void);
void FinishFlash(void);
int WriteFlash(uint32_t data32, uint32_t flashAddress );
int EraseFlash(uint32_t flashStart, uint32_t flashEnd);
uint32_t GetFlashSector(uint32_t flashAddress);
