
#define SPM_LAPTIMER
#define SPM_LAPTIMER_GPIO             	GPIOA
#define SPM_LAPTIMER_PIN              	GPIO_Pin_10
#define SPM_LAPTIMER_PINSOURCE        	GPIO_PinSource10
#define SPM_LAPTIMER_PERIPH           	RCC_AHB1Periph_GPIOA
#define SPM_LAPTIMER_TIM              	TIM1
#define	SPM_LAPTIMER_CHANNEL			TIM_Channel_3
#define	SPM_LAPTIMER_IT_CC				TIM_IT_CC3
#define SPM_LAPTIMER_TIM_AF      	    GPIO_AF_TIM1
#define SPM_LAPTIMER_TIM_PERIPH       	RCC_APB2Periph_TIM1

#define SPM_LAPTIMER_TIM_CC_IRQN		TIM1_CC_IRQn
#define SPM_LAPTIMER_TIM_UP_IRQN		TIM1_UP_TIM10_IRQn
#define SPM_LAPTIMER_TIM_CC_IRQ_HANDLER	TIM1_CC_IRQHandler
#define SPM_LAPTIMER_TIM_UP_IRQ_HANDLER	TIM1_UP_TIM10_IRQHandler

#define SPM_LAPTIMER_DMA_CH           	DMA_Channel_6
#define SPM_LAPTIMER_DMA_ST           	DMA2_Stream6
#define SPM_LAPTIMER_DMA_IRQ          	DMA2_Stream6_IRQn
#define SPM_LAPTIMER_DMA_FLAG         	DMA_FLAG_TCIF6
#define SPM_LAPTIMER_DMA_IRQ_HANDLER  	DMA2_Stream6_IRQHandler
#define SPM_LAPTIMER_DMA_PERIPH       	RCC_AHB1Periph_DMA2


#define START_PULSES
#define END_PULSES
#ifdef START_PULSES
#define PULSE_SHIFT     2
#else
#define PULSE_SHIFT     0
#endif

#define SEQUENCE_TIMEOUT	1200		//Max 0XFFFF OR 65535
#define DISABLE_PERIOD		0XFFFF	//Period to be used when the update interrupt is being used as a counter for disable time
#define DISABLE_TIME		15		//Time for lap capture to be deactivated after passing a gate. Value = (Desired delay in seconds)/.065536
#define CODE_SIZE			8		//Number of bits expected in IR code

//0 Pulse ranges
#define SHORT_MIN_PULSE         150
#define SHORT_MAX_PULSE         420

//1 Pulse ranges
#define LONG_MIN_PULSE          440
#define LONG_MAX_PULSE          710

//Duration of Off pulse (IR off in between 0 and 1 pulses)
#define OFF_DURATION            300

typedef enum
{
	READING_PULSES = 0,
	GATE_PASSED
  
} LAP_STATE;


void laptimerInit(void);