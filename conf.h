#ifndef __CONF_H__
#define __CONF_H__

#include <sys/types.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <string.h>

#include "data.h"
#include "types.h"

extern config_t gconf;

int fill_interfaces_list();
int get_if_num(struct sockaddr_in addr);

void deinit_conf();

#endif /* __CONF_H__ */
