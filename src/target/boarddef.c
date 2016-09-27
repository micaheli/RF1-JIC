#include "includes.h"

GPIO_TypeDef *ports[10];
USART_TypeDef *usarts[6];


int InitializeMCUSettings() {
	//target_pinout pins;

	bzero(ports, sizeof(ports));
	ports[0]=GPIOA;
	ports[1]=GPIOB;
	ports[2]=GPIOC;
	ports[3]=GPIOD;
	ports[4]=GPIOE;
	ports[5]=GPIOF;
	ports[6]=GPIOG;
	ports[7]=GPIOH;
	ports[8]=GPIOJ;
	ports[9]=GPIOK;

	bzero(usarts, sizeof(usarts));
	usarts[0]=USART1;
	usarts[1]=USART2;
	usarts[2]=USART3;
	usarts[3]=USART4;
	usarts[4]=USART5;
	usarts[5]=USART6;


	return(1);
}
