#include "lib.h"

void run_daemon();
void print_help();
void run_cli(command_t cmd, void * arg, unsigned size);
void *daemon_thread_func(void * arg);

const char * help_str = "Usage: {start | stop | show <ip> | select <iface> | stat [iface]}\n" \
						 "\tstart - begin sniffing on all interfaces\n" \
						 "\tstop - stop sniffing on all interfaces\n" \
						 "\tshow <ip> - show count of packets for specified ip\n"\
						 "\tselect <iface> - switch iface mode (sniffing on/off)\n" \
						 "\tstat [iface] - show statistics for specified interface\n" \
						 "\t\tfor all interfaces if ommited\n" \
						 "\tIf run without parameters - run in daemon mode\n";

int main(int argc, char * argv[]) {
	char * _all_str = "all";
	if(argc == 1) {
		run_daemon();
	} else {
		command_t cmd = START;
		void *arg = NULL;
		unsigned size = 0;

		if (!strncmp(argv[1], command_str[START], strlen(command_str[START]))) {
			cmd = START;
			_log("start!\n");
		} else if (!strncmp(argv[1], command_str[STOP],
					strlen(command_str[STOP]))) {
			cmd = STOP;
			_log("stop!\n");
		} else if (!strncmp(argv[1], command_str[SHOW_IP_COUNT],
					strlen(command_str[SHOW_IP_COUNT]))) {
			if (argc < 3) {
				_log_cli("IP parameter is absent!\n");
				print_help();
				return 1;
			}
			arg = argv[2];
			size = strlen(argv[2]) + 1;
			cmd = SHOW_IP_COUNT;
			_log("show_ip_count!\n");
		} else if (!strncmp(argv[1], command_str[SELECT_IFACE],
					strlen(command_str[SELECT_IFACE]))) {
			if (argc < 3) {
				_log_cli("IFACE parameter is absent!\n");
				print_help();
				return 1;
			}
			arg = argv[2];
			size = strlen(argv[2]) + 1;
			cmd = SELECT_IFACE;
			_log("select_iface!\n");
		} else if (!strncmp(argv[1], command_str[STAT_IFACE],
					strlen(command_str[STAT_IFACE]))) {
			if (argc == 3) {
				arg = argv[2];
				size = strlen(argv[2]) + 1;
			} else {
				arg = _all_str;
				size = strlen(_all_str) + 1;
			}
			cmd = STAT_IFACE;
			_log("stat_iface!\n");
		} else {
			print_help();
			return 1;
		}
		run_cli(cmd, arg, size);
	}

	return 0;
}


void run_daemon() {
	int retval = 0;
	unsigned char *buffer = (unsigned char *)malloc(DEF_BUFSIZE);
	int data_size;

	if(!buffer) {
		exit_err("malloc");
	}
	//TODO: check for repeat run
#if 0
	daemon(0, 0);
#endif

	pthread_create(&gconf.wait_thread, NULL,
			daemon_thread_func, NULL);
	retval = init_ip_socket();
	if(retval < 0)
		exit_err("init_ip_socket");
	gconf.ip_socket = retval;

	if(fill_interfaces_list())
		exit_err("fill_interfaces_list");

#ifdef NO_AUTO_SNIFF
	gconf.sniffing_stopped = 1;
#endif

	while(1) {
		if(!gconf.sniffing_stopped) {
			data_size = capture_packet(buffer, DEF_BUFSIZE);
			//TODO: better error handling
			if(data_size < 0) {
				free(buffer);
				exit_err("capture_packet()");
			}

			if (process_ip_packet(buffer, data_size)) {
				free(buffer);
				exit_err("process_ip_packet()");
			}
		}
	}
}

void run_cli(command_t cmd, void *arg, unsigned size) {
	int rv;
	int sock;
	struct sockaddr_un addr;
	unsigned char buf[DEF_BUFSIZE] = {0};
	unsigned bufsize = 0;
	unsigned char *data_ptr = buf;

	sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if(sock == -1)
		exit_err("socket");

	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

	rv = connect(sock, (const struct sockaddr *) &addr,
			sizeof(struct sockaddr_un));
	if (rv == -1) {
		exit_err("connect");
	}
	_log_cli("Connection successful\n");

	memcpy(data_ptr, &cmd, sizeof(cmd));
	data_ptr += sizeof(cmd);
	bufsize += sizeof(cmd);
	if(arg) {
		memcpy(data_ptr, arg, size);
		data_ptr += size;
		bufsize += size;
	}

	rv = write(sock, buf, bufsize);
	if(rv == -1)
		exit_err("write");
	_log_cli("Successful write\n");

	rv = read(sock, buf, DEF_BUFSIZE);
	if(rv == -1)
		exit_err("read");

	_log_cli("%s\n", buf);

	close(sock);
}

void print_help() {
	printf("%s", help_str);
}

void * daemon_thread_func(void * arg) {
	int rv;
	struct sockaddr_un addr;
	unsigned char buf[DEF_BUFSIZE] = {0};
	command_t cmd;

	rv = init_unix_socket();
	if(rv < 0)
		exit_err("init_unix_socket");
	gconf.cli_socket = rv;

	unlink(SOCKET_PATH);
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

	rv = bind(gconf.cli_socket, (const struct sockaddr *) &addr,
			sizeof(struct sockaddr_un));
	if (rv == -1) {
		exit_err("bind");
	}

	rv = listen(gconf.cli_socket, 0);
	if (rv == -1) {
		exit_err("listen");
	}

	while(1) {
		int data_socket;
		void *arg;
		unsigned char send_buf[DEF_BUFSIZE] = {0};
		unsigned send_buf_size = 0;

		data_socket = accept(gconf.cli_socket, NULL, NULL);
		if(data_socket == -1)
			exit_err("accept");
		_log("Client connected\n");

		rv = read(data_socket, buf, DEF_BUFSIZE);
		if (rv == -1) {
			exit_err("read");
		}
		_log("Successful read\n");

		cmd = (command_t)(*buf);
		arg = buf + sizeof(command_t);
		process_cli_command(cmd, arg, send_buf, &send_buf_size);

		rv = write(data_socket, send_buf, send_buf_size);
		if (rv == -1) {
			exit_err("write");
		}
		_log("Successful write %d bytes\n", rv, send_buf_size);
		close(data_socket);
	}

	return NULL;
}
