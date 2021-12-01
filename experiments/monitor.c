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
	union IT {
		int *i;
		unsigned char *raw;
	} ptr;
	while (buffer_is_ready(shm)) {
		printf("--- read ---\n");
		size_t size = buffer_read(shm, local);
		ptr.raw = data;
		for (; ptr.raw - data < size; ++ptr.i) {
			printf("%d\n", *ptr.i);
		}
		fflush(stdout);
	}

	free(local);
	// FIXME: we do not have the right filedescr.
	//release_shared_buffer(shm);
}
