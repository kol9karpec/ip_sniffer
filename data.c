#include "data.h"

int equal(struct sockaddr_in *a, struct sockaddr_in *b) {
#if 0
	char str1[256];
	char str2[256];
	strcpy(str1, inet_ntoa(a->sin_addr));
	strcpy(str2, inet_ntoa(b->sin_addr));
	_log("Comparing %s vs %s\n", str1, str2);
#endif
	return a->sin_addr.s_addr - b->sin_addr.s_addr;
}

ip_list_t *ip_in_list(ip_list_t *head, struct sockaddr_in addr) {
	while(head) {
		if(!equal(&head->data.addr, &addr)) {
			return head;
		}
		head = head->next;
	}

	_log("%s: ip %s is not in list!\n", __func__, inet_ntoa(addr.sin_addr));
	return head;
}

ip_list_t *add_ip(ip_list_t **head, struct sockaddr_in addr) {
	ip_list_t *new = malloc(sizeof(ip_list_t));
	ip_list_t *tmp = *head;
	if(!new) {
		perror("malloc");
		return NULL;
	}
	new->data.addr = addr;
	new->data.if_count = malloc(sizeof(unsigned)*gconf.if_num);
	new->next = NULL;
	memset(new->data.if_count, 0, sizeof(unsigned)*gconf.if_num);

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

void inc_ip_count(ip_stat_t *data, int if_num) {
	if(check_iface_on(if_num))
		data->if_count[if_num]++;
	_log("[%s] %s: %u\n",gconf.if_list[if_num].name,
			inet_ntoa(data->addr.sin_addr), data->if_count[if_num]);
}

void deinit_list(ip_list_t *head) {
	ip_list_t *next = head;
	do {
		head = next;
		next = head->next;
		free(head->data.if_count);
		free(head);
	} while(next);
}
