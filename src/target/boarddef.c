#include "includes.h"

GPIO_TypeDef *pins[256]; // last byte is count

int InitializeMCUSettings() {
	//target_pinout pins;

	bzero(pins, sizeof(pins));

	pins[0]=GPIO_PIN_0;
	pins[1]=GPIO_PIN_1;
	pins[2]=GPIO_PIN_2;
	pins[3]=GPIO_PIN_3;//gyro_cs
	pins[4]=GPIO_PIN_4;//gyro exti
	pins[5]=GPIO_PIN_5;//sbus inverter
	pins[6]=GPIO_PIN_6;//vbus sense
	pins[7]=GPIO_PIN_7;//usart 1 rx
	pins[8]=GPIO_PIN_8;//usart 1 tx
	pins[9]=GPIO_PIN_9;//usart 3 rx
	pins[10]=GPIO_PIN_10;//usart 3 tx
	pins[11]=GPIO_PIN_11;//usart 6 rx
	pins[12]=GPIO_PIN_12;//usart 6 tx
	pins[13]=GPIO_PIN_13;//rfbl bind1
	pins[14]=GPIO_PIN_14;//rfbl bind2
	pins[15]=GPIO_PIN_15;
	pins[255]=16;


	return(1);

}
