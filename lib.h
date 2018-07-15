#ifndef __LIB_H__
#define __LIB_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <stdarg.h>

#include "conf.h"

static void inline exit_err(const char * str) {
	//TODO: print error to daemon log if daemon
	perror(str);
	exit(1);
}

static void inline _log(const char * fmt,...) {
	va_list args;
	va_start(args, fmt);

	vprintf(fmt, args);

	va_end(args);
}

int init_daemon_log(const char * filename);

/*
 * Function does fork() and run daemon() in the child process
 *
 * @return 1 if parent process, 0 if child process
 */
int run_daemon();

int init_ip_socket();

int fill_interfaces_list();

int capture_packets();

#endif /* __LIB_H__ */
