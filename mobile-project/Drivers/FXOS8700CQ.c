#include "FXOS8700CQ.h"

uint8_t accelFSR;
uint8_t accelODR;
uint8_t magOSR;

bool initFXOS8700CQ()
{
	accelFSR = AFS_2g;     	// Set the scale below either 2, 4 or 8
	accelODR = AODR_200HZ; 	// In hybrid mode, accel/mag data sample rates are half of this value
	magOSR = MOSR_5;     	// Choose magnetometer oversample rate

	// initialize I2C if not already
    if (!getI2CStatus()) initI2C();

    // check device returns correct who am i
	if(ReadU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_WHO_AM_I)!=0xC7) return false;

	standbyFXOS8700CQ(true);  // Must be in standby to change registers

	WriteU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_CTRL_REG1, 0);
	WriteU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_M_CTRL_REG1, 0x1F);
	WriteU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_M_CTRL_REG2, 0x20);
	WriteU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_XYZ_DATA_CFG, 0x01);
	WriteU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_CTRL_REG1, 0x0D);

	WriteU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_XYZ_DATA_CFG, accelFSR);  // Choose the full scale range to 2, 4, or 8 g.

	if (accelODR <= 7) WriteU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_CTRL_REG1, ReadU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_CTRL_REG1) | (accelODR << 3));
	
    // Configure the magnetometer
	WriteU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_M_CTRL_REG1, 0x80 | magOSR << 2 | 0x03); // Set auto-calibration, set oversampling, enable hybrid mode

	standbyFXOS8700CQ(false);  // Set to active to start reading

	return true;
}

bool standbyFXOS8700CQ(bool bIsStandby)
{
	uint8_t b = ReadU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_CTRL_REG1);
	if(bIsStandby)	b = b & ~(0x01);
			else 	b = b | (0x01);
	return WriteU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_CTRL_REG1, b);
}

bool readAccel(AccelerometerData_t *pData)
{
	uint8_t rawData[6] = {0,0,0,0,0,0};

	if(!Read(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_OUT_X_MSB, &rawData[0], 6 ))	return false;
    
	pData->x = ((int16_t) rawData[0] << 8 | rawData[1]) >> 2;
	pData->y = ((int16_t) rawData[2] << 8 | rawData[3]) >> 2;
	pData->z = ((int16_t) rawData[4] << 8 | rawData[5]) >> 2;
	
    return true;
}

bool readMagn(MagnetometerData_t *pData)
{
	uint8_t rawData[6] = {0,0,0,0,0,0};

	if(!Read(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_M_OUT_X_MSB, &rawData[0], 6)) return false;

	pData->x = ((int16_t) rawData[0] << 8 | rawData[1]);
	pData->y = ((int16_t) rawData[2] << 8 | rawData[3]);
	pData->z = ((int16_t) rawData[4] << 8 | rawData[5]);
	return true;
}

uint8_t readTempFXOS8700CQ(void)
{
	return ReadU8(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_TEMP);
}