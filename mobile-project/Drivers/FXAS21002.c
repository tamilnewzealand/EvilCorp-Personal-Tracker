#include "FXAS21002.h"

bool initFXAS21002()
{
    // initialize I2C if not already
    if (!getI2CStatus()) initI2C();

    // check device returns correct who am i
	if(ReadU8(FXAS21002_SLAVE_ADDR, FXAS21002_WHO_AM_I)!=0xD7) return false;

    // High-pass filter disabled, +/-250 dps range -> 7.8125 mdps/LSB = 128 LSB/dps
    WriteU8(FXAS21002_SLAVE_ADDR, FXAS21002_CTRL_REG0, 0x03);
    // Enable DRDY interrupt, routed to INT1 - PTA5, push-pull, active low interrupt
    WriteU8(FXAS21002_SLAVE_ADDR, FXAS21002_CTRL_REG2, 0x0C);
    // ODR = 25Hz, Active mode
    WriteU8(FXAS21002_SLAVE_ADDR, FXAS21002_CTRL_REG1, 0x16);

    return true;
}

bool readGryo(GyroscopeData_t *pData)
{
	uint8_t rawData[6] = {0,0,0,0,0,0};

	if(!Read(FXAS21002_SLAVE_ADDR, FXAS21002_OUT_X_MSB, &rawData[0], 6 ))	return false;
    
	pData->x = ((int16_t) rawData[0] << 8 | rawData[1]);
	pData->y = ((int16_t) rawData[2] << 8 | rawData[3]);
	pData->z = ((int16_t) rawData[4] << 8 | rawData[5]);
	
    return true;
}

int8_t readTempFXAS21002(void)
{
	return ReadS8(FXAS21002_SLAVE_ADDR, FXAS21002_TEMP);
}
