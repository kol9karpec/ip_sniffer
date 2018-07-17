#include "lib.h"

void run_daemon();
void print_help();
void run_cli(command_t cmd);
void *daemon_thread_func(void * arg);
const char * help_str = "Usage:...";

int main(int argc, const char * argv[]) {
	if(argc == 1)
		run_daemon();
	else {
		command_t cmd = START;

		if (!strncmp(argv[1], command_str[START], strlen(command_str[START]))) {
			cmd = START;
			_log("start!\n");
		} else if (!strncmp(argv[1], command_str[STOP],
					strlen(command_str[STOP]))) {
			cmd = STOP;
			_log("stop!\n");
		} else if (!strncmp(argv[1], command_str[SHOW_IP_COUNT],
					strlen(command_str[SHOW_IP_COUNT]))) {
			//TODO: handle parameter
			cmd = SHOW_IP_COUNT;
			_log("show_ip_count!\n");
		} else if (!strncmp(argv[1], command_str[SELECT_IFACE],
					strlen(command_str[SELECT_IFACE]))) {
			//TODO: handle parameter
			cmd = SELECT_IFACE;
			_log("select_iface!\n");
		} else if (!strncmp(argv[1], command_str[STAT_IFACE],
					strlen(command_str[STAT_IFACE]))) {
			//TODO: handle parameter
			cmd = STAT_IFACE;
			_log("stat_iface!\n");
		} else if (!strncmp(argv[1], command_str[HELP],
					strlen(command_str[HELP]))) {
			print_help();
			return 1;
		}
		run_cli(cmd);
	}

	return 0;
}


void run_daemon() {
	int retval = 0;
	unsigned char *buffer = (unsigned char *)malloc(DEF_BUFSIZE);
	int data_size;

	pthread_create(&gconf.wait_thread, NULL,
			daemon_thread_func, NULL);
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

	gconf.sniffing_stopped = 1;
	while(1) {
		if(!gconf.sniffing_stopped) {
			data_size = capture_packet(buffer, DEF_BUFSIZE);
			if(data_size < 0)
				exit_err("capture_packet");
			process_ip_packet(buffer, data_size);
		}
	}
}

void run_cli(command_t cmd) {
	int rv;
	int sock;
	struct sockaddr_un addr;
	unsigned char buf[DEF_BUFSIZE] = {0};

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
	_log("Connection successful!\n");


	rv = write(sock, (void *)&cmd, sizeof(command_t));
	if(rv == -1)
		exit_err("write");

	_log("Successful write");

	rv = read(sock, buf, DEF_BUFSIZE);
	if(rv == -1)
		exit_err("read");
	if(!strcmp(buf, "OK"))
		_log("Command successfully derived!\n");

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
		data_socket = accept(gconf.cli_socket, NULL, NULL);
		if(rv == -1)
			exit_err("accept");

		_log("Client connected\n");

		rv = read(data_socket, buf, DEF_BUFSIZE);
		if (rv == -1) {
			exit_err("read");
		}
		_log("Successful read\n");
		cmd = (command_t)*buf;
		process_cli_command(cmd, NULL);

		rv = write(data_socket, "OK", strlen("OK") + 1);
		if (rv == -1) {
			exit_err("write");
		}
		_log("Successful write\n");
		close(data_socket);
	}

	return NULL;
}
