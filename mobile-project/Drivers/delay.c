#include "includes.h"

void delay_us(uint32_t uSec)
{
	volatile uint32_t v = 0;
	//	delay coefficients for 48MHz HF clock
	uSec = (uSec / 1000) * 5328 ;
	while (uSec--) v = v+1;
}

void delay_ms(uint32_t mSec)
{
	delay_us(mSec * 1000);
}