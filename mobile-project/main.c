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
#include "Drivers/includes.h"

#include "Drivers/i2c.h"
#include "Drivers/FXAS21002.h"
#include "Drivers/FXOS8700CQ.h"

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

//#define SHOW_RSSI_MODE

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

AccelerometerData_t accel_data;
MagnetometerData_t mag_data;
GyroscopeData_t gyro_data;
int8_t temp_data_1;
int8_t temp_data_2;

uint8 signals[100][2] = {0};
uint8 location[6] = {0, 0, 0, 0, 0, 0};

/*
 * Resets Variables
 */
static void reset_variables() {
	_conn_handle = 0xFF;
	_main_state = DISCONNECTED;
	_service_handle = 0;
	_char_handle = 0;
}

/*
 * Tries to find desired Bluetooth service and characteristic
 */
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

/*
 * Sends a max 20 byte character array to the base station
 */
/*
static void send_line(char* data, uint8 len) {
	uint16 result;

	// stack may return "out-of-memory" error if the local buffer is full -> in that case, just keep trying until the command succeeds
	do {
		result = gecko_cmd_gatt_write_characteristic_value_without_response(_conn_handle, _char_handle, len, data)->result;
	} while (result == bg_err_out_of_memory);

	if (result != 0) {
		printf("ERROR: %x\r\n", result);
	}
}
*/
/*
 * Formats and sends a float
 */
/*
void send_float(char* intro, float data) {
	char float_line[20];
	snprintf(float_line, 20, intro, data);
	send_line(float_line, 20);
}

void float2Bytes(uint8* bytes_temp[4], float float_variable){
	memcpy(bytes_temp, (unsigned char*) (&float_variable), 4);
}
*/
/*
 * Runs every second, sends data to base station
 */
void send_data() {
	/*
	#ifdef SHOW_RSSI_MODE
		char signal_line[20];
		for (int i=0;i<10;i++) {
			if (signals[i][1] != 0) {
				snprintf(signal_line, 20, "B: %d\r\n", signals[i][0]);
				send_line(signal_line, 20);
				snprintf(signal_line, 20, "RSSI:%d\r\n", (int8)signals[i][1]);
				send_line(signal_line, 20);
			}
		}
	#else
	*/
		readAccel(&accel_data);
		readMagn(&mag_data);
		readGryo(&gyro_data);
		temp_data_1 = readTempFXAS21002();
		temp_data_2 = readTempFXOS8700CQ();

		location[0] += 1;
/*
		send_float("A X: %.4f\r\n", (float)accel_data.x/SENSITIVITY_2G);
		send_float("A Y: %.4f\r\n", (float)accel_data.y/SENSITIVITY_2G);
		send_float("A Z: %.4f\r\n", (float)accel_data.z/SENSITIVITY_2G);

		send_float("M X: %.4f\r\n", (float)mag_data.x/SENSITIVITY_MAG);
		send_float("M Y: %.4f\r\n", (float)mag_data.y/SENSITIVITY_MAG);
		send_float("M Z: %.4f\r\n", (float)mag_data.z/SENSITIVITY_MAG);

		send_float("G R: %.4f\r\n", (float)gyro_data.x/SENSITIVITY_250);
		send_float("G P: %.4f\r\n", (float)gyro_data.y/SENSITIVITY_250);
		send_float("G Y: %.4f\r\n", (float)gyro_data.z/SENSITIVITY_250);

		send_float("T 1: %.0f\r\n", (float) temp_data_1);
		send_float("T 2: %.0f\r\n", (float) temp_data_2);
		*/
		gecko_cmd_gatt_write_characteristic_value_without_response(_conn_handle, _char_handle, 6, location)->result;
	//#endif
}

/*
 * Main loop
 */
int main(void) {
	initMcu(); // Initialize device
	initBoard(); // Initialize board
	initApp(); // Initialize application
	gecko_init(&config); // Initialize stack

	if (!initFXOS8700CQ()) while (1); // Initialize Temperature Sensors
	if (!initFXAS21002()) while (1);

	RETARGET_SerialInit();
	associated = 0;

	printf("Starting Mobile Device\r\n");

	/* Bluetooth Main Loop */
	while (1) {
		struct gecko_cmd_packet* evt; // Event pointer for handling events
		evt = gecko_wait_event(); // Check for stack event

		/* Handle events */
		switch (BGLIB_MSG_ID(evt->header)) {
			// After Reset or Boot
			case gecko_evt_system_boot_id:
				printf("REBOOTED!\r\n");

				reset_variables();
				gecko_cmd_le_gap_discover(le_gap_discover_generic); // Start discovery
				break;

			// When responses received from discovery
			case gecko_evt_le_gap_scan_response_id:
				if (evt->data.evt_le_gap_scan_response.data.len >= 29) {
					for (int i=0;i<99;i++) {
						signals[i][0] = signals[i+1][0];
						signals[i][1] = signals[i+1][1];
					}
					signals[99][0] = evt->data.evt_le_gap_scan_response.data.data[28];
					signals[99][1] = (uint8)evt->data.evt_le_gap_scan_response.rssi;
				}

				if ((associated == 0) && (process_scan_response(&(evt->data.evt_le_gap_scan_response)) > 0)) {
					associated = 1;
					struct gecko_msg_le_gap_open_rsp_t *pResp;
					pResp = gecko_cmd_le_gap_open(evt->data.evt_le_gap_scan_response.address, evt->data.evt_le_gap_scan_response.address_type);
					_conn_handle = pResp->connection; // Make copy of connection handle for later use
				}
				break;

			// Connected
			case gecko_evt_le_connection_opened_id:
				printf("CONNECTED!\r\n");

				gecko_cmd_gatt_discover_primary_services_by_uuid(_conn_handle, 16, serviceUUID); // Start service discovery
				_main_state = FIND_SERVICE;
				break;

			// Disconnected
			case gecko_evt_le_connection_closed_id:
				printf("DISCONNECTED!\r\n");

				reset_variables();
				gecko_cmd_hardware_set_soft_timer(0, SPP_TX_TIMER, 0); // Stop Transmit Timer:

				SLEEP_SleepBlockEnd(sleepEM2); // Enable sleeping after disconnect
				gecko_cmd_hardware_set_soft_timer(32768, RESTART_TIMER, true); // Create timer to restart discovery after 1s delay
				break;

			// Connection Parameters Received
			case gecko_evt_le_connection_parameters_id:
				printf("Conn.parameters: interval %u units, txsize %u\r\n",
				evt->data.evt_le_connection_parameters.interval,
				evt->data.evt_le_connection_parameters.txsize);
				break;

			// Service Parameters Received
			case gecko_evt_gatt_service_id:
				if(evt->data.evt_gatt_service.uuid.len == 16) {
					if(memcmp(serviceUUID, evt->data.evt_gatt_service.uuid.data,16) == 0) {
						printf("Service Discovered\r\n");
						_service_handle = evt->data.evt_gatt_service.service;
					}
				}
				break;

			// Characteristic Parameters Received
			case gecko_evt_gatt_characteristic_id:
				if(evt->data.evt_gatt_characteristic.uuid.len == 16) {
					if(memcmp(charUUID, evt->data.evt_gatt_characteristic.uuid.data,16) == 0) {
						printf("Char discovered");
						_char_handle = evt->data.evt_gatt_characteristic.characteristic;
					}
				}
				break;

			// Completed a procedure
			case gecko_evt_gatt_procedure_completed_id:
				switch(_main_state) {
					// Found a service
					case FIND_SERVICE:
						if (_service_handle > 0) { // Service found -> search for characteristics
							gecko_cmd_gatt_discover_characteristics(_conn_handle, _service_handle);
							_main_state = FIND_CHAR;
						} else { // No service found -> disconnect
							printf("SPP service not found\r\n");
							gecko_cmd_endpoint_close(_conn_handle);
						}
						break;

					// Found a characteristic
					case FIND_CHAR:
						if (_char_handle > 0) { // Characteristic found -> turn on indications
							gecko_cmd_gatt_set_characteristic_notification(_conn_handle, _char_handle, gatt_notification);
							_main_state = ENABLE_NOTIF;
						} else { // No characteristic found -> disconnect
							printf("SPP char not found\r\n");
							gecko_cmd_endpoint_close(_conn_handle);
						}
						break;

					// Notify (Write without response) has started
					case ENABLE_NOTIF:
						_main_state = DATA_MODE;
						printf("SPP mode ON\r\n");
						gecko_cmd_hardware_set_soft_timer(32768, SPP_TX_TIMER, 0); // start transmit timer
						SLEEP_SleepBlockBegin(sleepEM2); // Disable sleeping when SPP mode active
						break;

					default:
						break;
				}
				break;

			// Transmit Timer Event
			case gecko_evt_hardware_soft_timer_id:
				switch (evt->data.evt_hardware_soft_timer.handle) {
					case SPP_TX_TIMER:
						send_data();
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
