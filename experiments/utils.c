#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "stream.h"
#include "source.h"

shm_stream *shm_stream_create(const char *name,
                              struct source_control **control,
                              int argc,
                              char *argv[]);

shm_stream *create_stream(int argc, char *argv[],
                          const char *expected_stream_name,
                          struct source_control **control) {
    char streamname[256];
    const char *dc = strchr(argv[1], ':');
    if (!dc) {
        fprintf(stderr, "Failed to parse the name of stream.");
        return NULL;
    }
    strncpy(streamname, argv[1], dc - argv[1]);
    streamname[dc - argv[1]] = 0;

    shm_stream *stream
            = shm_stream_create(streamname, control,
                                argc, argv);
    assert(stream);
    if (strcmp(shm_stream_get_name(stream), expected_stream_name) != 0) {
        fprintf(stderr, "A wrong source was specified for this monitor.\n"
                        "Got '%s' but expected '%s'",
                shm_stream_get_name(stream), expected_stream_name);
    }

    return stream;
}
