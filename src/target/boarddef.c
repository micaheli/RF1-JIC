#include "includes.h"

GPIO_TypeDef *ports[10];


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


	return(1);

}
