#include "includes.h"

#define TRAMP_RETRIES 3
#define TRAMP_BUFFER_SIZE 16

static uint8_t  trampIoBuffer[TRAMP_BUFFER_SIZE];     //set
static uint32_t trampIoBufferCount;                   //set

static uint8_t  TrampChecksum(uint8_t trampBuffer[]);
static void     TrampSendFreq(uint16_t frequency);
static void     TrampSendRfPower(uint16_t power);
static void     TrampSendCommand(uint8_t cmd, uint16_t param);

typedef enum
{
	TRAMP_ERROR  = -1,
	TRAMP_OFF    = 0,
	TRAMP_ON     = 1,
	TRAMP_SET    = 2,
	TRAMP_CHECK  = 3,
} tramp_status_record;

typedef struct {
    tramp_status_record trampStatus;
    uint32_t trampRfFreqMin;
    uint32_t trampRfFreqMax;
    uint32_t trampRfPowerMax;
    uint32_t trampCurFreq;     //current frequency
    uint32_t trampReqFreq;     //requested frequency
    uint32_t trampRetFreq;     //retries for setting frequency
    uint32_t trampBand;
    uint32_t trampChannel;
    uint32_t trampCurPower;    //current power
    uint32_t trampReqPower;    //requested power
    uint32_t trampRetPower;    //retries for setting power
    uint32_t trampTemp;
    uint32_t trampPitMode;
} tramp_info_record;

tramp_info_record trampInfo;

int InitTrampTelemetry(uint32_t usartNumber)
{

    trampInfo.trampStatus     = TRAMP_OFF;
    trampInfo.trampRfFreqMin  = 0;
    trampInfo.trampRfFreqMax  = 0;
    trampInfo.trampRfPowerMax = 0;
    trampInfo.trampCurFreq    = 0;
    trampInfo.trampReqFreq    = 0;
    trampInfo.trampRetFreq    = 0;
    trampInfo.trampBand       = 0;
    trampInfo.trampChannel    = 0;
    trampInfo.trampCurPower   = 0;
    trampInfo.trampReqPower   = 0;
    trampInfo.trampRetPower   = 0;
    trampInfo.trampTemp       = 0;
    trampInfo.trampPitMode    = 0;

    //not using DMA
	board.dmasSerial[board.serials[usartNumber].TXDma].enabled  = 0;
	board.dmasSerial[board.serials[usartNumber].RXDma].enabled  = 0;

	UsartDeInit(usartNumber); //deinits serial and associated pins and DMAs
	UsartInit(usartNumber);   //inits serial and associated pins and DMAs if used. Serial settings are set in serial.c

    return(1);
}


int TrampGetSettings(void)
{
	TrampSendCommand('r', 0);
	TrampSendCommand('v', 0);
	TrampSendCommand('s', 0);
	return(0);
	if (boardArmed)
		return(0);
/*
	//fill buffer
	rxBufferCount = 5;
	smartAudioTxRxBuffer[0] = SM_START_CODE1;
	smartAudioTxRxBuffer[1] = SM_START_CODE2;
	smartAudioTxRxBuffer[2] = ShiftSmartAudioCommand(SM_GET_SETTINGS);
	smartAudioTxRxBuffer[3] = 0x00;
	smartAudioTxRxBuffer[4] = SmCrc8(smartAudioTxRxBuffer, 4);
	SendSoftSerialBlocking(smartAudioTxRxBuffer, rxBufferCount, 50);
	DelayMs(2);
	rxBufferCount = 0;
	rxBufferCount = ReceiveSoftSerialBlocking(smartAudioTxRxBuffer, &rxBufferCount, 150);
	for (tries=4;tries>0;tries--)
	{
		if ( CheckSmartAudioRxCrc(smartAudioTxRxBuffer, rxBufferCount) )
		{
			if (smartAudioTxRxBuffer[2] == SM_VERSION_1)
			{
				vtxRecord.vtxDevice = VTX_DEVICE_SMARTV1;
			}
			else if (smartAudioTxRxBuffer[2] == SM_VERSION_2)
			{
				vtxRecord.vtxDevice = VTX_DEVICE_SMARTV2;
			}
			//set vtxBandChannel (0 through 39)
			vtxRecord.vtxBandChannel = smartAudioTxRxBuffer[4];
			//set vtxBand and vtxChannel (A,1 through R,8)
			VtxChannelToBandAndChannel(vtxRecord.vtxBandChannel, &vtxRecord.vtxBand, &vtxRecord.vtxChannel);
			//set vtxFreqency from band and channel
			vtxRecord.vtxFrequency = VtxBandChannelToFrequency(vtxRecord.vtxBandChannel);
			//set vtx power
			vtxRecord.vtxPower    = smartAudioTxRxBuffer[5];
			//no region info, assume US:
			vtxRecord.vtxRegion   = VTX_REGION_US;
			//VTX in pit mode or active?
			if ( BITMASK_CHECK(smartAudioTxRxBuffer[6], SM_OPMODE_PM) )
			{
				vtxRecord.vtxPit = VTX_MODE_ACTIVE;
			}
			else
			{
				vtxRecord.vtxPit = VTX_MODE_PIT;
			}
			//not used right now, set frequency dependant from bands
			//vtxRecord.vtxFrequency = ((smartAudioTxRxBuffer[8] << 8) | smartAudioTxRxBuffer[9]);
			//only set first time
			if (vtxRequested.vtxDevice == VTX_DEVICE_NONE)
			{
				vtxRequested.vtxDevice      = vtxRecord.vtxDevice;
				vtxRequested.vtxBand        = vtxRecord.vtxBand;
				vtxRequested.vtxChannel     = vtxRecord.vtxChannel;
				vtxRequested.vtxBandChannel = vtxRecord.vtxBandChannel;
				vtxRequested.vtxPower       = vtxRecord.vtxPower;
				vtxRequested.vtxPit         = vtxRecord.vtxPit;
				vtxRequested.vtxRegion      = vtxRecord.vtxRegion;
				vtxRequested.vtxFrequency   = vtxRecord.vtxFrequency;
			}
			return(1);
		}
	}
	return(0);
*/
}

void DeInitTrampTelemetry(void)
{

    int serialNumber; //set
 
    for (serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
    {
        if ( (board.serials[serialNumber].enabled) && (mainConfig.telemConfig.telemTramp) )
        {
            if (board.serials[serialNumber].Protocol == USING_TRAMP)
            {
                UsartDeInit(serialNumber);
                return;
            }
        }
    }

}

static uint8_t TrampChecksum(uint8_t trampBuffer[])
{
    uint8_t checksum = 0;
    int32_t x; //set

    for (x=1;x<14;x++)
        checksum += trampBuffer[x];

    return(checksum);

}

static void TrampSendCommand(uint8_t cmd, uint16_t param)
{

    int32_t x;            //set
    int32_t serialNumber; //set

    bzero(trampIoBuffer, sizeof(trampIoBuffer));
    trampIoBuffer[0]  = 15;
    trampIoBuffer[1]  = cmd;
    trampIoBuffer[2]  = param & 0xff;
    trampIoBuffer[3]  = (param >> 8) & 0xff;
    trampIoBuffer[14] = TrampChecksum(trampIoBuffer);

    for (x=TRAMP_RETRIES;x>=0;x--)
    {
        for (serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
        {
            if ( (board.serials[serialNumber].enabled) && (mainConfig.telemConfig.telemTramp) )
            {
                if (board.serials[serialNumber].Protocol == USING_TRAMP)
                {
                    HAL_UART_Transmit(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)trampIoBuffer,TRAMP_BUFFER_SIZE, 20);
                    bzero(trampIoBuffer, sizeof(trampIoBuffer));
                    HAL_UART_Receive(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)trampIoBuffer, TRAMP_BUFFER_SIZE, 60);
                }
            }
        }
    }

	volatile uint8_t aa1 =  trampIoBuffer[0];
	volatile uint8_t aaa1 =  trampIoBuffer[1];
	volatile uint8_t aaaa1 =  trampIoBuffer[2];
	volatile uint8_t aaaaaa1 =  trampIoBuffer[3];
	volatile uint8_t aaaaaaa1 =  trampIoBuffer[4];
	volatile uint8_t aaaaaaaa1 =  trampIoBuffer[5];
	volatile uint8_t aaaaaaaaa1 =  trampIoBuffer[6];
	volatile uint8_t aaaaaaaaaa1 =  trampIoBuffer[7];
	volatile uint8_t aaaaaaaaaaa1 =  trampIoBuffer[8];
	volatile uint8_t aaaaaaaaaaaa1 =  trampIoBuffer[9];
	volatile uint8_t aaaaaaaaaaaaa1 =  trampIoBuffer[10];
	volatile uint8_t aaaaaaaaaaaaaa1 =  trampIoBuffer[11];
	volatile uint8_t aaaaaaaaaaaaaaa1 =  trampIoBuffer[12];
	volatile uint8_t aaaaaaaaaaaaaaaa1 =  trampIoBuffer[13];
	volatile uint8_t aaaaaaaaaaaaaaaaa1 =  trampIoBuffer[14];
 //   if ( (trampIoBufferCount == 14) && CheckSmartAudioRxCrc(trampRespBuffer) )
 //   {
 //       return(1);
 //   }
}

void TrampSetPitMode(uint32_t on)
{
    if (on)
        TrampSendCommand('I', 0);
    else
        TrampSendCommand('I', 1);
}

void TrampSetFreq(uint16_t freq)
{
    trampInfo.trampReqFreq = freq;
    if(trampInfo.trampReqFreq != trampInfo.trampCurFreq)
        trampInfo.trampRetFreq = TRAMP_RETRIES;
}

static void TrampSendFreq(uint16_t frequency)
{
    TrampSendCommand('F', frequency);
}
void TrampSetRfPower(uint16_t power)
{
    trampInfo.trampReqPower = power;
    if(trampInfo.trampReqPower != trampInfo.trampCurPower)
        trampInfo.trampRetPower = TRAMP_RETRIES;
}
static void TrampSendRfPower(uint16_t power)
{
    TrampSendCommand('P', power);
}
void TrampSetBandChannel(uint32_t bandChannel)
{
    TrampSendFreq((uint16_t)VtxBandChannelToFrequency(bandChannel));
}
void ProcessTrampTelemetry(void)
{
}