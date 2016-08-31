#pragma once

#include <stdbool.h>
#include "usbd_hid.h" //needed for HID_EPIN_SIZE delcaration

#define RFBU					0x52464255
#define RFBL					0x5246424C
#define RFFW					0x52464657
#define MANU					0x4D414E55
#define AUTO					0x4155544F
#define FWCF					0x46574346
#define FWSZ					0x4657535A
#define FWTP					0x46575450
#define FWAD					0x46574144
#define FWMD					0x46574D44
#define FWER					0x46574552
#define RFPM					0x5246504D
#define PL						0x504C

#define RFBL_VERSION			0x06
#define CFG1_VERSION			0x06
#define RFBL_TAG	"RFBLVERSION#00060006" //must be 20 bytes max

#define APP_ADDRESS				0x08020000
#define BOOT_TO_RFBL_COMMAND	0xDEADFEAD
#define BOOT_TO_APP_COMMAND		0xB01DFEED
#define BOOT_TO_DFU_COMMAND		0xB01DCA77
#define BOOT_TO_ADDRESS			0xDEFEC7ED
#define BOOT_TO_SPEKTRUM5		0x0005B14D
#define BOOT_TO_SPEKTRUM9		0x0009B14D
#define FAST_BOOT				0xFA57B007
#define MAGIC_F1EAF00D			0xF1EAF00D
#define MAGIC_A0DDBA11			0xA0DDBA11

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */
#define ADDR_FLASH_SECTOR_12    ((uint32_t)0x08100000) /* Base @ of Sector 11, 128 Kbytes */

#define APPLICATION_ADDRESS			ADDR_FLASH_SECTOR_5
#define FLASH_END_AFTER_ADDRESS		ADDR_FLASH_SECTOR_12

#define RFBL_ADDR_START			ADDR_FLASH_SECTOR_0
#define RFBL_ADDR_END			ADDR_FLASH_SECTOR_1

#define FLASH_CFG1_ADDR_START	ADDR_FLASH_SECTOR_1
#define FLASH_CFG1_ADDR_END		ADDR_FLASH_SECTOR_2

#define FLASH_PROG_ADDR_START   ADDR_FLASH_SECTOR_5
#define FLASH_PROG_ADDR_END     ADDR_FLASH_SECTOR_11

#define RFBL_BKR_RFBL_VERSION_REG		RTC_BKP_DR1
#define RFBL_BKR_CFG1_VERSION_REG		RTC_BKP_DR2
#define RFBL_BKR_BOOT_DIRECTION_REG		RTC_BKP_DR3
#define RFBL_BKR_BOOT_CYCLES_REG		RTC_BKP_DR4
#define RFBL_BKR_BOOT_ADDRESSS_REG		RTC_BKP_DR5

#define RFBL1	0x631e47d9
#define RFBL2	0x8ef26ec3
#define RFBL3	0x3516864e
#define RFBL4	0x461085c1

typedef enum {
	RFBLS_IDLE,
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

void systemReset(void);
void systemResetToBootloader(void);
void systemResetToDfuBootloader(void);

uint32_t GetSector(uint32_t Address);
uint32_t GetPageSize(uint32_t Address);

typedef void (*pFunction)(void);
pFunction JumpToApplication;
uint32_t jumpAddress;
