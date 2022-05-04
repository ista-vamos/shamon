#ifndef SHAMON_STREAMS_H
#define SHAMON_STREAMS_H

#include "event.h"

typedef struct _shm_arbiter_buffer shm_arbiter_buffer;

typedef size_t (*shm_stream_buffer_events_fn)(struct _shm_stream *,
                                              shm_arbiter_buffer *buffer);
typedef bool (*shm_stream_is_ready_fn)(struct _shm_stream *);

typedef bool (*shm_stream_filter_fn)(shm_stream *, shm_event *);
typedef void (*shm_stream_alter_fn)(shm_stream *, shm_event *, shm_event *);

// TODO: make this opaque
typedef struct _shm_stream {
    uint64_t id;
    const char *name;
    size_t event_size;
    struct buffer *incoming_events;
    /* callbacks */
    shm_stream_is_ready_fn is_ready;
    shm_stream_filter_fn filter;
    shm_stream_alter_fn alter;
    /* shm_stream_get_next_event_fn get_next_event; */
} shm_stream;

void shm_stream_init(shm_stream *stream,
                     struct buffer *incoming_events,
                     size_t event_size,
                     shm_stream_is_ready_fn is_ready,
                     shm_stream_filter_fn filter,
                     shm_stream_alter_fn alter,
                     const char * const name);

const char *shm_stream_get_name(shm_stream *);

void *shm_stream_read_events(shm_stream *, size_t *);
bool shm_stream_consume(shm_stream *stream, size_t num);
void shm_buffer_release_str(shm_stream *stream, uint64_t elem);
const char *shm_stream_get_str(shm_stream *stream, uint64_t elem);

/*
 * Fill the 'dropped' event for the given stream
 */
void shm_stream_get_dropped_event(shm_stream *stream,
                                  shm_event_dropped *dropped_ev,
                                  size_t id,
                                  uint64_t n);

bool shm_stream_is_ready(shm_stream *);
#endif // SHAMON_STREAMS_H
