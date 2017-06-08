#pragma once

#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_it.h"
#include "stm32f3xx_hal_conf.h"
#include "flash.h"
//#include <stm32f303xc.h>
//#include <stm32f301xk.h>


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
extern void VectorIrqInit(uint32_t address);
extern int FULL_32;


void USB_DEVICE_Init(void);
void USB_DEVICE_DeInit(void);
#define EXTI2_IRQn 0

#define USE_RFBL
#define ADDRESS_FLASH_START		(uint32_t)(0x08000000)
#define ADDRESS_RECOVERY_START	(uint32_t)(0x08000000)
#define ADDRESS_RFBL_START		(uint32_t)(0x08008000)
#define ADDRESS_CONFIG_START	(uint32_t)(0x08010000)
#define ADDRESS_RFFW_START		(uint32_t)(0x08014000)
#define ADDRESS_ESC_START		(uint32_t)(0x08040000)
#define ADDRESS_FLASH_END		(uint32_t)(0x08040000)

#define _TIM1 TIM1
#define _TIM2 TIM2
#define _TIM3 TIM3
#define _TIM4 TIM4
#define _TIM5 TIM5
#define _TIM6 TIM6
#define _TIM7 TIM7
#define _TIM8 TIM8
#define _TIM9 TIM9
#define _TIM10 TIM10
#define _TIM11 TIM11
#define _TIM12 TIM12
#define _TIM13 TIM13
#define _TIM14 TIM14

#define _GPIOA GPIOA
#define _GPIOB GPIOB
#define _GPIOC GPIOC
#define _GPIOD GPIOD

#define _PORTA 0
#define _PORTB 1
#define _PORTC 2
#define _PORTD 3
#define _PORTE 4
#define _PORTF 5
#define _PORTG 6
#define _PORTH 7
#define _PORTI 8

#define STM32_UUID ((uint32_t *)0x1FFF7A10)

extern uint32_t TimerPrescalerDivisor(uint32_t timer);
