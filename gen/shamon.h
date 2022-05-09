#ifndef SHAMON_SINGLE_H_
#define SHAMON_SINGLE_H_

#include "stream.h"

#include <stdbool.h>
#include <unistd.h>

/*
 * The high-level workflow is as follows:
 *
 *  initialize_events();
 *
 *  // create a stream -- stream specific
 *  shm_stream *stream = shm_stream_XXX_create(...);
 *
 *  // create the arbiter's buffer
 *  shm_arbiter_buffer buffer.
 *  shm_arbiter_buffer_init(&buffer, stream,
 *                          ... output event size...,
 *                          ... buffer's capacity...);
 *
 *  // create thread that buffers events (code for it is below)
 *  thrd_t thread_id;
 *  thrd_create(&thread_id, buffer_manager_thrd, &buffer);
 *
 *  // let the thread know that the buffer is ready (optional,
 *  // depends on the thread's code)
 *  shm_arbiter_buffer_set_active(buffer, true);
 *
 *  // the main loop
 *  shm_event_dropped dropped;
 *  while (true) {
 *    avail_events_num = shm_arbiter_buffer_size(buffer);
 *    if (avail_events_num > 0) {
 *        if (avail_events_num > ... some threshold ...) {
 *            // drop half of the buffer
 *            shm_eventid id = shm_event_id(shm_arbiter_buffer_top(buffer));
 *            shm_arbiter_buffer_drop(buffer, c/4);
 *            shm_stream_get_dropped_event(stream, &dropped, id, c/4);
 *
 *            __monitor_send(&dropped, ...);
 *
 *            // the dropping can be replaced by summarizing and sending
 *            // some more informative event
 *        }
 *
 *        shm_arbiter_buffer_pop(buffer, ...memory for event...);
 *        __monitor_send(...memory for event ..., ...);
 *    } else {
 *      ... sleep for a while? ...
 *  }
 *
 *
 * ///////////////////////////////////////////////////////////////////
 * // The code for the thread that buffers events can look like this:
 * int buffer_manager_thrd(void *data) {
 *  shm_arbiter_buffer *buffer = (shm_arbiter_buffer*) data;
 *  shm_stream *stream = shm_arbiter_buffer_stream(buffer);
 *
 *  // wait until the buffer is active
 *  while (!shm_arbiter_buffer_active(buffer))
 *      _mm_pause();
 *
 *  void *ev, *out;
 *  while (true) {
 *      ev = stream_fetch(stream, buffer);
 *      if (!ev) {
 *          continue;
 *      }
 *      if (filter && !filter(stream, ev)) {
 *          shm_stream_consume(stream, 1);
 *          continue;
 *      }
 *
 *      out = shm_arbiter_buffer_write_ptr(buffer);
 *      copy_and_maybe_alter(stream, ev, out);
 *      shm_arbiter_buffer_write_finish(buffer);
 *      shm_stream_consume(stream, 1);
 *  }
 *
 *  thrd_exit(EXIT_SUCCESS);
 * }
 */

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
void *stream_fetch(shm_stream *stream,
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
