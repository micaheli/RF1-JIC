#pragma once

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_it.h"
#include "stm32f1xx_hal_conf.h"
#include "flash.h"

//#define STM32_UUID ((uint32_t *)0x1FFF7A10)

//asm delay used for not so accurate delays needed before the board is initialized
#define simpleDelay_ASM(us) do {\
	asm volatile (	"MOV R0,%[loops]\n\t"\
			"1: \n\t"\
			"SUB R0, #1\n\t"\
			"CMP R0, #0\n\t"\
			"BNE 1b \n\t" : : [loops] "r" (16*us) : "memory"\
		      );\
} while(0)

extern uint32_t TimerPrescalerDivisor(uint32_t timer);
