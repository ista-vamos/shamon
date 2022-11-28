#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct ulonglist {
    unsigned long     number;
    struct ulonglist *next;
} intlist;

int isPrime(long number) {
    intlist       base    = {.number = 2, .next = NULL};
    intlist      *last    = &base;
    unsigned long current = 3;
    if (number == 2) {
        return 1;
    }
    while (current <= number) {
        intlist *curnode = &base;
        int      found   = 0;
        while (curnode != NULL) {
            if (current % curnode->number == 0) {
                found = 1;
                break;
            }
            curnode = curnode->next;
        }
        if (!found) {
            if (current == number) {
                return 1;
            } else {
                intlist *newnode = (intlist *)malloc(sizeof(intlist));
                newnode->number  = current;
                newnode->next    = NULL;
                last->next       = newnode;
                last             = newnode;
            }
        }
        current += 2;
    }
    last = base.next;
    while (last != NULL) {
        intlist *node = last;
        last          = last->next;
        free(node);
    }
    return 0;
}

long findPrime(int target) {
    intlist       base    = {.number = 2, .next = NULL};
    intlist      *last    = &base;
    unsigned long count   = 1;
    unsigned long current = 3;
    if (target <= 1) {
        return 2;
    }
    while (count < target) {
        intlist *curnode = &base;
        int      found   = 0;
        while (curnode != NULL) {
            if (current % curnode->number == 0) {
                found = 1;
                break;
            }
            curnode = curnode->next;
        }
        if (!found) {
            intlist *newnode = (intlist *)malloc(sizeof(intlist));
            newnode->number  = current;
            newnode->next    = NULL;
            last->next       = newnode;
            last             = newnode;
            count++;
        }
        current++;
    }
    current = last->number;
    last    = base.next;
    while (last != NULL) {
        intlist *node = last;
        last          = last->next;
        free(node);
    }
    return current;
}

int main(int argc, char **argv) {
    struct timespec begin, end;
    int             target         = 10;
    int             errnum         = 0;
    size_t          generated_errs = 0;
    if (argc > 1) {
        if (argc > 2) {
            errnum = atoi(argv[2]);
        }

        target = atoi(argv[1]);
    }

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin) == -1) {
        perror("clock_gettime");
        return 1;
    }
    intlist  base = {.number = 2, .next = NULL};
    intlist *last = &base;

    unsigned long current   = 3;
    unsigned long count     = 1;
    int           curerrnum = 0;
    if (target > 0) {
        if (++curerrnum == errnum) {
            printf("#1: 666\n");
            ++generated_errs;
            curerrnum = 0;
        } else {
            printf("#1: 2\n");
        }
    }

    while (count < target) {
        intlist *curnode = &base;
        int      found   = 0;
        while (curnode != NULL) {
            if (current % curnode->number == 0) {
                found = 1;
                break;
            }
            curnode = curnode->next;
        }
        if (!found) {
            intlist *newnode = (intlist *)malloc(sizeof(intlist));
            newnode->number  = current;
            newnode->next    = NULL;
            last->next       = newnode;
            last             = newnode;
            count++;
            if (++curerrnum == errnum) {
                printf("#%lu: %lu\n", count, current + 1);
                ++generated_errs;
                curerrnum = 0;
            } else {
                printf("#%lu: %lu\n", count, current);
            }
        }
        current++;
    }

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1) {
        perror("clock_gettime");
        return 1;
    }
    long   seconds     = end.tv_sec - begin.tv_sec;
    long   nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed     = seconds + nanoseconds * 1e-9;
    fprintf(stderr, "time: %lf seconds.\n", elapsed);
    fprintf(stderr, "Errors generated: %lu\n", generated_errs);
}
