#include "bluetooth.h"

#define ENABLE_LOG
#include "log.h"

static BluetoothSerial bluetooth;

#define BT_STATE_OFF	  0
#define BT_STATE_ON		  1
#define BT_CLIENT_DISCONN 0
#define BT_CLIENT_CONN	  1

static uint8_t _bt_state = BT_STATE_OFF;
static uint8_t _bt_client_state = BT_CLIENT_DISCONN;
static uint8_t _bt_drdy = 0;

#define INPUT_BUFFER_SIZE 32U
static char input_buffer[INPUT_BUFFER_SIZE] = {0};
#define OUTPUT_BUFFER_SIZE 256U
static char output_buffer[OUTPUT_BUFFER_SIZE] = {0};

static char device_name[BT_DEVICE_NAME_SIZE] = BT_DEVICE_NAME;
static const char *bluetooth_greeting_msg =
	"\n ______  _____ _____  _                      "
	"\n|  ____|/ ____|  __ \\| |                     "
	"\n| |__  | (___ | |__) | |_ ___ _ __ _ __ ___  "
	"\n|  __|  \\___ \\|  ___/| __/ _ \\ '__| '_ ` _ \\ "
	"\n| |____ ____) | |    | ||  __/ |  | | | | | |"
	"\n|______|_____/|_|     \\__\\___|_|  |_| |_| |_|"
	"\n\nType \'help\' to get list of commands."
	"\n\n> ";

const char *invalid_cmd_msg = "Command not found!\n> ";

#define CMD_LIST_SIZE  5U
#define CMD_MAX_LENGHT 8U

typedef struct {
	char cmd[CMD_MAX_LENGHT];
	uint8_t len;
	void (*func)(void);
} bluetooth_cmd;

static bluetooth_cmd cmd_list[CMD_LIST_SIZE];

static void _bt_register_cmd(const char *cmd, void (*func)(void));

static void _bt_cmd_help(void);
static void _bt_cmd_test(void);

/* Read payload into input buffer */
static void _bluetooth_read()
{
	uint16_t index = 0;
	bzero(input_buffer, INPUT_BUFFER_SIZE);
	while (bluetooth.available()) {
		int ch = bluetooth.read();
		if (((char)ch == '\r') || ((char)ch == '\n')) { break; }
		input_buffer[index] = (char)ch;
		if (index < INPUT_BUFFER_SIZE) { index++; }
	}
	input_buffer[index] = '\0';
	// LOG("Data in: %s\n", input_buffer);
}

/*  Bluetooth Event Handler CallBack Function Definition */
void bluetooth_event_handler(esp_spp_cb_event_t event,
	esp_spp_cb_param_t *param)
{
	switch (event) {
		case ESP_SPP_INIT_EVT:
			LOG("Bluetooth SPP initialized successfully\n");
			_bt_state = BT_STATE_ON;
			break;
		case ESP_SPP_UNINIT_EVT:
			LOG("Bluetooth SPP deinitialized successfully\n");
			_bt_state = BT_STATE_OFF;
			break;
		case ESP_SPP_SRV_OPEN_EVT:
			LOG("Client connected\n");
			_bt_client_state = BT_CLIENT_CONN;
			bluetooth.write((uint8_t *)bluetooth_greeting_msg,
				strlen(bluetooth_greeting_msg));
			break;
		case ESP_SPP_CLOSE_EVT:
			_bt_client_state = BT_CLIENT_DISCONN;
			LOG("Client disconnected\n");
			break;
		case ESP_SPP_DATA_IND_EVT:
			_bluetooth_read();
			_bt_drdy = 1;
			break;
	};
}

void bluetooth_device_init()
{
	LOG("Device name: %s\n", device_name);

	if (!bluetooth.begin(device_name)) {
		LOG("Bluetooth init failed!!!\n");
		abort();
	}
	LOG("Bluetooth state: ON\n");

	_bt_register_cmd("help", &_bt_cmd_help);
	_bt_register_cmd("test", &_bt_cmd_test);

	bluetooth.register_callback(bluetooth_event_handler);
}

void bluetooth_device_deinit()
{
	if (_bt_state == BT_STATE_OFF) { return; }

	LOG("Bluetooth power down procedure started\n");
	if (_bt_client_state == BT_CLIENT_CONN) {
		bluetooth.disconnect();
		delay(200);
	}
	bluetooth.end();
	_bt_state = BT_STATE_OFF;
	LOG("Bluetooth state: OFF\n");
}

/* Variable for keeping track of cmd index that was called */
uint8_t _cmd_list_idx = 0;

void bluetooth_cmd_parser()
{
	if (_bt_drdy == 0) { return; }
	// LOG("Input: %s\n", input_buffer);
	for (uint8_t i = 0; i < CMD_LIST_SIZE; i++) {
		if (!strncmp(input_buffer, cmd_list[i].cmd, cmd_list[i].len)) {
			/* Check to prevent input without separators */
			if ((input_buffer[cmd_list[i].len] != 0x20) &&
				(input_buffer[cmd_list[i].len] != 0x00)) {
				break;
			}
			LOG("Command call _bt_cmd_%s()\n", cmd_list[i].cmd);
			_cmd_list_idx = i;
			cmd_list[i].func();
			_bt_drdy = 0;
			return;
		}
	}

	bluetooth.write((uint8_t *)invalid_cmd_msg, strlen(invalid_cmd_msg));

	_bt_drdy = 0;
}

void _bt_register_cmd(const char *cmd, void (*func)(void))
{
	static uint8_t idx = 0;

	strcpy(&cmd_list[idx].cmd[0], cmd);
	cmd_list[idx].len = strlen(cmd);
	cmd_list[idx].func = func;
	LOG("Bluetooth command \'%s\' registered\n", cmd);

	if (idx < CMD_LIST_SIZE)
		idx++;
	else
		LOG("Command list size limit reached! Increase value of "
			"CMD_LIST_SIZE.\n");
}

void _bt_cmd_help()
{
	char *_resp = "ESPTerm test terminal emulator over Bluetooth\n"
				  "\nThere is only one command available for testing\n"
				  "test - This command can accept input argument or not\n"
				  "> ";
	bluetooth.write((uint8_t *)_resp, strlen(_resp));
}

#define NO_ARG -1

void _bt_cmd_test()
{
	int8_t input_arg = NO_ARG;
	if (input_buffer[cmd_list[_cmd_list_idx].len + 1] != '\0') {
		input_arg = atoi(&input_buffer[cmd_list[_cmd_list_idx].len + 1]);
		LOG("Input argument value: %d\n", input_arg);
	}

	snprintf(output_buffer, OUTPUT_BUFFER_SIZE,
		"Command \"test\" with argument %d\n> ", input_arg);
	bluetooth.write((uint8_t *)output_buffer, strlen(output_buffer));
}
