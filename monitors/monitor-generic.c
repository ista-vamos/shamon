#include <stdio.h>
#include <string.h>
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
#include "stream.h"
#include "vector.h"

#define CHECK_IDS
#define CHECK_IDS_ABORT

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
                          const char *expected_stream_name);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: prog name:source:shmkey [name:source:shmkey ...]\n");
        return -1;
    }

    shm_event *ev = NULL;
    shamon *shmn = shamon_create(NULL, NULL);
    assert(shmn);

#ifdef DUMP_STATS
    size_t total_events_num = shm_get_dropped_kind() + 1; /* ids up to 'dropped' are reserved */
#endif
    for (int i = 1; i < argc; ++i) {
        fprintf(stderr, "Connecting stream '%s' ...\n", argv[i]);
        shm_stream *stream = create_stream(argc, argv, i, NULL);
        assert(stream && "Creating stream failed");
        assert(shm_stream_id(stream) == (size_t)i);
        shamon_add_stream(shmn, stream,
                          /* buffer capacity = */4*4096);
#ifdef DUMP_STATS
        size_t events_num;
        shm_stream_get_avail_events(stream, &events_num);
        total_events_num += events_num;
#endif
    }

#ifdef DUMP_STATS
    /* count dropped per a stream and different kinds of events */
    uint64_t dropped_count[argc];
    uint64_t dropped_sum_count[argc];
    uint64_t kinds_count[total_events_num][argc];
    memset(kinds_count, 0, total_events_num*argc*sizeof(uint64_t));
    memset(dropped_count, 0, argc*sizeof(uint64_t));
    memset(dropped_sum_count, 0, argc*sizeof(uint64_t));
#endif

    shm_kind kind;
    size_t n = 0, drp = 0, drpn = 0;
    size_t id;
    size_t stream_id;
    shm_stream *stream;

#ifdef CHECK_IDS
    size_t next_id[argc];
    for (int i = 1; i < argc; ++i)
        next_id[i] = 1;
#endif
    while (shamon_is_ready(shmn)) {
        while ((ev = shamon_get_next_ev(shmn, &stream))) {
            ++n;

            id = shm_event_id(ev);
            printf("\033[0;34mEvent id %lu\033[0m\n", id);
            kind = shm_event_kind(ev);
            stream_id = shm_stream_id(stream);
#ifdef DUMP_STATS
            assert(kind < total_events_num && "OOB access");
            assert(stream_id < (size_t)argc && "OOB access");
            ++kinds_count[kind][stream_id];
#endif
            printf("Event kind %lu ('%s')\n", kind, shm_event_kind_name(kind));
            printf("Event size %lu\n", shm_event_size(ev));
            printf("Stream %lu ('%s')\n", stream_id, shm_stream_get_name(stream));

#ifdef CHECK_IDS
            if (id != next_id[stream_id]) {
                printf("\033[0;31mWrong ID: %lu, should be %lu\033[0m\n",
                       id, next_id[stream_id]);
#ifdef CHECK_IDS_ABORT
                abort();
#endif
                next_id[stream_id] = id;
            }
#endif

            if (shm_event_is_dropped(ev)) {
                printf("'dropped(%lu)'\n", ((shm_event_dropped*)ev)->n);
                drpn += ((shm_event_dropped*)ev)->n;
#ifdef DUMP_STATS
                assert(stream_id < (size_t)argc && "OOB access");
                dropped_sum_count[stream_id] += ((shm_event_dropped*)ev)->n;
                ++dropped_count[stream_id];
#endif
#ifdef CHECK_IDS
                next_id[stream_id] += ((shm_event_dropped*)ev)->n;
#endif
                ++drp;
                continue;
            }

#ifdef CHECK_IDS
            ++next_id[stream_id];
#endif
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
    size_t evs_num;
    size_t totally_came = 0;
    shm_stream **streams = shamon_get_streams(shmn, &streams_num);
    shm_vector *buffers = shamon_get_buffers(shmn);
    for (size_t i = 0; i < streams_num; ++i) {
        shm_stream *stream = streams[i];
        stream_id = shm_stream_id(stream);
        assert(stream_id < (size_t)argc && "OOB access");

        printf("-- Stream '%s':\n", shm_stream_get_name(stream));
        printf("  Event 'dropped': %lu (sum of arguments: %lu)\n",
               dropped_count[stream_id], dropped_sum_count[stream_id]);
        totally_came += dropped_count[stream_id];

        struct event_record *evs = shm_stream_get_avail_events(stream, &evs_num);
        for (size_t n = 0; n < evs_num; ++n) {
            kind = evs[n].kind;
            assert(kind < total_events_num && "OOB access");
            assert(strcmp(shm_event_kind_name(kind), evs[n].name) == 0 &&
                   "Inconsistent names");
            printf("  Event '%s': %lu\n", evs[n].name, kinds_count[kind][stream_id]);
            totally_came += kinds_count[kind][stream_id];
        }

        shm_arbiter_buffer *buff = (shm_arbiter_buffer *)shm_vector_at(buffers, i);
        shm_arbiter_buffer_dump_stats(buff);
    }
    assert(totally_came == n);
#endif

    shamon_destroy(shmn);
}
