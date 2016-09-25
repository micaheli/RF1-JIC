#include "includes.h"



int initializeTarget() {
	target_pinout pins;

	bzero(pins, sizeof(pins));

	pins.usart_ports[0].rxGPIO = GPIOA;





	return(1);

}
