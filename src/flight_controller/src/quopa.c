#include "includes.h"

#define QUAD_HAS_FLOWN_MS_LIMIT 1000
#define QUOPA_PATTERN_HOLD_MS 250

volatile quopa_state quopaState;
volatile quopa_state dshotBeepState;
int oldEscProtocol;
int oldEscFrequency;


int InitDshotBeep(void)
{
    dshotBeepState = QUOPA_INACTIVE;
    return(0);
}

int InitQuopaMode(void)
{
    oldEscProtocol = -1;
    oldEscFrequency = -1;
    quopaState = QUOPA_INACTIVE;
    return(0);
}

int HandleQuopaMode(void)
{
    static int quopaModeLatch = 0;
    uint8_t serialOutBuffer[3];

    if( 
        !quopaModeLatch &&
        !boardArmed &&
        (armedTimeSincePower > QUAD_HAS_FLOWN_MS_LIMIT) &&
        !ModeActive(M_QUOPA) &&
        quopaState == QUOPA_INACTIVE
    )
    {
        //switch is inactive and is allowed to be active
        quopaModeLatch = 1;
    }

    if(!boardArmed && !ModeActive(M_QUOPA) && quopaState == QUOPA_ACTIVE)
    {
        if ( IsDshotEnabled() )
        {
            SKIP_GYRO=1;
            DelayMs(2); //let MCU stabilize 
            CommandToDshot(serialOutBuffer, DSHOT_CMD_SPIN_DIRECTION_NORMAL);
            for(uint32_t x=0;x<60;x++)
            {
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
                DelayMs(3); //let MCU stabilize 
            }
            CommandToDshot(serialOutBuffer, DSHOT_CMD_BEEP1);
            for(uint32_t x=0;x<60;x++)
            {
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
                DelayMs(3); //let MCU stabilize 
            }
            SKIP_GYRO=0;
        }
        if(oldEscProtocol != -1)
        {
            SKIP_GYRO=1;
            DeinitFlight();
            DelayMs(5); //let MCU stabilize 
            mainConfig.mixerConfig.escProtocol = oldEscProtocol;
            mainConfig.mixerConfig.escUpdateFrequency = oldEscFrequency;
            oldEscProtocol = -1;
            oldEscFrequency = -1;
            InitFlight();
            DelayMs(5); //let MCU stabilize 
            SKIP_GYRO=0;
            DelayMs(5); //let MCU stabilize 
        }
        quopaState = QUOPA_INACTIVE;
    }

    if( 
        quopaModeLatch &&
        !boardArmed &&
        (armedTimeSincePower > QUAD_HAS_FLOWN_MS_LIMIT) &&
        ModeActive(M_QUOPA) &&
        quopaState == QUOPA_INACTIVE
    )
    {
        //quopa mode has been latched and all conditions for quopa mode have been met
        quopaState = QUOPA_INIT;
        quopaModeLatch = 0;
    }        

    if(quopaState == QUOPA_INIT)
    {
        if ( !IsDshotEnabled() )
        {
            //SKIP_GYRO=1;
            DeinitFlight();
            DelayMs(10); //let MCU stabilize 
            //DelayMs(20); //let MCU stabilize 
            oldEscProtocol  = mainConfig.mixerConfig.escProtocol;
			oldEscFrequency = mainConfig.mixerConfig.escUpdateFrequency;
            mainConfig.mixerConfig.escProtocol = ESC_DSHOT300;
            mainConfig.mixerConfig.escUpdateFrequency = 8000;
            InitFlight();
            DelayMs(5); //let MCU stabilize 
            SKIP_GYRO=1;
            //DelayMs(3000); //let MCU stabilize 
            CommandToDshot(serialOutBuffer, 0);
            for(uint32_t x=0;x<3000;x++)
            {
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
                delayUs(500); //let MCU stabilize 
            }
        }

        SKIP_GYRO=1;
        DelayMs(2);
        CommandToDshot(serialOutBuffer, DSHOT_CMD_SPIN_DIRECTION_REVERSED);
        for(uint32_t x=0;x<60;x++)
        {
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
            DelayMs(3); //let MCU stabilize 
        }
        CommandToDshot(serialOutBuffer, DSHOT_CMD_BEEP4);
        for(uint32_t x=0;x<60;x++)
        {
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
            DelayMs(3); //let MCU stabilize 
        }

        SKIP_GYRO=0;
        quopaState = QUOPA_ACTIVE;

    }
    return(0);
}

int HandleDshotBeep(void)
{
    static int dshotBeepLatch = 0;
    static uint32_t lastDshotBeep = 0;
    uint8_t serialOutBuffer[3];

    if( 
        !dshotBeepLatch &&
        !boardArmed &&
        !ModeActive(M_BEEP) &&
        dshotBeepState == QUOPA_INACTIVE
    )
    {
        //switch is inactive and is allowed to be active
        dshotBeepLatch = 1;
    }

    if(!boardArmed && !ModeActive(M_BEEP) && dshotBeepState == QUOPA_ACTIVE)
    {
        if ( IsDshotEnabled() )
        {
            SKIP_GYRO=1;
            DelayMs(2); //let MCU stabilize 
            CommandToDshot(serialOutBuffer, DSHOT_CMD_SPIN_DIRECTION_NORMAL);
            for(uint32_t x=0;x<60;x++)
            {
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
                DelayMs(3); //let MCU stabilize 
            }
            CommandToDshot(serialOutBuffer, DSHOT_CMD_BEEP1);
            for(uint32_t x=0;x<60;x++)
            {
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
                DelayMs(3); //let MCU stabilize 
            }
            SKIP_GYRO=0;
        }
        if(oldEscProtocol != -1)
        {
            SKIP_GYRO=1;
            DeinitFlight();
            DelayMs(5); //let MCU stabilize 
            mainConfig.mixerConfig.escProtocol = oldEscProtocol;
            mainConfig.mixerConfig.escUpdateFrequency = oldEscFrequency;
            oldEscProtocol = -1;
            oldEscFrequency = -1;
            InitFlight();
            DelayMs(5); //let MCU stabilize 
            SKIP_GYRO=0;
            DelayMs(5); //let MCU stabilize 
        }
        dshotBeepState = QUOPA_INACTIVE;
    }

    if( 
        dshotBeepLatch &&
        !boardArmed &&
        ModeActive(M_BEEP) &&
        dshotBeepState == QUOPA_INACTIVE
    )
    {
        //quopa mode has been latched and all conditions for quopa mode have been met
        dshotBeepState = QUOPA_INIT;
        dshotBeepLatch = 0;
    }        

    if( (InlineMillis() - lastDshotBeep > 4000) && (dshotBeepState == QUOPA_ACTIVE) && ModeActive(M_BEEP))
    {
        lastDshotBeep = InlineMillis();
        SKIP_GYRO=1;
        CommandToDshot(serialOutBuffer, DSHOT_CMD_BEEP2);
        for(uint32_t x=0;x<60;x++)
        {
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
            DelayMs(3); //let MCU stabilize 
        }
        CommandToDshot(serialOutBuffer, DSHOT_CMD_BEEP4);
        for(uint32_t x=0;x<60;x++)
        {
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
            DelayMs(3); //let MCU stabilize 
        }
    }

    if(dshotBeepState == QUOPA_INIT)
    {
        if ( !IsDshotEnabled() )
        {
            DeinitFlight();
            DelayMs(10); //let MCU stabilize 
            oldEscProtocol  = mainConfig.mixerConfig.escProtocol;
			oldEscFrequency = mainConfig.mixerConfig.escUpdateFrequency;
            mainConfig.mixerConfig.escProtocol = ESC_DSHOT300;
            mainConfig.mixerConfig.escUpdateFrequency = 8000;
            InitFlight();
            DelayMs(5); //let MCU stabilize 
            SKIP_GYRO=1;
            CommandToDshot(serialOutBuffer, 0);
            for(uint32_t x=0;x<3000;x++)
            {
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
                OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
                delayUs(500); //let MCU stabilize 
            }
        }
        SKIP_GYRO=1;
        DelayMs(2);
        CommandToDshot(serialOutBuffer, DSHOT_CMD_BEEP2);
        for(uint32_t x=0;x<60;x++)
        {
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
            DelayMs(3); //let MCU stabilize 
        }
        CommandToDshot(serialOutBuffer, DSHOT_CMD_BEEP4);
        for(uint32_t x=0;x<60;x++)
        {
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
            DelayMs(3); //let MCU stabilize 
        }
        SKIP_GYRO=1;
        dshotBeepState = QUOPA_ACTIVE;
    }
    return(0);
}

void CommandToDshot(uint8_t *serialOutBuffer, uint32_t command)
{
	uint32_t digitalThrottle;
	int      checksum = 0;
	int      checksumData;
	int      i;

	//if (command < 48)
	//{
		digitalThrottle = ( ( (uint32_t)( command ) << 1 ) | 0); //0 is no telem request, 1 is telem request
	//}
	//else
	//{
	//	digitalThrottle = 0;
	//}

	checksumData = digitalThrottle;

    for (i = 0; i < 3; i++)
	{
    	checksum ^=  checksumData;   // xor data by nibbles
    	checksumData >>= 4;
    }
    checksum &= 0xf;
    // append checksum
    digitalThrottle = ((digitalThrottle << 4) | checksum);

    serialOutBuffer[0] = (uint8_t)(digitalThrottle >> 8);
    serialOutBuffer[1] = (uint8_t)(digitalThrottle & 0x00ff);

}