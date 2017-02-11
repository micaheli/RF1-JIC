#pragma once

extern volatile uint32_t systemUsTicks;




extern uint32_t InlineMillis(void);
extern uint32_t Micros(void);

extern void DelayMs(uint32_t mSec);
extern void delayUs(uint32_t uSec);
extern uint32_t rtc_read_backup_reg(uint32_t BackupRegister);
extern void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data);

extern void InitUsb(void);

extern void inlineDigitalHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
extern void inlineDigitalLo(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
extern int inlineIsPinStatusHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

extern void InlineUpdateMillisClock (void);

extern void VectorIrqInit(uint32_t address);

extern void SystemReset(void);
extern void SystemResetToDfuBootloader(void);

extern uint32_t GetExtinFromPin(uint16_t GPIO_Pin);
extern uint32_t GetExtiCallbackFromPin(uint16_t GPIO_Pin);
extern uint32_t GetDmaCallbackFromDmaStream(uint32_t dmaEnum);
extern void InitializeGpio(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t on);
extern void InitializeGpioInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

extern void ErrorHandler(uint32_t error);
extern void InitFlight(void);

extern uint32_t errorMask;

enum {
	GYRO_INIT_FAILIURE                =  (1 << 0),
	SERIAL_INIT_FAILIURE              =  (1 << 1),
	DMA_INIT_FAILIURE                 =  (1 << 2),
	FLASH_INIT_FAILIURE               =  (1 << 3),
	WS2812_LED_INIT_FAILIURE          =  (1 << 4),
	FLASH_SPI_INIT_FAILIURE           =  (1 << 5),
	GYRO_SETUP_COMMUNICATION_FAILIURE =  (1 << 6),
	SERIAL_HALF_DUPLEX_INIT_FAILURE   =  (1 << 7),
	SERIAL_INIT_FAILURE               =  (1 << 8),
	MSP_DMA_GYRO_RX_INIT_FAILIURE     =  (1 << 9),
	MSP_DMA_GYRO_TX_INIT_FAILIURE     =  (1 << 10),
	MSP_DMA_SPI_RX_INIT_FAILIURE      =  (1 << 11),
	MSP_DMA_SPI_TX_INIT_FAILIURE      =  (1 << 12),
	NOT_USED1					      =  (1 << 13),
	NOT_USED2					      =  (1 << 14),
	NOT_USED3					      =  (1 << 15),
	NOT_USED4					      =  (1 << 16),
	HARD_FAULT                        =  (1 << 17),
	MEM_FAULT                         =  (1 << 18),
	BUS_FAULT                         =  (1 << 19),
	USAGE_FAULT                       =  (1 << 20),
	GYRO_SPI_INIT_FAILIURE            =  (1 << 21),
	TIMER_INPUT_INIT_FAILIURE         =  (1 << 22),
	ADC_INIT_FAILIURE                 =  (1 << 23),
	ADC_DMA_INIT_FAILIURE             =  (1 << 24),
};

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

#define RECOVERY_VERSION		16
#define RFBL_VERSION			16
#define CFG1_VERSION			16
#define RCVR_TAG				"RCVRVERSION#00160016" //must be 20 bytes max
#define RFBL_TAG				"RFBLVERSION#00160016" //must be 20 bytes max

enum {
	APP_ADDRESS						= 0x08008000,
	BOOT_TO_RECOVERY_COMMAND		= 0xCA77F154,
	BOOT_TO_RFBL_COMMAND			= 0xDEADFEAD,
	BOOT_TO_APP_COMMAND				= 0xB01DFEED,
	BOOT_TO_APP_AFTER_SPEK_COMMAND	= 0xCA7F157,
	BOOT_TO_APP_AFTER_RECV_COMMAND	= 0xDEADF157,
	BOOT_TO_DFU_COMMAND				= 0xB01DCA77,
	BOOT_TO_ADDRESS					= 0xDEFEC7ED,
	BOOT_TO_SPEKTRUM5				= 0x0005B14D,
	BOOT_TO_SPEKTRUM9				= 0x0009B14D,
	FAST_BOOT						= 0xFA57B007,
	MAGIC_F1EAF00D					= 0xF1EAF00D,
	MAGIC_A0DDBA11					= 0xA0DDBA11,
};

enum {
	RFBL_BKR_RFBL_VERSION_REG		= RTC_BKP_DR1,
	RFBL_BKR_CFG1_VERSION_REG		= RTC_BKP_DR2,
	RFBL_BKR_BOOT_DIRECTION_REG		= RTC_BKP_DR3,
	RFBL_BKR_BOOT_CYCLES_REG		= RTC_BKP_DR4,
	RFBL_BKR_BOOT_ADDRESSS_REG		= RTC_BKP_DR5,
	RFBL_BKR_REBOOT_PENDING_REG		= RTC_BKP_DR6,
	FC_STATUS_REG					= RTC_BKP_DR7,
};

#define RFBL1	0x631e47d9
#define RFBL2	0x8ef26ec3
#define RFBL3	0x3516864e
#define RFBL4	0x461085c1
#define RFBLM1	0xfade43f4
#define RFBLM2	0xa62fe81a
#define RFBLMR1	0xF443DEFA
#define RFBLMR2	0x1AE82FA6

#define FC_STATUS_INFLIGHT 100
#define FC_STATUS_IDLE     101
#define FC_STATUS_CONFIG   102
#define FC_STATUS_STARTUP  103

