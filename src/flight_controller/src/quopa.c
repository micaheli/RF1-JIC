#include "includes.h"

#define QUAD_HAS_FLOWN_MS_LIMIT 1000
#define QUOPA_PATTERN_HOLD_MS 250

volatile quopa_state quopaState;
int oldEscProtocol;
int oldEscFrequency;

static void CommandToDshot(uint8_t *serialOutBuffer, uint32_t command);

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
            SKIP_GYRO=1;
            DeinitFlight();
            DelayMs(20); //let MCU stabilize 
            oldEscProtocol  = mainConfig.mixerConfig.escProtocol;
			oldEscFrequency = mainConfig.mixerConfig.escUpdateFrequency;
            mainConfig.mixerConfig.escProtocol = ESC_DSHOT600;
            mainConfig.mixerConfig.escUpdateFrequency = 8000;
            InitFlight();
            DelayMs(1); //let MCU stabilize 
            SKIP_GYRO=0;
            DelayMs(50); //let MCU stabilize 
        }
        SKIP_GYRO=1;
        DelayMs(2); //let MCU stabilize 
        CommandToDshot(serialOutBuffer, 0);
        for(uint32_t x=0;x<50;x++)
        {
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
            DelayMs(2); //let MCU stabilize 
        }
        CommandToDshot(serialOutBuffer, DSHOT_CMD_SPIN_DIRECTION_REVERSED);
        for(uint32_t x=0;x<50;x++)
        {
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
            DelayMs(3); //let MCU stabilize 
        }
        CommandToDshot(serialOutBuffer, DSHOT_CMD_BEEP4);
        for(uint32_t x=0;x<30;x++)
        {
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[0], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[1], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[2], 1, 0, 1);
            OutputSerialDmaByte(serialOutBuffer, 2, board.motors[3], 1, 0, 1);
            DelayMs(10); //let MCU stabilize 
        }
        SKIP_GYRO=0;
        quopaState = QUOPA_ACTIVE;
        //}
        //else
        //{
        //    quopaState = QUOPA_INACTIVE;
        //}
    }
    return(0);
}

int StartQuopaMode(void)
{
    //flight is deinit, we need to reverse all ESCs
    //step 1, check what ESCs are set to:
    uint32_t x, outputNumber; //set
    uint16_t value; // set

    oneWireActive = 1;
    if (OneWireInit() == 0)
    {
        //esc read failiure, do nothing
        quopaState = QUOPA_FAILED;
        return(0);
    }
    else
    {
        for (x = 0; x < MAX_MOTOR_NUMBER; x++)
        {
            outputNumber = mainConfig.mixerConfig.motorOutput[x];
            if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
            {
                if (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].enabled)
                {
                    //const oneWireParameterValue_t motorDirectionParameterList[] = {
                    //    {0x01, "normal"},
                    //    {0x02, "reversed"},
                    //    {0x03, "bidirectional"},
                    //    {0x04, "bidirectional_reversed"},
                    //    {0, NULL},
                    //};

                    //put what the ESCs are supposed to be in this variable, 0 means we're not doing the operation
                    mainConfig.mixerConfig.bitReverseEscHidden[x] = escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].oneWireCurrentValues.direction;
                    switch(escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].oneWireCurrentValues.direction)
                    {
                        case 0x01:
                            value = 0x02;
                            break;
                        case 0x02:
                            value = 0x01;
                            break;
                        case 0x03:
                            value = 0x04;
                            break;
                        case 0x04:
                            value = 0x03;
                            break;
                        default:
                            value = 0x01;
                            break;
                    }
                    escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].oneWireCurrentValues.direction = value;
                }
                else
                {
                    //esc read unsuccessful, quopa mode failure
                    quopaState = QUOPA_FAILED;
                    return(0);
                }
            }
        }
    }
    //save config here
    mainConfig.mixerConfig.quopaReversed = 1;    
    SaveConfig(ADDRESS_CONFIG_START);

    //save ESC config here
    for (x = 0; x < MAX_MOTOR_NUMBER; x++)
    {
        outputNumber = mainConfig.mixerConfig.motorOutput[x];
        if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
        {
            if (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].enabled)
            {
                if (!OneWireSaveConfig(board.motors[outputNumber]))
                {
                    quopaState = QUOPA_FAILED;
                }
            }
        }
    }
    oneWireActive = 0;
    quopaState = QUOPA_ACTIVE;
    OneWireDeinit();
    //mainConfig.mixerConfig.bitReverseEscHidden
    //set config flag to reverse
    return(0);
}

int CleanupQuopaMode(void)
{
    //flight is deinit, we need to reverse all ESCs
    //step 1, check what ESCs are set to:
    uint32_t x, outputNumber; //set
    uint16_t value; // set

    return(0);
    if(mainConfig.mixerConfig.quopaReversed)
    {

        oneWireActive = 1;
        if (OneWireInit() == 0)
        {
            //esc read failiure, do nothing
            quopaState = QUOPA_FAILED;
            return(0);
        }
        else
        {
            for (x = 0; x < MAX_MOTOR_NUMBER; x++)
            {
                outputNumber = mainConfig.mixerConfig.motorOutput[x];
                if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
                {
                    if (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].enabled)
                    {
                        //const oneWireParameterValue_t motorDirectionParameterList[] = {
                        //    {0x01, "normal"},
                        //    {0x02, "reversed"},
                        //    {0x03, "bidirectional"},
                        //    {0x04, "bidirectional_reversed"},
                        //    {0, NULL},
                        //};

                        //put what the ESCs are supposed to be in this variable, 0 means we're not doing the operation
                        mainConfig.mixerConfig.bitReverseEscHidden[x] = escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].oneWireCurrentValues.direction;
                        switch(escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].oneWireCurrentValues.direction)
                        {
                            case 0x01:
                                value = 0x02;
                                break;
                            case 0x02:
                                value = 0x01;
                                break;
                            case 0x03:
                                value = 0x04;
                                break;
                            case 0x04:
                                value = 0x03;
                                break;
                            default:
                                value = 0x01;
                                break;
                        }
                        escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].oneWireCurrentValues.direction = value;
                    }
                    else
                    {
                        //esc read unsuccessful, quopa mode failure
                        quopaState = QUOPA_FAILED;
                        return(0);
                    }
                }
            }
        }
        //save config here
        mainConfig.mixerConfig.quopaReversed = 0;    
        SaveConfig(ADDRESS_CONFIG_START);

        //save ESC config here
        for (x = 0; x < MAX_MOTOR_NUMBER; x++)
        {
            outputNumber = mainConfig.mixerConfig.motorOutput[x];
            if (board.motors[outputNumber].enabled == ENUM_ACTUATOR_TYPE_MOTOR)
            {
                if (escOneWireStatus[board.motors[outputNumber].actuatorArrayNum].enabled)
                {
                    if (!OneWireSaveConfig(board.motors[outputNumber]))
                    {
                        quopaState = QUOPA_FAILED;
                    }
                }
            }
        }
        oneWireActive = 0;
        quopaState = QUOPA_INACTIVE;
        OneWireDeinit();
        //mainConfig.mixerConfig.bitReverseEscHidden
        //set config flag to reverse
        return(0);
    }
    return(0);
}

static void CommandToDshot(uint8_t *serialOutBuffer, uint32_t command)
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