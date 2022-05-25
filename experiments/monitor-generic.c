#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "event.h"
#include "arbiter.h"
#include "shamon.h"
#include "stream-generic.h"
#include "utils.h"
#include "signatures.h"
#include "source.h"
#include "vector.h"


static inline void dump_args(shm_stream *stream, shm_event_generic *ev) {
    unsigned char *p = ev->args;
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

shm_stream *create_stream(int argc, char *argv[], int arg_i,
                          const char *expected_stream_name,
                          struct source_control **control);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: prog name:source:shmkey [name:source:shmkey ...]\n");
        return -1;
    }

    shm_event *ev = NULL;
    shamon *shmn = shamon_create(NULL, NULL);
    assert(shmn);

    for (int i = 1; i < argc; ++i) {
        fprintf(stderr, "Connecting stream '%s'", argv[i]);
        struct source_control *control;
        shm_stream *stream = create_stream(argc, argv, i, NULL, &control);
        assert(stream && "Creating stream failed");
        shamon_add_stream(shmn, stream,
                          /* buffer capacity = */4*4096);
    }

    shm_kind kind;
    size_t n = 0, drp = 0, drpn = 0;
    size_t id/*, next_id = 1*/;
    shm_stream *stream;
    while (shamon_is_ready(shmn)) {
        while ((ev = shamon_get_next_ev(shmn, &stream))) {
            ++n;

            id = shm_event_id(ev);
            printf("\033[0;34mEvent id %lu\033[0m\n", id);
            kind = shm_event_kind(ev);
            printf("Event kind %lu ('%s')\n", kind, shm_event_kind_name(kind));
            printf("Event size %lu\n", shm_event_size(ev));
            printf("Stream %lu ('%s')\n", shm_stream_id(stream), shm_stream_get_name(stream));

            /*
            if (id != next_id) {
                printf("\033[0;31mWrong ID: %lu, should be %lu\033[0m\n", id, next_id);
                next_id = id;
            }
            */

            if (shm_event_is_dropped(ev)) {
                printf("'dropped(%lu)'\n", ((shm_event_dropped*)ev)->n);
                drpn += ((shm_event_dropped*)ev)->n;
                //next_id += ((shm_event_dropped*)ev)->n;
                ++drp;
                continue;
            }

            //++next_id;
            printf("{");
            dump_args(stream, (shm_event_generic*)ev);
            printf("}\n");
            puts("--------------------");
        }
    }
    fflush(stdout);
    fflush(stderr);
    printf("Processed %lu events, %lu dropped events (sum of args: %lu)... totally came: %lu\n",
           n, drp, drpn, n + drpn - drp);
#ifdef DUMP_STATS
    size_t streams_num;
    shm_stream **streams = shamon_get_streams(shmn, &streams_num);
    (void)streams;
    shm_vector *buffers = shamon_get_buffers(shmn);
    for (size_t i = 0; i < streams_num; ++i) {
        shm_arbiter_buffer *buff = (shm_arbiter_buffer *)shm_vector_at(buffers, i);
        shm_arbiter_buffer_dump_stats(buff);
    }
#endif

    shamon_destroy(shmn);
}
