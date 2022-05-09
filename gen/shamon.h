#ifndef SHAMON_SINGLE_H_
#define SHAMON_SINGLE_H_

#include "stream.h"

#include <stdbool.h>
#include <unistd.h>

typedef uint64_t shm_kind;
typedef uint64_t shm_eventid;
typedef struct _shm_event shm_event;
typedef struct _shm_stream shm_stream;
typedef struct _shm_arbiter_buffer shm_arbiter_buffer;

/************************************************************************
 * EVENTS
 ************************************************************************/

/* Must be called before using event API.
 * It is called from shamon_create */
void initialize_events(void);
/* called from shamon_destroy */
void deinitialize_events(void);

shm_kind shm_mk_event_kind(const char* name,
                           size_t event_size);
const char *shm_event_kind_name(shm_kind kind);

shm_eventid shm_event_id(shm_event *event);
size_t shm_event_size(shm_event *event);
shm_kind shm_event_kind(shm_event *event);
size_t shm_event_size_for_kind(shm_kind kind);

bool shm_event_is_dropped(shm_event *);
shm_kind shm_get_dropped_kind(void);

/************************************************************************
 * STREAMS
 ************************************************************************/

typedef bool (*shm_stream_filter_fn)(shm_stream *, shm_event *);
typedef void (*shm_stream_alter_fn)(shm_stream *, shm_event *, shm_event *);

const char *shm_stream_get_name(shm_stream *);
bool shm_stream_consume(shm_stream *stream, size_t num);
const char *shm_stream_get_str(shm_stream *stream, uint64_t elem);

/* * Fill the 'dropped' event for the given stream */
void shm_stream_get_dropped_event(shm_stream *stream,
                                  shm_event_dropped *dropped_ev,
                                  size_t id,
                                  uint64_t n);


/************************************************************************
 * ARBITER BUFFER
 ************************************************************************/

/* the important function -- get the pointer to the next event in the stream
 * (or NULL if the event was dropped or there is none). \param buffer
 * is taken only to handle dropping events, the next event on the stream
 * is not queued there by this function */
void *shm_stream_fetch(shm_stream *stream,
                       shm_arbiter_buffer *buffer);

void shm_arbiter_buffer_init(shm_arbiter_buffer *buffer, shm_stream *stream,
                             size_t out_event_size, size_t capacity);
void shm_arbiter_buffer_destroy(shm_arbiter_buffer *buffer);
void shm_arbiter_buffer_set_active(shm_arbiter_buffer *buffer, bool val);
size_t shm_arbiter_buffer_elem_size(shm_arbiter_buffer *q);
shm_stream * shm_arbiter_buffer_stream(shm_arbiter_buffer *buffer);
bool shm_arbiter_buffer_active(shm_arbiter_buffer *buffer);
size_t shm_arbiter_buffer_size(shm_arbiter_buffer *buffer);
size_t shm_arbiter_buffer_capacity(shm_arbiter_buffer *buffer);
size_t shm_arbiter_buffer_free_space(shm_arbiter_buffer *buffer);

/* writer's API */
void shm_arbiter_buffer_push(shm_arbiter_buffer *q, const void *elem, size_t size);
void shm_arbiter_buffer_push_k(shm_arbiter_buffer *q, const void *elems, size_t size);

void *shm_arbiter_buffer_write_ptr(shm_arbiter_buffer *q);
void shm_arbiter_buffer_write_finish(shm_arbiter_buffer *q);
void shm_arbiter_buffer_get_str(shm_arbiter_buffer *q, size_t elem);

/* reader's API */
/* multiple threads can use top and peek if none of them uses drop/pop
 * at possibly the same time */
shm_event *shm_arbiter_buffer_top(shm_arbiter_buffer *buffer);
size_t shm_arbiter_buffer_peek(shm_arbiter_buffer *buffer, size_t n,
                               void **data1, size_t *size1,
                               void **data2, size_t *size2);
bool shm_arbiter_buffer_drop(shm_arbiter_buffer *buffer, size_t n);
bool shm_arbiter_buffer_pop(shm_arbiter_buffer *q, void *buff);

#endif // SHAMON_H_
