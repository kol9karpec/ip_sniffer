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

	retval = init_ip_socket();
	if(retval < 0)
		exit_err("init_ip_socket");
	gconf.ip_socket = retval;

	if(fill_interfaces_list())
		exit_err("fill_interfaces_list");

	capture_packets();



	//TODO: thread for waiting of cli

	return 0;
}

int capture_packets() {
	int saddr_size , data_size;
	struct sockaddr saddr;
	struct sockaddr_in *sin;
	unsigned char *buffer = (unsigned char *)malloc(65536);

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
		process_packet_addr(sin);

		_log("received packet from %s\n", inet_ntoa(sin->sin_addr));

	}
    return 0;
}

int process_packet_addr(struct sockaddr_in *addr) {
	ip_list_t *list = gconf.ip_list;
	ip_list_t *node = ip_in_list(list, *addr);
	if (!node) {
		node = add_ip(&list, *addr);
	}

	inc_ip_count(&node->data);

	return 0;
}

int init_ip_socket() {
	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if(fd < 0)
		exit_err("socket");

	return fd;
}

