#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "monitor.h"
#include "shm.h"

int main(void) {
	struct buffer *shm = get_shared_buffer();
	assert(shm && "Failed getting shared buffer");
	struct buffer *local = get_local_buffer(shm);
	if (!local) {
		release_shared_buffer(shm);
		fprintf(stderr, "Out of memory");
		return -1;
	}

	buffer_register_sync_monitor(shm);
	unsigned char *data = buffer_get_beginning(local);
	while (1) {
		printf("--- read ---\n");
		size_t size = buffer_read(shm, local);
		for (int *a = (int*)data, *end = (int*)(data + size); a < end; ++a) {
			printf("%d\n", *a);
		}
	}

	free(local);
	release_shared_buffer(shm);
}
