#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    const int       NUM = atoi(argv[1]);
    struct timespec begin, end;
    unsigned        primes[NUM];
    unsigned        primes_num = 0;
    unsigned        cur        = 2;
    unsigned        n          = 0;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin) == -1) {
        perror("clock_gettime");
        return 1;
    }
    while (primes_num < NUM) {
        int prime = 1;
        for (unsigned i = 0; i < primes_num; ++i) {
            if (cur % primes[i] == 0)
                prime = 0;
        }
        if (prime) {
            primes[primes_num++] = cur;
            printf("#%d: %d\n", primes_num, cur);
        }
        ++cur;
    }
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1) {
        perror("clock_gettime");
        return 1;
    }
    long   seconds     = end.tv_sec - begin.tv_sec;
    long   nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed     = seconds + nanoseconds * 1e-9;
    fprintf(stderr, "time: %lf seconds.\n", elapsed);
}
