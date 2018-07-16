#ifndef __CONF_H__
#define __CONF_H__

#include <sys/types.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>

#include "data.h"
#include "lib.h"

typedef struct {
	char *daemon_log_file;
	char *daemon_pid_file;

	/* Interfaces list may change. Now it is not handled. */
	struct ifaddrs *if_list;
	unsigned ip_socket;
	unsigned cli_socket;
	ip_list_t *ip_list;

	int sniffing_stopped;
} config_t;

extern config_t gconf;

int fill_interfaces_list();

void deinit_conf();

#endif /* __CONF_H__ */
