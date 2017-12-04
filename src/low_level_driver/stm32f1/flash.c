#include "includes.h"


bool EraseFlash(uint32_t flashStart, uint32_t flashEnd) {

    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

	HAL_FLASH_Unlock();

	//todo made this configurable
	/* Fill EraseInit structure*/

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = flashStart;
	EraseInitStruct.NbPages     = (flashEnd - flashStart) / FLASH_PAGE_SIZE;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{
		//FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
		return false;
	}
	return true;

}
