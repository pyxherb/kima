#include "vpgdesc.h"

kima_vpgdesc_poolpg_t *kima_vpgdesc_poolpg_list = NULL;
kima_rbtree_t kima_vpgdesc_query_tree;

bool kima_vpgdesc_nodecmp(const kima_rbtree_node_t* x, const kima_rbtree_node_t* y) {
	kima_vpgdesc_t *_x = KIMA_CONTAINER_OF(kima_vpgdesc_t, node_header, x),
				   *_y = KIMA_CONTAINER_OF(kima_vpgdesc_t, node_header, y);

	return _x->ptr < _y->ptr;
}

void kima_vpgdesc_nodefree(kima_rbtree_node_t* p) {
	kima_vpgdesc_t *_p = KIMA_CONTAINER_OF(kima_vpgdesc_t, node_header, p);

	_p->ptr = NULL;
}

kima_vpgdesc_t* kima_lookup_vpgdesc(void* ptr) {
	kima_vpgdesc_t query_desc = {
		.ptr = ptr
	};

	kima_rbtree_node_t *node = kima_rbtree_find(&kima_vpgdesc_query_tree, &query_desc);

	if (!node)
		return NULL;

	return KIMA_CONTAINER_OF(kima_vpgdesc_t, node_header, node);
}

void kima_free_vpgdesc(kima_vpgdesc_t* vpgdesc) {
	kima_vpgdesc_poolpg_t *poolpg = KIMA_PGFLOOR(vpgdesc);

	kima_rbtree_remove(&kima_vpgdesc_query_tree, &vpgdesc->node_header);

	if (!(--poolpg->header.used_num)) {
		if (poolpg->header.prev)
			poolpg->header.prev->header.next = poolpg->header.next;
		if (poolpg->header.next)
			poolpg->header.next->header.prev = poolpg->header.prev;
		kima_vpgfree(poolpg, KIMA_PAGESIZE);
	}
}

kima_vpgdesc_t* kima_alloc_vpgdesc(void* ptr) {
	for (kima_vpgdesc_poolpg_t* pg = kima_vpgdesc_poolpg_list;
		pg;
		pg = pg->header.next) {
		if (pg->header.used_num >= KIMA_ARRAYLEN(pg->slots)) {
			continue;
		}
		for (size_t i = 0; i < KIMA_ARRAYLEN(pg->slots); ++i) {
			if (!pg->slots[i].ptr) {
				++pg->header.used_num;
				pg->slots[i].ptr = ptr;
				pg->slots[i].ref_count = 0;
				memset(&pg->slots[i].node_header, 0, sizeof(pg->slots[i].node_header));
				kima_rbtree_insert(&kima_vpgdesc_query_tree, &pg->slots[i].node_header);
				return &pg->slots[i];
			}
		}
	}

	kima_vpgdesc_poolpg_t *pg = kima_vpgalloc(NULL, KIMA_PAGESIZE);

	memset(pg->slots, 0, sizeof(pg->slots));

	pg->header.next = kima_vpgdesc_poolpg_list;
	if (kima_vpgdesc_poolpg_list) {
		kima_vpgdesc_poolpg_list->header.prev = pg;
	}
	kima_vpgdesc_poolpg_list = pg;

	pg->header.used_num = 1;
	pg->slots[0].ptr = ptr;
	pg->slots[0].ref_count = 0;
	memset(&pg->slots[0].node_header, 0, sizeof(pg->slots[0].node_header));
	kima_rbtree_insert(&kima_vpgdesc_query_tree, &pg->slots[0].node_header);
	return &pg->slots[0];
}
