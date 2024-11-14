#ifndef _KIMA_VMALLOC_H_
#define _KIMA_VMALLOC_H_

#include <stdint.h>

#if _WIN32
#include <Windows.h>
#endif

void *kima_vpgalloc(void *addr, size_t size);
void kima_vpgfree(void *addr, size_t size);

#endif
