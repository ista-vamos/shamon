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

void printPrime(size_t n, size_t p) {
    printf("#%lu: %lu\n", n, p);
}

int main(int argc, char **argv) {
    struct timespec begin, end;
    int             target      = 10;
    int             interactive = 0;
    if (argc > 1) {
        if (strcmp(argv[1], "-i") == 0) {
            interactive = 1;
        } else {
            target = atoi(argv[1]);
        }
    }
    if (interactive) {
        char buffer[256];
        char ipb[8];
        while (1) {
            int index;
            if (fgets(buffer, 256, stdin) == NULL) {
                printf("byeeof\n");
                break;
            }
            if (strcmp(buffer, "quit\n") == 0) {
                printf("byequit\n");
                break;
            }
            strncpy(ipb, buffer, 8);
            ipb[7] = 0;
            if (strcmp(ipb, "isprime") == 0) {
                long num;
                if (strlen(buffer) <= 8) {
                    continue;
                }
                num = atol(buffer + 8);
                printf("%li is%s prime\n", num, (isPrime(num) ? "" : " not"));
                continue;
            }
            index = atoi(buffer);
            if (index < 1) {
                index = 1;
            }
            printf("#%i: %lu\n", index, findPrime(index));
        }
    } else {
        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin) == -1) {
            perror("clock_gettime");
            return 1;
        }
        intlist  base = {.number = 2, .next = NULL};
        intlist *last = &base;

        unsigned long current = 3;
        unsigned long count   = 1;
        if (target > 0) {
            printPrime(1, 2);
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
                printPrime(count, current);
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
    }
}
