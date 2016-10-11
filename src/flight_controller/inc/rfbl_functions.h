#pragma once

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
#define RFBL_VERSION			13
#define CFG1_VERSION			13
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

//only for F4
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

#define APPLICATION_ADDRESS		ADDR_FLASH_SECTOR_5
#define RFBL_ADDR_START			ADDR_FLASH_SECTOR_0
#define RFBL_ADDR_END			ADDR_FLASH_SECTOR_0
#define FLASH_CFG1_ADDR_START	ADDR_FLASH_SECTOR_1
#define FLASH_CFG1_ADDR_END		ADDR_FLASH_SECTOR_1
#define FLASH_PROG_ADDR_START   ADDR_FLASH_SECTOR_5
#define FLASH_PROG_ADDR_END     ADDR_FLASH_SECTOR_11

#define RFBL_BKR_RFBL_VERSION_REG		RTC_BKP_DR1
#define RFBL_BKR_CFG1_VERSION_REG		RTC_BKP_DR2
#define RFBL_BKR_BOOT_DIRECTION_REG		RTC_BKP_DR3
#define RFBL_BKR_BOOT_CYCLES_REG		RTC_BKP_DR4
#define RFBL_BKR_BOOT_ADDRESSS_REG		RTC_BKP_DR5
#define FC_STATUS_REG					RTC_BKP_DR6

enum{FC_STATUS_INFLIGHT=100,FC_STATUS_IDLE,FC_STATUS_CONFIG,};

#define RFBL1	0x631e47d9
#define RFBL2	0x8ef26ec3
#define RFBL3	0x3516864e
#define RFBL4	0x461085c1
#define RFBLM1	0xfade43f4
#define RFBLM2	0xa62fe81a
#define RFBLMR1	0xF443DEFA
#define RFBLMR2	0x1AE82FA6

uint32_t rfblVersion;
uint32_t cfg1Version;
uint32_t rebootAddress;
uint32_t bootCycles;
uint32_t bootDirection;

void WriteRfblBkRegs (void);
void ReadRfblBkRegs (void);

void upgradeRfbl(void);
void eraseRfbl(uint32_t firmwareSize);
void HandleRfbl(void);
