#ifndef _KIMA_RBTREE_H_
#define _KIMA_RBTREE_H_

#include <stdbool.h>
#include <stdint.h>
#include "base.h"

#define KIMA_RBTREE_BLACK 0
#define KIMA_RBTREE_RED 1

#define kima_rbtree_parent(n) ((n)->p)
#define kima_rbtree_setparent(n, _p) ((n)->p = (_p))

#define kima_rbtree_color(n) ((n)->color)
#define kima_rbtree_setcolor(n, c) ((n)->color = c);

#define kima_rbtree_isblack(n) ((!n) || (kima_rbtree_color(n) == KIMA_RBTREE_BLACK))
#define kima_rbtree_isred(n) ((n) && (kima_rbtree_color(n) == KIMA_RBTREE_RED))

typedef struct _kima_rbtree_node_t {
	struct _kima_rbtree_node_t *l, *r, *p;
	bool color;
} kima_rbtree_node_t;

typedef bool (*kima_rbtree_nodecmp_t)(const kima_rbtree_node_t *x, const kima_rbtree_node_t *y);
typedef void (*kima_rbtree_nodefree_t)(kima_rbtree_node_t *p);

typedef struct _kima_rbtree_t {
	kima_rbtree_node_t *root;
	kima_rbtree_nodecmp_t node_cmp;
	kima_rbtree_nodefree_t node_free;
} kima_rbtree_t;

kima_rbtree_node_t *kima_rbtree_minnode(kima_rbtree_node_t *node);
kima_rbtree_node_t *kima_rbtree_maxnode(kima_rbtree_node_t *node);

bool kima_rbtree_insert(kima_rbtree_t *tree, kima_rbtree_node_t *node);
void kima_rbtree_remove(kima_rbtree_t *tree, kima_rbtree_node_t *node);
kima_rbtree_node_t *kima_rbtree_find(kima_rbtree_t *tree, kima_rbtree_node_t *node);
kima_rbtree_node_t **kima_rbtree_find_slot(kima_rbtree_t *tree, kima_rbtree_node_t *node, kima_rbtree_node_t **p_out);
kima_rbtree_node_t *kima_rbtree_find_max_node(kima_rbtree_t *tree, kima_rbtree_node_t *node);
kima_rbtree_node_t *kima_rbtree_find_max_node(kima_rbtree_t *tree, kima_rbtree_node_t *node);
void kima_rbtree_free(kima_rbtree_t *tree);

#define kima_rbtree_clear(tree) kima_rbtree_free(tree)

void kima_rbtree_init(kima_rbtree_t *dest,
	kima_rbtree_nodecmp_t node_cmp,
	kima_rbtree_nodefree_t node_free);

#define kima_rbtree_begin(tree) ((tree)->root ? kima_rbtree_minnode((tree)->root) : NULL)
#define kima_rbtree_end(tree) ((tree)->root ? kima_rbtree_maxnode((tree)->root) : NULL)
kima_rbtree_node_t *kima_rbtree_next(kima_rbtree_node_t *node);

#define kima_rbtree_foreach(i, tree) for (kima_rbtree_node_t *i = kima_rbtree_begin(tree); i; i = kima_rbtree_next(i))

#define kima_rbtree_isempty(tree) ((tree)->root == NULL)

#endif
