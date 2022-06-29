#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <assert.h>

#include "shmbuf/buffer.h"
#include "shmbuf/client.h"
#include "event.h"
#include "signatures.h"
#include "source.h"

#define MAXMATCH 20

static void usage_and_exit(int ret) {
    fprintf(stderr, "Usage: shmkey N\n");
    exit(ret);
}

#define WITH_STDOUT
#ifndef WITH_STDOUT
#define printf(...) do{}while(0)
#endif

struct event {
    shm_event base;
    unsigned char args[];
};

static size_t waiting_for_buffer = 0;

int main (int argc, char *argv[]) {
    if (argc != 3) {
        usage_and_exit(1);
    }
    const char *shmkey = argv[1];
    long N = atol(argv[2]);

    /* Initialize the info about this source */
    size_t control_size = sizeof(size_t) + sizeof(struct event_record);
    struct source_control *control
        = initialize_shared_control_buffer(shmkey, control_size);
    assert(control);
    control->size = control_size;
    strncpy(control->events[0].name, "addr", 5);
    strncpy((char*)control->events[0].signature, "p", 2);
    control->events[0].kind = 0;
    control->events[0].size
        = signature_get_size((const unsigned char *)"p") + sizeof(struct event);

    struct buffer *shm = initialize_shared_buffer(shmkey,
                                                  control->events[0].size,
                                                  control);
    assert(shm);
    fprintf(stderr, "info: waiting for the monitor to attach... ");
    buffer_wait_for_monitor(shm);
    fprintf(stderr, "done\n");

    struct event ev;
    ev.base.id = 0;
    ev.base.kind = control->events[0].kind;
    void *addr, *p;
    while (--N > 0) {
       while (!(addr = buffer_start_push(shm))) {
           ++waiting_for_buffer;
       }
       ++ev.base.id;
       p = addr;
       addr = buffer_partial_push(shm, addr, &ev, sizeof(ev));
       buffer_partial_push(shm, addr, &p, sizeof(void*));
       buffer_finish_push(shm);
    }

    fprintf(stderr, "info: sent %lu events, busy waited on buffer %lu cycles\n",
            ev.base.id, waiting_for_buffer);
    destroy_shared_buffer(shm);

    return 0;

}