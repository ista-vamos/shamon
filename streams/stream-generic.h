#ifndef SHMN_STREAM_GENERIC_H_
#define SHMN_STREAM_GENERIC_H_

#include <stdio.h>
#include <unistd.h>

#include "event.h"
#include "stream.h"
#include "buffer.h"

/* A generic stream for events stored in shared memory buffer,
   no filter nor modification of events supported (unless done manually). */

typedef struct _shm_event_generic {
    shm_event base;
    /* the event arguments */
    unsigned char args[];
} shm_event_generic;

typedef struct _shm_stream_generic {
	shm_stream base;
    struct buffer *shmbuffer;
} shm_stream_generic;

shm_stream *shm_create_generic_stream(const char *key,
                                      struct source_control **control);

#endif /* SHMN_STREAM_GENERIC_H_ */