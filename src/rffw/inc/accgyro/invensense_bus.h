#pragma once

// all gyroscopes should define this function which initializes all requesite
// hardware resources
bool accgyroInit(void);

// functions used to read and write to hardware
bool accgyroWriteRegister(uint8_t reg, uint8_t data);
bool accgyroVerifyWriteRegister(uint8_t reg, uint8_t data);
bool accgyroReadRegister(uint8_t reg, uint8_t length, uint8_t *data);
bool accgyroSlowReadRegister(uint8_t reg, uint8_t length, uint8_t *data);
