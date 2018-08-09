#ifndef __TREE_H__
#define __TREE_H__

#include "log.h"
#include <stdlib.h>


typedef struct _leaf_t leaf_t;
struct _leaf_t {
	unsigned int value;
	leaf_t *left;
	leaf_t *right;
};

/*
 * Compare function that must return:
 *		-1 if first arg < second arg
 *		0 if first arg == second arg
 *		1 if first arg > second arg
 */
typedef int (*comp_func_t)(unsigned int, unsigned int);

static inline def_comp_func(unsigned a, unsigned b) {
	if (a > b) return 1;
	else if (a < b) return -1;
	else return 0;
}

void destroy_tree(leaf_t *root);
leaf_t * insert_tree(leaf_t **root, unsigned int value, comp_func_t cf);
leaf_t * search_tree(leaf_t *root, unsigned int value, comp_func_t cf);

#endif /* __TREE_H__ */
