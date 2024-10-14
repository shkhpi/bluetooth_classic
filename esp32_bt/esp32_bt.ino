#include "bluetooth.h"
#define ENABLE_LOG
#include "log.h"

#define MAIN_LOOP_DELAY_TIME 20

void setup()
{
	LOG_INIT;
	LOG("------------------------------------\n");

	bluetooth_device_init();
}

void loop()
{
	bluetooth_cmd_parser();

	vTaskDelay(MAIN_LOOP_DELAY_TIME / portTICK_PERIOD_MS);
}
