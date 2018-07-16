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
#include <stdbool.h>

#include "conf.h"
#include "data.h"
#include "log.h"

static void inline exit_err(const char * str) {
	//TODO: print error to daemon log if daemon
	perror(str);
	exit(1);
}


int init_daemon_log(const char * filename);

/*
 * Function does fork() and run daemon() in the child process
 *
 * @return 1 if parent process, 0 if child process
 */
int run_daemon();

int init_ip_socket();


int capture_packets();

int process_packet_addr(struct sockaddr_in *addr);

bool daemon_repeat();

#endif /* __LIB_H__ */
