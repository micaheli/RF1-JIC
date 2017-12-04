#pragma once


extern uint32_t SKIP_PWM_ISR;

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

extern motor_state motorState;

extern void ZeroMotor(void);
extern void InitFetTimerGpios(uint32_t pwmHz, uint32_t timerHz);