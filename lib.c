#include "lib.h"

int init_daemon_log(const char * filename) {
	//TODO

	return 0;
}

int run_daemon() {
	int retval = 0;
#if 0
	retval = fork();
	if (retval == -1)
		exit_err("fork()");
	if (retval =! 0)
		return 1;
	daemon(0, 0);
#endif

	//TODO: check for repeat run


	//TODO: init IP socket
	retval = init_ip_socket();
	if(retval < 0)
		exit_err("init_ip_socket");
	gconf.ip_socket = retval;

	if(fill_interfaces_list())
		exit_err("fill_interfaces_list");

	capture_packets();


	//TODO: read from IP socket


	//TODO: thread for waiting of cli


	//TODO: Update statistics


	//TODO: Log packet (to log, cli)

	return 0;
}

int capture_packets() {
	int saddr_size , data_size;
	struct sockaddr saddr;
	struct sockaddr_in *sin;
	unsigned char *buffer = (unsigned char *)malloc(65536); //Its Big!

	while(1)
	{
		saddr_size = sizeof saddr;
		data_size = recvfrom(gconf.ip_socket, buffer, 65536, 0, &saddr,
				&saddr_size);
		if(data_size < 0)
		{
			printf("Recvfrom error, failed to get packets\n");
			return -1;
		}
		sin = (struct sockaddr_in *)&saddr;
		_log("received packet from %s\n", inet_ntoa(sin->sin_addr));


	}
    return 0;
}

int init_ip_socket() {
	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if(fd < 0)
		exit_err("socket");

	return fd;
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
			_log("%s: %s\n", tmp->ifa_name, ip_addr?ip_addr:"");
		}

		tmp = tmp->ifa_next;
	}

	return 0;
}

