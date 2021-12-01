#include <stdio.h>
#include <assert.h>
#include "shm.h"
#include "client.h"

int main(void) {
	struct buffer *shm = initialize_shared_buffer();
	printf("Waiting for monitor...\n");
	buffer_wait_for_monitor(shm);
	printf("Got monitor...\n");
	assert(shm);
	for (int i = 0; i < 53; ++i) {
		buffer_write(shm, &i, sizeof(i));
	}
	destroy_shared_buffer(shm);
}
