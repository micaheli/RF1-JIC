#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../flight_controller/inc/rf_math.h"
#include "includes.h"

GPIO_TypeDef *ports[11];
serial_type usarts[6];
spi_type spis[5];
TIM_TypeDef *timers[14];
board_type board;


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
	usarts[0].port=_USART1;
	usarts[0].async=_USART1s;

	usarts[1].port=_USART2;
	usarts[1].async=_USART2s;

	usarts[2].port=_USART3;
	usarts[2].async=_USART3s;

	usarts[3].port=_USART4;
	usarts[3].async=_USART4s;

	usarts[4].port=_USART5;
	usarts[4].async=_USART5s;

	usarts[5].port=_USART6;
	usarts[5].async=_USART6s;

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



	bzero(&board, sizeof(board));

	board.internalLeds[0].enabled=1;
	board.internalLeds[0].pin=GPIO_PIN_4;
	board.internalLeds[0].port=1;

	board.internalLeds[1].enabled=1;
	board.internalLeds[1].pin=GPIO_PIN_5;
	board.internalLeds[1].port=1;

	board.internalLeds[1].enabled=1;
	board.internalLeds[1].pin=GPIO_PIN_1;
	board.internalLeds[1].port=0;

	board.buzzerPort=0;
	board.buzzerPin=GPIO_PIN_0;






	return(1);
}

