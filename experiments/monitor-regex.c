#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "event.h"
#include "shamon.h"
#include "stream-regex.h"
#include "utils.h"
#include "signatures.h"
#include "source.h"


static inline void dump_args(shm_stream *stream, shm_event_regex *ev) {
    void *p = ev->args;
    const char *signature = shm_event_signature((shm_event*)ev);
    for (const char *o = signature; *o; ++o) {
        if (o != signature)
            printf(", ");
        if (*o == 'S' || *o == 'L' || *o == 'M') {
            printf("S[%lu, %lu](%s)",
                   (*(uint64_t*)p) >> 32,
                   (*(uint64_t*)p) & 0xffffffff,
                   shm_stream_get_str(stream, (*(uint64_t*)p)));
            p += sizeof(uint64_t);
            continue;
        }

        size_t size = signature_op_get_size(*o);
        if (*o == 'f') {
            printf("%f", *((float*)p));
        } else if (*o == 'd') {
            printf("%lf", *((double*)p));
        } else {
            switch(size) {
            case 1: printf("%c", *((char*)p)); break;
            case 4: printf("%d", *((int*)p)); break;
            case 8: printf("%ld", *((long int*)p)); break;
            default: printf("?");
            }
        }
        p += size;
    }
}

shm_stream *shm_stream_create(const char *name,
                              struct source_control **control,
                              int argc,
                              char *argv[]);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: prog shm1\n");
        return -1;
    }

    shm_event *ev = NULL;
    shamon *shmn = shamon_create(NULL, NULL);
    assert(shmn);
    struct source_control *control;
    shm_stream *fstream
            = shm_stream_create("regex", &control,
                                argc, argv);
    assert(fstream);
    shamon_add_stream(shmn, fstream,
                      /* buffer capacity = */4*4096);

    shm_kind kind;
    int cur_arg, last_arg = 0;
    size_t n = 0, drp = 0, drpn = 0;
    size_t id, last_id = 0;
    while (shamon_is_ready(shmn)) {
        while ((ev = shamon_get_next_ev(shmn))) {
            ++n;

            kind = shm_event_kind(ev);
            if (shm_event_is_dropped(ev)) {
                printf("Event 'dropped(%lu)'\n", ((shm_event_dropped*)ev)->n);
                drpn += ((shm_event_dropped*)ev)->n;
                ++drp;
                last_id = 0;
                continue;
            }
            id = shm_event_id(ev);
            if (last_id > 0) {
                if (last_id + 1 != id) {
                    fprintf(stderr, "Inconsistent IDs, %lu + 1 != %lu\n",
                            last_id, id);
                    abort();
                }
            }
            last_id = id;
            /*
            printf("Event kind %lu ('%s')\n", kind, shm_event_kind_name(kind));
            puts("--------------------");
            printf("\033[0;34mEvent id %lu\033[0m\n", shm_event_id(ev));
            printf("Event kind %lu ('%s')\n", kind, shm_event_kind_name(kind));
            printf("Event size %lu\n", shm_event_size(ev));
            */
            shm_event_regex *reev = (shm_event_regex*)ev;
            printf("{");
            dump_args(fstream, reev);
            printf("}\n");
            /*
            */
        }
    }
    printf("Processed %lu events, %lu dropped events (sum of args: %lu)... totally came: %lu\n",
           n, drp, drpn, n + drpn - drp);
    shamon_destroy(shmn);
    /* FIXME: do this a callback of shamon_destroy, so that
     * we do not have to think about the order */
    shm_destroy_sregex_stream((shm_stream_sregex*)fstream);
}
