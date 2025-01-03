#ifndef _KIMA_VPGDESC_H_
#define _KIMA_VPGDESC_H_

#include "vmalloc.h"
#include "rbtree.h"
#include <assert.h>

typedef struct _kima_vpgdesc_t {
	kima_rbtree_node_t node_header;
	size_t ref_count;
	void *ptr;
} kima_vpgdesc_t;

typedef struct _kima_vpgdesc_poolpg_t kima_vpgdesc_poolpg_t;

typedef struct _kima_vpgdesc_poolpg_header_t {
	kima_vpgdesc_poolpg_t *prev, *next;
	size_t used_num;
} kima_vpgdesc_poolpg_header_t;

typedef struct _kima_vpgdesc_poolpg_t {
	kima_vpgdesc_poolpg_header_t header;
	kima_vpgdesc_t slots[(KIMA_PAGESIZE - sizeof(kima_vpgdesc_poolpg_header_t)) / sizeof(kima_vpgdesc_t)];
} kima_vpgdesc_poolpg_t;

extern kima_vpgdesc_poolpg_t *kima_vpgdesc_poolpg_list;
extern kima_rbtree_t kima_vpgdesc_query_tree;

bool kima_vpgdesc_nodecmp(const kima_rbtree_node_t *x, const kima_rbtree_node_t *y);
void kima_vpgdesc_nodefree(kima_rbtree_node_t *p);

kima_vpgdesc_t *kima_lookup_vpgdesc(void *ptr);
kima_vpgdesc_t *kima_alloc_vpgdesc(void *ptr);
kima_vpgdesc_t *kima_lookup_nearest_vpgdesc(void *ptr);
void kima_free_vpgdesc(kima_vpgdesc_t *vpgdesc);

#endif
