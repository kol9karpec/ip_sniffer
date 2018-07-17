#include "lib.h"

const char* command_str[COMMANDS_NUM] = {
	"start",
	"stop",
	"show",
	"select",
	"stat",
	"--help"
};

int init_daemon_log(const char * filename) {
	//TODO

	return 0;
}

int capture_packet(unsigned char *buffer, unsigned size) {
	int saddr_size, data_size;
	struct sockaddr saddr;

	saddr_size = sizeof saddr;
	data_size = recvfrom(gconf.ip_socket, buffer, size, 0, &saddr,
			&saddr_size);
	if(data_size < 0)
		_log("Recvfrom error, failed to get packets\n");

	_log("received packet\n");

    return data_size;
}

int process_ip_packet(unsigned char *buffer, unsigned size) {
	struct iphdr *iph = (struct iphdr*)buffer;
	struct in_addr *_dest = (struct in_addr *)&iph->daddr;
	struct in_addr *_src = (struct in_addr *)&iph->saddr;
	struct sockaddr_in dest, src;
	dest.sin_addr = *_dest;
	src.sin_addr = *_src;

	ip_list_t *node = ip_in_list(gconf.ip_list, src);
	if (!node) {
		node = add_ip(&gconf.ip_list, src);
	}

	inc_ip_count(&node->data, get_if_num(dest));

	return 0;
}

int init_ip_socket() {
	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if(fd < 0)
		exit_err("socket");

	return fd;
}

int run_cli();

int process_cli_command(command_t cmd, void *val) {
#if 0
	switch(cmd) {
		case START:
			//TODO
			run_daemon();
			break;
		case STOP:
			//TODO
			stop_daemon();
			break;
		case SHOW_IP_COUNT:
			//TODO
			show_ip_count();
			break;
		case SELECT_IFACE:
			//TODO
			select_iface();
			break;
		default:
			_log("Wrong command!\n");
			return 1;
	}
#endif

	return 0;
}
