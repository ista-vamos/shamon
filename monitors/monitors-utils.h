#ifndef VAMOS_MONITORS_UTILS_H_
#define VAMOS_MONITORS_UTILS_H_

#include "stream.h"

shm_stream *create_stream(int argc, char *argv[], int arg_i,
                          const char *expected_stream_name,
                          const shm_stream_hole_handling *hole_handling);

#endif
