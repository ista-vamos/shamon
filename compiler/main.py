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

program = f'''#include "shamon.h"
#include "mmlib.h"


struct _EVENT_hole
{"{"}
  uint64_t n;
{"}"};
typedef struct _EVENT_hole EVENT_hole;

{event_stream_structs(ast[1])}

{declare_arbiter_buffers(ast)}
int main(int argc, char **argv) {"{"}
    initialize_events(); // Always call this first
    
{event_sources_conn_code(ast)}
{"}"}
'''

output_file.write(program)


