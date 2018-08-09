#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static void inline _log(const char * fmt,...) {
	va_list args;
	va_start(args, fmt);

	vprintf(fmt, args);

	va_end(args);
}

static void inline _log_cli(const char * fmt,...) {
	va_list args;
	va_start(args, fmt);

	vprintf(fmt, args);

	va_end(args);
}

#endif
