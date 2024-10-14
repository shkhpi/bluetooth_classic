#ifndef _LOG_H_
#define _LOG_H_

#define LOG_BUFFER_SIZE 512U
#define LOG_SERIAL_PORT Serial
#define LOG_BAUD_RATE	115200UL

#if defined(ENABLE_LOG)
	#define LOG_INIT LOG_SERIAL_PORT.begin(LOG_BAUD_RATE)
	#define LOG(...) log_msg(__FUNCTION__, __LINE__, __VA_ARGS__)

void log_msg(const char *func, unsigned int line, const char *fmt, ...);
#else
	#define LOG_INIT
	#define LOG(...)
#endif

#endif /* _LOG_H_ */