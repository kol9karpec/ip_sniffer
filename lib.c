#include "lib.h"

const char* command_str[COMMANDS_NUM] = {
	"start",
	"stop",
	"show",
	"select",
	"stat",
	"help"
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

	if(check_iface_on(get_if_num(dest))) {
		ip_list_t *node = ip_in_list(gconf.ip_list, src);
		if (!node) {
			node = add_ip(&gconf.ip_list, src);
		}

		inc_ip_count(&node->data, get_if_num(dest));
	}

	return 0;
}

int init_ip_socket() {
	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if(fd < 0)
		exit_err("socket");

	return fd;
}

int init_unix_socket() {
	int fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if(fd < 0)
		exit_err("socket");

	return fd;
}

int process_cli_command(command_t cmd, void *arg,
		void *res_dest, int *size_dest) {
	_log("%s command arrived\n", command_str[cmd]);
	unsigned char *data_ptr = res_dest;
	unsigned data_size = 0;
	struct in_addr addr;
	int rv;

	char *def_ans = "OK";
	int def_ans_size = strlen("OK") + 1;

	switch(cmd) {
		case START:
			gconf.sniffing_stopped = 0;
			strncpy(data_ptr, def_ans, def_ans_size);
			data_size += def_ans_size;
			data_ptr += def_ans_size;
			break;
		case STOP:
			gconf.sniffing_stopped = 1;
			strncpy(data_ptr, def_ans, def_ans_size);
			data_size += def_ans_size;
			data_ptr += def_ans_size;
			break;
		case SHOW_IP_COUNT:
			
			break;
		case SELECT_IFACE:
			break;
		case STAT_IFACE:
			break;
		default:
			_log("Wrong command!\n");
			return 1;
	}

	*size_dest = data_size;

	return 0;
}

static int get_ip_count(struct sockaddr_in *sin) {
	ip_list_t *node = gconf.ip_list;
	int result = 0;
	int i = 0;
	while(node) {
		if(!equal(sin, &node->data.addr)) {
			for(i = 0; i < gconf.if_num; i++)
				result += node->data.if_count[i];
			break;
		}
		node = node->next;
	}
	_log("%s: %s\t%d", __func__, inet_ntoa(sin->sin_addr), result);
	return result;
}

static void switch_iface_status(int if_num) {
	if(check_iface_on(if_num))
		turn_iface_off(if_num);
	else
		turn_iface_on(if_num);
}

static int get_stats(int if_num, export_ip_stat_t **arr, int *arr_size) {
	ip_list_t *node = gconf.ip_list;
	int i = 0;
	int _arr_size = 0;

	_arr_size = 0;
	while (node) {
		_arr_size++;
		node = node->next;
	}

	*arr = malloc(sizeof(export_ip_stat_t)*_arr_size);
	node = gconf.ip_list;
	while(node) {
		(*arr)[i].addr = node->data.addr.sin_addr;
		(*arr)[i++].count = node->data.if_count[if_num];
		node = node->next;
	}
	*arr_size = _arr_size;

	_log("%s: %d", __func__, if_num);
	for(i=0;i<_arr_size;i++)
		_log("\t%s\t%d", inet_ntoa((*arr)[i].addr), (*arr)[i++].count);

	return 0;
}
