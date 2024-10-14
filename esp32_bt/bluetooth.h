#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include "BluetoothSerial.h"
// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
	#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
	# error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

#define BT_DEVICE_NAME_SIZE 16U
#define BT_DEVICE_NAME		"esp32-test-dev"

void bluetooth_device_init(void);
void bluetooth_device_deinit(void);

void bluetooth_event_handler(esp_spp_cb_event_t event,
	esp_spp_cb_param_t *param);
void bluetooth_cmd_parser(void);

#endif /* _BLUETOOTH_H_ */