/*
*****************************************************************************
**
**  File        : stm32_flash.ld
**
**  Abstract    : Linker script for STM32F103C8 Device with
**                32KByte FLASH, 6KByte RAM
**
*****************************************************************************
*/

/* Specify the memory areas. */
MEMORY
{
	FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 32K
	RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 6K
	MEMORY_B1 (rx)  : ORIGIN = 0x60000000, LENGTH = 0K
}

INCLUDE "stm32_flash.ld"
