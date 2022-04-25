#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "event.h"
#include "shamon.h"
#include "stream-funs.h"
#include "utils.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: prog shm1\n");
        return -1;
    }

    shm_event *ev = NULL;
    shamon *shmn = shamon_create(NULL, NULL);
    shm_stream *stream = shm_create_funs_stream(argv[1]);
    shamon_add_stream(shmn, stream);

    while (1) {
        while ((ev = shamon_get_next_ev(shmn))) {
            puts("--------------------");
            shm_stream *stream = shm_event_stream(ev);
            printf("\033[0;34mEvent id %lu on stream '%s'\033[0m\n",
                   shm_event_id(ev), shm_stream_get_name(stream));
            shm_kind kind = shm_event_kind(ev);
            printf("Event kind %lu ('%s')\n", kind, shm_event_kind_name(kind));
            printf("Event size %lu\n", shm_event_size(ev));
            /*
            shm_event_io *shm_ev = (shm_event_io *) ev;
	    assert(shm_ev->str_ref.size < INT_MAX);
            printf("Event time %lu\n", shm_ev->time);
            printf("Data: fd: %d, size: %lu:\n'%.*s'\n",
                   shm_ev->fd, shm_ev->str_ref.size,
                   (int)shm_ev->str_ref.size, shm_ev->str_ref.data);
            */
            puts("--------------------");
        }
        sleep_ms(100);
    }
    shamon_destroy(shmn);
}
