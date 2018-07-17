#include "lib.h"

void run_daemon();
void run_cli(int argc, const char * argv[]);
const char * help_str = "Usage:...";

int main(int argc, const char * argv[]) {
	if(argc == 1)
		run_daemon();
	else
		run_cli(argc, argv);

	return 0;
}

void run_daemon() {
	int retval = 0;
	unsigned char *buffer = (unsigned char *)malloc(DEF_BUFSIZE);
	int data_size;

	//TODO: check for repeat run
#if 0
	daemon(0, 0);
#endif

	retval = init_ip_socket();
	if(retval < 0)
		exit_err("init_ip_socket");
	gconf.ip_socket = retval;

	if(fill_interfaces_list())
		exit_err("fill_interfaces_list");

	while(1) {
		data_size = capture_packet(buffer, DEF_BUFSIZE);
		if(data_size < 0)
			exit_err("capture_packet");
		process_ip_packet(buffer, data_size);
	}

	//TODO: thread for waiting of cli
}

void run_cli(int argc, const char * argv[]) {
	//TODO: try to connect to daemon socket
	int sock = socket(AF_LOCAL, SOCK_SEQPACKET, 0);
	if(sock == -1)
		exit_err("socket");
	command_t cmd;
	if (!strncmp(argv[1], command_str[START], strlen(command_str[START]))) {
		cmd = START;
		_log("start!\n");
	} else if (!strncmp(argv[1], command_str[STOP], strlen(command_str[STOP]))) {
		cmd = STOP;
		_log("stop!\n");
	} else if (!strncmp(argv[1], command_str[SHOW_IP_COUNT], strlen(command_str[SHOW_IP_COUNT]))) {
		//TODO: handle parameter
		cmd = SHOW_IP_COUNT;
		_log("show_ip_count!\n");
	} else if (!strncmp(argv[1], command_str[SELECT_IFACE], strlen(command_str[SELECT_IFACE]))) {
		//TODO: handle parameter
		cmd = SELECT_IFACE;
		_log("select_iface!\n");
	} else if (!strncmp(argv[1], command_str[STAT_IFACE], strlen(command_str[STAT_IFACE]))) {
		//TODO: handle parameter
		cmd = STAT_IFACE;
		_log("stat_iface!\n");
	} else if (!strncmp(argv[1], command_str[HELP], strlen(command_str[HELP]))) {
		print_help();
	}
}

void print_help() {
	printf("%s", help_str);
}
