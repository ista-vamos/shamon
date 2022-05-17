#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "stream.h"
#include "source.h"

shm_stream *shm_stream_create(const char *name,
                              struct source_control **control,
                              int argc,
                              char *argv[]);

shm_stream *create_stream(int argc, char *argv[], int arg_i,
                          const char *expected_stream_name,
                          struct source_control **control) {
    assert(arg_i < argc && "Index too large");

    char streamname[256];
    const char *dc = strchr(argv[arg_i], ':');
    if (!dc) {
        fprintf(stderr, "Failed to parse the name of stream.");
        return NULL;
    }
    strncpy(streamname, argv[arg_i], dc - argv[arg_i]);
    streamname[dc - argv[arg_i]] = 0;

    shm_stream *stream
            = shm_stream_create(streamname, control,
                                argc, argv);
    assert(stream);
    if (expected_stream_name &&
        strcmp(shm_stream_get_name(stream), expected_stream_name) != 0) {
        fprintf(stderr, "A wrong source was specified for this monitor.\n"
                        "Got '%s' but expected '%s'",
                shm_stream_get_name(stream), expected_stream_name);
        shm_stream_destroy(stream);
        return NULL;
    }

    return stream;
}
