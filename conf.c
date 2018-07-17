#include "conf.h"

config_t gconf = {0};

void deinit_conf() {
	close(gconf.ip_socket);
	free(gconf.if_list);
}

int fill_interfaces_list() {
	struct ifaddrs *list,*tmp;
	int rv, i;

	rv = getifaddrs(&list);
	if (rv)
		exit_err("getifaddrs");

	tmp = list;
	gconf.if_num = 0;
	while (tmp)
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
			gconf.if_num++;
		}

		tmp = tmp->ifa_next;
	}

	gconf.if_list = malloc(sizeof(iface_t)*gconf.if_num);
	i=0;
	tmp = list;
	while (tmp)
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
			struct sockaddr_in *sin = (struct sockaddr_in *)(tmp->ifa_addr);
			char *ip_addr = inet_ntoa(sin->sin_addr);
			memcpy(gconf.if_list[i].name, tmp->ifa_name, strlen(tmp->ifa_name)+1);
			gconf.if_list[i].addr = *sin;
#ifndef NO_AUTO_IFACES_SNIFF
			turn_iface_on(i);
#endif
			_log("Added interface %s to the list: %s\n", tmp->ifa_name,
					ip_addr?ip_addr:"");
			i++;
		}

		tmp = tmp->ifa_next;
	}

	for(i=0;i<gconf.if_num;i++) {
		_log("Interface #%d: %s\t%s\n", i, gconf.if_list[i].name,
				inet_ntoa(gconf.if_list[i].addr.sin_addr));
	}
	freeifaddrs(list);

	return 0;
}

int get_if_num(struct sockaddr_in addr) {
	int i = 0;

	for(i=0;i<gconf.if_num; i++) {
		if(!equal(&gconf.if_list[i].addr, &addr))
			return i;
	}

	return -1;
}

int get_if_num_name(char name[]) {
	int i = 0;

	for(i=0;i<gconf.if_num; i++) {
		if(!strcmp(name, gconf.if_list[i].name))
			return i;
	}

	return -1;
}

void turn_iface_on(int num) {
	gconf.if_mask |= (1 << num);
}

int check_iface_on(int num) {
	return gconf.if_mask & (1 << num);
}

void turn_iface_off(int num) {
	gconf.if_mask &= ~(1 << num);
}
