#include <stdio.h>
#include <unistd.h>

#include "event.h"
#include "shmbuf/buffer.h"
#include "stream.h"

typedef struct _shm_event_regexrw {
    shm_event base;
    /* the event arguments */
    unsigned char args[];
} shm_event_regexrw;

typedef struct _shm_stream_sregexrw {
    shm_stream base;
    struct buffer *shmbuffer;
} shm_stream_sregexrw;

shm_stream *shm_create_sregexrw_stream(const char *key, const char *name);
void shm_destroy_sregexrw_stream(shm_stream_sregexrw *ss);
