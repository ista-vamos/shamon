#include <stdio.h>
#include <time.h>

#define REP 1000000

int main(int argc, char *argv[]) {
    volatile size_t cycles;
    struct timespec begin, end, res = {0};

    unsigned long N;
    printf("How many cycles to wait? ");
    scanf("%lu", &N);

    for (int i = 0; i < REP; ++i) {
        cycles = 0;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin);
        while (cycles < N)
            ++cycles;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

        unsigned long seconds     = end.tv_sec - begin.tv_sec;
        unsigned long nanoseconds = end.tv_nsec - begin.tv_nsec;
        res.tv_nsec += nanoseconds;
        res.tv_sec += seconds;
    }

    unsigned long long ns  = res.tv_sec * 1e9 + res.tv_nsec;
    double             avg = ns / (double)REP;
    fprintf(stderr,
            "%lu cycles take %lf ns (%lf ms) on average\nThat is %lf ns per "
            "cycle.\n",
            N, avg, avg / 1000000, avg / N);
    return 0;
}
