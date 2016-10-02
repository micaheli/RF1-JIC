#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "includes.h"

GPIO_TypeDef *ports[11];
USART_TypeDef *usarts[6];


int InitializeMCUSettings() {
	//target_pinout pins;

	bzero(ports, sizeof(ports));
	ports[0]=_GPIOA;
	ports[1]=_GPIOB;
	ports[2]=_GPIOC;
	ports[3]=_GPIOD;
	ports[4]=_GPIOE;
	ports[5]=_GPIOF;
	ports[6]=_GPIOG;
	ports[7]=_GPIOH;
	ports[8]=_GPIOI;
	ports[9]=_GPIOJ;
	ports[10]=_GPIOK;

	bzero(usarts, sizeof(usarts));
	usarts[0]=USART1;
	usarts[1]=USART2;
	usarts[2]=USART3;
	usarts[3]=USART4;
	usarts[4]=USART5;
	usarts[5]=USART6;


	return(1);
}

#ifdef F4
#define _GPIOA null
#elf
