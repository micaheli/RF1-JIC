#include "includes.h"

int InitializeMcu() {
	//target_pinout pins;

	bzero(pins, sizeof(pins));
	pins[0]=GPIOA;

	//pins.usart_ports[0].rxGPIO = GPIOA;

	return(1);

}
