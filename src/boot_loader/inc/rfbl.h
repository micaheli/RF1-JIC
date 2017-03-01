#pragma once

#include <stdbool.h>
#include "usbd_hid.h" //needed for HID_EPIN_SIZE delcaration

typedef enum {
	RFBLS_NONE,
	RFBLS_REBOOT_TO_DFU,
	RFBLS_REBOOT_TO_RFBL,
	RFBLS_REBOOT_TO_APP,
	RFBLS_REBOOT_TO_CUSTOM,
	RFBLS_PREPARING_FOR_UPDATE,
	RFBLS_AWAITING_FW_DATA,
	RFBLS_WRITE_FW_DATA,
	RFBLS_DONE_UPGRADING,
	RFBLS_LOAD_TO_BL,
	RFBLS_LOAD_FROM_BL,
	RFBLS_TOGGLE_LEDS,
	RFBLS_ERASE_CFG1_FLASH,
	RFBLS_ERASE_CFG2_FLASH,
	RFBLS_ERASE_ALL_FLASH,
	RFBLS_BOOT_TO_APP,
	RFBLS_VERSION,
	RFBLS_IDLE,
	RFBLS_ERROR,
	RFBLS_LAST,
} RfblState_e;

typedef enum {
	RFBLC_NONE,
	RFBLC_REBOOT_TO_DFU,
	RFBLC_REBOOT_TO_RFBL,
	RFBLC_REBOOT_TO_APP,
	RFBLC_TOGGLE_LEDS,
	RFBLC_UPGRADE_FW,
	RFBLC_ERASE_CFG1_FLASH,
	RFBLC_ERASE_CFG2_FLASH,
	RFBLC_ERASE_ALL_FLASH,
	RFBLC_BOOT_TO_APP,
	RFBLC_VERSION,
	RFBLC_REBOOT_TO_CUSTOM,
	RFBLC_ERROR,
	RFBLC_LAST,
} RfblCommand_e;

// EEPROM variables for the bootloader.
typedef struct cfg1_t {
	uint8_t cfg1_version;		//256 version numbers to go!
	uint8_t rfbl_version;		//256 version numbers to go!
	uint16_t size;				//good for 64 KB. Plenty big.
	uint32_t magic_f1eaf00d;	//incase flash in magic numbers.
	uint32_t boot_direction;	//boot to app or RFBL?
	uint32_t boot_cycles;		//How many times have we rebooted?
	uint32_t reboot_address;	//Reboot to address other than defined app address?
	uint32_t magic_a0ddba11;	//incase flash in magic numbers.
	uint8_t czechsum;			//Do we have enough korunas?
} cfg1_t;

typedef struct FwInfo_t {
	uint32_t size;
	uint32_t type;
	uint32_t address;
	uint32_t mode;
	uint32_t erase;
	uint32_t wordOffset;
	uint32_t data_packets;
	uint32_t expected_packets;
	uint32_t packet_size;
	uint32_t time_last_packet;
	uint32_t skipTo;
	uint8_t data[HID_EPIN_SIZE-1];
} FwInfo_t;

/* Private define ------------------------------------------------------------*/
#define MAX_FW_PACKET_WAIT_TIME	5000000	//wait 0.5 seconds

extern RfblCommand_e RfblCommand;
extern RfblState_e   RfblState;
extern uint32_t      ledTime;

void startupBlink (uint16_t blinks, uint32_t delay);
void check_rfbl_command(RfblCommand_e *RfblCommand, RfblState_e *RfblState);
void rfbl_report_state (RfblState_e *RfblState);
void boot_to_app (void);
uint32_t checkOldConfigDirection (uint32_t bootDirection, uint32_t bootCycles);

void rfbl_parse_load_command(void);
void rfbl_execute_load_command(void);
void rfbl_prepare_flash(void);
void rfbl_write_packet(void);
void rfbl_finish_flash(void);
void errorBlink(void);
void erase_all_flash (void);
void ZeroActuators(uint32_t delayUs);
