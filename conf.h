#ifndef __CONF_H__
#define __CONF_H__

#include <sys/types.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	char * daemon_log_file;
	char * daemon_run_file;

	struct ifaddrs * if_list;
	unsigned ip_socket;
} config_t;

extern config_t gconf;

void deinit_conf();

#endif /* __CONF_H__ */
