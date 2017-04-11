#include "includes.h"

TIM_HandleTypeDef pwmTimer;

typedef struct
{
    uint32_t acclim;               //Config Acceleration limit. how much is the throttle allowed to change per step
    uint32_t declim;               //Config Deceleration limit. how much is the throttle allowed to change per step
    uint32_t alignmentdc;          //Config 0 to XXXX represents 0 to 100% duty cycle. This is the duty cycle applied to the bridge during alignment.
    uint32_t rampupdc;             //Config 0 to XXXX represents 0 to 100% duty cycle. This is the duty cycle applied to the bridge during the ramp-up and hold portions of the starting sequence.
    uint32_t beepVolumedc;         //Config duty cycle of beep volume
    uint32_t polepairs;            //Config Number of motor pole pairs. Total number of poles/2.
    uint32_t zcthreshold;          //Config 0 to 4095 represents 0 to 3.3 V DC. This is the reference value that the BEMF sample is compared against to determine zero crossings. This should normally be set very low but may be set higher with high BEMF motors to improve noise immunity.
    uint32_t alignmenttime;        //Config Units are PWM cycles (50 microseconds). This parameter sets the time that the motor start-up sequence spends in rotor alignment. This should be set long enough that the rotor comes to a stop before ramp-up commences. The required time will be influenced by rotor + load inertia and system mechanical damping.
    uint32_t demagallowance;       //Config 0 to 255 represents 0 to 255/256 of a step time. This sets the time that the state machine will wait after commutation before beginning to sample BEMF.
    uint32_t holdrpm;              //Config Units are revolutions per minute. This sets the motor speed at the end of the start-up ramp.
    uint32_t holdtime;             //Config Units are PWM cycles (50 microseconds at 24 KHz). This parameter sets the time that the motor is held (in stepping mode) at hold rpm before rotor sync. is started.
    uint32_t startuprpmpersecond;  //Config Controls acceleration during ramp-up.
    uint32_t overloadseconds;      //Config Sets the number of seconds that motor current is permitted to stay above the overload threshold before overload mode is entered. In overload mode, current is "pulled back" to the overload threshold level.
    uint32_t overloadsecondsreset; //Config Sets the number of seconds that current must stay below the overload threshold before overload mode is cancelled.
    uint32_t continuouscurrent;    //Config Units are milliamperes. This is the continuous current rating used by the overload function. Extended operation above this level will activate the overload mode.
    uint32_t direction;            //Config commutate forwards or backwards
    uint32_t pwmHz;                //Config commutate forwards or backwards
    uint32_t timerHz;              //Config commutate forwards or backwards
    uint32_t currentDcSteps;       //Config Max Duty Cycle

	uint32_t autostepFlag;
    uint32_t zcfoundFlag;
	uint32_t runFlag;
    uint32_t risingEdgeFlag;
	uint32_t commcounter;
	uint32_t phase;
    uint32_t position;             //Each count is one step or 60 electrical degrees. This variable holds a relative rotor position. It is incremented with each motor commutation but it is occasionally corrected so that it will not overflow. It serves as the primary input to the speed observer.
    uint32_t positionest;          //This is a state variable of the speed observer. In the steady state, it will follow position but is scaled to be 4096 times bigger to enhance resolution.
	uint32_t startState;
    uint32_t risingdelay;
    uint32_t fallingdelay;
    uint32_t holdcounter;          //This is a software timer that is used to measure the time that the motor is held at a constant speed (in open loop stepping mode) at the end of the ramp-up period, before BEMF sampling is commenced. Each "tick" in 50 microseconds.
	uint32_t runningdc;            //0 to XXXX represents 0 to 100%. This is the actual duty cycle being applied to the bridge when the motor is running.
    uint32_t zccounter;            //Used to measure the time between zero crossings . Each count represents one PWM period or 50 microseconds. zccounter value is incremented each PWM cycle. Its value is transferred to step and it is cleared when ZC is detected.
	uint32_t bemfsample;           //0 to 4095 (ADC counts) represents 0 to 3.3 V DC. This is the most recent reading of motor terminal voltage from the floating phase. Scaling is 1 to 1 but the value is clamped in hardware since we are only looking for zero crossings (positive or negative).
	uint32_t demagcounter;         //Used to measure the demag time allowance, which is the required waiting time from commutation until valid BEMF sampling can resume. This allows time for the current in the floating phase to fall to zero. Each count represents one PWM period or 50 microseconds.
    uint32_t commthreshold;        //Sets the time interval between zero crossing and commutation. Each count represents one PWM period or 50 microseconds.
    uint32_t demagthreshold;       //Sets the time interval between commutation and the resumption of BEMF sampling. Each count represents one PWM period or 50 microseconds.
	uint32_t alignmentCounter;     //This is a software timer that is used to measure the time that the motor is held at the alignment stage before the ramp-up period. Each "tick" in 50 microseconds.
    uint32_t transitioncounter;    //Each "tick" is 1 millisecond. This software timer is used to measure out an interval of 100 milliseconds after rotor sync. is achieved and before the speed regulator is enabled, when the duty cycle is held constant to allow the system to stabilize.
    uint32_t rampspeed;            //This variable controls the stepping rate during ramp-up. It increases in value during ramp-up as the stepping rate is increased. The constant value of 4,000,000,000 is divided by rampspeed to get the current step time (in units of PWM cycles)
    uint32_t ramprate;
    uint32_t heartbeat1time;       //Holds the value of globalcounter at the last execution of the 100 microsecond routine. Each tick is one PWM cycle. It is used to control the execution rate of the routine
    uint32_t heartbeat2time;       //Holds the value of globalcounter at the last execution of the 1 millisecond routine. Each tick is one PWM cycle. It is used to control the execution rate of the routine.
    uint32_t heartbeat3time;       //Holds the value of globalcounter at the last execution of the 10 millisecond routine. Each tick is one PWM cycle. It is used to control the execution rate of the routine.
    uint32_t globalcounter;        //This is a global software timer which is incremented with each run of the PWM interrupt service routine. It is a free running timer which is allowed to naturally roll over.
    uint32_t step;                 //Holds the last measured zero crossing to zero crossing interval. Each count represents one PWM period or 50 microseconds.
    uint32_t minstep;
    uint32_t ifbcount;             //Used to count the number of IFB samples summed into ifbsum. This is used during the calculation of ifbave.
    uint32_t ifbave;               //Average motor current over one full electrical cycle. Used in overload determination.
    uint32_t ifbsum;               //Summation of current samples taken in a given electrical cycle. It is used to calculate the average.
    uint32_t ifb;                  //Most recent motor current sample.
    uint32_t speedest;
    uint32_t rpm;                  //Scaled speed derived from speedest and polepairs. Unit is revolutions per minute.
    uint32_t ledTime;              //Used by the ledstate state machine to time out various time intervals. Each "tick" is one PWM cycle.
} motor_state;

motor_state motorState;


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

	GPIO_InitTypeDef        GPIO_InitStruct;
	TIM_OC_InitTypeDef      sConfigOC;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_ClockConfigTypeDef  sClockSourceConfig;

	timerPrescaler = (uint16_t)(SystemCoreClock / timerHz) - 1;

	InitializeGpio(A_FET_LO_GPIO, A_FET_LO_PIN, 1);
	InitializeGpio(B_FET_LO_GPIO, B_FET_LO_PIN, 1);
	InitializeGpio(C_FET_LO_GPIO, C_FET_LO_PIN, 1);

	// Initialize GPIO
	HAL_GPIO_DeInit(A_FET_HI_GPIO, A_FET_HI_PIN);
	HAL_GPIO_DeInit(B_FET_HI_GPIO, B_FET_HI_PIN);
	HAL_GPIO_DeInit(C_FET_HI_GPIO, C_FET_HI_PIN);

	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = PWM_AF;

	GPIO_InitStruct.Pin       = A_FET_HI_PIN;
	HAL_GPIO_Init(A_FET_HI_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin       = B_FET_HI_PIN;
	HAL_GPIO_Init(B_FET_HI_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin       = C_FET_HI_PIN;
	HAL_GPIO_Init(C_FET_HI_GPIO, &GPIO_InitStruct);


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

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&pwmTimer, &sMasterConfig);

	// Initialize timer pwm channel

	sConfigOC.OCMode      = TIM_OCMODE_PWM2;
	sConfigOC.Pulse       = 0;
	sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode  = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;

	HAL_TIM_PWM_ConfigChannel(&pwmTimer, &sConfigOC, A_FET_HI_TIM_CH);
	HAL_TIM_PWM_ConfigChannel(&pwmTimer, &sConfigOC, B_FET_HI_TIM_CH);
	HAL_TIM_PWM_ConfigChannel(&pwmTimer, &sConfigOC, C_FET_HI_TIM_CH);
	HAL_TIM_PWM_ConfigChannel(&pwmTimer, &sConfigOC, PWM_CH);

	HAL_TIM_Base_Start(&pwmTimer);
	HAL_TIM_PWM_Start(&pwmTimer, B_FET_HI_TIM_CH);
	HAL_TIM_PWM_Start(&pwmTimer, C_FET_HI_TIM_CH);
	HAL_TIM_PWM_Start(&pwmTimer, PWM_CH);
	HAL_TIM_PWM_Start(&pwmTimer, A_FET_HI_TIM_CH);

}