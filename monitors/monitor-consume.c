#include <assert.h>
#include <immintrin.h> /* _mm_pause */
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

#include "arbiter.h"
#include "event.h"
#include "signatures.h"
#include "source.h"
#include "stream.h"
#include "utils.h"

shm_stream *shm_stream_create_from_argv(const char *name, int argc, char *argv[]);
#define SLEEP_NS_INIT (50)
#define SLEEP_THRESHOLD_NS (10000000)

shm_stream *create_stream(int argc, char *argv[], int arg_i,
                          const char *expected_stream_name);

static int buffer_manager_thrd(void *data) {
    shm_arbiter_buffer *buffer = (shm_arbiter_buffer *)data;
    shm_stream *stream = shm_arbiter_buffer_stream(buffer);

    // wait for buffer->active
    while (!shm_arbiter_buffer_active(buffer))
        _mm_pause();

    printf("Running fetch & autodrop for stream %s\n", stream->name);

    const size_t ev_size = shm_stream_event_size(stream);
    void *ev, *out;
    while (1) {
        ev = stream_fetch(stream, buffer);
        if (!ev) {
            break;
        }

        out = shm_arbiter_buffer_write_ptr(buffer);
        assert(out && "No space in the buffer");
        memcpy(out, ev, ev_size);
        shm_arbiter_buffer_write_finish(buffer);
        shm_stream_consume(stream, 1);
    }

    // TODO: we should check if the stream is finished and remove it
    // in that case
    printf("BMM for stream %lu (%s) exits\n", stream->id, stream->name);
    thrd_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: prog source-spec\n");
        return -1;
    }

    initialize_events();

    shm_stream *stream = create_stream(argc, argv, 1, NULL);
    assert(stream && "Creating stream failed");

    shm_arbiter_buffer *buffer =
        shm_arbiter_buffer_create(stream, shm_stream_event_size(stream),
                                  /*capacity=*/4 * 4096);
    thrd_t tid;
    thrd_create(&tid, buffer_manager_thrd, (void *)buffer);
    shm_arbiter_buffer_set_active(buffer, true);

    size_t n = 0, tmp, trials = 0;
    while (1) {
        tmp = shm_arbiter_buffer_size(buffer);
        if (tmp > 0) {
            n += shm_arbiter_buffer_drop(buffer, tmp);
            trials = 0;
        } else {
            ++trials;
        }

        if (trials > 1000) {
            if (!shm_stream_is_ready(stream))
                break;
        }
    }
    printf("Processed %lu events\n", n);

    thrd_join(tid, NULL);

    shm_stream_destroy(stream);
    shm_arbiter_buffer_free(buffer);

    return 0;
}