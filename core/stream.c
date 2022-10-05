#include <assert.h>

#include "shmbuf/buffer.h"
#include "stream.h"

/*****
 * STREAM
 *****/

const char *shm_stream_get_name(shm_stream *stream) {
    assert(stream);
    return stream->name;
}

static uint64_t last_stream_id = 0;

void shm_stream_init(shm_stream *stream, struct buffer *incoming_events_buffer,
                     size_t event_size, shm_stream_is_ready_fn is_ready,
                     shm_stream_filter_fn filter, shm_stream_alter_fn alter,
                     shm_stream_destroy_fn destroy, const char *const name) {
    stream->id = ++last_stream_id;
    stream->event_size = event_size;
    stream->incoming_events_buffer = incoming_events_buffer;
    /* TODO: maybe we could just have a boolean flag that would be set
     * by a particular stream implementation instead of this function call?
     * Or a special universal event that is sent by the source...*/
    stream->is_ready = is_ready;
    stream->filter = filter;
    stream->alter = alter;
    stream->destroy = destroy;
    stream->name = name;
#ifndef NDEBUG
    stream->last_event_id = 0;
#endif
#ifdef DUMP_STATS
    stream->read_events = 0;
    stream->fetched_events = 0;
    stream->consumed_events = 0;
    stream->dropped_events = 0;
    stream->slept_waiting_for_ev = 0;
#endif
}

size_t shm_stream_id(shm_stream *stream) {
    return stream->id;
}

struct event_record *shm_stream_get_avail_events(shm_stream *s, size_t *sz) {
    return buffer_get_avail_events(s->incoming_events_buffer, sz);
}

/* the number of elements in the (shared memory) buffer of the stream */
size_t shm_stream_buffer_size(shm_stream *s) {
    return buffer_size(s->incoming_events_buffer);
}

/* the capacity the (shared memory) buffer of the stream */
size_t shm_stream_buffer_capacity(shm_stream *s) {
    return buffer_capacity(s->incoming_events_buffer);
}

/* FIXME: no longer related to stream */
void shm_stream_get_dropped_event(shm_stream *stream,
                                  shm_event_dropped *dropped_ev, size_t id,
                                  uint64_t n) {
    dropped_ev->base.id = id;
    dropped_ev->base.kind = shm_get_dropped_kind();
    dropped_ev->n = n;
#ifdef DUMP_STATS
    stream->dropped_events += n;
#else
    (void)stream;
#endif
}

bool shm_stream_is_ready(shm_stream *s) {
    return s->is_ready(s);
}

bool shm_stream_is_finished(shm_stream *s) {
    return shm_stream_buffer_size(s) == 0 && !shm_stream_is_ready(s);
}

void *shm_stream_read_events(shm_stream *s, size_t *num) {
    /* the buffer may be already destroyed on the client's side,
     * but still may have some events to read */
    /* assert(shm_stream_is_ready(s)); */
    return buffer_read_pointer(s->incoming_events_buffer, num);
}

bool shm_stream_consume(shm_stream *stream, size_t num) {
#ifdef DUMP_STATS
    stream->consumed_events += num;
#endif
    return buffer_drop_k(stream->incoming_events_buffer, num);
}

const char *shm_stream_get_str(shm_stream *stream, uint64_t elem) {
    return buffer_get_str(stream->incoming_events_buffer, elem);
}

size_t shm_stream_event_size(shm_stream *s) {
    return s->event_size;
}

void shm_stream_notify_last_processed_id(shm_stream *stream, shm_eventid id) {
    buffer_set_last_processed_id(stream->incoming_events_buffer, id);
}

void shm_stream_notify_dropped(shm_stream *stream, uint64_t begin_id,
                               uint64_t end_id) {
    buffer_notify_dropped(stream->incoming_events_buffer, begin_id, end_id);
}

void shm_stream_destroy(shm_stream *stream) {
    if (stream->destroy)
        stream->destroy(stream);
}