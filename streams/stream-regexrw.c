#include "stream-regexrw.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "arbiter.h"
#include "buffer.h"
#include "signatures.h"
#include "source.h"

bool sregexrw_is_ready(shm_stream *stream) {
    struct buffer *b = ((shm_stream_sregexrw *)stream)->shmbuffer;
    /* buffer must be ready or it may not be ready anymore, but it
     * still has some data that we haven't read */
    return buffer_is_ready(b) || buffer_size(b) > 0;
}

void sregexrw_alter(shm_stream *stream, shm_event *in, shm_event *out) {
    memcpy(out, in, stream->event_size);
}

shm_stream *shm_create_sregexrw_stream(const char *key, const char *name) {
    shm_stream_sregexrw *ss = malloc(sizeof *ss);
    struct buffer *shmbuffer = get_shared_buffer(key);
    assert(shmbuffer && "Getting the shm buffer failed");
    size_t elem_size = buffer_elem_size(shmbuffer);
    assert(elem_size > 0);
    shm_stream_init((shm_stream *)ss, shmbuffer, elem_size, sregexrw_is_ready,
                    NULL, sregexrw_alter, NULL, NULL, "regexrw-stream", name);
    ss->shmbuffer = shmbuffer;

    return (shm_stream *)ss;
}
