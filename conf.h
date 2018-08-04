#ifndef __CONF_H__
#define __CONF_H__

#include <sys/types.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <string.h>
#include <errno.h>

#include "data.h"
#include "types.h"

extern config_t gconf;

int fill_interfaces_list();
int get_if_num(struct sockaddr_in addr);
int get_if_num_name(char name[]);
void turn_iface_on(int num);
int check_iface_on(int num);
void turn_iface_off(int num);

void deinit_conf();

static void inline exit_err(const char * str) {
	//TODO: print error to daemon log if daemon
	deinit_conf();
	perror(str);
	exit(1);
}
#endif /* __CONF_H__ */
