#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "monitor.h"
#include "shm.h"

int isprime(int);

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
		unsigned *i;
		unsigned char *raw;
	} ptr;
	size_t n = 0;
	while (buffer_is_ready(shm)) {
		size_t size = buffer_read(shm, local);
		ptr.raw = data;
		for (; ptr.raw - data < size; ++ptr.i) {
			if (!isprime(*ptr.i)) {
				fprintf(stderr, "Is not prime: %u\n", *ptr.i);
			}
		}
	}

	//printf("Number of primes: %lu\n", n);

	free(local);
	// FIXME: we do not have the right filedescr.
	//release_shared_buffer(shm);
}
