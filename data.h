#ifndef __DATA_H__
#define __DATA_H__

#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "conf.h"
#include "types.h"

/*
 * Checks addr for presence in list
 *
 * @return pointer to ip_list_t if present, NULL otherwise
 */
ip_list_t *ip_in_list(ip_list_t *head, struct sockaddr_in addr);

/* Must be called after fill_ifaces_list */
ip_list_t *add_ip(ip_list_t **head, struct sockaddr_in addr);

/*
 * Not thread-safe
 */
void inc_ip_count(ip_stat_t *data, int if_num);

/*
 * @return 0 if equal, non-zero otherwise
 */
int equal(struct sockaddr_in *a, struct sockaddr_in *b);

void deinit_list(ip_list_t *head);


#endif /* __DATA_H__ */
