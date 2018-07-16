#include "data.h"

int equal(struct sockaddr_in *a, struct sockaddr_in *b) {
	return a->sin_addr.s_addr - b->sin_addr.s_addr;
}

ip_list_t *ip_in_list(ip_list_t *head, struct sockaddr_in addr) {
	while(head) {
		if(!equal(&head->data.addr, &addr)) {
#if 0
			_log("%s: ip %s is in list!\n", __func__, inet_ntoa(addr.sin_addr));
#endif
			return head;
		}
		head = head->next;
	}

#if 0
	_log("%s: ip %s is not in list!\n", __func__, inet_ntoa(addr.sin_addr));
#endif
	return head;
}

ip_list_t *add_ip(ip_list_t **head, struct sockaddr_in addr) {
	ip_list_t *new = malloc(sizeof(ip_list_t));
	ip_list_t *tmp = *head;
	if(!new) {
		perror("malloc");
		return NULL;
	}
	//TODO: per interface
	new->data.addr = addr;
	new->data.count = 0;

	if (tmp) {
		while(tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	} else {
		*head = new;
	}

	_log("%s: ip %s added!\n", __func__, inet_ntoa(addr.sin_addr));
	return new;
}

void inc_ip_count(ip_stat_t *data) {
	//TODO: per interface
	data->count++;
	_log("%s: %u\n",inet_ntoa(data->addr.sin_addr), data->count);
}

void deinit_list(ip_list_t *head) {
	ip_list_t *next = head;
	do {
		//TODO: deinit per interface
		head = next;
		next = head->next;
		free(head);
	} while(next);
}
