#include <kima/malloc.h>
#include <stdio.h>

int main() {
	kima_init();

	for (int i = 1; i < 4096 ; i += 1) {
		void *ptr = kima_malloc(i, 0);
		printf("%p-%p\n", ptr, ((char *)ptr) + i);
		void *ptr2 = kima_malloc(i * 2, 0);
		printf("%p-%p\n", ptr2, ((char *)ptr2) + i * 2);
		void *ptr3 = kima_malloc(i, 0);
		printf("%p-%p\n", ptr3, ((char *)ptr3) + i);
		printf("Freeing:%p-%p\n", ptr2, ((char *)ptr2) + i * 2);
		kima_free(ptr2);
	}

	kima_deinit();

	return 0;
}
