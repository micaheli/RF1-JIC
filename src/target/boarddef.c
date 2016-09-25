#include "includes.h"

uint16_t pins[16];
GPIO_TypeDef *ports[10];


int InitializeMCUSettings() {
	//target_pinout pins;

	bzero(pins, sizeof(pins));

	pins[0]=GPIO_PIN_0;
	pins[1]=GPIO_PIN_1;
	pins[2]=GPIO_PIN_2;
	pins[3]=GPIO_PIN_3;
	pins[4]=GPIO_PIN_4;
	pins[5]=GPIO_PIN_5;
	pins[6]=GPIO_PIN_6;
	pins[7]=GPIO_PIN_7;
	pins[8]=GPIO_PIN_8;
	pins[9]=GPIO_PIN_9;
	pins[10]=GPIO_PIN_10;
	pins[11]=GPIO_PIN_11;
	pins[12]=GPIO_PIN_12;
	pins[13]=GPIO_PIN_13;
	pins[14]=GPIO_PIN_14;
	pins[15]=GPIO_PIN_15;

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

	return(1);

}
