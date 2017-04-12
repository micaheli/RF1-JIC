#include "includes.h"

#define WHOLE_NOTE 1.0f 
#define HALF_NOTE .5f
#define QUARTER_NOTE .25f 
#define EIGHT_NOTE .125f
#define SIXTEENTH_NOTE .0625f 


TIM_HandleTypeDef ledTimer;

typedef struct {
	uint16_t flat;
	uint16_t natural;
	uint16_t sharp;
} notes_record;


#define nC 0
#define nD 1
#define nE 2
#define nF 3
#define nG 4
#define nA 5
#define nB 6


notes_record notes[7][7];


typedef struct {
	uint16_t note;
	float noteLength; 
} note_record;


uint32_t imperialMarchLength;
note_record imperialMarch[256];

 
void LoadSongs(void)

{
	uint32_t noteCount=0;
	imperialMarch[noteCount].note = notes[4][nG].natural; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 	
	imperialMarch[noteCount].note = notes[4][nG].natural; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 
	imperialMarch[noteCount].note = notes[4][nG].natural; imperialMarch[noteCount++].noteLength = QUARTER_NOTE;
	imperialMarch[noteCount].note = notes[4][nE].flat; imperialMarch[noteCount++].noteLength = EIGHT_NOTE + SIXTEENTH_NOTE; 
	imperialMarch[noteCount].note = notes[4][nB].flat; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE; 

	imperialMarch[noteCount].note = notes[4][nG].natural; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 	
	imperialMarch[noteCount].note = notes[4][nE].flat; imperialMarch[noteCount++].noteLength = EIGHT_NOTE + SIXTEENTH_NOTE; 
	imperialMarch[noteCount].note = notes[4][nB].flat; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE;
	imperialMarch[noteCount].note = notes[4][nG].natural; imperialMarch[noteCount++].noteLength = HALF_NOTE; 

	imperialMarch[noteCount].note = notes[5][nD].natural; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 	
	imperialMarch[noteCount].note = notes[5][nD].natural; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 
	imperialMarch[noteCount].note = notes[5][nD].natural; imperialMarch[noteCount++].noteLength = QUARTER_NOTE;
	imperialMarch[noteCount].note = notes[5][nE].flat; imperialMarch[noteCount++].noteLength = EIGHT_NOTE + SIXTEENTH_NOTE; 
	imperialMarch[noteCount].note = notes[4][nB].flat; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE; 

	imperialMarch[noteCount].note = notes[4][nF].sharp; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 	
	imperialMarch[noteCount].note = notes[4][nE].flat; imperialMarch[noteCount++].noteLength = EIGHT_NOTE + SIXTEENTH_NOTE; 
	imperialMarch[noteCount].note = notes[4][nB].flat; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE;
	imperialMarch[noteCount].note = notes[4][nG].natural; imperialMarch[noteCount++].noteLength = HALF_NOTE; 

/****/

	imperialMarch[noteCount].note = notes[5][nG].natural; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 	
	imperialMarch[noteCount].note = notes[4][nG].natural; imperialMarch[noteCount++].noteLength = EIGHT_NOTE + SIXTEENTH_NOTE; 
	imperialMarch[noteCount].note = notes[4][nG].natural; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE;
	imperialMarch[noteCount].note = notes[5][nG].natural; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 
	imperialMarch[noteCount].note = notes[5][nG].flat; imperialMarch[noteCount++].noteLength = EIGHT_NOTE + SIXTEENTH_NOTE; 
	imperialMarch[noteCount].note = notes[5][nF].natural; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE; 	

	imperialMarch[noteCount].note = notes[5][nE].natural; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE; 	
	imperialMarch[noteCount].note = notes[5][nE].flat; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE; 
	imperialMarch[noteCount].note = notes[5][nE].natural; imperialMarch[noteCount++].noteLength = EIGHT_NOTE;
	imperialMarch[noteCount].note = 0; imperialMarch[noteCount++].noteLength = EIGHT_NOTE; /***************************/
	imperialMarch[noteCount].note = notes[4][nA].flat; imperialMarch[noteCount++].noteLength = EIGHT_NOTE; 
	imperialMarch[noteCount].note = notes[5][nD].flat; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 
	imperialMarch[noteCount].note = notes[5][nC].natural; imperialMarch[noteCount++].noteLength = EIGHT_NOTE + SIXTEENTH_NOTE;
	imperialMarch[noteCount].note = notes[4][nB].natural; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE; 


	imperialMarch[noteCount].note = notes[4][nB].flat; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE; 	
	imperialMarch[noteCount].note = notes[4][nA].natural; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE; 
	imperialMarch[noteCount].note = notes[4][nB].flat; imperialMarch[noteCount++].noteLength = EIGHT_NOTE;
	imperialMarch[noteCount].note = 0; imperialMarch[noteCount++].noteLength = EIGHT_NOTE; /*************************************/
	imperialMarch[noteCount].note = notes[4][nE].flat; imperialMarch[noteCount++].noteLength = EIGHT_NOTE; 
	imperialMarch[noteCount].note = notes[4][nF].sharp; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 
	imperialMarch[noteCount].note = notes[4][nE].flat; imperialMarch[noteCount++].noteLength = EIGHT_NOTE + SIXTEENTH_NOTE;
	imperialMarch[noteCount].note = notes[4][nG].natural; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE; 

	imperialMarch[noteCount].note = notes[4][nB].flat; imperialMarch[noteCount++].noteLength = QUARTER_NOTE; 	
	imperialMarch[noteCount].note = notes[4][nG].natural; imperialMarch[noteCount++].noteLength = EIGHT_NOTE + SIXTEENTH_NOTE; 
	imperialMarch[noteCount].note = notes[4][nB].flat; imperialMarch[noteCount++].noteLength = SIXTEENTH_NOTE;
	imperialMarch[noteCount].note = notes[5][nD].natural; imperialMarch[noteCount++].noteLength = HALF_NOTE; 







	imperialMarchLength=noteCount;






}





void SetupNotes(void)
{

	notes[7][nC].flat    = 3951;
	notes[7][nC].natural = 4186;
	notes[7][nC].sharp   = 4435;

	notes[7][nD].flat    = 4435;
	notes[7][nD].natural = 4699;
	notes[7][nD].sharp   = 4978;

	notes[7][nE].flat    = 4978;
	notes[7][nE].natural = 5274;
	notes[7][nE].sharp   = 5587;

	notes[7][nF].flat    = 5274;
	notes[7][nF].natural = 5587;
	notes[7][nF].sharp   = 5920;

	notes[7][nG].flat    = 5920;
	notes[7][nG].natural = 6272;
	notes[7][nG].sharp   = 6645;

	notes[7][nA].flat    = 6645;
	notes[7][nA].natural = 7040;
	notes[7][nA].sharp   = 7459;

	notes[7][nB].flat    = 7459;
	notes[7][nB].natural = 7902;
	notes[7][nB].sharp   = 8372;


    for (uint32_t octave=7;octave>=1;octave--) 
	{
		for (uint32_t note=0;note<=7;note++) 
		{
			notes[octave-1][note].flat = notes[octave][note].flat / 2;
			notes[octave-1][note].natural = notes[octave][note].natural / 2;
			notes[octave-1][note].sharp = notes[octave][note].sharp / 2;
		}
	}

}



#define MUS_C_0 262
#define MUS_Cs_0 277
#define MUS_D_0 294
#define MUS_Ds_0 311
#define MUS_Ef_0 MUS_Ds_0
#define MUS_E_0 330
#define MUS_Es_0 349
#define MUS_F_0 MUS_Es_0
#define MUS_Fs_0 370
#define MUS_G_0 392
#define MUS_Gs_0 415
#define MUS_A_0 440
#define MUS_As_0 466
#define MUS_Bf_0 MUS_As_0
#define MUS_B_0 494

#define MUS_C_1 523
#define MUS_Cs_1 554
#define MUS_D_1 587
#define MUS_Ds_1 622
#define MUS_Ef_1 MUS_Ds_1
#define MUS_E_1 659
#define MUS_Es_1 698
#define MUS_F_1 MUS_Es_1
#define MUS_Fs_1 740
#define MUS_G_1 784
#define MUS_Gs_1 831
#define MUS_A_1 880
#define MUS_As_1 932
#define MUS_Bf_1 MUS_As_1
#define MUS_B_1 988

#define MUS_C_2 1046
#define MUS_Cs_2 1109
#define MUS_D_2 1175
#define MUS_Ds_2 1244
#define MUS_Ef_2 MUS_Ds_2
#define MUS_E_2 1328
#define MUS_Es_2 1397
#define MUS_F_2 1397
#define MUS_Fs_2 1480
#define MUS_G_2 1568
#define MUS_Gs_2 1661
#define MUS_A_2 1760
#define MUS_As_2 1865
#define MUS_Bf_2 MUS_As_2
#define MUS_B_2 1975

#define MUS_C_3 2093
#define MUS_Cs_3 2217
#define MUS_D_3 2349
#define MUS_Ds_3 2489
#define MUS_Ef_3 MUS_Ds_3
#define MUS_E_3 2637
#define MUS_Es_3 2794
#define MUS_F_3 2794
#define MUS_Fs_3 2960
#define MUS_G_3 3136
#define MUS_Gs_3 3322
#define MUS_A_3 3520
#define MUS_As_3 3729
#define MUS_Bf_3 MUS_As_3
#define MUS_B_3 3951




uint32_t noteSpace;
uint32_t noteBaseLength;

#define TENUTO 10
#define STACCATO 100
#define LEGATO 50




void SetTempo(uint32_t tempo, uint32_t articulation) {
	noteBaseLength = (uint32_t)((float)(60000.0f / (float)tempo) * 4.0f) - articulation;
	noteSpace = articulation;
}

void CommutateAB(uint16_t ccr, uint32_t ms)
{
	AFetLoOn();
	B_FET_HI_CCR = ccr;
	DelayMs(ms);
	B_FET_HI_CCR = 0;
	AFetLoOff();
}
void CommutateAC(uint16_t ccr, uint32_t ms)
{
	AFetLoOn();
	C_FET_HI_CCR = ccr;
	DelayMs(ms);
	C_FET_HI_CCR = 0;
	AFetLoOff();
}
void CommutateBC(uint16_t ccr, uint32_t ms)
{
	BFetLoOn();
	C_FET_HI_CCR = ccr;
	DelayMs(ms);
	C_FET_HI_CCR = 0;
	BFetLoOff();
}
void CommutateBA(uint16_t ccr, uint32_t ms)
{
	BFetLoOn();
	A_FET_HI_CCR = ccr;
	DelayMs(ms);
	A_FET_HI_CCR = 0;
	BFetLoOff();
}
void CommutateCA(uint16_t ccr, uint32_t ms)
{
	CFetLoOn();
	A_FET_HI_CCR = ccr;
	DelayMs(ms);
	A_FET_HI_CCR = 0;
	CFetLoOff();
}
void CommutateCB(uint16_t ccr, uint32_t ms)
{
	CFetLoOn();
	B_FET_HI_CCR = ccr;
	DelayMs(ms);
	B_FET_HI_CCR = 0;
	CFetLoOff();
}
void RunTimerSound(uint32_t frequency, uint32_t ms)
{
	static uint32_t commutationStep = 0;

	commutationStep++;
	if (commutationStep >= 6)
		commutationStep = 0;

	uint16_t ccr = (uint16_t)lrintf(0.010f * (float)((SystemCoreClock / 4) / frequency));
	AFetLoOff();
    BFetLoOff();
    CFetLoOff();
	InitFetTimerGpios(frequency, SystemCoreClock / 4);

	switch (commutationStep)
	{
		case 0:
			CommutateAB(ccr, ms);
			break;
		case 1:
			CommutateAC(ccr, ms);
			break;
		case 2:
			CommutateBC(ccr, ms);
			break;
		case 3:
			CommutateBA(ccr, ms);
			break;
		case 4:
			CommutateCA(ccr, ms);
			break;
		case 5:
		default:
			CommutateCB(ccr, ms);
			break;
	}

	//BFetLoOn();
	//C_FET_HI_CCR = ccr;
	//DelayMs(ms);
	//C_FET_HI_CCR = 0;
	//BFetLoOff();
}


void PlayNote(uint32_t note, float noteLength)
{
	if (!note) 
	{
		DelayMs(noteLength);
	}
	else
	{
		RunTimerSound(note, (uint32_t)(noteLength*noteBaseLength));
	}

	DelayMs(noteSpace);
}


void PlayImperialMarch(void)
{

	SetTempo(108, LEGATO);

	for (uint32_t x=0;x<imperialMarchLength;x++)
	{
		//TIM2->CCR1 = (uint16_t)lrintf(imperialMarch[x].noteLength * (float)((SystemCoreClock) / 32000)); 
		PlayNote(imperialMarch[x].note, imperialMarch[x].noteLength);
	}
}







void ledtimer(uint32_t pwmHz, uint32_t timerHz)
{
	uint16_t timerPrescaler = 0;

	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_OC_InitTypeDef sConfigOC;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_ClockConfigTypeDef sClockSourceConfig;

	timerPrescaler = (uint16_t)(SystemCoreClock / timerHz) - 1;

	// Initialize GPIO
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;

	GPIO_InitStruct.Pin       = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	// Initialize timer
	ledTimer.Instance           = TIM2;
	ledTimer.Init.Prescaler     = timerPrescaler;
	ledTimer.Init.CounterMode   = TIM_COUNTERMODE_UP;
	ledTimer.Init.Period        = (timerHz / pwmHz) - 1;
	ledTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&ledTimer);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&ledTimer, &sClockSourceConfig);

	HAL_TIM_PWM_Init(&ledTimer);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&ledTimer, &sMasterConfig);

	// Initialize timer pwm channel

	sConfigOC.OCMode      = TIM_OCMODE_PWM2;
	sConfigOC.Pulse       = 0;
	sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_PWM_ConfigChannel(&ledTimer, &sConfigOC, TIM_CHANNEL_1);

	HAL_TIM_Base_Start(&ledTimer);
	HAL_TIM_PWM_Start(&ledTimer, TIM_CHANNEL_1);

}




int main(void)
{

	int32_t count;
	int32_t x;

	count = 16;

	VectorIrqInit(0x08000000);
    BoardInit();

	//InitLeds();
	//DelayMs(200);
	//InitFets();
	//DelayMs(5);

	//for (x = 0; x < 5; x++) {
	//	Beep(2000 + x * 6000, 10, 10);
	//	delayUs(1000);
	//}

	ZeroMotor();
	// set timer to output 2000 Hz tone
	InitFetTimerGpios(2093, SystemCoreClock / 2);

	//turn on low fet

	//InitFets();
	//Beep(3000, 3000, 10);

	ledtimer(32000, SystemCoreClock);
	TIM2->CCR1 = (uint16_t)lrintf(0.5f * (float)((SystemCoreClock) / 32000));


    while (1)
    {
/*
		RunTimerSound(MUS_G_1, 400);
		RunTimerSound(MUS_G_1, 100);
		RunTimerSound(MUS_A_1, 500);
		RunTimerSound(MUS_G_1, 500);
		RunTimerSound(MUS_C_2, 500);
		RunTimerSound(MUS_B_1, 1000);
		DelayMs(250);
		RunTimerSound(MUS_G_1, 400);
		RunTimerSound(MUS_G_1, 100);
		RunTimerSound(MUS_A_1, 500);
		RunTimerSound(MUS_G_1, 500);
		RunTimerSound(MUS_D_2, 500);
		RunTimerSound(MUS_C_2, 1000);
		DelayMs(250);
		RunTimerSound(MUS_G_1, 400);
		RunTimerSound(MUS_G_1, 100);
		RunTimerSound(MUS_G_2, 500);
		RunTimerSound(MUS_E_2, 500);
		RunTimerSound(MUS_C_2, 250);DelayMs(10);
		RunTimerSound(MUS_C_2, 250);
		RunTimerSound(MUS_B_1, 500);
		RunTimerSound(MUS_A_1, 1000);
		DelayMs(250);
		RunTimerSound(MUS_F_2, 250);DelayMs(10);
		RunTimerSound(MUS_F_2, 250);
		RunTimerSound(MUS_E_2, 500);
		RunTimerSound(MUS_C_2, 500);
		RunTimerSound(MUS_D_2, 500);
		RunTimerSound(MUS_C_2, 1000);
		DelayMs(250);

		RunTimerSound(MUS_G_0, 400);
		RunTimerSound(MUS_G_0, 100);
		RunTimerSound(MUS_A_0, 500);
		RunTimerSound(MUS_G_0, 500);
		RunTimerSound(MUS_C_1, 500);
		RunTimerSound(MUS_B_0, 1000);
		DelayMs(250);
		RunTimerSound(MUS_G_0, 400);
		RunTimerSound(MUS_G_0, 100);
		RunTimerSound(MUS_A_0, 500);
		RunTimerSound(MUS_G_0, 500);
		RunTimerSound(MUS_D_1, 500);
		RunTimerSound(MUS_C_1, 1000);
		DelayMs(250);
		RunTimerSound(MUS_G_0, 400);
		RunTimerSound(MUS_G_0, 100);
		RunTimerSound(MUS_G_1, 500);
		RunTimerSound(MUS_E_1, 500);
		RunTimerSound(MUS_C_1, 250);DelayMs(10);
		RunTimerSound(MUS_C_1, 250);
		RunTimerSound(MUS_B_0, 500);
		RunTimerSound(MUS_A_0, 1000);
		DelayMs(250);
		RunTimerSound(MUS_F_1, 250);DelayMs(10);
		RunTimerSound(MUS_F_1, 250);
		RunTimerSound(MUS_E_1, 500);
		RunTimerSound(MUS_C_1, 500);
		RunTimerSound(MUS_D_1, 500);
		RunTimerSound(MUS_C_1, 1000);
		DelayMs(250);
*/

/*
		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_Ef_0, (uint32_t)(600 * 0.75));
		RunTimerSound(MUS_Bf_0, (uint32_t)(600 * 0.25));
		DelayMs(50);

		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_Ef_0, (uint32_t)(600 * 0.75));
		RunTimerSound(MUS_Bf_0, (uint32_t)(600 * 0.25));
		DelayMs(50);
		RunTimerSound(MUS_G_0, 1200);
		DelayMs(50);



		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);

		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_Ef_0, (uint32_t)(600 * 0.75));
		DelayMs(50);
		RunTimerSound(MUS_G_0, 1200);
		DelayMs(50);


		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_Ef_0, (uint32_t)(600 * 0.75));
		RunTimerSound(MUS_Bf_0, (uint32_t)(600 * 0.25));
		DelayMs(50);

		RunTimerSound(MUS_G_0, 600);
		DelayMs(50);
		RunTimerSound(MUS_Ef_0, (uint32_t)(600 * 0.75));
		RunTimerSound(MUS_Bf_0, (uint32_t)(600 * 0.25));
		DelayMs(50);
		RunTimerSound(MUS_G_0, 1200);
		DelayMs(50);
*/

		SetupNotes();
		LoadSongs();
		PlayImperialMarch();

		//set volume for low fet
		//AFetLoOn();
		//BFetLoOn();
		//AFetLoOn();

		//AFetLoOff();
		//AFetLoOff();
		//B_FET_HI_CCR = 0;

/*
		AFetLoOn();
		C_FET_HI_CCR = ccr;
		delayUs(1000);
		//AFetLoOff();
		AFetLoOff();
		C_FET_HI_CCR = 0;

		BFetLoOn();
		C_FET_HI_CCR = ccr;
		delayUs(1000);
		//AFetLoOff();
		BFetLoOff();
		C_FET_HI_CCR = 0;
	
		BFetLoOn();
		A_FET_HI_CCR = ccr;
		delayUs(1000);
		//AFetLoOff();
		BFetLoOff();
		A_FET_HI_CCR = 0;

		CFetLoOn();
		A_FET_HI_CCR = ccr;
		delayUs(1000);
		//AFetLoOff();
		CFetLoOff();
		A_FET_HI_CCR = 0;

		CFetLoOn();
		B_FET_HI_CCR = ccr;
		delayUs(1000);
		//AFetLoOff();
		CFetLoOff();
		B_FET_HI_CCR = 0;
*/
		//B_FET_HI_CCR = ccr;
		//C_FET_HI_CCR = ccr;
		//DelayMs(100); //let timer run for 100 ms
		//A_FET_HI_CCR = 0; // turn off pwm
		//B_FET_HI_CCR = 0; // turn off pwm
		//C_FET_HI_CCR = 0; // turn off pwm
		//DelayMs(100); //let timer run for 100 ms

		//BFetLoOn();
		//A_FET_HI_CCR = 0;
		//B_FET_HI_CCR = ccr;
		//C_FET_HI_CCR = 0;
		//DelayMs(100); //let timer run for 100 ms
		//A_FET_HI_CCR = 0; // turn off pwm
		//B_FET_HI_CCR = 0; // turn off pwm
		//C_FET_HI_CCR = 0; // turn off pwm
		//DelayMs(100); //let timer run for 100 ms
		//BFetLoOff();

		//CFetLoOn();
		//A_FET_HI_CCR = 0;
		//B_FET_HI_CCR = 0;
		//C_FET_HI_CCR = ccr;
		//DelayMs(100); //let timer run for 100 ms
		//A_FET_HI_CCR = 0; // turn off pwm
		//B_FET_HI_CCR = 0; // turn off pwm
		//C_FET_HI_CCR = 0; // turn off pwm
		//DelayMs(100); //let timer run for 100 ms
		//CFetLoOff();

		//inlineDigitalHi(LED0_GPIO, LED0_PIN);
		//DelayMs(100);
		//inlineDigitalHi(LED1_GPIO, LED1_PIN);
		//DelayMs(100);
		//inlineDigitalHi(LED2_GPIO, LED2_PIN);
		//DelayMs(100);

		//inlineDigitalLo(LED0_GPIO, LED0_PIN);
		//DelayMs(100);
		//inlineDigitalLo(LED1_GPIO, LED1_PIN);
		//DelayMs(100);
		//inlineDigitalLo(LED2_GPIO, LED2_PIN);
		//DelayMs(100);

    	//Scheduler(count--);

    	if (count == -1)
    		count = 16;
    }

}
