#ifndef __DATA_H__
#define __DATA_H__

#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

typedef struct _if_count if_count_t;
struct _if_count {
	struct sockaddr_in if_addr;
	unsigned count;
};

typedef struct _ip_stat {
	struct sockaddr_in addr;
	if_count_t *ifaces;
} ip_stat_t;

typedef struct _ip_list ip_list_t;

struct _ip_list {
	ip_stat_t data;
	ip_list_t *next;
};

/*
 * Checks addr for presence in list
 *
 * @return pointer to ip_list_t if present, NULL otherwise
 */
ip_list_t *ip_in_list(ip_list_t *head, struct sockaddr_in addr);

ip_list_t *add_ip(ip_list_t **head, struct sockaddr_in addr);

/*
 * Not thread-safe
 */
void inc_ip_count(ip_stat_t *data);

/*
 * @return 0 if equal, non-zero otherwise
 */
int equal(struct sockaddr_in *a, struct sockaddr_in *b);

void deinit_list(ip_list_t *head);


#endif /* __DATA_H__ */
