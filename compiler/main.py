import sys
from typing import Dict

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
stream_types : Dict[str, Tuple[str, str]] = dict() # maps an event source to (input_stream_type, output_stream_type)
get_stream_types(ast[2], stream_types)

arbiter_event_source = get_arbiter_event_source(ast[3])


program = f'''#include "shamon.h"
#include "mmlib.h"
#include "monitor.c"
#include <threads.h>
#include <stdio.h>


struct _EVENT_hole
{"{"}
  uint64_t n;
{"}"};
typedef struct _EVENT_hole EVENT_hole;

{event_stream_structs(ast[1])}

{build_should_keep_funcs(ast[2], streams_to_events_map)}

int count_event_streams = {get_count_events_sources(ast)};

// declare event streams
{declare_event_sources(ast)}
//declare flags for event streams
{declare_event_sources_flags(ast)}
// event sources threads
{declare_evt_srcs_threads(ast)}
// declare arbiter thread
thrd_t ARBITER_THREAD;

{declare_arbiter_buffers(ast)}
// monitor buffer
shm_monitor_buffer *monitor_buffer;

{event_sources_thread_funcs(ast[2], streams_to_events_map)}
{exists_open_streams(ast)}
bool check_n_events(shm_arbiter_buffer* b, size_t n) {"{"}
    // checks if there are exactly n elements on a given stream s
    void* e1; size_t i1;
	void* e2; size_t i2;
	return shm_arbiter_buffer_peek(b,0, &e1, &i1, &e2, &i2) == n;
{"}"}

bool are_events_in_head(shm_arbiter_buffer *b, size_t ev_size, int event_kinds[], int n_events) {"{"}
    char* e1; size_t i1;
	char* e2; size_t i2;
	int count = shm_arbiter_buffer_peek(b, n_events, (void **)&e1, &i1,(void**) &e2, &i2);
	if (count < n_events) {"{"}
	    return false;
	{"}"}
	
	int i = 0;
	while (i < i1) {"{"}
	    shm_event * ev = (shm_event *) (e1);
	     if (ev->kind != event_kinds[i]) {"{"}
	        return false;
	    {"}"}
	    i+=1;
	    e1 += ev_size;
	{"}"}

	i = 0;
	while (i < i2) {"{"}
	    shm_event * ev = (shm_event *) e2;
	     if (ev->kind != event_kinds[i1+i]) {"{"}
	        return false;
	    {"}"}
	    i+=1;
	    e2 += ev_size;
	{"}"}
	
	return true;
{"}"}

shm_event * get_event_at_index(char* e1, size_t i1, char* e2, size_t i2, size_t size_event, int element_index) {"{"}
	if (element_index < i1) {"{"}
		return (shm_event *) (e1 + (element_index*size_event));
	{"}"} else {"{"}
		element_index -=i1;
		return (shm_event *) (e2 + (element_index*size_event));
	{"}"}
{"}"}

{declare_rule_sets(ast[3])}
{build_rule_set_functions(ast[3], streams_to_events_map, stream_types)}
{arbiter_code(ast[3])}
int main(int argc, char **argv) {"{"}
    initialize_events(); // Always call this first
    
{event_sources_conn_code(ast)}
    // activate buffers
{activate_buffers(ast)}
	monitor_buffer = shm_monitor_buffer_create(sizeof(STREAM_{arbiter_event_source}_out), 64);

    // create source-events threads
{activate_threads(ast)}

    // create arbiter thread
    thrd_create(&ARBITER_THREAD, arbiter, 0);
    
    
{monitor_code(ast[4], streams_to_events_map[arbiter_event_source], arbiter_event_source)}
     
    // destroy event sources
{destroy_streams(ast)}
    // destroy arbiter buffers
{destroy_buffers(ast)}
	// destroy monitor buffer
	// shm_monitor_buffer_destroy(monitor_buffer); TODO: should I call this?
{"}"}

'''

output_file.write(program)


