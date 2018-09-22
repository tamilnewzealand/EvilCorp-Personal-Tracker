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

/* Device initialization header */
#include "hal-config.h"

#include <stdio.h>
#include "retargetserial.h"
#include "sleep.h"

#include "Drivers/i2c.h"

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
#define DISCONNECTED	0
#define SCANNING		1
#define FIND_SERVICE	2
#define FIND_CHAR		3
#define ENABLE_NOTIF 	4
#define DATA_MODE		5


// SPP service UUID: 2df1425f-32ca-4e67-aad2-3176631e6559
const uint8 serviceUUID[16] = {0x59, 0x65, 0x1e, 0x63, 0x76, 0x31, 0xd2, 0xaa, 0x67, 0x4e, 0xca, 0x32, 0x5f, 0x42, 0xf1, 0x2d};

// SPP data UUID: dde0c840-7bc7-4a70-9c69-3295c2535cd9
const uint8 charUUID[16] = {0xd9, 0x5c, 0x53, 0xc2, 0x95, 0x32, 0x69, 0x9c, 0x70, 0x4a, 0xc7, 0x7b, 0x40, 0xc8, 0xe0, 0xdd};

#define RESTART_TIMER 1
#define SPP_TX_TIMER  2

/***************************************************************************************************
 Local Variables
 **************************************************************************************************/
static uint8 _conn_handle = 0xFF;
static int _main_state;
static uint32 _service_handle;
static uint16 _char_handle;
static uint8 associated;

static void reset_variables() {
	_conn_handle = 0xFF;
	_main_state = DISCONNECTED;
	_service_handle = 0;
	_char_handle = 0;
}

static int process_scan_response(struct gecko_msg_le_gap_scan_response_evt_t *responses) {
    int address_num = 0;
    int address_match_found = 0;
	int address_len;
    int address_type;
    char name[32];

    while (address_num < (responses->data.len - 1)) {
        address_len  = responses->data.data[address_num];
        address_type = responses->data.data[address_num+1];

        // Name Provided
        if (address_type == 0x08 || address_type == 0x09 ) {
            memcpy(name, &(responses->data.data[address_num+2]), address_len-1);
            name[address_len-1] = 0;
            printf(name);
            printf("\r\n");
        }

        // Address Provided
        if (address_type == 0x06 || address_type == 0x07) {
        	if(memcmp(serviceUUID, &(responses->data.data[address_num+2]),16) == 0) {
        		printf("Found SPP device\r\n");
        		address_match_found = 1;
        	}
        }

        address_num = address_num + address_len + 1;
    }
    return(address_match_found);
}

static void send_spp_data() {
	uint8 len = 0;
	uint8 data[20];
	uint16 result;
	int c;

	while(len < 20) {
		  c = RETARGET_ReadChar();

		  if (c < 0) {
			  break;
		  } else {
			  data[len++] = (uint8) c;
		  }
	}

	if (len > 0) {
		// stack may return "out-of-memory" error if the local buffer is full -> in that case, just keep trying until the command succeeds
		do {
			result = gecko_cmd_gatt_write_characteristic_value_without_response(_conn_handle, _char_handle, len, data)->result;
		}
		while (result == bg_err_out_of_memory);
		if (result != 0) {
			printf("WTF: %x\r\n", result);
		}
	}
}

/**
 * @brief  SPP client mode main loop
 */
int main(void) {
	// Initialize device
	initMcu();
	// Initialize board
	initBoard();
	// Initialize application
	initApp();
	// Initialize stack
	gecko_init(&config);
	// Initialize I2C
	initI2C();

	RETARGET_SerialInit();
	associated = 0;

	uint16_t res = 0;
	bool result = false;

	// Read Status and WHO AM I from Accelerometer/Magnetometer
	result = i2c_read_register(0x1E, 0x00, &res);
	printf("accelerometer status value: %d\r\n", res);
	result = i2c_read_register(0x1E, 0x0D, &res);
	printf("accelerometer WHO AM I value: %d\r\n", res);

	// Read Status and WHO AM I from Gyroscope
	result = i2c_read_register(0x20, 0x00, &res);
	printf("gyroscope status value: %d\r\n", res);
	result = i2c_read_register(0x20, 0x0C, &res);
	printf("gyroscope WHO AM I value: %d\r\n", res);

	while (1) {

	}

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

				// start discovery
				gecko_cmd_le_gap_discover(le_gap_discover_generic);
				break;

			case gecko_evt_le_gap_scan_response_id:
				// process scan responses: this function returns 1 if we found the service we are looking for
				if(process_scan_response(&(evt->data.evt_le_gap_scan_response)) > 0) {
					struct gecko_msg_le_gap_open_rsp_t *pResp;

					// match found -> stop discovery and try to connect
					gecko_cmd_le_gap_end_procedure();

					pResp = gecko_cmd_le_gap_open(evt->data.evt_le_gap_scan_response.address, evt->data.evt_le_gap_scan_response.address_type);


					// make copy of connection handle for later use (for example, to cancel the connection attempt)
					_conn_handle = pResp->connection;

				}
				break;

			/* Connection opened event */
			case gecko_evt_le_connection_opened_id:
				printf("CONNECTED!\r\n");

				// start service discovery (we are only interested in one UUID)
				gecko_cmd_gatt_discover_primary_services_by_uuid(_conn_handle, 16, serviceUUID);
				_main_state = FIND_SERVICE;

				break;

			case gecko_evt_le_connection_closed_id:
				printf("DISCONNECTED!\r\n");

				reset_variables();
				// stop TX timer:
				gecko_cmd_hardware_set_soft_timer(0, SPP_TX_TIMER, 0);

				SLEEP_SleepBlockEnd(sleepEM2); // enable sleeping after disconnect

				// create one-shot soft timer that will restart discovery after 1 second delay
				gecko_cmd_hardware_set_soft_timer(32768, RESTART_TIMER, true);
				break;

			case gecko_evt_le_connection_parameters_id:
				printf("Conn.parameters: interval %u units, txsize %u\r\n",
				evt->data.evt_le_connection_parameters.interval,
				evt->data.evt_le_connection_parameters.txsize);
				break;

			case gecko_evt_gatt_service_id:
				if(evt->data.evt_gatt_service.uuid.len == 16) {
					if(memcmp(serviceUUID, evt->data.evt_gatt_service.uuid.data,16) == 0) {
						printf("Service Discovered\r\n");
						_service_handle = evt->data.evt_gatt_service.service;
					}
				}
				break;

			case gecko_evt_gatt_procedure_completed_id:
				switch(_main_state) {
					case FIND_SERVICE:
						if (_service_handle > 0) {
							// Service found, next search for characteristics
							gecko_cmd_gatt_discover_characteristics(_conn_handle, _service_handle);
							_main_state = FIND_CHAR;
						} else {
							// No service found -> disconnect
							printf("SPP service not found?\r\n");
							gecko_cmd_endpoint_close(_conn_handle);
						}
						break;

					case FIND_CHAR:
						if (_char_handle > 0) {
							// Char found, turn on indications
							gecko_cmd_gatt_set_characteristic_notification(_conn_handle, _char_handle, gatt_notification);
							_main_state = ENABLE_NOTIF;
						} else {
							// No characteristic found -> disconnect
							printf("SPP char not found?\r\n");
							gecko_cmd_endpoint_close(_conn_handle);
						}
						break;

					case ENABLE_NOTIF:
						_main_state = DATA_MODE;
						printf("SPP mode ON\r\n");
						// start soft timer that is used to offload local TX buffer
						gecko_cmd_hardware_set_soft_timer(328, SPP_TX_TIMER, 0);
						SLEEP_SleepBlockBegin(sleepEM2); // disable sleeping when SPP mode active
						break;

					default:
						break;
				}
				break;

			case gecko_evt_gatt_characteristic_id:
				if(evt->data.evt_gatt_characteristic.uuid.len == 16) {
					if(memcmp(charUUID, evt->data.evt_gatt_characteristic.uuid.data,16) == 0) {
						printf("Char discovered");
						_char_handle = evt->data.evt_gatt_characteristic.characteristic;
					}
				}
				break;

			/* Software Timer event */
			case gecko_evt_hardware_soft_timer_id:
				switch (evt->data.evt_hardware_soft_timer.handle) {
					case SPP_TX_TIMER:
						// send data from local TX buffer
						send_spp_data();
						break;

					case RESTART_TIMER:
						gecko_cmd_le_gap_discover(le_gap_discover_generic);
						break;

					default:
						break;
				}
				break;

			default:
				break;
    	}
  	}
}
