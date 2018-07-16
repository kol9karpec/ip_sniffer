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
#include <string.h>

#include "conf.h"
#include "data.h"
#include "log.h"

#define DEF_LOG_FILE "/var/log/"

static void inline exit_err(const char * str) {
	//TODO: print error to daemon log if daemon
	perror(str);
	exit(1);
}

typedef enum {
	START,
	STOP,
	SHOW_IP_COUNT,
	SELECT_IFACE,
	STAT_IFACE,
	HELP,
	COMMANDS_NUM
} command_t;

const char* command_str[COMMANDS_NUM] = {
	"start",
	"stop",
	"show",
	"select",
	"stat",
	"--help"
};


int init_daemon_log(const char * filename);

int run_daemon();

int init_ip_socket();

int capture_packets();

int process_packet_addr(struct sockaddr_in *addr);

bool daemon_repeat();

int process_cli_command(command_t cmd, void *val);

#endif /* __LIB_H__ */
