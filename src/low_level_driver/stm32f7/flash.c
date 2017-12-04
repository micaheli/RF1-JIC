#include "includes.h"

void PrepareFlash()
{
	HAL_FLASH_Unlock();
}

void FinishFlash()
{
	HAL_FLASH_Lock();
}

int WriteFlash(uint32_t data32, uint32_t flashAddress )
{

	if (HAL_FLASH_Program(TYPEPROGRAM_WORD, flashAddress, data32) == HAL_OK)
	{
		return 1;
	}
	else
	{
		//FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
		//todo: return error code.
		return 0;
	}
}


int EraseFlash(uint32_t flashStart, uint32_t flashEnd)
{

    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

	HAL_FLASH_Unlock();

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = GetFlashSector(flashStart);
	EraseInitStruct.NbSectors = GetFlashSector(flashEnd) - GetFlashSector(flashStart) + 1; //sectors are ordered in order from 0 to 11, so NbSectors is just the difference plus 1


	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{
		//todo: return error code.
		//FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
		HAL_FLASH_Lock();
		return 0;
	}
	HAL_FLASH_Lock();
	return 1;

}

uint32_t GetFlashSector(uint32_t flashAddress)
{

    if ((flashAddress < ADDR_FLASH_SECTOR_1) && (flashAddress >= ADDR_FLASH_SECTOR_0))
	{
        return (FLASH_SECTOR_0);
    }
	else if ((flashAddress < ADDR_FLASH_SECTOR_2) && (flashAddress >= ADDR_FLASH_SECTOR_1))
	{
        return (FLASH_SECTOR_1);
    }
	else if ((flashAddress < ADDR_FLASH_SECTOR_3) && (flashAddress >= ADDR_FLASH_SECTOR_2))
	{
        return (FLASH_SECTOR_2);
    }
	else if ((flashAddress < ADDR_FLASH_SECTOR_4) && (flashAddress >= ADDR_FLASH_SECTOR_3))
	{
        return (FLASH_SECTOR_3);
    }
	else if ((flashAddress < ADDR_FLASH_SECTOR_5) && (flashAddress >= ADDR_FLASH_SECTOR_4))
	{
        return (FLASH_SECTOR_4);
    }
	else if ((flashAddress < ADDR_FLASH_SECTOR_6) && (flashAddress >= ADDR_FLASH_SECTOR_5))
	{
        return (FLASH_SECTOR_5);
    }
	else if ((flashAddress < ADDR_FLASH_SECTOR_7) && (flashAddress >= ADDR_FLASH_SECTOR_6))
	{
        return (FLASH_SECTOR_6);
    }
	else
	{
        return (FLASH_SECTOR_7);
    }

}
