#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "shm.h"
#include "client.h"

int isprime(int);

#define NUM 300000

int main(void) {
#ifdef INSTR
	struct buffer *shm = initialize_shared_buffer();
	printf("Waiting for monitor...\n");
	buffer_wait_for_monitor(shm);
	printf("Got monitor...\n");
	assert(shm);
#endif // INSTR

	unsigned primes[NUM];
	unsigned primes_num = 0;
	unsigned cur = 2;
	unsigned n = 0;
	while (n++ < NUM) {
		int prime = 1;
		for (unsigned i = 0; i < primes_num; ++i) {
			if (cur % primes[i] == 0)
				prime = 0;
		}
		if (prime) {
			primes[primes_num++] = cur;
			printf("prime: %u\n", cur);
#ifdef INSTR
			buffer_write(shm, &cur, sizeof(cur));
#else
			if (!isprime(cur)) {
				fprintf(stderr, "Not a prime: %u\n", cur);
			}
#endif // INSTR
		}
		++cur;
	}
#ifdef INSTR
	destroy_shared_buffer(shm);
#endif // INSTR
}
