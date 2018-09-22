#include "i2c.h"


//----------------------------------------------------------------------------
//	Init operations
//----------------------------------------------------------------------------


void initI2C() {
	/* Enable peripheral clock */
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_I2C0, true);

	GPIO_PinModeSet(I2C_PORT, I2C_SDA_PIN, gpioModeWiredAnd, 1);
	GPIO_PinModeSet(I2C_PORT, I2C_SCL_PIN, gpioModeWiredAnd, 1);

	I2C_Init_TypeDef init = I2C_INIT_DEFAULT;

	I2C0->ROUTELOC0 = I2C_ROUTELOC0_SDALOC_LOC15 | I2C_ROUTELOC0_SCLLOC_LOC15;
	I2C0->ROUTEPEN = I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;

	for (int i=0; i<9; i++) {
		GPIO_PinOutSet(I2C_PORT, I2C_SCL_PIN);
		GPIO_PinOutClear(I2C_PORT, I2C_SCL_PIN);
	}
	I2C_Init(I2C0, &init);
}


//----------------------------------------------------------------------------
//	Legacy operations
//----------------------------------------------------------------------------


bool i2c_read_register(uint8_t addr, uint8_t reg, uint16_t *val) {
	uint8_t tx_buffer[1];
	uint8_t rx_buffer[3];

	tx_buffer[0] = reg;

	I2C_TransferSeq_TypeDef sequence;
	sequence.addr = addr << 1;
	sequence.flags = I2C_FLAG_WRITE_READ;
	sequence.buf[0].data = tx_buffer;
	sequence.buf[0].len = 1;
	sequence.buf[1].data = rx_buffer;
	sequence.buf[1].len = 3;

	I2C_TransferReturn_TypeDef result;
	result = I2C_TransferInit(I2C0, &sequence);
	while (result == i2cTransferInProgress) {
		result = I2C_Transfer(I2C0);
	}

	if (result == i2cTransferDone) {
		(*val) = ((uint16_t)rx_buffer[0] | ((uint16_t)rx_buffer[1] << 8));
		return true;
	} else {
		return false;
	}
}


//----------------------------------------------------------------------------
//	WRITE operations
//----------------------------------------------------------------------------


bool WriteU8(uint8_t uDevAddr, uint8_t uRegAddr, uint8_t uValue){
	uint8_t data[2] = { uRegAddr, uValue};
	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	/* Initializing I2C transfer */
	i2cTransfer.addr = uDevAddr << 1;
	/* Master write */
	i2cTransfer.flags = I2C_FLAG_WRITE;

	/* Transmit buffer, 2 bytes to send */
	i2cTransfer.buf[0].data = data;
	i2cTransfer.buf[0].len = 2;

	i2cTransfer.buf[1].data = 0UL;
	i2cTransfer.buf[1].len = 0;

	I2C_TransferReturn_TypeDef result;
	result = I2C_TransferInit(I2C0, &i2cTransfer);
	while (result == i2cTransferInProgress) result = I2C_Transfer(I2C0);

	if (result == i2cTransferDone) return true;
	return false;
}


//----------------------------------------------------------------------------
//	READ operations
//----------------------------------------------------------------------------


bool Read(uint8_t uDevAddr, uint8_t uRegAddr, void *pBuffer, uint8_t uLenBytes)
{
	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	/* Initializing I2C transfer */
	i2cTransfer.addr = uDevAddr << 1;
	/* Master write */
	i2cTransfer.flags = I2C_FLAG_WRITE_READ;

	/* Transmit buffer, no data to send */
	i2cTransfer.buf[0].data = &uRegAddr;
	i2cTransfer.buf[0].len = 1;

	/* Receive buffer, multiple bytes to receive */
	i2cTransfer.buf[1].data = (uint8_t  *)pBuffer;
	i2cTransfer.buf[1].len = uLenBytes;

	I2C_TransferReturn_TypeDef result;
	result = I2C_TransferInit(I2C0, &i2cTransfer);
	while (result == i2cTransferInProgress) result = I2C_Transfer(I2C0);

	if (result == i2cTransferDone) return true;
	return false;
}

uint8_t ReadU8(uint8_t uDevAddr, uint8_t uRegAddr)
{
	uint8_t data = 0;

	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	/* Initializing I2C transfer */
	i2cTransfer.addr = uDevAddr << 1;
	/* Master write */
	i2cTransfer.flags = I2C_FLAG_WRITE_READ;

	/* Transmit buffer, no data to send */
	i2cTransfer.buf[0].data = &uRegAddr;
	i2cTransfer.buf[0].len = 1;

	/* Receive buffer, one byte to receive */
	i2cTransfer.buf[1].data = &data;
	i2cTransfer.buf[1].len = 1;

	I2C_TransferReturn_TypeDef result;
	result = I2C_TransferInit(I2C0, &i2cTransfer);
	while (result == i2cTransferInProgress) result = I2C_Transfer(I2C0);

	if (result == i2cTransferDone) return data;
	else return 0;
}

int8_t ReadS8(uint8_t uDevAddr, uint8_t uRegAddr)
{
	return (int8_t)ReadU8(uDevAddr, uRegAddr);
}

uint16_t ReadU16(uint8_t uDevAddr, uint8_t uRegAddr)
{
	uint16_t data = 0;

	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	/* Initializing I2C transfer */
	i2cTransfer.addr = uDevAddr << 1;
	/* Master write */
	i2cTransfer.flags = I2C_FLAG_WRITE_READ;

	/* Transmit buffer, no data to send */
	i2cTransfer.buf[0].data = &uRegAddr;
	i2cTransfer.buf[0].len = 1;

	/* Receive buffer, two bytes to receive */
	i2cTransfer.buf[1].data = (uint8_t *)&data;
	i2cTransfer.buf[1].len = 2;

	I2C_TransferReturn_TypeDef result;
	result = I2C_TransferInit(I2C0, &i2cTransfer);
	while (result == i2cTransferInProgress) result = I2C_Transfer(I2C0);

	if (result == i2cTransferDone) return data;
	else return 0;
}

int16_t ReadS16(uint8_t uDevAddr, uint8_t uRegAddr)
{
	return (int16_t)ReadU16(uDevAddr, uRegAddr);
}

uint16_t ReadU16LE(uint8_t uDevAddr, uint8_t uRegAddr)
{
	uint16_t temp = ReadU16(uDevAddr, uRegAddr);
	return (temp >> 8) | (temp << 8);
}

int16_t ReadS16LE(uint8_t uDevAddr, uint8_t uRegAddr)
{
	return (int16_t)ReadU16LE(uDevAddr, uRegAddr);
}
