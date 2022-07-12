#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "shmbuf/buffer.h"
#include "shmbuf/client.h"
#include "event.h"
#include "signatures.h"
#include "source.h"


static void usage_and_exit(int ret) {
    fprintf(stderr, "Usage: source shmkey freq  [num of events] ...\n");
    exit(ret);
}

// #define WITH_LINES
// #define WITH_STDOUT

#ifndef WITH_STDOUT
#define printf(...) do{}while(0)
#endif

static size_t waiting_for_buffer = 0;

int main (int argc, char *argv[]) {
    size_t events_num = ~0UL;
    size_t freq;
    if (argc < 3) {
            usage_and_exit(1);
    }

    const char *shmkey = argv[1];

    if (argc >= 3) {
            freq = atoll(argv[2]);
            if (argc == 4) {
                    events_num = atoll(argv[3]);
            } else if (argc != 3) {
                    usage_and_exit(1);
            }
    }
    /* Initialize the info about this source */
    struct source_control *control = source_control_define(1, "event", "l");
    assert(control);
    size_t event_size = control->events[0].size;
    size_t buff_event_size = event_size < sizeof(shm_event_dropped)
                                ? event_size : sizeof(shm_event_dropped);
    struct buffer *shm = create_shared_buffer(shmkey, buff_event_size, control);
    assert(shm);
    free(control);
    fprintf(stderr, "info: waiting for the monitor to attach... ");
    buffer_wait_for_monitor(shm);
    fprintf(stderr, "done\n");

    size_t num;
    struct event_record *events = buffer_get_avail_events(shm, &num);
    assert(num == 1);

    shm_event base = { .id = 0, .kind = events[0].kind };
    assert(base.kind != 0 && "Monitor did not set kind");

    void *addr;
    size_t n = 0;
    volatile size_t cycles;
    while (++n <= events_num) {
         while (!(addr = buffer_start_push(shm))) {
             ++waiting_for_buffer;
         }
         /* push the base info about event */
         ++base.id;
         addr = buffer_partial_push(shm, addr, &base, sizeof(base));
         buffer_partial_push(shm, addr, &n, sizeof(n));
         buffer_finish_push(shm);

         /* wait `freq` cycles before sending next event */
         cycles = 0;
         while (cycles < freq)
                 ++cycles;
    }

    /* Free up memory held within the regex memory */
    fprintf(stderr, "info: sent %lu events, busy waited on buffer %lu cycles\n",
            base.id, waiting_for_buffer);
    destroy_shared_buffer(shm);

    return 0;
}
