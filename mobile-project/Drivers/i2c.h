#ifndef DRIVERS_I2C_H_
#define DRIVERS_I2C_H_

#include "includes.h"

#define I2C_PORT gpioPortC
#define I2C_SDA_PIN 10
#define I2C_SCL_PIN 11

void initI2C();
bool i2c_read_register(uint8_t addr, uint8_t reg, uint16_t *val);

bool WriteU8(uint8_t uDevAddr, uint8_t uRegAddr, uint8_t uValue);

uint8_t ReadU8(uint8_t uDevAddr, uint8_t uRegAddr);
int8_t ReadS8(uint8_t uDevAddr, uint8_t uRegAddr);

bool Read(uint8_t uDevAddr, uint8_t uRegAddr, void *pBuffer, uint8_t uLenBytes);

uint16_t ReadU16(uint8_t uDevAddr, uint8_t uRegAddr);
int16_t ReadS16(uint8_t uDevAddr, uint8_t uRegAddr);
uint16_t ReadU16LE(uint8_t uDevAddr, uint8_t uRegAddr);
int16_t ReadS16LE(uint8_t uDevAddr, uint8_t uRegAddr);

#endif /* DRIVERS_I2C_H_ */
