#include "includes.h"

int InitializeMcu() {

	target_pinout pins;

	bzero(pins, sizeof(pins));

	pins.usart_ports[0].rxGPIO = GPIOA;





	return(1);

}
