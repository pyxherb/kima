#include "malloc.h"

void *kima_malloc(size_t size) {
	bool retried = false;
retry_alloc:;
	void *filter_base = NULL;

	kima_rbtree_foreach(i, &kima_vpgdesc_query_tree) {
		kima_vpgdesc_t *cur_desc = KIMA_CONTAINER_OF(kima_vpgdesc_t, node_header, i);

		if (cur_desc->ptr < filter_base)
			continue;

		for (size_t j = 0;
			 j < KIMA_PGCEIL(size);
			 j += KIMA_PAGESIZE) {
			if (!kima_lookup_vpgdesc(((char *)cur_desc->ptr) + j)) {
				filter_base = ((char *)cur_desc->ptr) + j;
				goto noncontinuous;
			}
		}

		{
			void *const limit = ((char *)cur_desc->ptr) + (KIMA_PGCEIL(size) - size);

			for (void *cur_base = cur_desc->ptr;
				 cur_base <= limit;) {
				kima_ublk_t *nearest_ublk;
				if ((nearest_ublk = kima_lookup_nearest_ublk(cur_base))) {
					if (KIMA_ISOVERLAPPED((char *)cur_base, size, (char *)nearest_ublk->ptr, nearest_ublk->size)) {
						cur_base = ((char *)nearest_ublk->ptr) + nearest_ublk->size;
						continue;
					}
				}
				if ((nearest_ublk = kima_lookup_nearest_ublk(((char *)cur_base) + size - 1))) {
					if (KIMA_ISOVERLAPPED((char *)cur_base, size, (char *)nearest_ublk->ptr, nearest_ublk->size)) {
						cur_base = ((char *)nearest_ublk->ptr) + nearest_ublk->size;
						continue;
					}
				}

				kima_ublk_t *ublk = kima_alloc_ublk(cur_base, size);
				assert(ublk);

				for (size_t j = 0;
					 j < KIMA_PGCEIL(size);
					 j += KIMA_PAGESIZE) {
					kima_vpgdesc_t *vpgdesc = kima_lookup_vpgdesc(((char *)cur_desc->ptr) + j);

					assert(vpgdesc);

					++vpgdesc->ref_count;
				}

				return cur_base;
			}
		}

	noncontinuous:;
	}

	assert(!retried);

	void *new_free_pg = kima_vpgalloc(NULL, KIMA_PGCEIL(size));

	assert(new_free_pg);

	for (size_t i = 0; i < PGROUNDUP(size); ++i) {
		kima_vpgdesc_t *vpgdesc = kima_alloc_vpgdesc(((char *)new_free_pg) + i * KIMA_PAGESIZE);

		assert(vpgdesc);
	}

	retried = true;

	goto retry_alloc;
}

void kima_free(void *ptr) {
	kima_ublk_t *ublk = kima_lookup_ublk(ptr);
	assert(ublk);
	for (uintptr_t i = KIMA_PGFLOOR(ublk->ptr);
		 i < KIMA_PGCEIL(((char *)ublk->ptr) + ublk->size);
		 i += KIMA_PAGESIZE) {
		kima_vpgdesc_t *vpgdesc = kima_lookup_vpgdesc((void *)i);

		assert(vpgdesc);

		if (!(--vpgdesc->ref_count))
			kima_free_vpgdesc(vpgdesc);
	}

	kima_free_ublk(ublk);
}
