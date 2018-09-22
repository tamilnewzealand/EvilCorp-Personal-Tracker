#ifndef DRIVERS_INCLUDES_H_
#define DRIVERS_INCLUDES_H_

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* Device initialization header */
#include "hal-config.h"

#include <stdio.h>
#include "retargetserial.h"
#include "sleep.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include "em_i2c.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

void delay_us(uint32_t uSec);
void delay_ms(uint32_t mSec);

#endif /* DRIVERS_INCLUDES_H_ */
