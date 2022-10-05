#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "event.h"
#include "shamon.h"
#include "signatures.h"
#include "source.h"
#include "stream-regex.h"
#include "utils.h"

static inline void dump_args(shm_stream *stream, shm_event_regex *ev) {
    unsigned char *p = ev->args;
    const char *signature = shm_event_signature((shm_event *)ev);
    for (const char *o = signature; *o; ++o) {
        if (o != signature)
            printf(", ");
        if (*o == 'S' || *o == 'L' || *o == 'M') {
            printf("S[%lu, %lu](%s)", (*(uint64_t *)p) >> 32,
                   (*(uint64_t *)p) & 0xffffffff,
                   shm_stream_get_str(stream, (*(uint64_t *)p)));
            p += sizeof(uint64_t);
            continue;
        }

        size_t size = signature_op_get_size(*o);
        if (*o == 'f') {
            printf("%f", *((float *)p));
        } else if (*o == 'd') {
            printf("%lf", *((double *)p));
        } else {
            switch (size) {
            case 1:
                printf("%c", *((char *)p));
                break;
            case 4:
                printf("%d", *((int *)p));
                break;
            case 8:
                printf("%ld", *((long int *)p));
                break;
            default:
                printf("?");
            }
        }
        p += size;
    }
}

shm_stream *create_stream(int argc, char *argv[], int arg_i,
                          const char *expected_stream_name);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: prog shm1\n");
        return -1;
    }

    shm_event *ev = NULL;
    shamon *shmn = shamon_create(NULL, NULL);
    assert(shmn);

    shm_stream *fstream = create_stream(argc, argv, 1, "regex-stream");
    assert(fstream && "Creating stream failed");
    shamon_add_stream(shmn, fstream,
                      /* buffer capacity = */ 4 * 4096);

    // shm_kind kind;
    size_t n = 0, drp = 0, drpn = 0;
    size_t id, next_id = 1;
    shm_stream *stream;
    while (shamon_is_ready(shmn)) {
        while ((ev = shamon_get_next_ev(shmn, &stream))) {
            ++n;

            id = shm_event_id(ev);
            if (id != next_id) {
                printf("Wrong ID: %lu, should be %lu\n", id, next_id);
                next_id = id; /* reset */
            }

            // kind = shm_event_kind(ev);
            if (shm_event_is_dropped(ev)) {
                // printf("Event 'dropped(%lu)'\n",
                // ((shm_event_dropped*)ev)->n);
                drpn += ((shm_event_dropped *)ev)->n;
                next_id += ((shm_event_dropped *)ev)->n;
                ++drp;
                continue;
            }

            ++next_id;
            /*
            if (last_id > 0) {
                if (last_id + 1 != id) {
                    fprintf(stderr, "Inconsistent IDs, %lu + 1 != %lu\n",
                            last_id, id);
                    abort();
                }
            }
            last_id = id;
            */
            shm_kind kind = shm_event_kind(ev);
            printf("Event kind %lu ('%s')\n", kind, shm_event_kind_name(kind));
            puts("--------------------");
            printf("\033[0;34mEvent id %lu\033[0m\n", shm_event_id(ev));
            printf("Event kind %lu ('%s')\n", kind, shm_event_kind_name(kind));
            printf("Event size %lu\n", shm_event_size(ev));
            shm_event_regex *reev = (shm_event_regex *)ev;
            printf("{");
            dump_args(stream, reev);
            printf("}\n");
        }
    }
    printf("Processed %lu events, %lu dropped events (sum of args: %lu)... "
           "totally came: %lu\n",
           n, drp, drpn, n + drpn - drp);

    shamon_destroy(shmn);
}