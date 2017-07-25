#pragma once

#define CRSF_SYNC_BYTE 0xC8

enum
{
	CRSF_TYPE_GPS       = 0x02,
	CRSF_TYPE_BATT      = 0x08,
	CRSF_TYPE_HB        = 0x0B,
	CRSF_TYPE_LINK_STAT = 0x14,
	CRSF_TYPE_RC        = 0x16,
	CRSF_TYPE_ATTITUDE  = 0x1E,
	CRSF_TYPE_FLIGHT_MD = 0x21,
};

extern uint8_t CrsfCrc8(uint8_t * ptr, uint8_t len);