#include "tree.h"

void destroy_tree(leaf_t *root) {
	if(root) {
		destroy_tree(root->left);
		destroy_tree(root->right);
		free(root);
	}
}

leaf_t * insert_tree(leaf_t **root, unsigned int value, comp_func_t cf) {
	leaf_t *result = NULL;
	if(cf == NULL)
		cf = def_comp_func;

	if (*root) {
		if (cf(value, (*root)->value) == 1) {
			result = insert_tree(&(*root)->right, value, cf);
		} else if (cf(value, (*root)->value) == -1) {
			result = insert_tree(&(*root)->left, value, cf);
		} else {
			_log("Tree node exists!\n");
			result = *root;
		}
	} else {
		*root = malloc(sizeof(leaf_t));
		if(*root) {
			(*root)->left = NULL;
			(*root)->right = NULL;
			(*root)->value = value;
			result = *root;
		} else {
			_log("Malloc error!\n");
		}

	}

	return result;
}

leaf_t * search_tree(leaf_t *root, unsigned int value, comp_func_t cf) {
	if(cf == NULL)
		cf = def_comp_func;

	if(!root)
		return NULL;

	if(cf(value, root->value) == 0)
		return root;
	else if(cf(value, root->value) == 1)
		return search_tree(root->right, value, cf);
	else /* (cf(value, root->value) == -1) */
		return search_tree(root->left, value, cf);

}
