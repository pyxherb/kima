#ifndef _KIMA_MALLOC_H_
#define _KIMA_MALLOC_H_

#include "vpgdesc.h"
#include "ublk.h"

void *kima_malloc(size_t size);
void kima_free(void *ptr);

#endif
