#include "lib.h"

int main(int argc, const char * argv[]) {

	if(argc < 2)
		return 1;

	command_t cmd;
	if (!strncmp(argv[1], command_str[START], strlen(command_str[START]))) {
		_log("start!\n");
	} else if (!strncmp(argv[1], command_str[STOP], strlen(command_str[STOP]))) {
		_log("stop!\n");
	} else if (!strncmp(argv[1], command_str[SHOW_IP_COUNT], strlen(command_str[SHOW_IP_COUNT]))) {
		//TODO: handle parameter
		_log("show_ip_count!\n");
	} else if (!strncmp(argv[1], command_str[SELECT_IFACE], strlen(command_str[SELECT_IFACE]))) {
		//TODO: handle parameter
		_log("select_iface!\n");
	} else if (!strncmp(argv[1], command_str[STAT_IFACE], strlen(command_str[STAT_IFACE]))) {
		//TODO: handle parameter
		_log("stat_iface!\n");
	} else if (!strncmp(argv[1], command_str[HELP], strlen(command_str[HELP]))) {
		_log("help!\n");
	}

#if 0
	run_daemon();
#endif

	return 0;
}
