#pragma once

#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_it.h"
#include "stm32f7xx_hal_conf.h"

//asm delay used for not so accurate delays needed before the board is initialized
#define simpleDelay_ASM(us) do {\
	asm volatile (	"MOV R0,%[loops]\n\t"\
			"1: \n\t"\
			"SUB R0, #1\n\t"\
			"CMP R0, #0\n\t"\
			"BNE 1b \n\t" : : [loops] "r" (16*us) : "memory"\
		      );\
} while(0)
