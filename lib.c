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
	int data_size;

	data_size = recv(gconf.ip_socket, buffer, size, 0);
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

		if (!node) {
			_log("add_ip failed!\n");
			return 1;
		}

		inc_ip_count(&node->data, get_if_num(dest));
	}

	return 0;
}

int init_ip_socket() {
	int fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
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

	_log("%s: %s\t%d\n", __func__, inet_ntoa(sin->sin_addr), result);
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
		if(node->data.if_count[if_num]) {
			_arr_size++;
		}
		node = node->next;
	}

	if(_arr_size) {
		*arr = malloc(sizeof(export_ip_stat_t)*_arr_size);
		if(!(*arr)) {
			_log("malloc error: %s\n", strerror(errno));
			return 1;
		}
		node = gconf.ip_list;
		while(node) {
			if(node->data.if_count[if_num]) {
				(*arr)[i].addr = node->data.addr.sin_addr;
				(*arr)[i++].count = node->data.if_count[if_num];
			}
			node = node->next;
		}
	}
	*arr_size = _arr_size;

	_log("%s: %d\n", __func__, if_num);
	for(i=0;i<_arr_size;i++)
		_log("\t%s\t%d\n", inet_ntoa((*arr)[i].addr), (*arr)[i].count);

	return 0;
}

#define ANS(data_ptr, data_size, src, size)		\
	do {										\
			strncpy((data_ptr), (src), (size));	\
			(data_size) += (size);				\
			(data_ptr) += (size);				\
	} while(0);

int process_cli_command(command_t cmd, void *arg,
		void *res_dest, int *size_dest) {
	unsigned char *data_ptr = res_dest;
	unsigned data_size = 0;
	struct in_addr _addr;
	struct sockaddr_in addr;
	int rv;
	export_ip_stat_t *arr;
	int arr_size = 0;
	int i, j;

	char *ok_ans = "OK";
	int ok_ans_size = strlen(ok_ans) + 1;

	char *err_ans = "ERROR";
	int err_ans_size = strlen(err_ans) + 1;

	char ans[DEF_BUFSIZE];

	_log("%s command arrived\n", command_str[cmd]);
	switch(cmd) {
		case START:
			gconf.sniffing_stopped = 0;
			ANS(data_ptr, data_size, ok_ans, ok_ans_size)
			break;

		case STOP:
			gconf.sniffing_stopped = 1;
			ANS(data_ptr, data_size, ok_ans, ok_ans_size)
			break;

		case SHOW_IP_COUNT:
			rv = inet_aton(arg, &_addr);
			if(!rv) {
				ANS(data_ptr, data_size, err_ans, err_ans_size)
			} else {
				addr.sin_addr = _addr;
				rv = get_ip_count(&addr);
				sprintf(ans, "%d", rv);
				ANS(data_ptr, data_size, ans, strlen(ans)+1)
			}
			break;

		case SELECT_IFACE:
			rv = get_if_num_name(arg);
			if(rv == -1)
				ANS(data_ptr, data_size, err_ans, err_ans_size)
			else {
				ANS(data_ptr, data_size, ok_ans, ok_ans_size)
				switch_iface_status(rv);
			}
			break;

		case STAT_IFACE:
			if(!strcmp(arg, "all")) {
				for(j=0;j<gconf.if_num;j++) {
					rv = get_stats(j, &arr, &arr_size);
					if(rv) {
						_log("get_stats() error!\n");
						return 1;
					}
					sprintf(ans, "\t%s: \n", gconf.if_list[j].name);
					ANS(data_ptr, data_size, ans, strlen(ans))
					for(i=0;i<arr_size;i++) {
						sprintf(ans, "%s:\t%d\n", inet_ntoa(arr[i].addr),
								arr[i].count);
						ANS(data_ptr, data_size, ans, strlen(ans))
					}
					if(arr_size)
						free(arr);
				}
				ANS(data_ptr, data_size, "", 1)
			} else {
				rv = get_if_num_name(arg);
				if(rv == -1) {
					ANS(data_ptr, data_size, err_ans, err_ans_size)
				} else {
					get_stats(rv, &arr, &arr_size);
					sprintf(ans, "\t%s: \n", arg);
					ANS(data_ptr, data_size, ans, strlen(ans))
					for(i=0;i<arr_size;i++) {
						sprintf(ans, "%s:\t%d\n", inet_ntoa(arr[i].addr),
									arr[i].count);
						ANS(data_ptr, data_size, ans, strlen(ans))
					}
					ANS(data_ptr, data_size, "", 1)
					if(arr_size)
						free(arr);
				}
			}
			break;

		default:
			ANS(data_ptr, data_size, err_ans, err_ans_size)
			_log("Wrong command!\n");
			return 1;
	}

	*size_dest = data_size;
	return 0;
}

#define TRY_FWRITE(ptr, size, memnum, fp, rv) \
	do { \
		(rv) = fwrite((ptr), (size), (memnum), (fp)); \
		if(!(rv)) { \
			_log("fwrite error: %s\n", strerror(errno)); \
			return 1; \
		} \
	} while(0);

/*
 * The function writes statistics to the file in the next format:
 *
 * <Interfaces count>
 *	[Interface #1 name (char[IF_NAME_SIZE])]
 *	[Interface #2 name (char[IF_NAME_SIZE])]
 *	...
 *	[Interface #last name (char[IF_NAME_SIZE])]
 * <Number of ip addresses count>
 *	[IP address #1 (struct in_addr)]
 *		[count for interface #1 (unsigned)]
 *		[count for interface #2 (unsigned)]
 *		...
 *		[count for interface #last (unsigned)]
 *	[IP address #2 (struct in_addr)]
 *		[count for interface #1 (unsigned)]
 *		[count for interface #2 (unsigned)]
 *		...
 *		[count for interface #last (unsigned)]
 *	...
 *	[IP address #last (struct in_addr)]
 *		[count for interface #1 (unsigned)]
 *		[count for interface #2 (unsigned)]
 *		...
 *		[count for interface #last (unsigned)]
 *
 */
static int save_stats(char * filename) {
	ip_list_t *node = gconf.ip_list;
	iface_t *if_list = gconf.if_list;
	unsigned if_num = gconf.if_num;

	int i = 0;
	int ip_num = 0;
	int rv = 0;

	FILE * fp = fopen(filename, "wb");
	if(!fp) {
		_log("fopen error: %s\n", strerror(errno));
		return 1;
	}

	/* Writing interfaces to the file */
	TRY_FWRITE(&if_num, sizeof(if_num), 1, fp, rv)

	for (i = 0; i < if_num; i++) {
		TRY_FWRITE(gconf.if_list[i].name, sizeof(gconf.if_list[i].name),
					1, fp, rv)
	}

	/* Writing ip addresse's count to the file */
	while (node) {
		ip_num++;
		node = node->next;
	}

	TRY_FWRITE(&ip_num, sizeof(ip_num), 1, fp, rv)
	node = gconf.ip_list;
	while (node) {
		TRY_FWRITE(&node->data.addr, sizeof(node->data.addr), 1, fp, rv);
		for(i = 0; i < if_num; i++) {
			TRY_FWRITE(&node->data.if_count[i],
					sizeof(node->data.if_count[i]), 1, fp, rv);
		}
		node = node->next;
	}

	_log("Successful write to file!\n");

	fclose(fp);
	return 0;
}
#undef TRY_FWRITE

#define TRY_FREAD(ptr, size, memnum, fp, rv) \
	do { \
		(rv) = fread((ptr), (size), (memnum), (fp)); \
		if(!(rv)) { \
			_log("fread error: %s\n", strerror(errno)); \
			return 1; \
		} \
	} while(0);

/*
 * The function reads statistics from file
 */
int read_stats(char * filename) {
	ip_list_t *ip_list = gconf.ip_list,
			  *node = NULL;

	unsigned _if_num = 0;
	unsigned _ip_num = 0;
	char **_if_names_list = NULL;
	struct sockaddr_in _sock_addr = {0};
	unsigned _count = 0;

	int i = 0, j = 0;
	int rv = 0;

	FILE * fp = fopen(filename, "rb");
	if(!fp) {
		_log("fopen error: %s\n", strerror(errno));
		return 1;
	}

	/* Reading interfaces list from the file */
	TRY_FREAD(&_if_num, sizeof(_if_num), 1, fp, rv)
	_log("Read from file: if_num = %u\n", _if_num);
	_if_names_list = malloc(sizeof(char *) * _if_num);
	if(!_if_names_list) {
		_log("malloc error: %s\n", strerror(errno));
		return 1;
	}

	for (i = 0; i < _if_num; i++) {
		_if_names_list[i] = malloc(IF_NAME_SIZE);
		if(!_if_names_list[i]) {
			_log("malloc error: %s\n", strerror(errno));
			free(_if_names_list);
			return 1;
		}

		TRY_FREAD(_if_names_list[i], IF_NAME_SIZE,
					1, fp, rv)
		_log("Read from file: if_name[%d] = %s\n", i, _if_names_list[i]);
	}

	/* Reading ip list from the file */
	TRY_FREAD(&_ip_num, sizeof(_ip_num), 1, fp, rv)
	_log("Read ip_num = %u from file\n", _ip_num);
	for (i = 0; i < _ip_num; i++) {
		TRY_FREAD(&_sock_addr, sizeof(_sock_addr), 1, fp, rv)
		_log("Read ip %s from file\n", inet_ntoa(_sock_addr.sin_addr));
		if (!(node = ip_in_list(ip_list, _sock_addr))) {
			node = add_ip(&gconf.ip_list, _sock_addr);
		}

		for(j = 0; j < _if_num; j++) {
			TRY_FREAD(&_count, sizeof(_count), 1, fp, rv);
			rv = get_if_num_name(_if_names_list[j]);
			if (rv < 0)
				_log("The interface from the backup file is not in current list!\n");
			else
				node->data.if_count[rv] += _count;

			_log("Read ip count for if #%d\t%u\n", rv, _count);
		}
	}

	_log("Successful read from the file!\n");

	for(i = 0; i < _if_num; i++)
		free(_if_names_list[i]);
	free(_if_names_list);

	fclose(fp);

	return 0;
}
#undef TRY_FREAD

void sigint_handler(int signum) {
	_log("SIGINT received!\n");
	//TODO: save stats to file here
	save_stats(DEF_BACKUP_FILE);
	deinit_conf();
	exit(0);
}
