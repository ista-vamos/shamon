#include "shmbuf/client.h"
#include "shmbuf/shm.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int __attribute__((noinline)) isprime(int p) {
    printf("prime: %u\n", p);
    return 1;
}

#define NUM 300000

int main(void) {
    struct timespec begin, end;
    unsigned        primes[NUM];
    unsigned        primes_num = 0;
    unsigned        cur        = 2;
    unsigned        n          = 0;
    if (clock_gettime(CLOCK_MONOTONIC, &begin) == -1)
        perror("clock_gettime");
    while (n++ < NUM) {
        int prime = 1;
        for (unsigned i = 0; i < primes_num; ++i) {
            if (cur % primes[i] == 0)
                prime = 0;
        }
        if (prime) {
            primes[primes_num++] = cur;
            isprime(cur);
#if 0
			if (!isprime(cur)) {
				fprintf(stderr, "Not a prime: %u\n", cur);
			}
#endif
        }
        ++cur;
    }
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
        perror("clock_gettime");
    long   seconds     = end.tv_sec - begin.tv_sec;
    long   nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed     = seconds + nanoseconds * 1e-9;
    printf("Time: %lf seconds.\n", elapsed);
}
