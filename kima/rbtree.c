#include "rbtree.h"
#include <assert.h>

static void kima_rbtree_lrot(kima_rbtree_t *tree, kima_rbtree_node_t *x);
static void kima_rbtree_rrot(kima_rbtree_t *tree, kima_rbtree_node_t *x);
static void kima_rbtree_insert_fixup(kima_rbtree_t *tree, kima_rbtree_node_t *node);
static kima_rbtree_node_t* kima_rbtree_remove_fixup(kima_rbtree_t *tree, kima_rbtree_node_t *node);
static void kima_rbtree_walk_nodes_for_freeing(kima_rbtree_t *tree, kima_rbtree_node_t *node);

void kima_rbtree_init(kima_rbtree_t *dest,
	kima_rbtree_nodecmp_t node_cmp,
	kima_rbtree_nodefree_t node_free) {
	dest->node_cmp = node_cmp;
	dest->node_free = node_free;
	dest->root = NULL;
}

bool kima_rbtree_insert(kima_rbtree_t *tree, kima_rbtree_node_t *node) {
	assert(!node->l);
	assert(!node->r);

	if (!tree->root) {
		tree->root = node;
		kima_rbtree_setcolor(node, KIMA_RBTREE_BLACK);
		return true;
	}

	kima_rbtree_node_t *x = tree->root, *y = NULL;
	while (x) {
		y = x;
		if (tree->node_cmp(node, x))
			x = x->l;
		else if (tree->node_cmp(x, node))
			x = x->r;
		else
			return false;
	}

	if (tree->node_cmp(node, y))
		y->l = node;
	else
		y->r = node;
	kima_rbtree_setparent(node, y);
	kima_rbtree_setcolor(node, KIMA_RBTREE_RED);

	kima_rbtree_insert_fixup(tree, node);

	return true;
}

void kima_rbtree_remove(kima_rbtree_t *tree, kima_rbtree_node_t *node) {
	kima_rbtree_node_t *y = kima_rbtree_remove_fixup(tree, node);
	y->r = NULL;
	y->l = NULL;

	tree->node_free(y);
}

kima_rbtree_node_t *kima_rbtree_find(kima_rbtree_t *tree, kima_rbtree_node_t *node) {
	kima_rbtree_node_t *i = tree->root;
	while (i) {
		if (tree->node_cmp(i, node))
			i = i->r;
		else if (tree->node_cmp(node, i))
			i = i->l;
		else
			return i;
	}
	return NULL;
}

kima_rbtree_node_t **kima_rbtree_find_slot(kima_rbtree_t *tree, kima_rbtree_node_t *node, kima_rbtree_node_t **p_out) {
	kima_rbtree_node_t **i = &tree->root;
	*p_out = NULL;

	while (*i) {
		*p_out = *i;

		if (tree->node_cmp(*i, node))
			i = &(*i)->r;
		else if (tree->node_cmp(node, *i))
			i = &(*i)->l;
		else
			return NULL;
	}
	return i;
}

void kima_rbtree_free(kima_rbtree_t *tree) {
	if (tree->root)
		kima_rbtree_walk_nodes_for_freeing(tree, tree->root);
}

kima_rbtree_node_t *kima_rbtree_minnode(kima_rbtree_node_t *node) {
	while (node->l)
		node = node->l;
	return node;
}

kima_rbtree_node_t *kima_rbtree_maxnode(kima_rbtree_node_t *node) {
	while (node->r)
		node = node->r;
	return node;
}

kima_rbtree_node_t *kima_rbtree_next(kima_rbtree_node_t *node) {
	if (!node)
		return NULL;

	if (node->r) {
		node = kima_rbtree_minnode(node->r);
	} else {
		while (kima_rbtree_parent(node) && (node == kima_rbtree_parent(node)->r))
			node = kima_rbtree_parent(node);
		node = kima_rbtree_parent(node);
	}

	return node;
}

static void kima_rbtree_lrot(kima_rbtree_t *tree, kima_rbtree_node_t *x) {
	kima_rbtree_node_t *y = x->r;
	assert(y);

	x->r = y->l;
	if (y->l)
		kima_rbtree_setparent(y->l, x);

	kima_rbtree_setparent(y, kima_rbtree_parent(x));

	if (!kima_rbtree_parent(x))
		tree->root = y;
	else if (kima_rbtree_parent(x)->l == x)
		kima_rbtree_parent(x)->l = y;
	else
		kima_rbtree_parent(x)->r = y;

	y->l = x;
	kima_rbtree_setparent(x, y);
}

static void kima_rbtree_rrot(kima_rbtree_t *tree, kima_rbtree_node_t *x) {
	kima_rbtree_node_t *y = x->l;
	assert(y);

	x->l = y->r;
	if (y->r)
		kima_rbtree_setparent(y->r, x);

	kima_rbtree_setparent(y, kima_rbtree_parent(x));
	if (!kima_rbtree_parent(x))
		tree->root = y;
	else if (kima_rbtree_parent(x)->l == x)
		kima_rbtree_parent(x)->l = y;
	else
		kima_rbtree_parent(x)->r = y;

	y->r = x;
	kima_rbtree_setparent(x, y);
}

static void kima_rbtree_insert_fixup(kima_rbtree_t *tree, kima_rbtree_node_t *node) {
	kima_rbtree_node_t *p, *gp = node, *u;  // Parent, grandparent and uncle

	while ((p = kima_rbtree_parent(gp)) && kima_rbtree_isred(p)) {
		gp = kima_rbtree_parent(p);

		if (p == gp->l) {
			u = gp->r;

			if (kima_rbtree_isred(u)) {
				kima_rbtree_setcolor(p, KIMA_RBTREE_BLACK);
				kima_rbtree_setcolor(u, KIMA_RBTREE_BLACK);
				kima_rbtree_setcolor(gp, KIMA_RBTREE_RED);
				node = gp;
				continue;
			} else {
				if (node == p->r) {
					kima_rbtree_lrot(tree, p);

					{
						kima_rbtree_node_t *tmp;
						tmp = node;
						node = p;
						p = tmp;
					}
				}
				kima_rbtree_rrot(tree, gp);
				kima_rbtree_setcolor(p, KIMA_RBTREE_BLACK);
				kima_rbtree_setcolor(gp, KIMA_RBTREE_RED);
			}
		} else {
			u = gp->l;

			if (kima_rbtree_isred(u)) {
				kima_rbtree_setcolor(p, KIMA_RBTREE_BLACK);
				kima_rbtree_setcolor(u, KIMA_RBTREE_BLACK);
				kima_rbtree_setcolor(gp, KIMA_RBTREE_RED);
				node = gp;
				continue;
			} else {
				if (node == p->l) {
					kima_rbtree_rrot(tree, p);

					{
						kima_rbtree_node_t *tmp;
						tmp = node;
						node = p;
						p = tmp;
					}
				}
				kima_rbtree_lrot(tree, gp);
				kima_rbtree_setcolor(p, KIMA_RBTREE_BLACK);
				kima_rbtree_setcolor(gp, KIMA_RBTREE_RED);
			}
		}
	}

	kima_rbtree_setcolor(tree->root, KIMA_RBTREE_BLACK);
}

static kima_rbtree_node_t* kima_rbtree_remove_fixup(kima_rbtree_t *tree, kima_rbtree_node_t *node) {
	// The algorithm was from SGI STL's stl_tree, with minor improvements.
	kima_rbtree_node_t *y = node, *x, *p;

	if (!y->l)
		// The node has right child only.
		x = y->r;
	else if (!y->r) {
		// The node has left child only.
		x = y->l;
	} else {
		// The node has two children.
		y = kima_rbtree_minnode(y->r);
		x = y->r;
	}

	if (y != node) {
		kima_rbtree_setparent(node->l, y);
		y->l = node->l;

		if (y != node->r) {
			p = kima_rbtree_parent(y);
			if (x)
				kima_rbtree_setparent(x, kima_rbtree_parent(y));
			kima_rbtree_parent(y)->l = x;
			y->r = node->r;
			kima_rbtree_setparent(node->r, y);
		} else
			p = y;

		if (tree->root == node)
			tree->root = y;
		else if (kima_rbtree_parent(node)->l == node)
			kima_rbtree_parent(node)->l = y;
		else
			kima_rbtree_parent(node)->r = y;

		kima_rbtree_setparent(y, kima_rbtree_parent(node));

		{
			bool color = kima_rbtree_color(y);
			kima_rbtree_setcolor(y, kima_rbtree_color(node));
			kima_rbtree_setcolor(node, color);
		}
		y = node;
	} else {
		p = kima_rbtree_parent(y);
		if (x)
			kima_rbtree_setparent(x, kima_rbtree_parent(y));

		if (tree->root == node)
			tree->root = x;
		else if (kima_rbtree_parent(node)->l == node)
			kima_rbtree_parent(node)->l = x;
		else
			kima_rbtree_parent(node)->r = x;
	}

	if (kima_rbtree_isblack(y)) {
		while (x != tree->root && kima_rbtree_isblack(x)) {
			if (x == p->l) {
				kima_rbtree_node_t *w = p->r;

				if (kima_rbtree_isred(w)) {
					kima_rbtree_setcolor(w, KIMA_RBTREE_BLACK);
					kima_rbtree_setcolor(p, KIMA_RBTREE_RED);
					kima_rbtree_lrot(tree, p);
					w = p->r;
				}

				if (kima_rbtree_isblack(w->l) && kima_rbtree_isblack(w->r)) {
					kima_rbtree_setcolor(w, KIMA_RBTREE_RED);
					x = p;
					p = kima_rbtree_parent(p);
				} else {
					if (kima_rbtree_isblack(w->r)) {
						if (w->l)
							kima_rbtree_setcolor(w->l, KIMA_RBTREE_BLACK);
						kima_rbtree_setcolor(w, KIMA_RBTREE_RED);
						kima_rbtree_rrot(tree, w);
						w = p->r;
					}
					kima_rbtree_setcolor(w, kima_rbtree_color(p));
					kima_rbtree_setcolor(p, KIMA_RBTREE_BLACK);
					if (w->r)
						kima_rbtree_setcolor(w->r, KIMA_RBTREE_BLACK);
					kima_rbtree_lrot(tree, p);
					break;
				}
			} else {
				kima_rbtree_node_t *w = p->l;

				if (kima_rbtree_isred(w)) {
					kima_rbtree_setcolor(w, KIMA_RBTREE_BLACK);
					kima_rbtree_setcolor(p, KIMA_RBTREE_RED);
					kima_rbtree_rrot(tree, p);
					w = p->l;
				}

				if (kima_rbtree_isblack(w->r) && kima_rbtree_isblack(w->l)) {
					kima_rbtree_setcolor(w, KIMA_RBTREE_RED);
					x = p;
					p = kima_rbtree_parent(p);
				} else {
					if (kima_rbtree_isblack(w->l)) {
						if (w->r)
							kima_rbtree_setcolor(w->r, KIMA_RBTREE_BLACK);
						kima_rbtree_setcolor(w, KIMA_RBTREE_RED);
						kima_rbtree_lrot(tree, w);
						w = p->l;
					}
					kima_rbtree_setcolor(w, kima_rbtree_color(p));
					kima_rbtree_setcolor(p, KIMA_RBTREE_BLACK);
					if (w->l)
						kima_rbtree_setcolor(w->l, KIMA_RBTREE_BLACK);
					kima_rbtree_rrot(tree, p);
					break;
				}
			}
		}
		if (x)
			kima_rbtree_setcolor(x, KIMA_RBTREE_BLACK);
	}

	return y;
}

static void kima_rbtree_walk_nodes_for_freeing(kima_rbtree_t *tree, kima_rbtree_node_t *node) {
	if (node->l)
		kima_rbtree_walk_nodes_for_freeing(tree, node->l);
	if (node->r)
		kima_rbtree_walk_nodes_for_freeing(tree, node->r);
	tree->node_free(node);
}
