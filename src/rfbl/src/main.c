/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "includes.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t tInBuffer[HID_EPIN_SIZE], tOutBuffer[HID_EPOUT_SIZE-1];

uint32_t StartSector = 0, EndSector = 0, Address = 0, i = 0 ;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0 ;
uint32_t toggle_led = 0;
bool bootToRfbl = false;
uint32_t ApplicationAddress = APP_ADDRESS;
uint8_t bindSpektrum = 0;

FwInfo_t FwInfo;
cfg1_t cfg1;


/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

int main(void)
{

	bool rfbl_plug_attatched = false;
	uint32_t rfblVersion, cfg1Version, bootDirection, bootCycles, rebootAddress;

	HAL_RCC_DeInit();
    HAL_DeInit();
    BoardInit();
    LedInit();
    USB_DEVICE_Init(); //start USB


    rfblVersion = rtc_read_backup_reg(RFBL_BKR_RFBL_VERSION_REG);
    cfg1Version = rtc_read_backup_reg(RFBL_BKR_CFG1_VERSION_REG);
    if ((rfblVersion != RFBL_VERSION) || (cfg1Version != CFG1_VERSION)) { //no data or wrong version, let's put defaults
		rtc_write_backup_reg(RFBL_BKR_RFBL_VERSION_REG,   RFBL_VERSION);
		rtc_write_backup_reg(RFBL_BKR_CFG1_VERSION_REG,   CFG1_VERSION);
		rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG, BOOT_TO_APP_COMMAND);
		rtc_write_backup_reg(RFBL_BKR_BOOT_CYCLES_REG,    (uint32_t)0x00000000);
		rtc_write_backup_reg(RFBL_BKR_BOOT_ADDRESSS_REG,  APP_ADDRESS);
    }

    //get config data from backup registers
	rfblVersion   = rtc_read_backup_reg(RFBL_BKR_RFBL_VERSION_REG);
	cfg1Version   = rtc_read_backup_reg(RFBL_BKR_CFG1_VERSION_REG);
	bootDirection = rtc_read_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG);
	bootCycles    = rtc_read_backup_reg(RFBL_BKR_BOOT_CYCLES_REG) + 1;
	rebootAddress = rtc_read_backup_reg(RFBL_BKR_BOOT_ADDRESSS_REG);

	rtc_write_backup_reg(RFBL_BKR_BOOT_CYCLES_REG, bootCycles);

#if defined(HARDWARE_BIND_PLUG) || defined(HARDWARE_DFU_PLUG) || defined(HARDWARE_RFBL_PLUG) || defined(VBUS_SENSE)
    GPIO_InitTypeDef GPIO_InitStructure;
#else
	simpleDelay_ASM(1000);
#endif

#ifdef HARDWARE_RFBL_PLUG
    simpleDelay_ASM(10); //let pin status stabilize

	rfbl_plug_attatched = true;

	//RX
    HAL_GPIO_DeInit(RFBL_GPIO1, RFBL_PIN1);

    GPIO_InitStructure.Pin   = RFBL_PIN1;
    GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
    HAL_GPIO_Init(RFBL_GPIO1, &GPIO_InitStructure);

    //TX
    HAL_GPIO_DeInit(RFBL_GPIO2, RFBL_PIN2);

    GPIO_InitStructure.Pin   = RFBL_PIN2;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    HAL_GPIO_Init(RFBL_GPIO2, &GPIO_InitStructure);

    inlineDigitalHi(RFBL_GPIO2, RFBL_PIN2);
	simpleDelay_ASM(1); //let pin status stabilize
    if ((inlineIsPinStatusHi(RFBL_GPIO1, RFBL_PIN1))) { //is RX hi
    	rfbl_plug_attatched = false;
    }

	inlineDigitalLo(RFBL_GPIO2, RFBL_PIN2);
	simpleDelay_ASM(1); //let pin status stabilize
    if (!(inlineIsPinStatusHi(RFBL_GPIO1, RFBL_PIN1))) { //is RX low
    	rfbl_plug_attatched = false;
    }

    inlineDigitalLo(RFBL_GPIO2, RFBL_PIN2);


	if (rfbl_plug_attatched) {

		startupBlink (150, 25);
		//pins attached, let's wait 4 seconds and see if they're still attached

		rfbl_plug_attatched = true;

	    inlineDigitalHi(RFBL_GPIO2, RFBL_PIN2);
		simpleDelay_ASM(1); //let pin status stabilize
	    if ((inlineIsPinStatusHi(RFBL_GPIO1, RFBL_PIN1))) { //is RX hi
	    	rfbl_plug_attatched = false;
	    }

		inlineDigitalLo(RFBL_GPIO2, RFBL_PIN2);
		simpleDelay_ASM(1); //let pin status stabilize
	    if (!(inlineIsPinStatusHi(RFBL_GPIO1, RFBL_PIN1))) { //is RX low
	    	rfbl_plug_attatched = false;
	    }

	    if (rfbl_plug_attatched) {
	    	//pin still attached, let's go to DFU mode
	    	systemResetToDfuBootloader();
	    } else {
	    	//pin not attached, let's go into RFBL proper
	    	bootToRfbl = true;
	    }

	}

#endif

	startupBlink(50, 25);

	if (!bootToRfbl) {
		//RFBL: pins set bootToRfbl true or false or puts board into DFU mode
		//the inside of these brackets means the RFBL pins are not shorted
		switch (bootDirection) {
			case BOOT_TO_APP_COMMAND:
				boot_to_app();  //jump to application
				break;
			case BOOT_TO_ADDRESS:
				ApplicationAddress = rebootAddress;
				boot_to_app();  //jump to application
				break;
			case BOOT_TO_SPEKTRUM5:
				bindSpektrum = 5; //set spektrum bind number and head to RFBL
				break;
			case BOOT_TO_SPEKTRUM9:
				bindSpektrum = 9; //set spektrum bind number and head to RFBL
				break;
			case BOOT_TO_DFU_COMMAND:
				systemResetToDfuBootloader(); //reset to DFU
				break;
			case BOOT_TO_RFBL_COMMAND:
			default:
				//default is to do nothing, continue to RFBL
				break;
		}
	}




	//initialize RFBL State and Command
	RfblCommand_e RfblCommand = RFBLC_NONE;
	RfblState_e RfblState = RFBLS_IDLE;

    while (1) {

		switch (RfblState) {

			case RFBLS_VERSION:
				rfbl_report_state(&RfblState);
				RfblState = RFBLS_IDLE;
				break;

			case RFBLS_REBOOT_TO_CUSTOM:
				rfbl_report_state(&RfblState);
				RfblState = RFBLS_IDLE;
				ApplicationAddress = ApplicationAddress; //todo: make this work
				boot_to_app();  //jump to application
				break;

			case RFBLS_ERROR:
				//blink a bunch of times and restart
				//TODO: Add reason blinks
				errorBlink();
				__disable_irq();
				NVIC_SystemReset();
				break;

			case RFBLS_TOGGLE_LEDS:
				LED1_TOGGLE;
				LED2_TOGGLE;
				LED3_TOGGLE;

				rfbl_report_state(&RfblState);
				RfblState = RFBLS_IDLE;

				break;

			case RFBLS_REBOOT_TO_DFU:
				rfbl_report_state(&RfblState);
				RfblState = RFBLS_IDLE;
				systemResetToDfuBootloader();
				break;

			case RFBLS_REBOOT_TO_RFBL:
				rfbl_report_state(&RfblState);
				RfblState = RFBLS_IDLE;
				__disable_irq();
				NVIC_SystemReset();
				break;

			case RFBLS_REBOOT_TO_APP: //todo: allow rebooting
			case RFBLS_BOOT_TO_APP:
				rfbl_report_state(&RfblState);
				RfblState = RFBLS_IDLE;
				boot_to_app();  //jump to application
				break;

			case RFBLS_PREPARING_FOR_UPDATE:
				//Not really used
				//in multithreading it would be useful
				break;

			case RFBLS_DONE_UPGRADING:
				//Last packet received and written to
				rfbl_finish_flash();
				RfblState = RFBLS_IDLE;
				rfbl_report_state(&RfblState); //reply back to PC that we are now ready for data //TODO: Quick mode, slow mode
				startupBlink(10, 40);
				startupBlink(10, 35);
				startupBlink(10, 30);
				startupBlink(10, 25);
				startupBlink(10, 20);
				startupBlink(10, 15);
				break;

			case RFBLS_AWAITING_FW_DATA:
				//preparing for update had finished and we've reported we're in this state now.
				//We are waiting for a packet of data now.
				check_rfbl_command(&RfblCommand, &RfblState); //fill FwInfo.data and increment FwInfo.data_packets
				break;

			case RFBLS_WRITE_FW_DATA:
				rfbl_write_packet(); //get data packet, then write it. //TODO: In slow mode we reply back. In fast mode we just wait for more packets
				RfblState = RFBLS_AWAITING_FW_DATA;
				break;

			case RFBLS_LOAD_TO_BL: //Load command sent. Check the rest of the packet to see what we need to do
				RfblState = RFBLS_PREPARING_FOR_UPDATE;
				rfbl_parse_load_command(); //get commands packets from command
				rfbl_execute_load_command(); //Sanity check command packets //TODO: Reply with error if bad packets //prepare flash
				RfblState = RFBLS_AWAITING_FW_DATA; //awaiting fw data state
				rfbl_report_state(&RfblState); //reply back to PC that we are now ready for data //TODO: Quick mode, slow mode

				break;

			case RFBLS_LOAD_FROM_BL:
			case RFBLS_ERASE_CFG1_FLASH:
			case RFBLS_ERASE_CFG2_FLASH:
				rfbl_report_state(&RfblState);
				RfblState = RFBLS_IDLE;
				break;

			case RFBLS_ERASE_ALL_FLASH:
				//erase_all_flash();
				RfblState = RFBLS_IDLE;
				rfbl_report_state(&RfblState);
				break;

			case RFBLS_LAST:
				rfbl_report_state(&RfblState);
				RfblState = RFBLS_IDLE;
				break;

			case RFBLS_IDLE:
			default:
				if (toggle_led < 1000000) {
					LED2_OFF;
					if (toggle_led % 8 == 0) {
						LED1_ON;
					} else {
						LED1_OFF;
					}
				} else if (toggle_led < 2000000) {
					LED1_OFF;
					if (toggle_led % 6 == 0) {
						LED1_ON;
					} else {
						LED1_OFF;
					}
				} else if (toggle_led < 3000000) {
					LED1_OFF;
					if (toggle_led % 4 == 0) {
						LED1_ON;
					} else {
						LED1_OFF;
					}
				} else if (toggle_led < 4000000) {
					LED1_OFF;
					if (toggle_led % 2 == 0) {
						LED2_ON;
					} else {
						LED2_OFF;
					}
				} else {
					toggle_led = 0;
				}
				toggle_led++;

				check_rfbl_command(&RfblCommand, &RfblState);
				break;

		}

    }

}

uint32_t rtc_read_backup_reg(uint32_t BackupRegister) {
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    return HAL_RTCEx_BKUPRead(&RtcHandle, BackupRegister);
}

void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data) {
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&RtcHandle, BackupRegister, data);
    HAL_PWR_DisableBkUpAccess();
}

void startupBlink (uint16_t blinks, uint32_t delay) {

	uint8_t a;

	//Startup Blink
	LED1_ON;
	LED2_OFF;
	for( a = 0; a < blinks; a = a + 1 ){ //fast blink
		LED1_TOGGLE;
		LED2_TOGGLE;
		HAL_Delay(delay);
	}
	LED1_OFF;
	LED2_OFF;
}


/*
void read_cfg1(void) {

	if (validate_cfg1() ) {

		memcpy(&cfg1, (char *) FLASH_CFG1_ADDR_START, sizeof(cfg1_t));
		if (cfg1.boot_direction == FAST_BOOT) {
			boot_to_app();
		}

	} else {

		reset_cfg1();
		write_cfg1();

	}

}

void reset_cfg1(void) {

	memset(&cfg1, 0, sizeof(cfg1_t));
	cfg1.boot_direction = BOOT_TO_APP_COMMAND;
	cfg1.boot_cycles 	= 0x00000001;
	cfg1.reboot_address = APPLICATION_ADDRESS;

}

static uint8_t calculateChecksum(const uint8_t *data, uint32_t length) {

	uint8_t checksum = 0;
    const uint8_t *byteOffset;

    for (byteOffset = data; byteOffset < (data + length); byteOffset++)
        checksum ^= *byteOffset;
    return checksum;
}

void erase_all_flash (void) {

    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

	HAL_FLASH_Unlock();

	//todo made this configurable, will 11 work on all f4/f7s?
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = GetSector(ADDR_FLASH_SECTOR_1);
	EraseInitStruct.NbSectors = 11;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{

		//FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
		ErrorHandler();
	}

	HAL_FLASH_Lock();
}

void write_cfg1(void) {

	uint32_t data32;
	uint32_t address;
	uint32_t wordOffset = 0;

    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

	//save consntants and checksums
	cfg1.rfbl_version 	= RFBL_VERSION;
	cfg1.cfg1_version 	= CFG1_VERSION;
	cfg1.magic_f1eaf00d	= MAGIC_F1EAF00D;
	cfg1.magic_a0ddba11	= MAGIC_A0DDBA11;
	cfg1.size			= sizeof(cfg1_t);
	cfg1.czechsum		= 0;
	cfg1.czechsum		= calculateChecksum((const uint8_t *) &cfg1, sizeof(cfg1_t));

	HAL_FLASH_Unlock();

	//todo made this configurable, will 11 work on all f4/f7s?
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = GetSector(FLASH_CFG1_ADDR_START);
	EraseInitStruct.NbSectors = 1;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{
		//FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
		ErrorHandler();
	}

	//todo made this configurable
	address = FLASH_CFG1_ADDR_START;

	for (wordOffset = 0; wordOffset < sizeof(cfg1_t); wordOffset += 4) {

		data32 = *(uint32_t *) ((char *) &cfg1 + wordOffset );

		if (HAL_FLASH_Program(TYPEPROGRAM_WORD, address + wordOffset, data32) == HAL_OK) {
		} else {
			//FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
			ErrorHandler();
		}

	}

	HAL_FLASH_Lock();

}

bool validate_cfg1(void) {

	const cfg1_t *temp = (const cfg1_t *) FLASH_CFG1_ADDR_START;
	uint8_t czechsum = 0;

	// check version number
    if (CFG1_VERSION != temp->cfg1_version)
        return false;

    // check size and magic numbers
    if (temp->size != sizeof(cfg1_t) || temp->magic_f1eaf00d != MAGIC_F1EAF00D || temp->magic_a0ddba11 != MAGIC_A0DDBA11)
        return false;

    // verify integrity of temporary copy
    czechsum = calculateChecksum((const uint8_t *) temp, sizeof(cfg1_t));
    if (czechsum != 0)
        return false;

    return true;
}

*/

void boot_to_app (void) {

	startupBlink(40, 25);

	HAL_Delay(500); //half second delay before booting into app to allow PDB power to stabilize

	USB_DEVICE_DeInit();
	HAL_RCC_DeInit();
    HAL_DeInit();

	pFunction Jump_To_Application;
	uint32_t JumpAddress;


	__disable_irq(); // disable interrupts

    JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) ApplicationAddress);
	__enable_irq(); // disable interrupts
    Jump_To_Application();

}

void errorBlink(void) {
	LED1_OFF;
	LED2_OFF;
	HAL_Delay(2000);

	LED2_ON;
	HAL_Delay(1000);
	LED2_OFF;
	HAL_Delay(1000);

	LED2_ON;
	HAL_Delay(1000);
	LED2_OFF;
	HAL_Delay(1000);

	LED2_ON;
	HAL_Delay(1000);
	LED2_OFF;
	HAL_Delay(1000);
}

//todo: add micros command
void check_rfbl_command(RfblCommand_e *RfblCommand, RfblState_e *RfblState) {

	if (*RfblState == RFBLS_AWAITING_FW_DATA) {
		//Let's limit how long we can wait between packets.
		//todo: add micros() command
		//if ( (micros() - FwInfo.time_last_packet) >= MAX_FW_PACKET_WAIT_TIME) {
		//	*RfblState = RFBLS_ERROR; //timed out, go to error mode
		//	return;
		//}

	}
	if (tOutBuffer[0]==2) { // We have a report
		if (*RfblState == RFBLS_AWAITING_FW_DATA) { //getting FW data packets now
			*RfblState = RFBLS_WRITE_FW_DATA;
			//FwInfo.time_last_packet	= micros(); //Set time of packet reception
			FwInfo.time_last_packet	= 0; //Set time of packet reception
			FwInfo.data_packets++; //we got a packet. Let's increment the value.
			LED1_TOGGLE;
			LED2_TOGGLE;
			if (FwInfo.data_packets >= FwInfo.expected_packets+1) { //Is this the last packet?
				*RfblState = RFBLS_DONE_UPGRADING; //yes
				LED1_OFF;
				LED2_OFF;
			}
			memcpy( &FwInfo.data[0], tOutBuffer, HID_EPOUT_SIZE-1 ); //capture outbuffer
			memset(tOutBuffer, 0, HID_EPOUT_SIZE-1); //clear outbuffer
			//command is still RFBLC_UPGRADE_FW
			return;
		} else
		if ( (tOutBuffer[1]==0x52) && (tOutBuffer[2]==0x46) && (tOutBuffer[3]==0x42) && (tOutBuffer[4]==0x4C) && (tOutBuffer[5]==0x43) ) { //RFBL Command
			if (tOutBuffer[6] >= RFBLC_LAST) {
				//ERROR, bad command
				*RfblCommand = RFBLC_ERROR;
			} else {
				*RfblCommand = tOutBuffer[6];
			}
		} else {
			*RfblCommand = RFBLC_NONE;
		}

		switch (*RfblCommand) {

			case RFBLC_NONE:
				*RfblState = RFBLS_IDLE;
				break;

			case RFBLC_TOGGLE_LEDS:
				*RfblState = RFBLS_TOGGLE_LEDS;
				break;

			case RFBLC_REBOOT_TO_DFU:
				*RfblState = RFBLS_REBOOT_TO_DFU;
				break;

			case RFBLC_ERROR:
				*RfblState = RFBLS_ERROR;
				break;

			case RFBLC_REBOOT_TO_RFBL:
				*RfblState = RFBLS_REBOOT_TO_RFBL;
				break;

			case RFBLC_REBOOT_TO_APP:
				*RfblState = RFBLS_REBOOT_TO_APP;
				break;

			case RFBLC_BOOT_TO_APP:
				*RfblState = RFBLS_BOOT_TO_APP;
				break;

			case RFBLC_UPGRADE_FW:
				*RfblState = RFBLS_LOAD_TO_BL;
				break;

			case RFBLC_ERASE_CFG1_FLASH:
				*RfblState = RFBLS_ERASE_CFG1_FLASH;
				break;

			case RFBLC_ERASE_CFG2_FLASH:
				*RfblState = RFBLS_ERASE_CFG2_FLASH;
				break;

			case RFBLC_ERASE_ALL_FLASH:
				*RfblState = RFBLS_ERASE_ALL_FLASH;
				break;

			case RFBLC_VERSION:
				*RfblState = RFBLS_VERSION;
				break;

			case RFBLC_REBOOT_TO_CUSTOM:
				*RfblState = RFBLS_REBOOT_TO_CUSTOM;
				ApplicationAddress = (int32_t)((tOutBuffer[10] << 24) | (tOutBuffer[9] << 16) | (tOutBuffer[8] << 8) | tOutBuffer[7]); //todo:make this work
				break;

			case RFBLC_LAST:
				*RfblState = RFBLS_ERROR;
				break;

		}
		tOutBuffer[0] = 0; //clear buffer id, only need to clear first byte.

	}

}

void rfbl_execute_load_command(void) {

	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	//TODO: Add some sanity checks
	if ( (FwInfo.type == RFFW) && (FwInfo.mode == AUTO) ) { //Auto load RFFW, RaceFlight FW
		FwInfo.address = APP_ADDRESS;
	} else
	if ( (FwInfo.type == RFBU) && (FwInfo.mode == AUTO) ) { //Auto load RFBU, RaceFlight Bootloader Uploader
		FwInfo.address = APP_ADDRESS;
	} else
	if (FwInfo.mode == MANU) {
		//nothing special needed for manual mode other than sanity checks
	}

	//RFBL can only do FW or FWCF, RFBU can do ALL, FWCG, or RFFW
	if ( (FwInfo.type == RFBU) && (FwInfo.erase != RFFW) ) {
		FwInfo.erase = FWCF;
	}

	if ( (FwInfo.type == RFFW) && (FwInfo.size) ) { //Does the firmware have size? //TODO: Verify size is sane
		FwInfo.expected_packets = ceil(FwInfo.size / FwInfo.packet_size);
		rfbl_prepare_flash(); //unlock and erase flash. Then wait for data packets
	}

}

//todo: only works for F4 and F7. F1 anf F3 require different flash handling.
void rfbl_prepare_flash(void) {

	LED1_ON;
	LED2_ON;
	LED3_ON;
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

	HAL_FLASH_Unlock();

	//todo made this configurable
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = GetSector(ADDR_FLASH_SECTOR_5);
	EraseInitStruct.NbSectors = 6;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{
		//FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
		ErrorHandler();
	}

	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
}


void rfbl_write_packet(void) {

	uint32_t data32;

	for (uint32_t wordoffsetter = 0; wordoffsetter < (floor((HID_EPOUT_SIZE-1) / 4) * 4); wordoffsetter += 4) {

		data32 = (uint32_t) ( (FwInfo.data[wordoffsetter+1] << 0) | (FwInfo.data[wordoffsetter+2] << 8) | (FwInfo.data[wordoffsetter+3] << 16) | (FwInfo.data[wordoffsetter+4] << 24));

		if (HAL_FLASH_Program(TYPEPROGRAM_WORD, FwInfo.address + FwInfo.wordOffset, data32) == HAL_OK) {
			FwInfo.wordOffset = FwInfo.wordOffset + 4;
		} else {
			//FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
			ErrorHandler();
		}

	}

}


void rfbl_finish_flash(void) {

	HAL_FLASH_Lock();

}

void rfbl_parse_load_command(void) {

	uint32_t name32;
	uint8_t command_packet[HID_EPOUT_SIZE-1];

	//initialize FW struct
	FwInfo.size    			= 0; //size in bytes of the FW
	FwInfo.type    			= 0; //RFBL, RFBU, RFFW
	FwInfo.address 			= 0; //Address to place firmware.
	FwInfo.mode    			= 0; //Auto, Manu (Manual will place the FW into the location specified by address. Auto will use compiled defaults.)
	FwInfo.erase   			= 0; //All, FW, FWCF, (All only the RFBL will do) (FW will only replace the FW area) (FW + Config will erase FW area and config location for it)
	FwInfo.data_packets		= 0;
	FwInfo.expected_packets	= 0;
	FwInfo.wordOffset		= 0;
	FwInfo.packet_size		= (floor((HID_EPOUT_SIZE-1) / 4) * 4);
	FwInfo.time_last_packet	= 0;
	//todo: add micros() functions
	//FwInfo.time_last_packet	= micros();

	memset(FwInfo.data, 0, HID_EPOUT_SIZE-1); //fw

	memcpy( &command_packet[0], tOutBuffer, HID_EPOUT_SIZE-1 ); //capture outbuffer

	for (uint8_t i=0;i<(HID_EPIN_SIZE-1);i++) {

		name32 = (int32_t)( (command_packet[i] << 24) | (command_packet[i+1] << 16) | (command_packet[i+2] << 8) | (command_packet[i+3]) );

		if (name32 == FWSZ) { //fwsz, firmware size
			FwInfo.size = (int32_t)( (command_packet[i+4] << 24) | (command_packet[i+5] << 16) | (command_packet[i+6] << 8) | (command_packet[i+7]) );
			i = i+6; //name/value found, skip to the next possible location
		} else
		if (name32 == FWTP) { //fwtp, firmware type
			FwInfo.type = (int32_t)( (command_packet[i+4] << 24) | (command_packet[i+5] << 16) | (command_packet[i+6] << 8) | (command_packet[i+7]) );
			i = i+6; //name/value found, skip to the next possible location
		} else
		if (name32 == FWAD) { //fwad, firmware address
			FwInfo.address = (int32_t)( (command_packet[i+4] << 24) | (command_packet[i+5] << 16) | (command_packet[i+6] << 8) | (command_packet[i+7]) );
			i = i+6; //name/value found, skip to the next possible location
		} else
		if (name32 == FWMD) { //fwmd, firmware mode
			FwInfo.mode = (int32_t)( (command_packet[i+4] << 24) | (command_packet[i+5] << 16) | (command_packet[i+6] << 8) | (command_packet[i+7]) );
			i = i+6; //name/value found, skip to the next possible location
		}
		if (name32 == FWER) { //fwer, firmware erase type
			FwInfo.erase = (int32_t)( (command_packet[i+4] << 24) | (command_packet[i+5] << 16) | (command_packet[i+6] << 8) | (command_packet[i+7]) );
			i = i+6; //name/value found, skip to the next possible location
		}

	}

}

void rfbl_report_state (RfblState_e *RfblState)  {

	tInBuffer[0]=1;

	int i = 1;
	while (i<(HID_EPIN_SIZE-1)) {
		tInBuffer[i]=0;
		i++;
	}

	tInBuffer[0]=0x01;
	tInBuffer[1]=0x52;
	tInBuffer[2]=0x46;
	tInBuffer[3]=0x42;
	tInBuffer[4]=0x4C;
	tInBuffer[5]=0x53;
	tInBuffer[6]=*RfblState;

	if (*RfblState == RFBLS_VERSION) {
		tInBuffer[7]=RFBL_VERSION;
		tInBuffer[8]=CFG1_VERSION;
	}

	USBD_HID_SendReport (&hUsbDeviceFS, tInBuffer, HID_EPIN_SIZE);

}


uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;
  }
  else
  {
    sector = FLASH_SECTOR_11;
  }

  return sector;
}


uint32_t GetPageSize(uint32_t Address)
{
  uint32_t page_size = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
	  page_size = 0x4000;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
	  page_size = 0x4000;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
	  page_size = 0x4000;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
	  page_size = 0x4000;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
	  page_size = 0x10000;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
	  page_size = 0x20000;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
	  page_size = 0x20000;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
	  page_size = 0x20000;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
	  page_size = 0x20000;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
	  page_size = 0x20000;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
	  page_size = 0x20000;
  }
  else
  {
	  page_size = 0x20000;
  }

  return page_size;
}

void systemReset(void)
{
	__disable_irq();
	NVIC_SystemReset();
}

void systemResetToBootloader(void) {
//todo make this work on all MCUs
	*((uint32_t *)0x2001FFFC) = 0xDEADBEEF; // 128KB SRAM STM32F4XX
	__disable_irq();
	NVIC_SystemReset();
}

void systemResetToDfuBootloader(void) {
	//todo make this work on all MCUs
	*((uint32_t *)0x2001FFFC) = 0xDEADBEEF; // 128KB SRAM STM32F4XX
	NVIC_SystemReset();
}

void ErrorHandler(void)
{
    while (1) {
        LED2_ON;
        LED3_OFF;
        HAL_Delay(40);
        LED2_OFF;
        LED3_ON;
        HAL_Delay(40);
    }
}

/* RFBL description

RaceFlight Bootloader Updater (RFBU) is small and fits into a single 16 KB sector (2nd one).
RFBU is simple. It's only job is to connect via HID to upgrade the RFBL.

RaceFlight Bootloader (RFBL) is small. It fits into a single sector of 16 KB (1st sector).
RFBL is more complex and handles FW upgrade and some file management.

RFBL Config data is stored in a single 16 KB sector (3rd one).
RaceFlight Pro Firmware Config data is stored in the last 16 KB sector.
First 64 KB sector is not used. This could be used for special functions like nav data or raceing features.
Firmware starts in the first of seven 128 KB sector. We have up to 896 KB for firmware or other storage across those 7 sectors.

RFBL Commands are transferred via name value pairs for easy backwards compatibility with future versions. 32 bit name and 8 bit value.
RFBL Command Parameters are transferred via name value pairs for easy backwards compatibility with future versions. 32 bit name and 32 bit value.

Normal boot cycle:
1. STM32 boots into RFBL.
2. RFBL checks boot config and increments the boot cycle number.
3. If boot cycle number is too high RFBL continues into boot loader mode.
4. If boot cycle number is not too high, unless config is marked for bootloader the bootloader will boot directly to the firmware.
5. Firmware resets boot cycle number after a successful boot after FC is able to arm.

FW upgrade boot cycle.
A.
	1. FW will mark RFBL config file to boot into bootloader.
	2. STM32 system restart command
	3. STM32 boots into RFBL.
	4. RFBL checks boot config and increments the boot cycle number.
	5. RFBL continues bootloader start as the boot direction is marked for RFBL bootup.

B. (Optional) (Needs some more thought)
	1. STM32 boots into RFBL.
	2. RFBL checks boot config and increments the boot cycle number.
	2. RFBL checks to see if certain pins are grounded (our own BL pins).
	3. RFBL continues bootloader start as if those pins are grounded.
	4. RFBL will look for a connection for 6 seconds. If none is received it'll restart and jump to FW.

RFBU boot cycle.
	1. RFBL marks boot direction as RFBU
	2. STM32 system restart command
	3. STM32 boots into RFBL.
	4. RFBL checks boot config and increments the boot cycle number.
	5. RFBL reset firmware direction to RFBL and jumps directly to the RFBU.
	6. RFBU connects via HID and listens for firmware upgrade or restart command.


Bootloader Upgrade Command Parameters:

FwInfo.size    = 0; //size in bytes of the FW
FwInfo.type    = 0; //RFBL, RFBU, RFFW
FwInfo.address = 0; //Address to place firmware.
FwInfo.mode    = 0; //Auto, Manu (Manual will place the FW into the location specified by address. Auto will use compiled defaults.)
FwInfo.erase   = 0; //All, FW, FWCF, (All only the RFBL will do) (FW will only replace the FW area) (FW + Config will erase FW area and config location for it)

RFBL Commands:
	RFBLC_NONE, //No command.
	RFBLC_REBOOT_TO_DFU,		//Will reboot STM32 into Factory DFU mode
	RFBLC_REBOOT_TO_RFBL,		//Will reboot into RFBL
	RFBLC_REBOOT_TO_APP,		//Will reboot into App
	RFBLC_TOGGLE_LEDS,			//Will toggle all LEDs
	RFBLC_UPGRADE_FW,			//Will start FW upgrade process. Must include upgrade parameters.
	RFBLC_ERASE_CFG1_FLASH,		//Will erase RFBL config flash sector
	RFBLC_ERASE_CFG2_FLASH,		//Will erase RFFW config flash sector
	RFBLC_ERASE_ALL_FLASH,		//Will erase all flash except the RFBU and RFBL
	RFBLC_ERROR,				//Will put the device into error mode
	RFBLC_LAST,					//Last enumeration. Same as RFBLC_ERROR

RFBL States:
	RFBLS_IDLE,						//RFBL is Idle and awaiting command
	RFBLS_REBOOT_TO_DFU,			//RFBL is rebooting into DFU
	RFBLS_REBOOT_TO_RFBL,			//RFBL is rebooting into RFBL
	RFBLS_REBOOT_TO_APP,			//RFBL is rebooting into APP
	RFBLS_PREPARING_FOR_UPDATE,		//RFBL is preparing for update
	RFBLS_LOAD_TO_BL,				//RFBL is upgrading firmware from PC
	RFBLS_LOAD_FROM_BL,				//RFBL is reading firmware to PC
	RFBLS_TOGGLE_LEDS,				//RFBL is toggling the LEDs
	RFBLS_ERASE_CFG1_FLASH,			//RFBL is erasing config1 flash
	RFBLS_ERASE_CFG2_FLASH,			//RFBL is erasing config2 flash
	RFBLS_ERASE_ALL_FLASH,			//RFBL is erasing chip flash
	RFBLS_ERROR,					//RFBL is in error mode
	RFBLS_LAST,						//Last enumeration. Same as RFBLS_ERROR


 */
