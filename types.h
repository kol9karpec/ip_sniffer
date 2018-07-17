#ifndef __TYPES_H__
#define __TYPES_H__

#define IF_NAME_SIZE 40
#include <pthread.h>

typedef struct {
	char name[IF_NAME_SIZE];
	struct sockaddr_in addr;
} iface_t;

typedef struct _ip_stat {
	struct sockaddr_in addr;

	/* Array of data per interface */
	unsigned *if_count;
} ip_stat_t;

typedef struct _ip_list ip_list_t;

struct _ip_list {
	ip_stat_t data;
	ip_list_t *next;
};

typedef struct {
	char *daemon_log_file;
	char *daemon_pid_file;

	pthread_t wait_thread;

	/* Interfaces list may change. Now it is not handled. */
	iface_t *if_list;
	unsigned if_mask;
	unsigned if_num;

	unsigned ip_socket;
	unsigned cli_socket;
	ip_list_t *ip_list;

	int sniffing_stopped;
} config_t;

#endif /* __TYPES_H__ */
