#pragma once

extern uint32_t systemUsTicks;

uint32_t InlineMillis(void);
uint32_t Micros(void);

void DelayMs(uint32_t mSec);
void delayUs(uint32_t uSec);
uint32_t rtc_read_backup_reg(uint32_t BackupRegister);
void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data);

void InitUsb(void);

void inlineDigitalHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void inlineDigitalLo(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
bool inlineIsPinStatusHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void InlineUpdateMillisClock (void);

void VectorIrqInit(uint32_t address);

void SystemReset(void);
void SystemResetToDfuBootloader(void);





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

#define RECOVERY_VERSION		01
#define RFBL_VERSION			13
#define CFG1_VERSION			13
#define RFBL_TAG				"RFBLVERSION#00130013" //must be 20 bytes max

#define APP_ADDRESS						0x08020000
#define BOOT_TO_RECOVERY_COMMAND		0xCA77F154
#define BOOT_TO_RFBL_COMMAND			0xDEADFEAD
#define BOOT_TO_APP_COMMAND				0xB01DFEED
#define BOOT_TO_APP_AFTER_SPEK_COMMAND	0xCA7F157
#define BOOT_TO_DFU_COMMAND				0xB01DCA77
#define BOOT_TO_ADDRESS					0xDEFEC7ED
#define BOOT_TO_SPEKTRUM5				0x0005B14D
#define BOOT_TO_SPEKTRUM9				0x0009B14D
#define FAST_BOOT						0xFA57B007
#define MAGIC_F1EAF00D					0xF1EAF00D
#define MAGIC_A0DDBA11					0xA0DDBA11

#define RFBL_BKR_RFBL_VERSION_REG		RTC_BKP_DR1
#define RFBL_BKR_CFG1_VERSION_REG		RTC_BKP_DR2
#define RFBL_BKR_BOOT_DIRECTION_REG		RTC_BKP_DR3
#define RFBL_BKR_BOOT_CYCLES_REG		RTC_BKP_DR4
#define RFBL_BKR_BOOT_ADDRESSS_REG		RTC_BKP_DR5
#define RFBL_BKR_REBOOT_PENDING_REG		RTC_BKP_DR6

#define RFBL1	0x631e47d9
#define RFBL2	0x8ef26ec3
#define RFBL3	0x3516864e
#define RFBL4	0x461085c1
