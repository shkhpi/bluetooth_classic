#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <HardwareSerial.h>

static char _buffer[LOG_BUFFER_SIZE] = {0};

void log_msg(const char *func, unsigned int line, const char *fmt, ...)
{
	snprintf(_buffer, LOG_BUFFER_SIZE, "%s:%-4d  ", func, line);
	size_t msg_len = strlen(_buffer);
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(&_buffer[msg_len], LOG_BUFFER_SIZE - msg_len, fmt, ap);
	va_end(ap);

	LOG_SERIAL_PORT.write(_buffer, strlen(_buffer));
}