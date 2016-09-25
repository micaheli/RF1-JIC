#include "includes.h"

GPIO_TypeDef *pins[256]; // last byte is count

int InitializeMCUSettings() {
	//target_pinout pins;

	bzero(pins, sizeof(pins));
	pins[0]=GPIOA;

	//pins.usart_ports[0].rxGPIO = GPIOA;

	return(1);

}
