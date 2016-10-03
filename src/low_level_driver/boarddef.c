#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "includes.h"

GPIO_TypeDef *ports[11];
serial_type usarts[6];
spi_type spis[5];
TIM_TypeDef timers[14];


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
	usarts.port[0]=_USART1;
	usarts.async[0]=_USART1s;

	usarts.port[1]=_USART2;
	usarts.async[1]=_USART2s;

	usarts.port[2]=_USART3;
	usarts.async[2]=_USART3s;

	usarts.port[3]=_USART4;
	usarts.async[3]=_USART4s;

	usarts.port[4]=_USART5;
	usarts.async[4]=_USART5s;

	usarts.port[5]=_USART6;
	usarts.async[5]=_USART6s;

	bzero(timers, sizeof(timers));
	timers[0]=_TIM1;
	timers[1]=_TIM2;
	timers[2]=_TIM3;
	timers[3]=_TIM4;
	timers[4]=_TIM5;
	timers[5]=_TIM6;
	timers[6]=_TIM7;
	timers[7]=_TIM8;
	timers[8]=_TIM9;
	timers[9]=_TIM10;
	timers[10]=_TIM11;
	timers[11]=_TIM12;
	timers[12]=_TIM13;
	timers[13]=_TIM14;


	return(1);
}

