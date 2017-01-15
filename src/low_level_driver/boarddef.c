#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#include "../flight_controller/inc/rf_math.h"
#include "includes.h"

board_type          board;
GPIO_TypeDef       *ports[11];
serial_type         usarts[6];
spi_type            spis[6];
TIM_TypeDef        *timers[14];
volatile uint32_t  *ccr[56];
DMA_Stream_TypeDef *dmaStream[16];
UART_HandleTypeDef  uartHandles[6];
DMA_HandleTypeDef   dmaHandles[16];
TIM_HandleTypeDef   pwmTimers[16];
TIM_OC_InitTypeDef  sConfigOCHandles[16];
SPI_HandleTypeDef   spiHandles[6];
SPI_TypeDef        *spiInstance[6];


unsigned char serialRxBuffer[3][RXBUFFERSIZE];
unsigned char serialTxBuffer[3][TXBUFFERSIZE];
uint32_t motorOutputBuffer[8][1500];


volatile function_pointer callbackFunctionArray[IRQH_FP_TOT];


int InitializeMCUSettings() {
	//target_pinout pins;

	//for (uint32_t x = 0;x<IRQH_FP_TOT;x++) {
	//	callbackFunctionArray[x] = 0;
	//}

	bzero(spiInstance, sizeof(spiInstance));
	spiInstance[0] = SPI1;
	spiInstance[1] = SPI2;
	spiInstance[2] = SPI3;//TODO: Add enum for other SPIs so all boards can use what they have avilable.

	bzero(dmaStream, sizeof(dmaStream));
	dmaStream[0]  = DMA1_Stream0;
	dmaStream[1]  = DMA1_Stream1;
	dmaStream[2]  = DMA1_Stream2;
	dmaStream[3]  = DMA1_Stream3;
	dmaStream[4]  = DMA1_Stream4;
	dmaStream[5]  = DMA1_Stream5;
	dmaStream[6]  = DMA1_Stream6;
	dmaStream[7]  = DMA1_Stream7;
	dmaStream[8]  = DMA2_Stream0;
	dmaStream[9]  = DMA2_Stream1;
	dmaStream[10] = DMA2_Stream2;
	dmaStream[11] = DMA2_Stream3;
	dmaStream[12] = DMA2_Stream4;
	dmaStream[13] = DMA2_Stream5;
	dmaStream[14] = DMA2_Stream6;
	dmaStream[15] = DMA2_Stream7;

	bzero(ports, sizeof(ports));
	ports[0]  = _GPIOA;
	ports[1]  = _GPIOB;
	ports[2]  = _GPIOC;
	ports[3]  = _GPIOD;
	ports[4]  = _GPIOE;
	ports[5]  = _GPIOF;
	ports[6]  = _GPIOG;
	ports[7]  = _GPIOH;
	ports[8]  = _GPIOI;
	ports[9]  = _GPIOJ;
	ports[10] = _GPIOK;

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

	bzero(ccr, sizeof(ccr));
	ccr[0] = &_TIM1->CCR1;
	ccr[1] = &_TIM1->CCR2;
	ccr[2] = &_TIM1->CCR3;
	ccr[3] = &_TIM1->CCR4;
	ccr[4] = &_TIM2->CCR1;
	ccr[5] = &_TIM2->CCR2;
	ccr[6] = &_TIM2->CCR3;
	ccr[7] = &_TIM2->CCR4;
	ccr[8] = &_TIM3->CCR1;
	ccr[9] = &_TIM3->CCR2;
	ccr[10] = &_TIM3->CCR3;
	ccr[11] = &_TIM3->CCR4;
	ccr[12] = &_TIM4->CCR1;
	ccr[13] = &_TIM4->CCR2;
	ccr[14] = &_TIM4->CCR3;
	ccr[15] = &_TIM4->CCR4;
	ccr[16] = &_TIM5->CCR1;
	ccr[17] = &_TIM5->CCR2;
	ccr[18] = &_TIM5->CCR3;
	ccr[19] = &_TIM5->CCR4;
	ccr[20] = &_TIM6->CCR1;
	ccr[21] = &_TIM6->CCR2;
	ccr[22] = &_TIM6->CCR3;
	ccr[23] = &_TIM6->CCR4;
	ccr[24] = &_TIM7->CCR1;
	ccr[25] = &_TIM7->CCR2;
	ccr[26] = &_TIM7->CCR3;
	ccr[27] = &_TIM7->CCR4;
	ccr[28] = &_TIM8->CCR1;
	ccr[29] = &_TIM8->CCR2;
	ccr[30] = &_TIM8->CCR3;
	ccr[31] = &_TIM8->CCR4;
	ccr[32] = &_TIM9->CCR1;
	ccr[33] = &_TIM9->CCR2;
	ccr[34] = &_TIM9->CCR3;
	ccr[35] = &_TIM9->CCR4;
	ccr[36] = &_TIM10->CCR1;
	ccr[37] = &_TIM10->CCR2;
	ccr[38] = &_TIM10->CCR3;
	ccr[39] = &_TIM10->CCR4;
	ccr[40] = &_TIM11->CCR1;
	ccr[41] = &_TIM11->CCR2;
	ccr[42] = &_TIM11->CCR3;
	ccr[43] = &_TIM11->CCR4;
	ccr[44] = &_TIM12->CCR1;
	ccr[45] = &_TIM12->CCR2;
	ccr[46] = &_TIM12->CCR3;
	ccr[47] = &_TIM12->CCR4;
	ccr[48] = &_TIM13->CCR1;
	ccr[49] = &_TIM13->CCR2;
	ccr[50] = &_TIM13->CCR3;
	ccr[51] = &_TIM13->CCR4;
	ccr[52] = &_TIM14->CCR1;
	ccr[53] = &_TIM14->CCR2;
	ccr[54] = &_TIM14->CCR3;
	ccr[55] = &_TIM14->CCR4;

	return(1);
}

uint32_t boardSize;
void getBoardHardwareDefs(void)
{

	boardSize = sizeof(board);
	bzero(&board, sizeof(board));


	//PLL settings
	board.fc_pllm = FC_PLLM;
	board.fc_plln = FC_PLLN;
	board.fc_pllp = FC_PLLP;
	board.fc_pllq = FC_PLLQ;


	//LED Settings
	board.internalLeds[0].enabled  = LED1_ENABLED;
	board.internalLeds[0].pin      = LED1_GPIO_Pin;
	board.internalLeds[0].port     = LED1_GPIO_Port;
	board.internalLeds[0].inverted = LED1_INVERTED;

	board.internalLeds[1].enabled  = LED2_ENABLED;
	board.internalLeds[1].pin      = LED2_GPIO_Pin;
	board.internalLeds[1].port     = LED2_GPIO_Port;
	board.internalLeds[1].inverted = LED2_INVERTED;

	board.internalLeds[2].enabled  = LED3_ENABLED;
	board.internalLeds[2].pin      = LED3_GPIO_Pin;
	board.internalLeds[2].port     = LED3_GPIO_Port;
	board.internalLeds[2].inverted = LED3_INVERTED;


	//Buzzer Settings
	board.buzzerPort = BUZZER_GPIO_Port;	//Port C
	board.buzzerPin  = BUZZER_GPIO_Pin;

	
	//Motor output assignments
 	board.motors[0].enabled           = ENUM_ACTUATOR_TYPE_MOTOR;
 	board.motors[0].timer             = MOTOR1_TIM;
 	board.motors[0].pin               = MOTOR1_PIN;
 	board.motors[0].port              = MOTOR1_GPIO;
 	board.motors[0].AF                = MOTOR1_ALTERNATE;
 	board.motors[0].timChannel        = MOTOR1_TIM_CH;
 	board.motors[0].timChannelC       = TIM_CHANNEL_1;
 	board.motors[0].activeTim	      = HAL_TIM_ACTIVE_CHANNEL_3;
 	board.motors[0].timCCR            = MOTOR1_TIM_CCR;
 	board.motors[0].polarity          = MOTOR1_POLARITY;
 	board.motors[0].actuatorArrayNum  = 0;
 	board.motors[0].Dma               = ENUM_DMA1_STREAM_7;
 	board.motors[0].CcDmaHandle       = TIM_DMA_ID_CC3;
 	board.motors[0].timerIRQn         = TIM3_IRQn;
 	board.motors[0].motorOutputLength = 16; //bits
 	board.motors[0].EXTIn             = EXTI0_IRQn; //used for input
 	board.motors[0].EXTICallback      = FP_EXTI0;   //used for input
 	board.motors[0].DmaCallback       = FP_DMA1_S7; //used for input

	board.dmasMotor[0].enabled            = 1;
	board.dmasMotor[0].dmaStream          = ENUM_DMA1_STREAM_7;    //motor out
	board.dmasMotor[0].dmaChannel         = DMA_CHANNEL_5;         //motor out
	board.dmasMotor[0].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasMotor[0].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasMotor[0].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasMotor[0].dmaPeriphAlignment = DMA_PDATAALIGN_WORD;
	board.dmasMotor[0].dmaMemAlignment    = DMA_MDATAALIGN_WORD;
	board.dmasMotor[0].dmaMode            = DMA_NORMAL;
	board.dmasMotor[0].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmasMotor[0].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasMotor[0].fifoThreshold      = DMA_FIFO_THRESHOLD_FULL;
	board.dmasMotor[0].MemBurst           = DMA_MBURST_SINGLE;
	board.dmasMotor[0].PeriphBurst        = DMA_PBURST_SINGLE;
	board.dmasMotor[0].dmaIRQn            = DMA1_Stream7_IRQn;     //motor out
	board.dmasMotor[0].dmaHandle          = ENUM_DMA1_STREAM_7;    //motor out


 	board.motors[1].enabled           = ENUM_ACTUATOR_TYPE_MOTOR;
 	board.motors[1].timer             = MOTOR2_TIM;
 	board.motors[1].pin               = MOTOR2_PIN;
 	board.motors[1].port              = MOTOR2_GPIO;
 	board.motors[1].AF                = MOTOR2_ALTERNATE;
 	board.motors[1].timChannel        = MOTOR2_TIM_CH;
 	board.motors[1].timChannelC       = TIM_CHANNEL_2;
 	board.motors[1].activeTim	      = HAL_TIM_ACTIVE_CHANNEL_4;
 	board.motors[1].timCCR            = MOTOR2_TIM_CCR;
 	board.motors[1].polarity          = MOTOR2_POLARITY;
 	board.motors[1].actuatorArrayNum  = 1;
 	board.motors[1].Dma               = ENUM_DMA1_STREAM_2;
 	board.motors[1].CcDmaHandle       = TIM_DMA_ID_CC4;
 	board.motors[1].timerIRQn         = TIM3_IRQn;
 	board.motors[1].motorOutputLength = 16; //bits
 	board.motors[1].EXTIn             = EXTI1_IRQn; //used for input
 	board.motors[1].EXTICallback      = FP_EXTI1; //used for input
 	board.motors[1].DmaCallback       = FP_DMA1_S2; //used for input

	board.dmasMotor[1].enabled            = 1;
	board.dmasMotor[1].dmaStream          = ENUM_DMA1_STREAM_2;    //motor out
	board.dmasMotor[1].dmaChannel         = DMA_CHANNEL_5;         //motor out
	board.dmasMotor[1].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasMotor[1].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasMotor[1].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasMotor[1].dmaPeriphAlignment = DMA_PDATAALIGN_WORD;
	board.dmasMotor[1].dmaMemAlignment    = DMA_MDATAALIGN_WORD;
	board.dmasMotor[1].dmaMode            = DMA_NORMAL;
	board.dmasMotor[1].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmasMotor[1].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasMotor[1].fifoThreshold      = DMA_FIFO_THRESHOLD_FULL;
	board.dmasMotor[1].MemBurst           = DMA_MBURST_SINGLE;
	board.dmasMotor[1].PeriphBurst        = DMA_PBURST_SINGLE;
	board.dmasMotor[1].dmaIRQn            = DMA1_Stream2_IRQn;     //motor out
	board.dmasMotor[1].dmaHandle          = ENUM_DMA1_STREAM_2;    //motor out


	board.motors[2].enabled           = ENUM_ACTUATOR_TYPE_MOTOR;
	board.motors[2].timer             = MOTOR3_TIM;
	board.motors[2].pin               = MOTOR3_PIN;
	board.motors[2].port              = MOTOR3_GPIO;
	board.motors[2].AF                = MOTOR3_ALTERNATE;
	board.motors[2].timChannel        = MOTOR3_TIM_CH;
	board.motors[2].timChannelC       = TIM_CHANNEL_2;
	board.motors[2].activeTim	      = HAL_TIM_ACTIVE_CHANNEL_4;
	board.motors[2].timCCR            = MOTOR3_TIM_CCR;
	board.motors[2].polarity          = MOTOR3_POLARITY;
	board.motors[2].actuatorArrayNum  = 2;
	board.motors[2].Dma               = ENUM_DMA1_STREAM_6;
	board.motors[2].CcDmaHandle       = TIM_DMA_ID_CC4;
	board.motors[2].timerIRQn         = TIM2_IRQn;
	board.motors[2].motorOutputLength = 16; //bits
	board.motors[2].EXTIn             = EXTI3_IRQn; //used for input
 	board.motors[2].EXTICallback      = FP_EXTI3; //used for input
 	board.motors[2].DmaCallback       = FP_DMA1_S6; //used for input

	board.dmasMotor[2].enabled            = 1;
	board.dmasMotor[2].dmaStream          = ENUM_DMA1_STREAM_6;    //motor out
	board.dmasMotor[2].dmaChannel         = DMA_CHANNEL_3;         //motor out
	board.dmasMotor[2].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasMotor[2].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasMotor[2].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasMotor[2].dmaPeriphAlignment = DMA_PDATAALIGN_WORD;
	board.dmasMotor[2].dmaMemAlignment    = DMA_MDATAALIGN_WORD;
	board.dmasMotor[2].dmaMode            = DMA_NORMAL;
	board.dmasMotor[2].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmasMotor[2].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasMotor[2].fifoThreshold      = DMA_FIFO_THRESHOLD_FULL;
	board.dmasMotor[2].MemBurst           = DMA_MBURST_SINGLE;
	board.dmasMotor[2].PeriphBurst        = DMA_PBURST_SINGLE;
	board.dmasMotor[2].dmaIRQn            = DMA1_Stream6_IRQn;     //motor out
	board.dmasMotor[2].dmaHandle          = ENUM_DMA1_STREAM_6;    //motor out


	board.motors[3].enabled           = ENUM_ACTUATOR_TYPE_MOTOR;
	board.motors[3].timer             = MOTOR4_TIM;
	board.motors[3].pin               = MOTOR4_PIN;
	board.motors[3].port              = MOTOR4_GPIO;
	board.motors[3].AF                = MOTOR4_ALTERNATE;
	board.motors[3].timChannel        = MOTOR4_TIM_CH;
	board.motors[3].timChannelC       = TIM_CHANNEL_1;
	board.motors[3].activeTim	      = HAL_TIM_ACTIVE_CHANNEL_3;
	board.motors[3].timCCR            = MOTOR4_TIM_CCR;
	board.motors[3].polarity          = MOTOR4_POLARITY;
	board.motors[3].actuatorArrayNum  = 3;
	board.motors[3].Dma               = ENUM_DMA1_STREAM_1; //motor out DMA
	board.motors[3].CcDmaHandle       = TIM_DMA_ID_CC3;
	board.motors[3].timerIRQn         = TIM2_IRQn;
	board.motors[3].motorOutputLength = 16; //bits
	board.motors[3].EXTIn             = EXTI2_IRQn; //used for input
 	board.motors[3].EXTICallback      = FP_EXTI2; //used for input
 	board.motors[3].DmaCallback       = FP_DMA1_S1; //used for input

	board.dmasMotor[3].enabled            = 1;
	board.dmasMotor[3].dmaStream          = ENUM_DMA1_STREAM_1;    //motor out
	board.dmasMotor[3].dmaChannel         = DMA_CHANNEL_3;         //motor out
	board.dmasMotor[3].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasMotor[3].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasMotor[3].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasMotor[3].dmaPeriphAlignment = DMA_PDATAALIGN_WORD;
	board.dmasMotor[3].dmaMemAlignment    = DMA_MDATAALIGN_WORD;
	board.dmasMotor[3].dmaMode            = DMA_NORMAL;
	board.dmasMotor[3].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmasMotor[3].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasMotor[3].fifoThreshold      = DMA_FIFO_THRESHOLD_FULL;
	board.dmasMotor[3].MemBurst           = DMA_MBURST_SINGLE;
	board.dmasMotor[3].PeriphBurst        = DMA_PBURST_SINGLE;
	board.dmasMotor[3].dmaIRQn            = DMA1_Stream1_IRQn;     //motor out
	board.dmasMotor[3].dmaHandle          = ENUM_DMA1_STREAM_1;    //motor out


	// motor 6 is ws2812 output, soft serial
	// PB6/I2C1_SCL/TIM4_CH1/CAN2_TX/DCMI_D5/USART1_TX/EVENTOU
	board.motors[6].enabled           = ENUM_ACTUATOR_TYPE_WS2812;  //do not enable for motor output, TODO: set this to a function type.
	board.motors[6].timer             = ENUMTIM4;
	board.motors[6].pin               = GPIO_PIN_6;
	board.motors[6].port              = _PORTB;
	board.motors[6].AF                = GPIO_AF2_TIM4;
	board.motors[6].timChannel        = TIM_CHANNEL_1;
	board.motors[6].timChannelC       = 0; //not used at the moment, dma input
	board.motors[6].activeTim	      = HAL_TIM_ACTIVE_CHANNEL_1;
	board.motors[6].timCCR            = TIM4CCR1;
	board.motors[6].polarity          = TIM_OCPOLARITY_LOW;
	board.motors[6].actuatorArrayNum  = 6;
	board.motors[6].Dma               = ENUM_DMA1_STREAM_0; //motor out DMA
	board.motors[6].CcDmaHandle       = TIM_DMA_ID_CC1;
	board.motors[6].timerIRQn         = TIM4_IRQn;
	board.motors[6].motorOutputLength = 64; //bits
	board.motors[6].EXTIn             = EXTI4_IRQn;
	//board.motors[6].EXTICallback      = FP_EXTI4; //used for input
	//board.motors[6].DmaCallback       = FP_DMA1_S0; //used for input

	board.dmasMotor[6].enabled            = 1;
	board.dmasMotor[6].dmaStream          = ENUM_DMA1_STREAM_0;    //motor out
	board.dmasMotor[6].dmaChannel         = DMA_CHANNEL_2;         //motor out
	board.dmasMotor[6].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasMotor[6].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasMotor[6].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasMotor[6].dmaPeriphAlignment = DMA_PDATAALIGN_WORD;
	board.dmasMotor[6].dmaMemAlignment    = DMA_MDATAALIGN_WORD;
	board.dmasMotor[6].dmaMode            = DMA_NORMAL;
	board.dmasMotor[6].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmasMotor[6].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasMotor[6].fifoThreshold      = DMA_FIFO_THRESHOLD_FULL;
	board.dmasMotor[6].MemBurst           = DMA_MBURST_SINGLE;
	board.dmasMotor[6].PeriphBurst        = DMA_PBURST_SINGLE;
	board.dmasMotor[6].dmaIRQn            = DMA1_Stream0_IRQn;     //motor out
	board.dmasMotor[6].dmaHandle          = ENUM_DMA1_STREAM_0;    //motor out

	// motor 7 is s.port output, soft serial
	board.motors[7].enabled           = ENUM_ACTUATOR_TYPE_SPORT;  //do not enable for motor output, TODO: set this to a function type.
	board.motors[7].timer             = ENUMTIM1;
	board.motors[7].pin               = GPIO_PIN_9;
	board.motors[7].port              = _PORTA;
	board.motors[7].AF                = GPIO_AF1_TIM1;
	board.motors[7].timChannel        = TIM_CHANNEL_2;
	board.motors[7].timChannelC       = 0; //not used at the moment, for DMA input
	board.motors[7].activeTim	      = HAL_TIM_ACTIVE_CHANNEL_2;
	board.motors[7].timCCR            = TIM1CCR2;
	board.motors[7].polarity          = TIM_OCPOLARITY_LOW;
	board.motors[7].actuatorArrayNum  = 7;
	board.motors[7].Dma               = ENUM_DMA2_STREAM_2; //motor out DMA
	board.motors[7].CcDmaHandle       = TIM_DMA_ID_CC2;
	board.motors[7].timerIRQn         = TIM1_CC_IRQn; //TIM1_CC_IRQn //TIM1_UP_TIM10_IRQn //TIM1_BRK_TIM9_IRQn //TIM1_TRG_COM_TIM11_IRQn
	board.motors[7].motorOutputLength = 64; //bits
	board.motors[7].EXTIn             = EXTI9_5_IRQn;
	board.motors[7].EXTICallback      = FP_EXTI9_5; //used for input
	board.motors[7].DmaCallback       = FP_DMA2_S2; //used for input
	//callbackFunctionArray[FP_EXTI9_5] = SoftSerialCallback;

	board.dmasMotor[7].enabled            = 1;
	board.dmasMotor[7].dmaStream          = ENUM_DMA2_STREAM_2;    //motor out
	board.dmasMotor[7].dmaChannel         = DMA_CHANNEL_6;         //motor out
	board.dmasMotor[7].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasMotor[7].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasMotor[7].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasMotor[7].dmaPeriphAlignment = DMA_PDATAALIGN_WORD;
	board.dmasMotor[7].dmaMemAlignment    = DMA_MDATAALIGN_WORD;
	board.dmasMotor[7].dmaMode            = DMA_NORMAL;
	board.dmasMotor[7].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmasMotor[7].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasMotor[7].fifoThreshold      = DMA_FIFO_THRESHOLD_FULL;
	board.dmasMotor[7].MemBurst           = DMA_MBURST_SINGLE;
	board.dmasMotor[7].PeriphBurst        = DMA_PBURST_SINGLE;
	board.dmasMotor[7].dmaIRQn            = DMA2_Stream2_IRQn;     //motor out
	board.dmasMotor[7].dmaHandle          = ENUM_DMA2_STREAM_2;    //motor out


	//GYRO connection settings	------------------------------------------------------------------------------------------------------------------------------------------------------------
	board.gyros[0].enabled     = 1;
	board.gyros[0].spiNumber   = GYRO_SPI_NUMBER;
	board.gyros[0].csPin       = GYRO_SPI_CS_GPIO_Pin;
	board.gyros[0].csPort      = GYRO_SPI_CS_GPIO_Port;
	board.gyros[0].extiPin     = GYRO_EXTI_GPIO_Pin;
	board.gyros[0].extiPort    = GYRO_EXTI_GPIO_Port;
	board.gyros[0].extiIRQn    = EXTI4_IRQn;
	board.gyros[0].spiFastBaud = GYRO_SPI_FAST_BAUD;
	board.gyros[0].spiSlowBaud = GYRO_SPI_SLOW_BAUD;
	callbackFunctionArray[FP_EXTI4]   = GyroExtiCallback;

	//Flash connection settings	------------------------------------------------------------------------------------------------------------------------------------------------------------
	board.flash[0].enabled     = 1;
	board.flash[0].spiNumber   = FLASH_SPI_NUMBER;
	board.flash[0].csPin       = FLASH_SPI_CS_GPIO_Pin;
	board.flash[0].csPort      = FLASH_SPI_CS_GPIO_Port;
	board.flash[0].extiPin     = 0;
	board.flash[0].extiPort    = 0;
	board.flash[0].extiIRQn    = 0;
	board.flash[0].spiFastBaud = FLASH_SPI_FAST_BAUD;
	board.flash[0].spiSlowBaud = FLASH_SPI_SLOW_BAUD;

	//SPI settings ------------------------------------------------------------------------------------------------------------------------------------------------------------
	board.spis[ENUM_SPI1].enabled                                 = SPI1_ENABLE;
	board.spis[ENUM_SPI1].instance                                = ENUM_SPI1;

	board.spis[ENUM_SPI1].NSSPin                                  = SPI1_NSS_PIN;
	board.spis[ENUM_SPI1].SCKPin                                  = SPI1_SCK_PIN;
	board.spis[ENUM_SPI1].MISOPin                                 = SPI1_MISO_PIN;
	board.spis[ENUM_SPI1].MOSIPin                                 = SPI1_MOSI_PIN;

	board.spis[ENUM_SPI1].NSSPort                                 = SPI1_NSS_GPIO_PORT;
	board.spis[ENUM_SPI1].SCKPort                                 = SPI1_SCK_GPIO_PORT;
	board.spis[ENUM_SPI1].MISOPort                                = SPI1_MISO_GPIO_PORT;
	board.spis[ENUM_SPI1].MOSIPort                                = SPI1_MOSI_GPIO_PORT;

	board.spis[ENUM_SPI2].SCKPull                                 = GPIO_PULLDOWN;

	board.spis[ENUM_SPI1].SCKAlternate                            = SPI1_SCK_AF;
	board.spis[ENUM_SPI1].MISOAlternate                           = SPI1_MISO_AF;
	board.spis[ENUM_SPI1].MOSIAlternate                           = SPI1_MOSI_AF;

	board.spis[ENUM_SPI1].SPI_IRQn                                = SPI1_IRQn;
	board.spis[ENUM_SPI1].spiHandle                               = ENUM_SPI1;
	board.spis[ENUM_SPI1].priority                                = SPI1_PRIORITY;


	board.spis[ENUM_SPI1].TXDMA_IRQn                              = SPI1_TX_DMA_IRQn;
	board.spis[ENUM_SPI1].RXDMA_IRQn                              = SPI1_RX_DMA_IRQn;

	board.spis[ENUM_SPI1].TXDma 		                          = ENUM_DMA2_STREAM_3;
	board.spis[ENUM_SPI1].RXDma 		                          = ENUM_DMA2_STREAM_0;

	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].enabled            = 1;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaStream          = SPI1_TX_DMA_STREAM;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaChannel         = SPI1_TX_DMA_CHANNEL;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaMode            = DMA_NORMAL;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaIRQn            = SPI1_TX_DMA_IRQn;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].dmaHandle          = SPI1_TX_DMA_STREAM;
	board.dmasSpi[board.spis[ENUM_SPI1].TXDma].priority           = SPI1_RX_DMA_PRIORITY;
	callbackFunctionArray[FP_DMA2_S3]                             = 0;

	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].enabled            = 1;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaStream          = SPI1_RX_DMA_STREAM;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaChannel         = SPI1_RX_DMA_CHANNEL;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaMode            = DMA_NORMAL;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaIRQn            = SPI1_RX_DMA_IRQn;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].dmaHandle          = SPI1_RX_DMA_STREAM;
	board.dmasSpi[board.spis[ENUM_SPI1].RXDma].priority           = SPI1_RX_DMA_PRIORITY;
	callbackFunctionArray[FP_DMA2_S0]                             = GyroRxDmaCallback;


	board.spis[ENUM_SPI2].enabled                                 = SPI2_ENABLE;
	board.spis[ENUM_SPI2].instance                                = ENUM_SPI2;

	board.spis[ENUM_SPI2].NSSPin                                  = SPI2_NSS_PIN;
	board.spis[ENUM_SPI2].SCKPin                                  = SPI2_SCK_PIN;
	board.spis[ENUM_SPI2].MISOPin                                 = SPI2_MISO_PIN;
	board.spis[ENUM_SPI2].MOSIPin                                 = SPI2_MOSI_PIN;

	board.spis[ENUM_SPI2].NSSPort                                 = SPI2_NSS_GPIO_PORT;
	board.spis[ENUM_SPI2].SCKPort                                 = SPI2_SCK_GPIO_PORT;
	board.spis[ENUM_SPI2].MISOPort                                = SPI2_MISO_GPIO_PORT;
	board.spis[ENUM_SPI2].MOSIPort                                = SPI2_MOSI_GPIO_PORT;

	board.spis[ENUM_SPI2].SCKPull                                 = GPIO_PULLDOWN;

	board.spis[ENUM_SPI2].SCKAlternate                            = SPI2_SCK_AF;
	board.spis[ENUM_SPI2].MISOAlternate                           = SPI2_MISO_AF;
	board.spis[ENUM_SPI2].MOSIAlternate                           = SPI2_MOSI_AF;

	board.spis[ENUM_SPI2].SPI_IRQn                                = SPI2_IRQn;
	board.spis[ENUM_SPI2].spiHandle                               = ENUM_SPI2;
	board.spis[ENUM_SPI2].priority                                = SPI2_PRIORITY;

	board.spis[ENUM_SPI2].TXDMA_IRQn                              = SPI2_TX_DMA_IRQn;
	board.spis[ENUM_SPI2].RXDMA_IRQn                              = SPI2_RX_DMA_IRQn;

	board.spis[ENUM_SPI2].TXDma 		                          = SPI2_TX_DMA_STREAM;
	board.spis[ENUM_SPI2].RXDma 		                          = SPI2_RX_DMA_STREAM;

	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].enabled            = 0;
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaStream          = SPI2_TX_DMA_STREAM;   //diff between all SPIs
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaChannel         = SPI2_TX_DMA_CHANNEL;  //diff
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaDirection       = DMA_MEMORY_TO_PERIPH; //same between all SPIs, diff between TX/RX
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;     //same
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaMemInc          = DMA_MINC_ENABLE;      //same
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;  //same
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;  //same
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaMode            = DMA_NORMAL;           //same
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaPriority        = DMA_PRIORITY_HIGH;    //same, maybe we should change them
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE; //same
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaIRQn            = SPI2_TX_DMA_IRQn;     //diff
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].dmaHandle          = SPI2_TX_DMA_STREAM;   //diff
	board.dmasSpi[board.spis[ENUM_SPI2].TXDma].priority           = SPI2_RX_DMA_PRIORITY;
	callbackFunctionArray[FP_DMA1_S4]                             = 0;

	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].enabled            = 0;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaStream          = SPI2_RX_DMA_STREAM;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaChannel         = SPI2_RX_DMA_CHANNEL;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaMode            = DMA_NORMAL;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaIRQn            = SPI2_RX_DMA_IRQn;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].dmaHandle          = SPI2_RX_DMA_STREAM;
	board.dmasSpi[board.spis[ENUM_SPI2].RXDma].priority           = SPI2_RX_DMA_PRIORITY;
	callbackFunctionArray[FP_DMA1_S3]                             = 0;


	board.spis[ENUM_SPI3].enabled                              = SPI3_ENABLE;
	board.spis[ENUM_SPI3].instance                             = ENUM_SPI3;

	board.spis[ENUM_SPI3].NSSPin                               = SPI3_NSS_PIN;
	board.spis[ENUM_SPI3].SCKPin                               = SPI3_SCK_PIN;
	board.spis[ENUM_SPI3].MISOPin                              = SPI3_MISO_PIN;
	board.spis[ENUM_SPI3].MOSIPin                              = SPI3_MOSI_PIN;

	board.spis[ENUM_SPI3].SCKPull                              = GPIO_PULLUP;

	board.spis[ENUM_SPI3].NSSPort                              = SPI3_NSS_GPIO_PORT;
	board.spis[ENUM_SPI3].SCKPort                              = SPI3_SCK_GPIO_PORT;
	board.spis[ENUM_SPI3].MISOPort                             = SPI3_MISO_GPIO_PORT;
	board.spis[ENUM_SPI3].MOSIPort                             = SPI3_MOSI_GPIO_PORT;

	board.spis[ENUM_SPI3].SCKAlternate                         = SPI3_SCK_AF;
	board.spis[ENUM_SPI3].MISOAlternate                        = SPI3_MISO_AF;
	board.spis[ENUM_SPI3].MOSIAlternate                        = SPI3_MOSI_AF;

	board.spis[ENUM_SPI3].SPI_IRQn                             = SPI3_IRQn;
	board.spis[ENUM_SPI3].spiHandle                            = ENUM_SPI3;
	board.spis[ENUM_SPI3].priority                             = SPI3_PRIORITY;

	board.spis[ENUM_SPI3].TXDMA_IRQn                           = SPI3_TX_DMA_IRQn;
	board.spis[ENUM_SPI3].RXDMA_IRQn                           = SPI3_RX_DMA_IRQn;

	board.spis[ENUM_SPI3].TXDMA_IRQn                           = SPI3_TX_DMA_IRQn;
	board.spis[ENUM_SPI3].RXDMA_IRQn                           = SPI3_RX_DMA_IRQn;

	board.spis[ENUM_SPI3].TXDma 		                       = ENUM_DMA1_STREAM_5;
	board.spis[ENUM_SPI3].RXDma 		                       = ENUM_DMA1_STREAM_2;

	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].enabled            = 1;
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaStream          = board.spis[ENUM_SPI3].TXDma;   //diff between all SPIs
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaChannel         = SPI3_TX_DMA_CHANNEL;  //diff
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaDirection       = DMA_MEMORY_TO_PERIPH; //same between all SPIs, diff between TX/RX
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;     //same
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaMemInc          = DMA_MINC_ENABLE;      //same
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;  //same
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;  //same
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaMode            = DMA_NORMAL;           //same
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaPriority        = DMA_PRIORITY_HIGH;    //same, maybe we should change them
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE; //same
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaIRQn            = SPI3_TX_DMA_IRQn;     //diff
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].dmaHandle          = board.spis[ENUM_SPI3].TXDma;   //diff
	board.dmasSpi[board.spis[ENUM_SPI3].TXDma].priority           = SPI3_TX_DMA_PRIORITY;
	callbackFunctionArray[FP_DMA1_S5]                             = 0;//FlashDmaTxCallback;

	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].enabled            = 1;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaStream          = board.spis[ENUM_SPI3].RXDma;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaChannel         = SPI3_RX_DMA_CHANNEL;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaMemInc          = DMA_MINC_ENABLE;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaMode            = DMA_NORMAL;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaPriority        = DMA_PRIORITY_HIGH;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaIRQn            = SPI3_RX_DMA_IRQn;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].dmaHandle          = board.spis[ENUM_SPI3].RXDma;
	board.dmasSpi[board.spis[ENUM_SPI3].RXDma].priority           = SPI3_RX_DMA_PRIORITY;
	callbackFunctionArray[FP_DMA1_S2]                             = FlashDmaRxCallback;



	//UART settings ------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	board.serials[ENUM_USART1].enabled        = 1;
	board.serials[ENUM_USART1].PinMode        = GPIO_MODE_AF_PP;
	board.serials[ENUM_USART1].Pull           = GPIO_PULLUP;
	board.serials[ENUM_USART1].Speed          = GPIO_SPEED_HIGH;
	board.serials[ENUM_USART1].TXAlternate    = GPIO_AF7_USART1;
	board.serials[ENUM_USART1].TXPin          = GPIO_PIN_9;
	board.serials[ENUM_USART1].TXPort         = ENUM_PORTA;
	board.serials[ENUM_USART1].RXAlternate    = GPIO_AF7_USART1;
	board.serials[ENUM_USART1].RXPin          = GPIO_PIN_10;
	board.serials[ENUM_USART1].RXPort         = ENUM_PORTA;

	board.serials[ENUM_USART1].SerialInstance = ENUM_USART1;  // loaded from port array

	board.serials[ENUM_USART1].usartHandle    = ENUM_USART1;  //JUST A Define for the function name

	board.serials[ENUM_USART1].serialTxBuffer = 1; //three buffers, give this one the first buffer
	board.serials[ENUM_USART1].serialRxBuffer = 1; //three buffers, give this one the first buffer

	board.serials[ENUM_USART1].TXDma 		  = ENUM_DMA2_STREAM_7;
	board.serials[ENUM_USART1].RXDma 		  = ENUM_DMA2_STREAM_5;

	board.dmasSerial[board.serials[ENUM_USART1].TXDma].enabled            = 1;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaStream          = ENUM_DMA2_STREAM_7;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaChannel         = DMA_CHANNEL_4;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaMemInc          = DMA_MINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaMode            = DMA_CIRCULAR;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaIRQn            = DMA2_Stream7_IRQn;
	board.dmasSerial[board.serials[ENUM_USART1].TXDma].dmaHandle          = ENUM_DMA2_STREAM_7;

	board.dmasSerial[board.serials[ENUM_USART1].RXDma].enabled            = 1;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaStream          = ENUM_DMA2_STREAM_5;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaChannel         = DMA_CHANNEL_4;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaMemInc          = DMA_MINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaMode            = DMA_CIRCULAR;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaIRQn            = DMA2_Stream5_IRQn;
	board.dmasSerial[board.serials[ENUM_USART1].RXDma].dmaHandle          = ENUM_DMA2_STREAM_5;



	board.serials[ENUM_USART3].enabled         = 1;
	board.serials[ENUM_USART3].PinMode         = GPIO_MODE_AF_PP;
	board.serials[ENUM_USART3].Pull            = GPIO_PULLUP;
	board.serials[ENUM_USART3].Speed           = GPIO_SPEED_HIGH;
	board.serials[ENUM_USART3].TXAlternate     = GPIO_AF7_USART3;
	board.serials[ENUM_USART3].TXPin           = GPIO_PIN_10;
	board.serials[ENUM_USART3].TXPort          = ENUM_PORTB;
	board.serials[ENUM_USART3].RXAlternate     = GPIO_AF7_USART3;
	board.serials[ENUM_USART3].RXPin           = GPIO_PIN_11;
	board.serials[ENUM_USART3].RXPort          = ENUM_PORTB;

	board.serials[ENUM_USART3].SerialInstance  = ENUM_USART3;  // loaded from port array

	board.serials[ENUM_USART3].usartHandle     = ENUM_USART3;  //JUST A Define for the function name

	board.serials[ENUM_USART3].serialTxBuffer  = 2; //three buffers, give this one the first buffer
	board.serials[ENUM_USART3].serialRxBuffer  = 2; //three buffers, give this one the first buffer

	board.serials[ENUM_USART3].TXDma 		   = ENUM_DMA1_STREAM_3;
	board.serials[ENUM_USART3].RXDma 		   = ENUM_DMA1_STREAM_1;

	board.dmasSerial[board.serials[ENUM_USART3].TXDma].enabled            = 1;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaStream          = ENUM_DMA1_STREAM_3;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaChannel         = DMA_CHANNEL_4;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaMemInc          = DMA_MINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaMode            = DMA_CIRCULAR;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaIRQn            = DMA1_Stream3_IRQn;
	board.dmasSerial[board.serials[ENUM_USART3].TXDma].dmaHandle          = ENUM_DMA1_STREAM_3;

	board.dmasSerial[board.serials[ENUM_USART3].RXDma].enabled            = 1;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaStream          = ENUM_DMA1_STREAM_1;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaChannel         = DMA_CHANNEL_4;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaMemInc          = DMA_MINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaMode            = DMA_CIRCULAR;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaIRQn            = DMA1_Stream1_IRQn;
	board.dmasSerial[board.serials[ENUM_USART3].RXDma].dmaHandle          = ENUM_DMA1_STREAM_1;




	board.serials[ENUM_USART4].enabled         = 1;
	board.serials[ENUM_USART4].PinMode         = GPIO_MODE_AF_PP;
	board.serials[ENUM_USART4].Pull            = GPIO_PULLUP;
	board.serials[ENUM_USART4].Speed           = GPIO_SPEED_HIGH;
	board.serials[ENUM_USART4].TXAlternate     = GPIO_AF8_UART4;
	board.serials[ENUM_USART4].TXPin           = GPIO_PIN_0;
	board.serials[ENUM_USART4].TXPort          = ENUM_PORTA;
	board.serials[ENUM_USART4].RXAlternate     = GPIO_AF8_UART4;
	board.serials[ENUM_USART4].RXPin           = GPIO_PIN_1;
	board.serials[ENUM_USART4].RXPort          = ENUM_PORTA;

	board.serials[ENUM_USART4].SerialInstance  = ENUM_USART4;  // loaded from port array

	board.serials[ENUM_USART4].usartHandle     = ENUM_USART4;  //JUST A Define for the function name

	board.serials[ENUM_USART4].serialTxBuffer  = 3; //three buffers, give this one the first buffer
	board.serials[ENUM_USART4].serialRxBuffer  = 3; //three buffers, give this one the first buffer

	board.serials[ENUM_USART4].TXDma 		   = ENUM_DMA1_STREAM_4;
	board.serials[ENUM_USART4].RXDma 		   = ENUM_DMA1_STREAM_2;

	board.dmasSerial[board.serials[ENUM_USART4].TXDma].enabled            = 1;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaStream          = board.serials[ENUM_USART4].TXDma;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaChannel         = DMA_CHANNEL_4;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaDirection       = DMA_MEMORY_TO_PERIPH;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaMemInc          = DMA_MINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaMode            = DMA_CIRCULAR;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaIRQn            = DMA1_Stream4_IRQn;
	board.dmasSerial[board.serials[ENUM_USART4].TXDma].dmaHandle          = board.serials[ENUM_USART4].TXDma;

	board.dmasSerial[board.serials[ENUM_USART4].RXDma].enabled            = 1;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaStream          = board.serials[ENUM_USART4].RXDma;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaChannel         = DMA_CHANNEL_4;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaDirection       = DMA_PERIPH_TO_MEMORY;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaPeriphInc       = DMA_PINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaMemInc          = DMA_MINC_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaPeriphAlignment = DMA_PDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaMemAlignment    = DMA_MDATAALIGN_BYTE;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaMode            = DMA_CIRCULAR;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaPriority        = DMA_PRIORITY_MEDIUM;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].fifoMode           = DMA_FIFOMODE_DISABLE;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaIRQn            = DMA1_Stream2_IRQn;
	board.dmasSerial[board.serials[ENUM_USART4].RXDma].dmaHandle          = board.serials[ENUM_USART4].RXDma;

}
