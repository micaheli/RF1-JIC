#include "includes.h"


static uint8_t SmartPortGetByte(uint8_t inByte, uint16_t *crcp);


static uint8_t SmartPortGetByte(uint8_t inByte, uint16_t *crcp) {

	uint8_t outByte;
	uint16_t crc;

	outByte = inByte;

    // smart port escape sequence
    if (inByte == 0x7D || inByte == 0x7E) {
    	outByte = BYTESTUFF;
    	inByte ^= 0x20;
    }

    if (crcp == NULL)
    	return (outByte);

    crc = *crcp;
    crc += inByte;
    crc += crc >> 8;
    crc &= 0x00FF;
    *crcp = crc;

    return (outByte);
}

void SmartPortSendPackage(uint32_t id, int32_t val) {

	uint8_t sPortPacket[8];
	uint16_t crc = 0;

	//Frame Header
	sPortPacket[0] = SmartPortGetByte(SPORT_FRAME_HEADER, &crc);

	//Data ID
	sPortPacket[1] = SmartPortGetByte( (uint8_t)( (id >> 0) & 0xff), &crc);
	sPortPacket[2] = SmartPortGetByte( (uint8_t)( (id >> 8) & 0xff), &crc);

	//Data
	sPortPacket[3] = SmartPortGetByte( (uint8_t)( (val >> 0)  & 0xff), &crc);
	sPortPacket[4] = SmartPortGetByte( (uint8_t)( (val >> 8)  & 0xff), &crc);
	sPortPacket[5] = SmartPortGetByte( (uint8_t)( (val >> 16) & 0xff), &crc);
	sPortPacket[6] = SmartPortGetByte( (uint8_t)( (val >> 24) & 0xff), &crc);

	//CRC
	sPortPacket[7] = SmartPortGetByte( (uint8_t)( (0xFF - (uint8_t)(crc & 0xff) ) ), NULL);

	(void)(sPortPacket);

//	OutputSerialDmaByte(sPortPacket, 8, board.motors[7], 0, 1); //send sPortPacket, which is 8 bytes, onto actuator 7, lsb, with serial frame, lo pulse is a one

//	OutputSerialDmaByte(uint8_t *serialOutBuffer, uint32_t outputLength, motor_type actuator, uint32_t msb)

}
