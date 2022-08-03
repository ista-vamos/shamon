import sys
from parser import parse_program
from type_checker import TypeChecker
from cfile_utils import *

input_file = sys.argv[1] # second argument should be input file
output_path = sys.argv[2]

file = " ".join(open(input_file).readlines())

# Type checker initialization
TypeChecker.clean_checker()
TypeChecker.add_reserved_keywords()

# Parser
ast = parse_program(file)
assert(ast[0] == "main_program")


# Produce C file
output_file = open(output_path, "w")

streams_to_events_map = dict()
get_stream_to_events_mapping(ast[1], streams_to_events_map )


program = f'''#include "shamon.h"
#include "mmlib.h"
#include <threads.h>


struct _EVENT_hole
{"{"}
  uint64_t n;
{"}"};
typedef struct _EVENT_hole EVENT_hole;

{event_stream_structs(ast[1])}

{build_should_keep_funcs(ast[2], streams_to_events_map)}

// event sources threads
{declare_evt_srcs_threads(ast)}
{declare_arbiter_buffers(ast)}

{event_sources_thread_funcs(ast[2], streams_to_events_map)}
int main(int argc, char **argv) {"{"}
    initialize_events(); // Always call this first
    
{event_sources_conn_code(ast)}
    // activate buffers
{activate_buffers(ast)}
    /* TODO: Correctness layer */
    
    // create source-events threads
{activate_threads(ast)}
    // destroy event sources
{destroy_streams(ast)}
    // destroy arbiter buffers
{destroy_buffers(ast)}
{"}"}
'''

output_file.write(program)


