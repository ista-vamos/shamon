#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "event.h"
#include "drfun/events.h"
#include "shamon.h"
#include "stream-funs.h"
#include "utils.h"

static inline void dump_args(shm_event_funcall *ev) {
    void *p = ev->args;
    for (const char *o = ev->signature; *o; ++o) {
        if (*o == '_') {
            printf(" _");
            continue;
        }
        size_t size = call_event_op_get_size(*o);
        switch(size) {
        case 1: printf(" %c", *((char*)p)); break;
        case 4: printf(" %d", *((int*)p)); break;
        case 8: printf(" %ld", *((long int*)p)); break;
        default: abort();
        }
        p += size;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: prog shm1\n");
        return -1;
    }

    shm_event *ev = NULL;
    shamon *shmn = shamon_create(NULL, NULL);
    assert(shmn);
    shm_stream *stream = shm_create_funs_stream(argv[1]);
    assert(stream);
    shamon_add_stream(shmn, stream);

    shm_kind kind;
    int cur_arg, last_arg = 0;
    size_t n = 0;
    while (shamon_is_ready(shmn)) {
        while ((ev = shamon_get_next_ev(shmn))) {
            ++n;

            kind = shm_event_kind(ev);
            if (shm_event_is_dropped(ev)) {
                printf("Event 'dropped(%lu)'\n", ((shm_event_dropped*)ev)->n);
                last_arg = 0;
                continue;
            }
            shm_event_funcall *callev = (shm_event_funcall *) ev;
            cur_arg = *((int*)callev->args);

            if (last_arg > 0) {
                if (cur_arg - last_arg != 1) {
                    printf("Inconsistent arguments: %d should be %d\n",
                            cur_arg, last_arg + 1);
                }
            }

            last_arg = cur_arg;

            puts("--------------------");
            shm_stream *stream = shm_event_stream(ev);
            printf("\033[0;34mEvent id %lu on stream '%s'\033[0m\n",
                   shm_event_id(ev), shm_stream_get_name(stream));
            shm_kind kind = shm_event_kind(ev);
            printf("Event kind %lu ('%s')\n", kind, shm_event_kind_name(kind));
            printf("Event size %lu\n", shm_event_size(ev));
            if (shm_event_is_dropped(ev)) {
                printf("Event 'dropped(%lu)')\n", ((shm_event_dropped*)ev)->n);
                continue;
            }
            //printf("Call '%s' [%lu], args", callev->name, callev->addr);
            printf("Args: ");
            dump_args(callev);
            putchar('\n');
            puts("--------------------");
        }
    }
    printf("Processed %lu events\n", n);
    shamon_destroy(shmn);
}
