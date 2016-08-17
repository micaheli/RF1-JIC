#pragma once

extern bool skipGyro;

// all gyroscopes should define this function which initializes all requesite
// hardware resources
bool accgyroInit(loopCtrl_e gyroLoop);

// functions used to read and write to hardware

bool accgyroWriteData(uint8_t *data, uint8_t length);
bool accgyroWriteRegister(uint8_t reg, uint8_t data);
bool accgyroVerifyWriteRegister(uint8_t reg, uint8_t data);

bool accgyroReadData(uint8_t reg, uint8_t *data, uint8_t length);
bool accgyroSlowReadData(uint8_t reg, uint8_t *data, uint8_t length);

bool accgyroDMAReadWriteData(uint8_t *txData, uint8_t *rxData, uint8_t length);
