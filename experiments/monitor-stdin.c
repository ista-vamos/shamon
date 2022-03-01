#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "eventinterface.h"
#include "stream-stdin.h"

int main(void) {
    shm_event *ev = NULL;
    shm_streams *streams = shm_streams_mgr();
	// custom stream
	shm_stream *stdin_stream = shm_create_stdin_stream();
    shm_streams_add_stream(streams, stdin_stream);

    while ((ev = shm_streams_get_next_ev(streams))) {
		printf("Event id %lu\n", shm_event_id(ev));
        shm_kind kind = shm_event_kind(ev);
        printf("Event kind %lu ('%s')\n", kind, shm_kind_get_name(kind));
		printf("Event size %lu\n", shm_event_size(ev));
		printf("Event time [%lu,%lu]\n",
		       shm_event_timestamp_lb(ev), shm_event_timestamp_ub(ev));
        shm_event_stdin *stdin_ev = (shm_event_stdin *) ev;
		printf("Data: fd: %d, size: %lu:'%s'\n",
		       stdin_ev->fd, stdin_ev->str_ref.size, stdin_ev->str_ref.data);
	}
    //shm_streams_destroy();
    // TODO: make this a callback called by the prev. function
    shm_destroy_stdin_stream((shm_stream_stdin *)stdin_stream);
}
