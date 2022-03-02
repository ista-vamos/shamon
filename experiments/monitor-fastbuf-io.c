#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "eventinterface.h"
#include "stream-fastbuf-io.h"


/*
bool is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
*/

int main(int argc, char *argv[]) {
    shm_event *ev = NULL;
    shm_streams *streams = shm_streams_mgr();

    if (argc == 1) {
        fprintf(stderr, "USAGE: prog file1 file2 ...\n");
        return -1;
    }

    // attach to monitors of IO of given processes
    for (int i = 1; i < argc; ++i) {
        pid_t pid = atoi(argv[i]);
        shm_stream *stream = shm_create_io_stream(pid);
        shm_streams_add_stream(streams, stream);
        assert(stream);
        puts(" OK");
    }

    while (1) {
        while ((ev = shm_streams_get_next_ev(streams))) {
            puts("--------------------");
            shm_stream *stream = shm_event_get_stream(ev);
            printf("\033[0;34mEvent id %lu on stream '%s'\033[0m\n",
                   shm_event_id(ev), shm_stream_get_name(stream));
            shm_kind kind = shm_event_kind(ev);
            printf("Event kind %lu ('%s')\n", kind, shm_kind_get_name(kind));
            printf("Event size %lu\n", shm_event_size(ev));
            printf("Event time [%lu,%lu]\n",
                   shm_event_timestamp_lb(ev), shm_event_timestamp_ub(ev));
            shm_event_io *ev = (shm_event_io *) ev;
            printf("Data: fd: %d, size: %lu:\n'%.*s'\n",
                   ev->fd, ev->str_ref.size,
                   ev->str_ref.size, ev->str_ref.data);
            puts("--------------------");
        }
        usleep(100);
    }
    //shm_streams_destroy();
    // TODO:  we leak all the streams
}
