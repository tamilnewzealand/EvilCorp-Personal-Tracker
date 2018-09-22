#ifndef DRIVERS_I2C_H_
#define DRIVERS_I2C_H_

#include "em_emu.h"
#include "em_cmu.h"
#include "em_i2c.h"

#define I2C_PORT gpioPortC
#define I2C_SDA_PIN 10
#define I2C_SCL_PIN 11

void initI2C();
bool i2c_read_register(uint8_t addr, uint8_t reg, uint16_t *val);

#endif /* DRIVERS_I2C_H_ */
