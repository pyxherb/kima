#include "../vmalloc.h"

void *kima_vpgalloc(void *addr, size_t size) {
	return VirtualAlloc(addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void kima_vpgfree(void* addr, size_t size) {
	VirtualFree(addr, size, MEM_DECOMMIT | MEM_RELEASE);
}
