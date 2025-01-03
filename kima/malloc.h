#ifndef _KIMA_MALLOC_H_
#define _KIMA_MALLOC_H_

#include "vpgdesc.h"
#include "ublk.h"

typedef uint32_t kima_malloc_flags_t;

#define _KIMA_MALLOC_REALLOC 0x80000000

void *kima_malloc(size_t size, kima_malloc_flags_t flags);
void kima_free(void *ptr);

#endif
