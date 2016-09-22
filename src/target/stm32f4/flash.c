#include "includes.h"


bool EraseFlash(uint32_t flashStart, uint32_t flashEnd) {

    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;
    uint32_t flashSectorStart = 0;

	//ghetto method until we have a better method
	if (flashStart == 0x08020000) {
		flashSectorStart = FLASH_SECTOR_5;
	} else {
		flashSectorStart = FLASH_SECTOR_5;
	}

	HAL_FLASH_Unlock();

	//todo made this configurable
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = flashSectorStart;
	if (flashEnd >= 0x080F0000) { //todo: Base off of MCU
		EraseInitStruct.NbSectors = 7;
	} else {
		EraseInitStruct.NbSectors = 3;
	}

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{
		//FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
		return false;
	}
	return true;

}
