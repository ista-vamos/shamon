#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "monitor.h"
#include "shm.h"

int main(void) {
	struct buffer *shm = get_shared_buffer();
	assert(shm);
	void *local = get_local_buffer(shm);
	if (!local) {
		release_shared_buffer(shm);
		fprintf(stderr, "Out of memory");
		return -1;
	}

	buffer_register_sync_monitor(shm);
	while (1) {
		size_t size = buffer_sync(shm, local);
	}

	free(local);
	release_shared_buffer(shm);
}
