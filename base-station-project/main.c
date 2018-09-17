/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

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

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"

/* Device initialization header */
#include "hal-config.h"

#include <stdio.h>
#include "retargetserial.h"
#include "sleep.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

// Gecko configuration parameters (see gecko_configuration.h)
static const gecko_configuration_t config = {
	.config_flags = 0,
	.sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
	.bluetooth.max_connections = MAX_CONNECTIONS,
	.bluetooth.heap = bluetooth_stack_heap,
	.bluetooth.heap_size = sizeof(bluetooth_stack_heap),
	.bluetooth.sleep_clock_accuracy = 100,
	.gattdb = &bg_gattdb_data,
	.ota.flags = 0,
	.ota.device_name_len = 3,
	.ota.device_name_ptr = "OTA",
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
	.pa.config_enable = 1,
	.pa.input = GECKO_RADIO_PA_INPUT_VBAT,
#endif
};

/***************************************************************************************************
  Local Macros and Definitions
 **************************************************************************************************/
#define SPP_TX_TIMER  2

#define STATE_ADVERTISING 1
#define STATE_CONNECTED   2
#define STATE_SPP_MODE    3

/***************************************************************************************************
 Local Variables
 **************************************************************************************************/
static uint8 _conn_handle = 0xFF;
static int _main_state;

static void reset_variables() {
	_conn_handle = 0xFF;
	_main_state = STATE_ADVERTISING;
}

/**
 * @brief  SPP server mode main loop
 */
void main(void) {
	// Initialize device
	initMcu();
	// Initialize board
	initBoard();
	// Initialize application
	initApp();
	// Initialize stack
	gecko_init(&config);

	RETARGET_SerialInit();
	char printbuf[128];

  while (1) {
    /* Event pointer for handling events */
    struct gecko_cmd_packet* evt;
    /* Check for stack event. */
    evt = gecko_wait_event();

    /* Handle events */
    switch (BGLIB_MSG_ID(evt->header)) {

    	/* This boot event is generated when the system boots up after reset.
      	* Here the system is set to start advertising immediately after boot procedure. */
    	case gecko_evt_system_boot_id:
    		reset_variables();

    		gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
    		break;

    	/* Connection opened event */
    	case gecko_evt_le_connection_opened_id:
    		_conn_handle = evt->data.evt_le_connection_opened.connection;
    		printf("CONNECTED!/r/n");
    		_main_state = STATE_CONNECTED;

    		/* request connection parameter update.
    		 * conn.interval min 20ms, max 40ms, slave latency 4 intervals,
    		 * supervision timeout 2 seconds
    		 * (These should be compliant with Apple Bluetooth Accessory Design Guidelines) */
    		gecko_cmd_le_connection_set_parameters(_conn_handle, 16, 32, 4, 200);
    		break;

    	case gecko_evt_le_connection_parameters_id:
    		printf("Conn.parameters: interval %u units, txsize %u\r\n",
    				evt->data.evt_le_connection_parameters.interval,
					evt->data.evt_le_connection_parameters.txsize);
    		break;

		case gecko_evt_le_connection_closed_id:
			printf("DISCONNECTED!");

			reset_variables();
			SLEEP_SleepBlockEnd(sleepEM2);

			gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
			break;

		case gecko_evt_gatt_server_characteristic_status_id:
			{
				break;
			}
			break;

		case gecko_evt_gatt_server_attribute_value_id:
			{
				 // data received from SPP client -> print to UART
				 memcpy(printbuf, evt->data.evt_gatt_server_attribute_value.value.data, evt->data.evt_gatt_server_attribute_value.value.len);
				 printbuf[evt->data.evt_gatt_server_attribute_value.value.len] = 0;
				 printf(printbuf);
			}
			break;


		default:
			break;
    	}
  	}
}
