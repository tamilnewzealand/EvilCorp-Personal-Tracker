#ifndef ALGORITHMS_TRILATERATION_H_
#define ALGORITHMS_TRILATERATION_H_

#include <math.h>
#include "Drivers/includes.h"

uint8 trilaterate3(uint16 *P1, uint16 *P2, uint16 *P3, float *L, uint16 *posi);
uint8 trilaterate4(uint16 *P1, uint16 *P2, uint16 *P3, uint16 *P4, float *L, uint16 *posi);
uint8 trilaterate4_2(uint16 *P1, uint16 *P2, uint16 *P3, uint16 *P4, float *L, uint16 *posi);
uint8 trilaterate4_3(uint16 *P1, uint16 *P2, uint16 *P3, uint16 *P4, float *L, uint16 *posi);

#endif /* ALGORITHMS_TRILATERATION_H_ */
