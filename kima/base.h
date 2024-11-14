#ifndef _KIMA_BASE_H_
#define _KIMA_BASE_H_

#include <stddef.h>

#define KIMA_PAGESIZE 4096
#define KIMA_PGOFF_MAX (KIMA_PAGESIZE - 1)

#define KIMA_CONTAINER_OF(t, m, p) ((t *)(((char *)p) - offsetof(t, m)))

#define KIMA_ARRAYLEN(a) (sizeof(a) / sizeof(*(a)))

#define KIMA_ISOVERLAPPED(p1, sz1, p2, sz2) (((p2) >= (p1) && (p2) < ((p1) + (sz1))) || ((p1) >= (p2) && (p1) < ((p2) + (sz2))))

/// @brief Round up a linear address into a paged address.
#define PGROUNDUP(addr) \
	(((uintptr_t)(addr) >> 12) + ((((uintptr_t)(addr)) & KIMA_PGOFF_MAX) ? 1 : 0))
/// @brief Round down a linear address into a paged address
#define PGROUNDDOWN(addr) ((((pgaddr_t)(addr))) >> 12)
/// @brief Round up a linear address into page-aligned.
#define KIMA_PGCEIL(addr) (((((size_t)(addr)) + KIMA_PGOFF_MAX) & (~KIMA_PGOFF_MAX)))
/// @brief Round down a linear address into page-aligned.
#define KIMA_PGFLOOR(addr) ((((size_t)(addr)) & (~KIMA_PGOFF_MAX)))

#endif
