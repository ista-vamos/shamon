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

static inline void dump_args(shm_stream_funs *ss, shm_event_funcall *ev) {
    void *p = ev->args;
    for (const char *o = ev->signature; *o; ++o) {
        if (*o == '_') {
            printf(" _");
            continue;
        }
        if (*o == 'S') {
            printf(" S[%lu, %lu](%s)",
                   (*(uint64_t*)p) >> 32,
                   (*(uint64_t*)p) & 0xffffffff,
                   shm_stream_funs_get_str(ss, (*(uint64_t*)p)));
            p += sizeof(uint64_t);
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

static inline void get_strings(shm_stream_funs *ss, shm_event_funcall *ev) {
    void *p = ev->args;
    for (const char *o = ev->signature; *o; ++o) {
        if (*o == '_') {
            continue;
        }
        if (*o == 'S') {
            shm_stream_funs_get_str(ss, (*(uint64_t*)p));
            p += sizeof(uint64_t);
            continue;
        }

        p += call_event_op_get_size(*o);
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
    shm_stream *fstream = shm_create_funs_stream(argv[1]);
    assert(fstream);
    shamon_add_stream(shmn, fstream,
                      /* buffer capacity = */4*4096);

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
            /*
            puts("--------------------");
            printf("\033[0;34mEvent id %lu on stream '%s'\033[0m\n",
                   shm_event_id(ev), shm_stream_get_name(stream));
            printf("Event kind %lu ('%s')\n", kind, shm_event_kind_name(kind));
            printf("Event size %lu\n", shm_event_size(ev));
            if (shm_event_is_dropped(ev)) {
                printf("Event 'dropped(%lu)')\n", ((shm_event_dropped*)ev)->n);
                continue;
            }
            */
            printf("Event kind %lu ('%s')\n", kind, shm_event_kind_name(kind));
            dump_args((shm_stream_funs*)fstream, callev);
            putchar('\n');
            /*
            puts("--------------------");
            get_strings((shm_stream_funs*)stream, callev);
            shm_event_funcall_release(callev);
            */
        }
    }
    printf("Processed %lu events\n", n);
    shamon_destroy(shmn);
    /* FIXME: do this a callback of shamon_destroy, so that
     * we do not have to think about the order */
    shm_destroy_funs_stream((shm_stream_funs*)fstream);
}
