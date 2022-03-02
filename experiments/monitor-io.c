#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "eventinterface.h"
#include "stream-fds.h"

int main(int argc, char *argv[]) {
    shm_event *ev = NULL;
    shm_streams *streams = shm_streams_mgr();

    // attach to monitors of IO of given processes
    shm_stream_fds *fdsstream = NULL;
    for (int i = 1; i < argc; ++i) {
        printf("Opening file '%s' ...", argv[i]);
        int fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            perror("open failed");
            continue;
        }

        if (fdsstream == NULL) {
            fdsstream = (shm_stream_fds *) shm_create_fds_stream();
            assert(fdsstream);
        }
        shm_stream_fds_add_fd(fdsstream, fd);
        puts(" OK");
    }

    if (fdsstream == NULL) {
        fprintf(stderr, "USAGE: prog file1 file2 ...\n");
        return -1;
    }

    shm_streams_add_stream(streams, (shm_stream *)fdsstream);

    while (1) {
        while ((ev = shm_streams_get_next_ev(streams))) {
            shm_stream *stream = shm_event_get_stream(ev);
            printf("Event id %lu on stream '%s'\n",
                   shm_event_id(ev), shm_stream_get_name(stream));
            shm_kind kind = shm_event_kind(ev);
            printf("Event kind %lu ('%s')\n", kind, shm_kind_get_name(kind));
            printf("Event size %lu\n", shm_event_size(ev));
            printf("Event time [%lu,%lu]\n",
                   shm_event_timestamp_lb(ev), shm_event_timestamp_ub(ev));
            shm_event_fd_in *fd_ev = (shm_event_fd_in *) ev;
            printf("Data: fd: %d, size: %lu:\n'%.*s'\n",
                   fd_ev->fd, fd_ev->str_ref.size,
                   fd_ev->str_ref.size, fd_ev->str_ref.data);
        }
        usleep(1000);
    }
    //shm_streams_destroy();
    // TODO: make this a callback called by the prev. function
    shm_destroy_fds_stream((shm_stream_fds *)fdsstream);
}
