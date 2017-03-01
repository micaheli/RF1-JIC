#pragma once

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
} rfbl_state_enum;

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
} rfbl_command_enum;

typedef struct {
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
} firmware_info;

/* Private define ------------------------------------------------------------*/
#define MAX_FW_PACKET_WAIT_TIME	5000000	//wait 0.5 seconds


typedef void (*pFunction)(void);
pFunction JumpToApplication;
uint32_t jumpAddress;
extern uint32_t bootloaderAddress;
extern uint32_t applicationAddress;
extern uint32_t rfblLedSpeed1;
extern uint32_t rfblLedSpeed2;


extern void InitRfbl(void);
extern void CheckRfblState(void);
extern void RfblUpdateLed(uint32_t heartbeatMs, uint32_t heartbeatMsHalf);
