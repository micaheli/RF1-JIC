#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "includes.h"

GPIO_TypeDef *ports[11];
serial_type usarts[6];
spi_type spis[5];



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



	return(1);
}

