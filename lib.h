#ifndef __LIB_H__
#define __LIB_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
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

#define DEF_LOG_FILE "/var/log/ip_sniffer.log"
#define SOCKET_PATH "/tmp/ip_sniffer.socket"
#define DEF_BUFSIZE 65536

extern const char* command_str[COMMANDS_NUM];

int init_daemon_log(const char * filename);

int init_ip_socket();
int init_unix_socket();

int capture_packet(unsigned char *buffer, unsigned size);
int process_ip_packet(unsigned char *buffer, unsigned size);

bool daemon_repeat();

int process_cli_command(command_t cmd, void *arg, void *res_dest,
		int *size_dest);

#endif /* __LIB_H__ */
