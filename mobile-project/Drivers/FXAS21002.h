#ifndef DRIVERS_FXAS21002_H_
#define DRIVERS_FXAS21002_H_

#include "i2c.h"

#define FXAS21002_SLAVE_ADDR 0x20

#define FXAS21002_STATUS		0x00
#define FXAS21002_OUT_X_MSB	    0x01
#define FXAS21002_OUT_X_LSB	    0x02
#define FXAS21002_OUT_Y_MSB	    0x03
#define FXAS21002_OUT_Y_LSB	    0x04
#define FXAS21002_OUT_Z_MSB	    0x05
#define FXAS21002_OUT_Z_LSB	    0x06
#define FXAS21002_DR_STATUS	    0x07
#define FXAS21002_F_STATUS	    0x08
#define FXAS21002_F_SETUP		0x09
#define FXAS21002_F_EVENT		0x0A
#define FXAS21002_INT_SRC_FLAG	0x0B
#define FXAS21002_WHO_AM_I	    0x0C
#define FXAS21002_CTRL_REG0	    0x0D
#define FXAS21002_RT_CFG		0x0E
#define FXAS21002_RT_SRC		0x0F
#define FXAS21002_RT_THS		0x10
#define FXAS21002_RT_COUNT	    0x11
#define FXAS21002_TEMP		    0x12
#define FXAS21002_CTRL_REG1	    0x13
#define FXAS21002_CTRL_REG2	    0x14
#define FXAS21002_CTRL_REG3	    0x15

#define SENSITIVITY_2000		16
#define SENSITIVITY_1000		32
#define SENSITIVITY_500		  	64
#define SENSITIVITY_250		  	128

typedef struct
{
    int16_t	x;
    int16_t	y;
    int16_t	z;
} GyroscopeData_t;

bool initFXAS21002();
bool readGryo(GyroscopeData_t *pData);
int8_t readTempFXAS21002(void);

#endif /* DRIVERS_FXAS21002_H_ */
