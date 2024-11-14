#include "malloc.h"

void *kima_malloc(size_t size) {
	if (size < KIMA_PAGESIZE) {
		kima_rbtree_foreach(i, &kima_vpgdesc_query_tree) {
			kima_vpgdesc_t *cur_desc = KIMA_CONTAINER_OF(kima_vpgdesc_t, node_header, i);
			void *const limit = ((char *)cur_desc->ptr) + (KIMA_PAGESIZE - size);

			for (void *cur_base = cur_desc->ptr;
				 cur_base < limit;) {
				kima_ublk_t *nearest_ublk = kima_lookup_nearest_ublk(cur_base);
				if (nearest_ublk) {
					if (KIMA_ISOVERLAPPED((char *)cur_base, size, (char *)nearest_ublk->ptr, nearest_ublk->size)) {
						cur_base = ((char *)nearest_ublk->ptr) + nearest_ublk->size;
						goto skip1;
					}
				}

				kima_ublk_t *ublk = kima_alloc_ublk(cur_base, size);
				assert(ublk);

				return cur_base;

			skip1:;
			}
		}
	} else {
		void *filter_base = NULL;

		kima_rbtree_foreach(i, &kima_vpgdesc_query_tree) {
			kima_vpgdesc_t *cur_desc = KIMA_CONTAINER_OF(kima_vpgdesc_t, node_header, i);

			if (cur_desc->ptr < filter_base)
				continue;

			for (size_t j = 0;
				 j < KIMA_PGCEIL(size);
				 j += KIMA_PAGESIZE) {
				if (!kima_lookup_vpgdesc(((char *)cur_desc->ptr) + j)) {
					filter_base = ((char*)cur_desc->ptr) + j;
					goto noncontinuous;
				}
			}

			{
				void *const limit = ((char *)cur_desc->ptr) + (KIMA_PGCEIL(size) - size);

				for (void *cur_base = cur_desc->ptr;
					 cur_base < limit;) {
					kima_ublk_t *nearest_ublk = kima_lookup_nearest_ublk(cur_base);
					if (nearest_ublk) {
						if (KIMA_ISOVERLAPPED((char *)cur_base, size, (char *)nearest_ublk->ptr, nearest_ublk->size)) {
							cur_base = ((char *)nearest_ublk->ptr) + nearest_ublk->size;
							goto skip2;
						}
					}

					kima_ublk_t *ublk = kima_alloc_ublk(cur_base, size);
					assert(ublk);

					return cur_base;

				skip2:;
				}
			}

		noncontinuous:;
		}
	}

	void *new_free_pg = kima_vpgalloc(NULL, KIMA_PGCEIL(size));

	assert(new_free_pg);

	for (size_t i = 0; i < PGROUNDUP(size); ++i) {
		kima_vpgdesc_t *vpgdesc = kima_alloc_vpgdesc(((char *)new_free_pg) + i * KIMA_PAGESIZE);

		assert(vpgdesc);
	}

	kima_ublk_t *ublk = kima_alloc_ublk(new_free_pg, size);
	assert(ublk);

	return new_free_pg;
}

void kima_free(void *ptr) {
	kima_ublk_t *ublk = kima_lookup_ublk(ptr);
	assert(ublk);

	kima_free_ublk(ublk);
}
