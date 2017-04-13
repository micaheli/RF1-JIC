#include "includes.h"

TIM_HandleTypeDef pwmTimer;
motor_state motorState;
uint32_t SKIP_PWM_ISR = 0;

void IncrementPhase(void);
void ExecuteStartState(void);
void MotorStartInit(void);



void BldcInit(void)
{

    bzero(&motorState, sizeof(motor_state));

    //floats read from config
    float    alignmentPower = 0.01f;
    float    startUpPower   = 0.01f;
    float    beepPower      = 0.01f;
    float    accelLimit     = 0.05f;
    float    decelLimit     = 0.09f;

    //precalculate config variables needed for runtime
    motorState.timerHz        = SystemCoreClock;  //needs to be a multiple of the SystemCoreClock, we want this number as high as possible. Beeps need half this number
    motorState.pwmHz          = 32000;            //should be a multiple of the timerHz
    motorState.currentDcSteps = motorState.timerHz / motorState.pwmHz; //multiples of the above make this number work perfect
    motorState.alignmentdc    = (uint32_t)lrintf(alignmentPower * (float)motorState.currentDcSteps); //set alignment power to duty cycles based on percentage
    motorState.rampupdc       = (uint32_t)lrintf(startUpPower   * (float)motorState.currentDcSteps); //set startup power to duty cycles based on percentage
    motorState.acclim         = (uint32_t)lrintf(accelLimit     * (float)motorState.currentDcSteps); //set accel limit to duty cycles based on percentage
    motorState.declim         = (uint32_t)lrintf(decelLimit     * (float)motorState.currentDcSteps); //set decel limit to duty cycles based on percentage
    motorState.beepVolumedc   = (uint32_t)lrintf(beepPower      * (float)motorState.currentDcSteps); //set beep power to duty cycles based on percentage
    motorState.polepairs      = 7;   //Config Number of motor pole pairs. Total number of poles/2. Read from config for usage in rpm
    motorState.zcthreshold;          //Config 0 to 4095 represents 0 to 3.3 V DC. This is the reference value that the BEMF sample is compared against to determine zero crossings. This should normally be set very low but may be set higher with high BEMF motors to improve noise immunity.
    motorState.alignmenttime;        //Config Units are PWM cycles (50 microseconds). This parameter sets the time that the motor start-up sequence spends in rotor alignment. This should be set long enough that the rotor comes to a stop before ramp-up commences. The required time will be influenced by rotor + load inertia and system mechanical damping.
    motorState.demagallowance;       //Config 0 to 255 represents 0 to 255/256 of a step time. This sets the time that the state machine will wait after commutation before beginning to sample BEMF.
    motorState.holdrpm;              //Config Units are revolutions per minute. This sets the motor speed at the end of the start-up ramp.
    motorState.holdtime;             //Config Units are PWM cycles (50 microseconds at 24 KHz). This parameter sets the time that the motor is held (in stepping mode) at hold rpm before rotor sync. is started.
    motorState.startuprpmpersecond;  //Config Controls acceleration during ramp-up.
    motorState.overloadseconds;      //Config Sets the number of seconds that motor current is permitted to stay above the overload threshold before overload mode is entered. In overload mode, current is "pulled back" to the overload threshold level.
    motorState.overloadsecondsreset; //Config Sets the number of seconds that current must stay below the overload threshold before overload mode is cancelled.
    motorState.continuouscurrent;    //Config Units are milliamperes. This is the continuous current rating used by the overload function. Extended operation above this level will activate the overload mode.
    motorState.direction;            //Config commutate forwards or backwards
    motorState.polepairs = 7;   //poles/2

}

void PwmIsr(void)
{

    if (motorState.autostepFlag) // ramping up or holding speed
    {
        motorState.commcounter++; // measures time between steps in units of PWM periods
        if (motorState.commcounter > motorState.step)
        {
            motorState.commcounter = 0;
            IncrementPhase();
            motorState.position++;
            //Lookup TIM1_CCER value from table based on phase to commutate bridge
        } 
    }

    if (!motorState.runFlag)
        motorState.startState = 0;

    ExecuteStartState(); //Execute the startstate state machine
    //Determine and store ADC channel of BEMF to convert on next PWMISR run // based on step
    //Convert motor current signal via ADC (with bipolar offset) and store in variable ifb
    motorState.globalcounter++;
    //Ifbsum = ifbsum + ifb // to be used in average current calculation Increment ifbcount // to be used in average current calculation Clear hardware interrupt

}

void ExecuteStartState(void)
{
    uint32_t long0;

    switch (motorState.startState)
    {
        case 0: // motor is stopped Turn off bridge
            if (motorState.runFlag)
            {
                MotorStartInit();//Execute motorstartinit function to initialize variables
            }
            motorState.startState = 5;
            break;
        case 5: //set up alignment
            //Set duty cycle per alignmentdc
            motorState.phase = 0;
            //Lookup TIM1_CCER value from table based on phase to commutate bridge
            motorState.alignmentCounter = 0;
            motorState.startState = 10;
            break;
        case 10: //timing out alignment
            if (motorState.alignmentCounter > motorState.alignmenttime)
            {
                motorState.rampspeed = 1;
                motorState.commcounter = 0;
                motorState.autostepFlag = 1;
                //Set dutycycle per rampupdc
                motorState.startState = 20;
            }
            break;
        case 20: // ramping up
            motorState.rampspeed = motorState.rampspeed + motorState.ramprate;
            long0 = 4000000000 / motorState.rampspeed; // convert speed to step period
            if (long0>30000)
                long0 = 30000; // limit step time to prevent math rollover Step = long0
            if (motorState.step < motorState.minstep) // check for end of speed ramp
            {
                motorState.holdcounter = 0;
                motorState.startState = 100;
            }
            break;
        case 100: // wait at constant speed for hold time
            if (motorState.holdcounter > motorState.holdtime)
                motorState.startState = 110;
            break;
        
        case 110: // wait until we are in step 5
            if (motorState.phase == 5)
                motorState.startState = 120;
            break;
        case 120: // wait for leading edge of step 0 (commutation)
            if (motorState.phase == 0)
            {
                motorState.demagcounter = 0;
                motorState.demagthreshold = motorState.step * motorState.demagallowance / 256;
                motorState.startState = 130;
            }
            break;
        case 130: // wait for leading edge of step 0 (commutation)
            motorState.demagcounter++;
            if (motorState.demagcounter > motorState.demagthreshold)
            {
                motorState.startState = 140;
            }
            break;
        case 140: // looking for zero crossing of BEMF
            if (motorState.risingEdgeFlag)
            {
                if (motorState.bemfsample > motorState.zcthreshold)
                {
                    if (motorState.zcfoundFlag)
                        motorState.step = motorState.zccounter;
                    motorState.commthreshold  = motorState.step * motorState.risingdelay / 256;
                    motorState.zccounter      = 0;
                    motorState.commcounter    = 0;
                    motorState.risingEdgeFlag = 0;
                    motorState.zcfoundFlag    = 1;
                    motorState.autostepFlag   = 0;
                    motorState.startState     = 150;
                }
            }
            else
            {
                if (motorState.bemfsample < motorState.zcthreshold)
                {
                    if (motorState.zcfoundFlag)
                        motorState.step = motorState.zccounter;
                    motorState.commthreshold  = motorState.step * motorState.fallingdelay / 256;
                    motorState.zccounter      = 0;
                    motorState.commcounter    = 0;
                    motorState.risingEdgeFlag = 1;
                    motorState.zcfoundFlag    = 1;
                    motorState.autostepFlag   = 0;
                    motorState.startState     = 150;
                }
            }
            break;
        case 150: // wait out commutation delay (from zero crossing)
            A_FET_HI_CCR = motorState.runningdc;
            B_FET_HI_CCR = motorState.runningdc;
            C_FET_HI_CCR = motorState.runningdc;
            motorState.commcounter++; //increment commcounter
            if (motorState.commcounter > motorState.commthreshold) // commutate
            {
                motorState.position++;    //used by speed observer
                IncrementPhase();
                //Lookup TIM1_CCER value from table based on phase to commutate bridge
                motorState.demagcounter = 0;
                motorState.demagthreshold = motorState.step * motorState.demagallowance / 256;
                motorState.startState = 130;//set state back to case 130 to wait out demag
                if (motorState.phase == 0) // calculate current average over one cycle
                {
                    motorState.ifbave = (motorState.ifbsum * 4) / motorState.ifbcount; // average current scaled for use by overload routine
                    motorState.ifbcount = 0;
                    motorState.ifbsum = 0;
                }
            }
            break;
    }
}

inline void ZeroMotor(void)
{
    motorState.runFlag   = 0;
    motorState.runningdc = 0;
    A_FET_HI_CCR = motorState.runningdc;
    B_FET_HI_CCR = motorState.runningdc;
    C_FET_HI_CCR = motorState.runningdc;
}

inline void IncrementPhase(void)
{
    motorState.phase++;
    if (motorState.phase > 5) //wrap from 5 to 0
        motorState.phase = 0;
}

void Routine100uf(void)
{
    uint32_t slong0;
    motorState.heartbeat1time = motorState.globalcounter;
    slong0 = motorState.position - (motorState.positionest/4096); // position observer error (positionest scaled 4096X to improve resolution)
    motorState.speedest = slong0;
    motorState.positionest = motorState.positionest + motorState.speedest; // integrate speedest to get positionest
    // check for and prevent eventual math rollover
    if (motorState.positionest & 0x80000000) // if positionest is greater than 1/2 full scale for variable size
    {
        motorState.positionest = motorState.positionest & 0x7FFFFFFF; // subtract 1/2 full scale to prevent eventual math rollover
        motorState.position = motorState.position - 524288; // subtract 1/2 full scale / 4096 (to account for scaling difference)
    }
}

void Routine1ms(void)
{
/*
    heartbeat2time = globalcounter // overlaod function
    If (ifbave>overloadthreshold AND overload flag is false) overloadcounter = overloadcounter + overloaduprate
    else overloadcounter = overloadcounter - overloaddownrate If (overloadcounter<0)
    {
    overloadcounter = 0
    overloadflag = 0
    }
    If (overloadcounter>1,000,000) set overloadflag true
    If (ifb>overloadsetpoint) AND overloadflag is true) overloaddclimit = overloaddclimit-3
    else overloaddclimit = everloaddclimit + 1
    If (overloaddclimit>1000) overloaddclimit = 1000; If (overloaddclimit<100) overloaddclimit = 100;
    // *************** speed regulator *********************************
    long0 = (speedest*6250)/256// calculate rpm from speedest and number of pole pairs
    rpm = long0/polepairs
    slong0 = rpmref - rpm; // speed error
    propterm = (slong0*propgain)/256;// calculate proportional term of proportional plus integral
    errorint = errorint + slong0; // integrate speed error to get raw integral term
    // set activedc to the lower of maxdc or overloaddclimit If (maxdc<overloaddclimit) activedclimit = maxdc;
    else activedclimit = overloaddclimit;
    // limit integral term so that P + I will be less than the active duty cycle limit
    slong0 = activedclimit - propterm;
    If (slong0<0) slong0 = 0; // slong0 holds maximum allowable intterm slong0 = slong0 * intclampscaler; // slong0 holds max error integral If (errorint>slong0) errorint = slong0;
    If (errorint<0) errorint = 0;
    intterm = (errorint*intgain)>>10; // apply integral gain
    slong0 = propterm + intterm; // combine proportional and integral
    If (slong0>activedclimit) slong0 = activedclimit;
    If (slong0<100) slong0 = 100;
    // count out delay after rotor is synced to enable speed regulator If ((zcfound) && (transitioncounter<100)) transitioncounter++;
    If (transitioncounter<100)
    {
    runningdc = 500;
    errorint = 500 * intclampscaler; rpmref = rpm;
    }
    else
    {
    runningdc = slong0;
    }
    If (motorState.zcfound)
    {
        //set throttle here
        TIM1->CCR1 = runningdc;
        TIM1->CCR2 = runningdc;
        TIM1->CCR3 = runningdc;
    }
*/
}

void Routine10ms(void)
{
    uint32_t slong0;
    motorState.heartbeat3time = motorState.globalcounter;

/*
    //all this will go into the communication ISR
    //set speed
        potvalue = 4095 - adcread(2); // read pot channel If (potvalue>200) set run flag true;
        If (potvalue<100) set run flag false;
        rpmcmd = potvalue*4;
        If (rpmcmd<100) rpmcmd = 100;
    // accel/decel control
    slong0 = rpmcmd-rpmref;
    if (slong0 > acclim) slong0 = acclim;
    if (slong0 < -declim) slong0 = -declim;
    rpmref = rpmref + slong0;
*/
}

void MotorStartInit(void)
{
    //motor runs at SystemCoreClock and whatever pwmHz we want
    //beeps run at SystemCoreClock/2 and whatever pwmHz we want, shouldn't go lower than about 1000 hz since tim1 is a 16 bit timer
    InitFetTimerGpios(motorState.pwmHz, motorState.timerHz);
}

void InitFetTimerGpios(uint32_t pwmHz, uint32_t timerHz)
{
	uint16_t timerPrescaler = 0;

	GPIO_InitTypeDef               GPIO_InitStruct1;
	GPIO_InitTypeDef               GPIO_InitStruct2;
	GPIO_InitTypeDef               GPIO_InitStruct3;
	TIM_OC_InitTypeDef             sConfigOC;
	TIM_MasterConfigTypeDef        sMasterConfig;
	TIM_ClockConfigTypeDef         sClockSourceConfig;
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

	timerPrescaler = (uint16_t)(SystemCoreClock / timerHz) - 1;

	//InitializeGpio(A_FET_LO_GPIO, A_FET_LO_PIN, 0);
	//InitializeGpio(B_FET_LO_GPIO, B_FET_LO_PIN, 0);
	//InitializeGpio(C_FET_LO_GPIO, C_FET_LO_PIN, 0);
	//InitializeGpio(A_FET_HI_GPIO, A_FET_HI_PIN, 0);
	//InitializeGpio(B_FET_HI_GPIO, B_FET_HI_PIN, 0);
	//InitializeGpio(C_FET_HI_GPIO, C_FET_HI_PIN, 0);



    HAL_GPIO_DeInit(A_FET_HI_GPIO, A_FET_HI_PIN);
	GPIO_InitStruct1.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct1.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct1.Speed     = GPIO_SPEED_HIGH;

	GPIO_InitStruct1.Alternate = A_FET_HI_AF;
	GPIO_InitStruct1.Pin       = A_FET_HI_PIN;
	HAL_GPIO_Init(A_FET_HI_GPIO, &GPIO_InitStruct1);

    HAL_GPIO_DeInit(B_FET_HI_GPIO, B_FET_HI_PIN);
	GPIO_InitStruct2.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct2.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct2.Speed     = GPIO_SPEED_HIGH;

	GPIO_InitStruct2.Alternate = B_FET_HI_AF;
	GPIO_InitStruct2.Pin       = B_FET_HI_PIN;
	HAL_GPIO_Init(B_FET_HI_GPIO, &GPIO_InitStruct2);

    HAL_GPIO_DeInit(C_FET_HI_GPIO, C_FET_HI_PIN);
	GPIO_InitStruct3.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct3.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct3.Speed     = GPIO_SPEED_HIGH;

	GPIO_InitStruct3.Alternate = C_FET_HI_AF;
	GPIO_InitStruct3.Pin       = C_FET_HI_PIN;
	HAL_GPIO_Init(C_FET_HI_GPIO, &GPIO_InitStruct3);

	// Initialize timer
	pwmTimer.Instance           = PWM_TIM;
	pwmTimer.Init.Prescaler     = timerPrescaler;
	pwmTimer.Init.CounterMode   = TIM_COUNTERMODE_UP;
	pwmTimer.Init.Period        = (timerHz / pwmHz) - 1;
	pwmTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&pwmTimer);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&pwmTimer, &sClockSourceConfig);

	HAL_TIM_PWM_Init(&pwmTimer);

	//sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	//sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	//HAL_TIMEx_MasterConfigSynchronization(&pwmTimer, &sMasterConfig);

	// Initialize timer pwm channel

	sConfigOC.OCMode      = TIM_OCMODE_PWM1;
	sConfigOC.Pulse       = 0;
	sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_PWM_ConfigChannel(&pwmTimer, &sConfigOC, A_FET_HI_TIM_CH);
	HAL_TIM_PWM_ConfigChannel(&pwmTimer, &sConfigOC, B_FET_HI_TIM_CH);
	HAL_TIM_PWM_ConfigChannel(&pwmTimer, &sConfigOC, C_FET_HI_TIM_CH);
	//HAL_TIM_PWM_ConfigChannel(&pwmTimer, &sConfigOC, PWM_CH);

    sBreakDeadTimeConfig.OffStateRunMode  = TIM_OSSR_ENABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
    sBreakDeadTimeConfig.LockLevel        = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime         = 0;
    sBreakDeadTimeConfig.BreakState       = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&pwmTimer, &sBreakDeadTimeConfig);


	HAL_TIM_Base_Start(&pwmTimer);
	//HAL_TIM_Base_Start_IT(&pwmTimer);
	//HAL_TIM_PWM_Start_IT(&pwmTimer, PWM_CH);
	HAL_TIM_PWM_Start(&pwmTimer, A_FET_HI_TIM_CH);
	HAL_TIM_PWM_Start(&pwmTimer, C_FET_HI_TIM_CH);
	HAL_TIM_PWM_Start(&pwmTimer, B_FET_HI_TIM_CH);


    //HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0, 2);
    //HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

    //HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 1);
    //HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
}


void TIM1_UP_TIM16_IRQHandler(void)
{
    //irq for timer finished, apply throttle here and set CCR for throttle value
    if (PWM_CCR > 1)
        CFetHiOn();

	HAL_TIM_IRQHandler(&pwmTimer);

}
void TIM1_CC_IRQHandler(void)
{
    //irq for CCr counted to. We turn off FETs here
    CFetHiOff();
	HAL_TIM_IRQHandler(&pwmTimer);
    //if (!SKIP_PWM_ISR)
        //PwmIsr();
}