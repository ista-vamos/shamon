#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <threads.h>
#include <time.h>

/* RDTSC */
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#undef DEBUG_STDOUT
#ifdef DEBUG_STDOUT
#include <stdio.h>
#endif

#include "core/source.h"
#include "core/utils.h"
#include "shmbuf/buffer.h"
#include "shmbuf/client.h"

static CACHELINE_ALIGNED _Atomic size_t last_thread_id = 1;
static CACHELINE_ALIGNED _Atomic size_t timestamp      = 1;
// static CACHELINE_ALIGNED _Thread_local size_t thread_id;

static struct buffer         *top_shmbuf;
static struct source_control *top_control;
static CACHELINE_ALIGNED _Thread_local struct _thread_data {
    size_t         thread_id;
    shm_eventid    last_id;
    struct buffer *shmbuf;
    size_t         waited_for_buffer;
} thread_data;

#ifdef DEBUG_STDOUT
static inline uint64_t rt_timestamp(void) {
    return __rdtsc();
}
#endif

const char           *shmkey = "/vrd";
static struct buffer *top_shmbuf;

#define EVENTS_NUM 8
enum {
    EV_READ   = 0,
    EV_WRITE  = 1,
    EV_LOCK   = 2,
    EV_UNLOCK = 3,
    EV_ALLOC  = 4,
    EV_FREE   = 5,
    EV_FORK   = 6,
    EV_JOIN   = 7,
};

/* local cache */
uint64_t event_kinds[EVENTS_NUM];

void __tsan_init() {
    /* Initialize the info about this source */
    top_control = source_control_define(
        EVENTS_NUM, "read", "tl", "write", "tl", "lock", "tl", "unlock", "tl",
        "alloc", "tll", "free", "tl", "fork", "ti", "join", "ti");
    assert(top_control);

    top_shmbuf = create_shared_buffer(shmkey, 512, top_control);
    assert(top_shmbuf);

    fprintf(stderr, "info: waiting for the monitor to attach... ");
    buffer_wait_for_monitor(top_shmbuf);
    fprintf(stderr, "done\n");

    fprintf(stderr, "Creating events kinds cache... ");
    size_t               num;
    struct event_record *events = buffer_get_avail_events(top_shmbuf, &num);
    assert(num == EVENTS_NUM && "Invalid number of events");
    for (unsigned i = 0; i < EVENTS_NUM; ++i) {
        event_kinds[i] = events[i].kind;
    }
    fprintf(stderr, "done\n");
}

static inline void *start_event(struct buffer *shm, int type) {
    shm_event     *ev;
    while (!(ev = buffer_start_push(shm))) {
        ++thread_data.waited_for_buffer;
    }
    /* push the base info about event */
    ev->id   = ++thread_data.last_id;
    ev->kind = event_kinds[type];
    /* push the timestamp */
    uint64_t ts =
        atomic_fetch_add_explicit(&timestamp, 1, memory_order_acq_rel);
    return buffer_partial_push(shm, (void *)(((unsigned char *)ev) + sizeof(ev->id) + sizeof(ev->kind)), &ts,
                               sizeof(ts));
}

void __tsan_func_entry(void *returnaddress) {
    (void)returnaddress;
}
void __tsan_func_exit(void) {}

struct __vrd_thread_data {
    void *data;
    uint64_t thread_id;
};

void *__vrd_create_thrd_data(void *original_data) {
    uint64_t tid = atomic_fetch_add(&last_thread_id, 1);
    /* FIXME: we leak these */
    struct __vrd_thread_data *data = malloc(sizeof *data);
    assert(data && "Allocation failed");

    data->data = original_data;
    data->thread_id = tid;

    return data;
}

void *__vrd_thrd_entry(void *data) {
    struct __vrd_thread_data *tdata = (struct __vrd_thread_data *)data;

    thread_data.waited_for_buffer = 0;
    thread_data.last_id           = 0;

    /* create a new sub-buffer (or use the top level one for the main thread) */
    if (data == NULL) {
        thread_data.thread_id = 0;
        thread_data.shmbuf = top_shmbuf;
        return NULL;
    }

    thread_data.thread_id = tdata->thread_id;
    thread_data.shmbuf = create_shared_sub_buffer(top_shmbuf, 0, top_control);
    if (!thread_data.shmbuf) {
        assert(thread_data.shmbuf && "Failed creating buffer");
        abort();
    }

#ifdef DEBUG_STDOUT
    printf("[%lu] thread %lu created\n", rt_timestamp(), thread_data.thread_id);
#endif
    return tdata->data;
}

void __vrd_thrd_exit(void) {
    struct buffer *shm = thread_data.shmbuf;
    /*
    void *addr = start_event(shm, EV_THRD_EXIT);
    buffer_partial_push(shm, addr, &thread_data.thread_id,
    sizeof(thread_data.thread_id)); buffer_finish_push(shm);
    */

    destroy_shared_sub_buffer(shm);

#ifdef DEBUG_STDOUT
    printf("[%lu] exitting thread %lu\n", rt_timestamp(),
           thread_data.thread_id);
#endif
}

void __vrd_thread_join(uint64_t tid) {
    struct buffer *shm  = thread_data.shmbuf;
    void          *addr = start_event(shm, EV_JOIN);
    buffer_partial_push(shm, addr, &tid, sizeof(tid));
    buffer_finish_push(shm);

#ifdef DEBUG_STDOUT
    printf("[%lu] thread %lu exits\n", rt_timestamp(), thread_data.thread_id);
#endif
}

void __tsan_read1(void *addr) {
    struct buffer *shm = thread_data.shmbuf;
    void          *mem = start_event(shm, EV_READ);
    buffer_partial_push(shm, mem, &addr, sizeof(addr));
    buffer_finish_push(shm);

#ifdef DEBUG_STDOUT
    printf("[%lu] thread %lu: read1(%p)\n", rt_timestamp(),
           thread_data.thread_id, addr);
#endif
}


void __tsan_read2(void *addr) {
    struct buffer *shm = thread_data.shmbuf;
    void          *mem = start_event(shm, EV_READ);
    buffer_partial_push(shm, mem, &addr, sizeof(addr));
    buffer_finish_push(shm);

#ifdef DEBUG_STDOUT
    printf("[%lu] thread %lu: read2(%p)\n", rt_timestamp(),
           thread_data.thread_id, addr);
#endif
}


void __tsan_read4(void *addr) {
    struct buffer *shm = thread_data.shmbuf;
    void          *mem = start_event(shm, EV_READ);
    buffer_partial_push(shm, mem, &addr, sizeof(addr));
    buffer_finish_push(shm);

#ifdef DEBUG_STDOUT
    printf("[%lu] thread %lu: read4(%p)\n", rt_timestamp(),
           thread_data.thread_id, addr);
#endif
}

void __tsan_read8(void *addr) {
    struct buffer *shm = thread_data.shmbuf;
    void          *mem = start_event(shm, EV_READ);
    buffer_partial_push(shm, mem, &addr, sizeof(addr));
    buffer_finish_push(shm);

#ifdef DEBUG_STDOUT
    printf("[%lu] thread %lu: read8(%p)\n", rt_timestamp(),
           thread_data.thread_id, addr);
#endif
}

void __tsan_write4(void *addr) {
    struct buffer *shm = thread_data.shmbuf;
    void          *mem = start_event(shm, EV_WRITE);
    buffer_partial_push(shm, mem, &addr, sizeof(addr));
    buffer_finish_push(shm);

#ifdef DEBUG_STDOUT
    printf("[%lu] thread %lu: write4(%p)\n", rt_timestamp(),
           thread_data.thread_id, addr);
#endif
}

void __tsan_write8(void *addr) {
    struct buffer *shm = thread_data.shmbuf;
    void          *mem = start_event(shm, EV_WRITE);
    buffer_partial_push(shm, mem, &addr, sizeof(addr));
    buffer_finish_push(shm);

#ifdef DEBUG_STDOUT
    printf("[%lu] thread %lu: write8(%p)\n", rt_timestamp(),
           thread_data.thread_id, addr);
#endif
}

void __vrd_mutex_lock(void *addr) {
    struct buffer *shm = thread_data.shmbuf;
    void          *mem = start_event(shm, EV_LOCK);
    buffer_partial_push(shm, mem, &addr, sizeof(addr));
    buffer_finish_push(shm);

#ifdef DEBUG_STDOUT
    printf("[%lu] thread %lu: mutex_lock(%p)\n", rt_timestamp(),
           thread_data.thread_id, addr);
#endif
}

void __vrd_mutex_unlock(void *addr) {
    struct buffer *shm = thread_data.shmbuf;
    void          *mem = start_event(shm, EV_UNLOCK);
    buffer_partial_push(shm, mem, &addr, sizeof(addr));
    buffer_finish_push(shm);

#ifdef DEBUG_STDOUT
    printf("[%lu] thread %lu: mutex_unlock(%p)\n", rt_timestamp(),
           thread_data.thread_id, addr);
#endif
}

int __tsan_atomic32_fetch_add(_Atomic int *x, int val, int memory_order) {
    int tmp = atomic_fetch_add_explicit(x, val, memory_order);
    return tmp;
}