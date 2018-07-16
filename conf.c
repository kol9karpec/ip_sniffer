#include "conf.h"

config_t gconf = {0};

void deinit_conf() {
	//TODO: Close opened fds
	close(gconf.ip_socket);
	freeifaddrs(gconf.if_list);
}

int fill_interfaces_list() {
	struct ifaddrs *tmp;
	int rv;

	rv = getifaddrs(&gconf.if_list);
	if (rv)
		exit_err("getifaddrs");

	tmp = gconf.if_list;
	while (tmp)
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
			struct sockaddr_in *sin = (struct sockaddr_in *)(tmp->ifa_addr);
			char *ip_addr = inet_ntoa(sin->sin_addr);

			_log("Added interface %s to list: %s\n", tmp->ifa_name,
					ip_addr?ip_addr:"");
		}

		tmp = tmp->ifa_next;
	}

	return 0;
}
