#include "includes.h"

volatile uint32_t fcStatusReg;
volatile uint32_t rebootAddressReg;
volatile uint32_t bootDirectionReg;
volatile uint32_t bootCyclesReg;
volatile uint32_t rebootPendingReg;
rfbl_command_enum rfblCommand;
rfbl_state_enum   rfblState;
uint32_t          ledTime;
firmware_info     fwInfo;

//bootloader 1 resides at 0x08000000, bootloader 2 resides at 0x08008000
//both bootloaders are the same except for position and some other small things which are set here
#ifdef RFBLTARGET //RFBL is bootloader 2
uint32_t bootloaderAddress  = ADDRESS_RFBL_START;
uint32_t applicationAddress = ADDRESS_RFFW_START;
uint32_t emergencyStart     = ADDRESS_RFFW_START;
uint32_t rfblLedSpeed1      = 1000;
uint32_t rfblLedSpeed2      = 125;
uint32_t checkTooManyStarts = 0;
uint32_t bootupDelay        = 155;
uint32_t upgradeSkip        = ADDRESS_RFBL_START;
uint32_t isRfbl             = 1;
#else //Not RFBL so it must be recovery which is bootloader 1
uint32_t bootloaderAddress  = ADDRESS_RECOVERY_START;
uint32_t applicationAddress = ADDRESS_RFBL_START;
uint32_t emergencyStart     = ADDRESS_RFFW_START;
uint32_t rfblLedSpeed1      = 500;
uint32_t rfblLedSpeed2      = 250;
uint32_t checkTooManyStarts = 1;
uint32_t bootupDelay        = 0;
uint32_t upgradeSkip        = 0;
uint32_t isRfbl             = 0;
#endif

char rfblTagString[20] = RFBL_TAG; //used to store a string in the flash. :)


static void HandleBackupRegistersAndBooting(void);
static void ResetBackupRegisters(void);
static void ReadBackupRegisters(void);
static void CheckRfblCommand(void);
static void RfblReportState(void);
static void RfblWritePacket(void);
static void RfblParseLoadCommand(void);
static void RfblExecuteLoadCommand(void);


void InitRfbl(void)
{

	HandleBackupRegistersAndBooting();

	//FC will either boot into app or continue on here after the HandleBackupRegistersAndBooting function runs
    InitLeds();
    InitUsb();
	StartupBlink(20, 20); //uses DelayMs, blocking function

	//set initial states
	rfblCommand = RFBLC_NONE;
	rfblState   = RFBLS_IDLE;
	ledTime     = 0;

}

void CheckRfblState(void)
{

	switch (rfblState)
	{

		case RFBLS_VERSION:
			RfblReportState();
			rfblState = RFBLS_IDLE;
			break;

		case RFBLS_REBOOT_TO_CUSTOM:
			RfblReportState();
			BootToAddress((int32_t)((tOutBuffer[10] << 24) | (tOutBuffer[9] << 16) | (tOutBuffer[8] << 8) | tOutBuffer[7]));
			break;

		case RFBLS_LAST:
		case RFBLS_ERROR:
			//blink a bunch of times and restart
			//TODO: Add reason blinks
			ErrorBlink();
			SystemReset();
			break;

		case RFBLS_REBOOT_TO_DFU:
			RfblReportState();
			SystemResetToDfuBootloader();
			break;

		case RFBLS_REBOOT_TO_RFBL:
			RfblReportState();
			SystemReset();
			break;

		case RFBLS_REBOOT_TO_APP: //todo: allow rebooting
		case RFBLS_BOOT_TO_APP:
			RfblReportState();
			BootToAddress(applicationAddress);
			break;

		case RFBLS_PREPARING_FOR_UPDATE:
			//Not really used
			//in multithreading it would be useful
			break;

		case RFBLS_DONE_UPGRADING:
			//Last packet received and written to
			FinishFlash();
			DoneFlashBlink();
			RfblReportState(); //tell PC we're done flashing
			rfblState = RFBLS_IDLE; //back to idle.
			break;

		case RFBLS_AWAITING_FW_DATA:
			//preparing for update had finished and we've reported we're in this state now.
			//We are waiting for a packet of data now.
			CheckRfblCommand(); //fill fwInfo.data and increment fwInfo.data_packets
			break;

		case RFBLS_WRITE_FW_DATA:
			RfblWritePacket(); //get data packet, then write it. //TODO: In slow mode we reply back. In fast mode we just wait for more packets
			rfblState = RFBLS_AWAITING_FW_DATA;
			break;

		case RFBLS_LOAD_TO_BL: //Load command sent. Check the rest of the packet to see what we need to do
			rfblState = RFBLS_PREPARING_FOR_UPDATE;
			RfblParseLoadCommand(); //get commands packets from command
			RfblExecuteLoadCommand(); //Sanity check command packets //TODO: Reply with error if bad packets //prepare flash
			rfblState = RFBLS_AWAITING_FW_DATA; //awaiting fw data state
			RfblReportState(); //reply back to PC that we are now ready for data //TODO: Quick mode, slow mode
			break;

		case RFBLS_LOAD_FROM_BL:
		case RFBLS_ERASE_CFG1_FLASH:
		case RFBLS_ERASE_CFG2_FLASH:
		case RFBLS_TOGGLE_LEDS:
		case RFBLS_ERASE_ALL_FLASH:
			RfblReportState();
			rfblState = RFBLS_IDLE;
			break;

		case RFBLS_IDLE:
		default:
			CheckRfblCommand();
			break;

	}

}

//PWM LED 0 based on Micros. We want it to glow on and off over 2 seconds, 1 glowing up and 1 glowing down. We base this on InlineMillis();
void RfblUpdateLed(uint32_t heartbeatMs, uint32_t heartbeatMsHalf)
{

	uint32_t dutyNumber;

	dutyNumber = (InlineMillis() % heartbeatMs);

	if (dutyNumber > heartbeatMsHalf)
	{
		dutyNumber = (heartbeatMs - dutyNumber) * 2;
	}
	else
	{
		dutyNumber *= 2;
	}

	if (rfblState == RFBLS_IDLE)
	{
		//update 1000 times faster
		CoolLedEffect(heartbeatMs, dutyNumber, 0);
	}

}

static void CheckRfblCommand(void) {

	uint32_t x;
	static uint32_t ledToggle = 0;

	if (rfblState == RFBLS_AWAITING_FW_DATA)
	{
		//Let's limit how long we can wait between packets.
		//todo: add micros() command
		//if ( (micros() - fwInfo.time_last_packet) >= MAX_FW_PACKET_WAIT_TIME) {
		//	rfblState = RFBLS_ERROR; //timed out, go to error mode
		//	return;
		//}
	}

	if (tOutBuffer[0]==2)
	{ // We have a report
		if (rfblState == RFBLS_AWAITING_FW_DATA)
		{ //getting FW data packets now
			rfblState = RFBLS_WRITE_FW_DATA;
			//fwInfo.time_last_packet	= micros(); //Set time of packet reception
			fwInfo.time_last_packet	= 0; //Set time of packet reception
			fwInfo.data_packets++; //we got a packet. Let's increment the value.
			if (ledToggle)
			{
				ledToggle=0;
				DoLed(0, 1);
				DoLed(1, 0);
			}
			else
			{
				ledToggle=1;
				DoLed(0, 0);
				DoLed(1, 1);
			}

			if (fwInfo.data_packets >= fwInfo.expected_packets+1)
			{ //Is this the last packet?
				rfblState = RFBLS_DONE_UPGRADING; //yes
				DoLed(0, 0);
				DoLed(1, 0);
			}
			else if ((fwInfo.wordOffset + (fwInfo.skipTo - upgradeSkip)) == fwInfo.size)
			{
				rfblState = RFBLS_DONE_UPGRADING; //yes
				DoLed(0, 0);
				DoLed(1, 0);
			}

			//copy and clear outBuffer
			for (x=0;x<(HID_EPIN_SIZE-1);x++)
			{
				fwInfo.data[x] = tOutBuffer[x];
				tOutBuffer[x] = 0;
			}

			//command is still RFBLC_UPGRADE_FW
			return;
		}
		else if ( (tOutBuffer[1]==0x52) && (tOutBuffer[2]==0x46) && (tOutBuffer[3]==0x42) && (tOutBuffer[4]==0x4C) && (tOutBuffer[5]==0x43) )
		{ //RFBL Command
			if (tOutBuffer[6] >= RFBLC_LAST)
			{
				//ERROR, bad command
				rfblCommand = RFBLC_ERROR;
			}
			else
			{
				rfblCommand = tOutBuffer[6];
			}
		}
		else
		{
			rfblCommand = RFBLC_NONE;
		}

		switch (rfblCommand)
		{

			case RFBLC_NONE:
				rfblState = RFBLS_IDLE;
				break;

			case RFBLC_REBOOT_TO_DFU:
				rfblState = RFBLS_REBOOT_TO_DFU;
				break;

			case RFBLC_ERROR:
				rfblState = RFBLS_ERROR;
				break;

			case RFBLC_REBOOT_TO_RFBL:
				rfblState = RFBLS_REBOOT_TO_RFBL;
				break;

			case RFBLC_REBOOT_TO_APP:
				rfblState = RFBLS_REBOOT_TO_APP;
				break;

			case RFBLC_BOOT_TO_APP:
				rfblState = RFBLS_BOOT_TO_APP;
				break;

			case RFBLC_UPGRADE_FW:
				rfblState = RFBLS_LOAD_TO_BL;
				break;

			case RFBLC_ERASE_CFG1_FLASH:
				rfblState = RFBLS_ERASE_CFG1_FLASH;
				break;

			case RFBLC_ERASE_CFG2_FLASH:
				rfblState = RFBLS_ERASE_CFG2_FLASH;
				break;

			case RFBLC_ERASE_ALL_FLASH:
				rfblState = RFBLS_ERASE_ALL_FLASH;
				break;

			case RFBLC_TOGGLE_LEDS:
				rfblState = RFBLS_TOGGLE_LEDS;
				break;

			case RFBLC_VERSION:
				rfblState = RFBLS_VERSION;
				break;

			case RFBLC_REBOOT_TO_CUSTOM:
				rfblState = RFBLS_REBOOT_TO_CUSTOM;
				break;

			case RFBLC_LAST:
				rfblState = RFBLS_ERROR;
				tOutBuffer[1] = rfblTagString[1];
				break;

		}
		tOutBuffer[0] = 0; //clear buffer id, only need to clear first byte.

	}

}

static void RfblReportState (void)  {


	uint32_t x;

	for (x=0;x<(HID_EPIN_SIZE-1);x++)
	{
		tInBuffer[x]=0;
	}

	tInBuffer[0]=0x01;
	tInBuffer[1]=0x52;
	tInBuffer[2]=0x46;
	tInBuffer[3]=0x42;
	tInBuffer[4]=0x4C;
	tInBuffer[5]=0x53;
	tInBuffer[6]=rfblState;

	if (rfblState == RFBLS_VERSION)
	{
		tInBuffer[7]=RECOVERY_VERSION;
		tInBuffer[8]=RECOVERY_VERSION;
		tInBuffer[9]=uid0_1;
		tInBuffer[10]=uid0_2;
		tInBuffer[11]=uid0_3;
		tInBuffer[12]=uid0_4;
		tInBuffer[13]=uid1_1;
		tInBuffer[14]=uid1_2;
		tInBuffer[15]=uid1_3;
		tInBuffer[16]=uid1_4;
		tInBuffer[17]=uid2_1;
		tInBuffer[18]=uid2_2;
		tInBuffer[19]=uid2_3;
		tInBuffer[20]=uid2_4;
	}

	USBD_HID_SendReport(&hUsbDeviceFS, tInBuffer, HID_EPIN_SIZE);

}

static void HandleBackupRegistersAndBooting(void)
{

	ReadBackupRegisters();

	//FC crashed while in flight. Immediately jump into program. Do not pass go, do not collect $200.
	if (fcStatusReg == FC_STATUS_INFLIGHT)
	{
		BootToAddress(emergencyStart);
	}

	RtcWriteBackupRegister(RFBL_BKR_BOOT_CYCLES_REG, bootCyclesReg);

	//special case where we boot to a specific location, clear registers and boot there
	if (rebootPendingReg == 222)
	{
		if(bootupDelay)
			DelayMs(bootupDelay);
		ResetBackupRegisters();
		BootToAddress(rebootAddressReg);
	}

	//failed to boot 6 times ins a row. Let's clear the registers and go into Recovery mode. This is only checked in the recovery loader.
	if ( (checkTooManyStarts) && (bootCyclesReg > 5) )
	{
		ResetBackupRegisters();
		bootDirectionReg = BOOT_TO_RECOVERY_COMMAND;
		RtcWriteBackupRegister(RFBL_BKR_BOOT_DIRECTION_REG, BOOT_TO_APP_AFTER_RECV_COMMAND);
	}

	//how should we boot?
	switch(bootDirectionReg)
	{
		case BOOT_TO_RECOVERY_COMMAND: //go into recovery mode, to do that we do nothing here
			break;
		case BOOT_TO_DFU_COMMAND:
			SystemResetToDfuBootloader(); //reset to DFU
			break;
		case BOOT_TO_RFBL_COMMAND:
			if(isRfbl) //we're in RFBL already, do nothing
				break;
			else //we're in Recovery loader, boot to RFBL
				BootToAddress(applicationAddress);
			break;
		case BOOT_TO_ADDRESS:
		case BOOT_TO_SPEKTRUM5:
		case BOOT_TO_SPEKTRUM9:
		case BOOT_TO_APP_COMMAND:
		default: //default is to boot
			if(bootupDelay)
				DelayMs(bootupDelay);
			BootToAddress(applicationAddress);
			break;
	}
}

static void ResetBackupRegisters(void)
{
	RtcWriteBackupRegister(RFBL_BKR_BOOT_DIRECTION_REG, BOOT_TO_APP_COMMAND); //default is always boot to app
	RtcWriteBackupRegister(RFBL_BKR_BOOT_CYCLES_REG, 0);    //Reset boot count
	RtcWriteBackupRegister(RFBL_BKR_BOOT_ADDRESSS_REG, 0);  //reset to 0
	RtcWriteBackupRegister(RFBL_BKR_REBOOT_PENDING_REG, 0); //clear the reboot pending since reboot happened
}

static void ReadBackupRegisters(void)
{
	fcStatusReg      = RtcReadBackupRegister(FC_STATUS_REG);
	bootDirectionReg = RtcReadBackupRegister(RFBL_BKR_BOOT_DIRECTION_REG);
	bootCyclesReg    = RtcReadBackupRegister(RFBL_BKR_BOOT_CYCLES_REG) + 1;
	rebootAddressReg = RtcReadBackupRegister(RFBL_BKR_BOOT_ADDRESSS_REG);
	rebootPendingReg = RtcReadBackupRegister(RFBL_BKR_REBOOT_PENDING_REG);
}

static void RfblWritePacket(void) {

	volatile uint32_t data32;
	uint32_t wordoffsetter;

	for (wordoffsetter = 0; wordoffsetter < (floor((HID_EPOUT_SIZE-1) / 4) * 4); wordoffsetter += 4)
	{ //for each packet

		if ( (fwInfo.address + fwInfo.wordOffset) >= fwInfo.skipTo  )
		{ //only write to FW at this point

			data32 = (uint32_t)( (fwInfo.data[wordoffsetter+1] << 0) | (fwInfo.data[wordoffsetter+2] << 8) | (fwInfo.data[wordoffsetter+3] << 16) | (fwInfo.data[wordoffsetter+4] << 24));

			WriteFlash(data32, fwInfo.address + fwInfo.wordOffset );

		}

		fwInfo.wordOffset = fwInfo.wordOffset + 4;

	}

}

static void RfblExecuteLoadCommand(void)
{

	DoLed(0, 0);

	if (fwInfo.mode == MANU) {
		//nothing special needed for manual mode other than sanity checks
		fwInfo.expected_packets = ceil(fwInfo.size / fwInfo.packet_size);
		DoLed(0, 1);
		if ( (fwInfo.erase > fwInfo.address) && (fwInfo.address >= ADDRESS_RFBL_START) ) {
			EraseFlash(fwInfo.address, fwInfo.erase);
	    	PrepareFlash();
		}
		DoLed(0, 0);
	} else {
		fwInfo.expected_packets = 0;
	}

}

static void RfblParseLoadCommand(void)
{

	uint32_t x;
	uint32_t name32;
	uint8_t command_packet[HID_EPOUT_SIZE-1];

	//initialize FW struct
	fwInfo.size    			= 0; //size in bytes of the FW
	fwInfo.type    			= 0; //RFBL, RFBU, RFFW
	fwInfo.address 			= 0; //Address to place firmware.
	fwInfo.mode    			= 0; //Auto, Manu (Manual will place the FW into the location specified by address. Auto will use compiled defaults.)
	fwInfo.erase   			= 0; //All, FW, FWCF, (All only the RFBL will do) (FW will only replace the FW area) (FW + Config will erase FW area and config location for it)
	fwInfo.data_packets		= 0;
	fwInfo.expected_packets	= 0;
	fwInfo.wordOffset		= 0;
	fwInfo.packet_size		= (floor((HID_EPOUT_SIZE-1) / 4) * 4);
	fwInfo.time_last_packet	= 0;
	//todo: add micros() functions
	//fwInfo.time_last_packet	= micros();

	memset(fwInfo.data, 0, HID_EPOUT_SIZE-1); //fw

	memcpy( &command_packet[0], tOutBuffer, HID_EPOUT_SIZE-1 ); //capture outbuffer

	for (x=0;x<(HID_EPIN_SIZE-1);x++)
	{

		name32 = (int32_t)( (command_packet[x] << 24) | (command_packet[x+1] << 16) | (command_packet[x+2] << 8) | (command_packet[x+3]) );

		if (name32 == FWSZ)
		{ //fwsz, firmware size
			fwInfo.size = (int32_t)( (command_packet[x+4] << 24) | (command_packet[x+5] << 16) | (command_packet[x+6] << 8) | (command_packet[x+7]) );
			x = x+6; //name/value found, skip to the next possible location
		}
		else if (name32 == FWTP)
		{ //fwtp, firmware type
			fwInfo.type = (int32_t)( (command_packet[x+4] << 24) | (command_packet[x+5] << 16) | (command_packet[x+6] << 8) | (command_packet[x+7]) );
			x = x+6; //name/value found, skip to the next possible location
		}
		else if (name32 == FWAD)
		{ //fwad, firmware address
			fwInfo.address = (int32_t)( (command_packet[x+4] << 24) | (command_packet[x+5] << 16) | (command_packet[x+6] << 8) | (command_packet[x+7]) );
			x = x+6; //name/value found, skip to the next possible location
		}
		else if (name32 == FWMD)
		{ //fwmd, firmware mode
			fwInfo.mode = (int32_t)( (command_packet[x+4] << 24) | (command_packet[x+5] << 16) | (command_packet[x+6] << 8) | (command_packet[x+7]) );
			x = x+6; //name/value found, skip to the next possible location
		}

		if (name32 == FWER)
		{ //fwer, firmware erase type
			fwInfo.erase = (int32_t)( (command_packet[x+4] << 24) | (command_packet[x+5] << 16) | (command_packet[x+6] << 8) | (command_packet[x+7]) );
			x = x+6; //name/value found, skip to the next possible location
		}

	}

}
