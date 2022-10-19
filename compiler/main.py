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
components = dict()
get_components_dict(ast[1], components)
# # Type checker again
TypeChecker.get_stream_events(components["stream_type"])
if "stream_processor" in components.keys():
	TypeChecker.get_stream_processors_data(components["stream_processor"])
# TypeChecker.check_event_sources_types(ast[PMAIN_PROGRAM_EVENT_SOURCES])
# TypeChecker.check_arbiter(ast[PMAIN_PROGRAM_ARBITER])
# TypeChecker.check_monitor(ast[PMAIN_PROGRAM_MONITOR])
#
# Produce C file
output_file = open(output_path, "w")
#
streams_to_events_map = get_stream_to_events_mapping(components["stream_type"], TypeChecker.stream_processors_data)

stream_types : Dict[str, Tuple[str, str]] = get_stream_types(components["event_source"])
print(stream_types)
arbiter_event_source = get_arbiter_event_source(ast[2])
existing_buffers = get_existing_buffers(TypeChecker)

TypeChecker.arbiter_output_type = arbiter_event_source


program = f'''#include "shamon.h"
#include "mmlib.h"
#include "monitor.h"
#include "./compiler/cfiles/compiler_utils.h"
#include <threads.h>
#include <signal.h>
#include <stdio.h>
#include <stdatomic.h>

struct _EVENT_hole
{"{"}
  uint64_t n;
{"}"};
typedef struct _EVENT_hole EVENT_hole;
{stream_type_structs(components["stream_type"])}
{stream_type_args_structs(components["stream_type"])}
{events_enum_kinds(components["event_source"], streams_to_events_map)}

{instantiate_stream_args()}
int *arbiter_counter;
// monitor buffer
shm_monitor_buffer *monitor_buffer;

bool is_selection_successful;
dll_node **chosen_streams; // used in rule set for get_first/last_n

// globals code
{get_globals_code(components, streams_to_events_map, stream_types)}
{build_should_keep_funcs(components["event_source"], streams_to_events_map)}

atomic_int count_event_streams = {get_count_events_sources()};

// declare event streams
{declare_event_sources(components["event_source"])}

// event sources threads
{declare_evt_srcs_threads()}

// declare arbiter thread
thrd_t ARBITER_THREAD;
{declare_const_rule_set_names(ast[2])}
{declare_rule_set_counters(ast[2])}
int current_rule_set = {get_first_const_rule_set_name(ast[2])};

{declare_arbiter_buffers(components, ast)}


// buffer groups
{declare_order_expressions()}
{declare_buffer_groups()}

{event_sources_thread_funcs(components["event_source"], streams_to_events_map)}

// variables used to debug arbiter
long unsigned no_consecutive_matches_limit = 1UL<<35;
int no_matches_count = 0;

bool are_there_events(shm_arbiter_buffer * b) {"{"}
  return shm_arbiter_buffer_size(b) > 0;
{"}"}

{are_buffers_empty()}

static int __work_done = 0;
/* TODO: make a keywork from this */
void done() {{
    __work_done = 1;
}}

static inline bool are_streams_done() {"{"}
    assert(count_event_streams >=0);
    return count_event_streams == 0 && are_buffers_empty() || __work_done;
{"}"}

static inline bool is_stream_done(shm_stream *s) {"{"}
    return !shm_stream_is_ready(s);
{"}"}


static inline
bool check_at_least_n_events(size_t count, size_t n) {"{"}
    // count is the result after calling shm_arbiter_buffer_peek
	return count >= n;
{"}"}

static
bool are_events_in_head(char* e1, size_t i1, char* e2, size_t i2, int count, size_t ev_size, int event_kinds[], int n_events) {"{"}
    assert(n_events > 0);
	if (count < n_events) {"{"}
	    return false;
    {"}"}

	int i = 0;
	while (i < i1) {"{"}
	    shm_event * ev = (shm_event *) (e1);
	     if (ev->kind != event_kinds[i]) {"{"}
	        return false;
	    {"}"}
        if (--n_events == 0)
            return true;
	    i+=1;
	    e1 += ev_size;
	{"}"}

	i = 0;
	while (i < i2) {"{"}
	    shm_event * ev = (shm_event *) e2;
	     if (ev->kind != event_kinds[i1+i]) {"{"}
	        return false;
	    {"}"}
        if (--n_events == 0)
            return true;
	    i+=1;
	    e2 += ev_size;
	{"}"}

	return true;
{"}"}

#define vamos_check(cond) do {{ if (!cond) {{fprintf(stderr, "\033[31m%s:%s:%d: check '" #cond "' failed!\033[0m\\n", __FILE__, __func__, __LINE__); print_buffers_state(); }} }} while(0)
#define vamos_assert(cond) do{{ if (!cond) {{fprintf(stderr, "\033[31m%s:%s:%d: assert '" #cond "' failed!\033[0m\\n", __FILE__, __func__, __LINE__); print_buffers_state(); __work_done = 1; }} }} while(0)
#define vamos_hard_assert(cond) do{{ if (!cond) {{fprintf(stderr, "\033[31m%s:%s:%d: assert '" #cond "' failed!\033[0m\\n", __FILE__, __func__, __LINE__); print_buffers_state(); __work_done = 1; abort();}} }} while(0)

{get_event_name(stream_types, streams_to_events_map)}

static inline dump_event_data(shm_event *ev, size_t ev_size) {{
    unsigned char *data = ev;
    fprintf(stderr, "[");
    for (unsigned i = sizeof(*ev); i < ev_size; ++i) {{
        fprintf(stderr, "0x%x ", data[i]);
    }}
    fprintf(stderr, "]");
}}

/* src_idx = -1 if unknown */
static void
print_buffer_prefix(shm_arbiter_buffer *b, int src_idx, size_t n_events, int cnt, char* e1, size_t i1, char* e2, size_t i2) {"{"}
    if (cnt == 0) {{
        fprintf(stderr, " empty\\n");
        return;
    }}
    const size_t ev_size = shm_arbiter_buffer_elem_size(b);
    int n = 0;
	int i = 0;
	while (i < i1) {"{"}
	    shm_event * ev = (shm_event *) (e1);
        fprintf(stderr, "  %d: {{id: %5lu, kind: %3lu", ++n,
                shm_event_id(ev), shm_event_kind(ev));
        if (src_idx != -1)
            fprintf(stderr, " -> %-12s", get_event_name(src_idx, shm_event_kind(ev)));
        /*dump_event_data(ev, ev_size);*/
        fprintf(stderr, "}}\\n");
        if (--n_events == 0)
            return;
	    i+=1;
	    e1 += ev_size;
	{"}"}

	i = 0;
	while (i < i2) {"{"}
	    shm_event * ev = (shm_event *) e2;
        fprintf(stderr, "  %d: {{id: %5lu, kind: %3lu", ++n,
                shm_event_id(ev), shm_event_kind(ev));
        if (src_idx != -1)
            fprintf(stderr, " -> %-12s", get_event_name(src_idx, shm_event_kind(ev)));
        /*dump_event_data(ev, ev_size);*/
        fprintf(stderr, "}}\\n");

        if (--n_events == 0)
            return;
	    i+=1;
	    e2 += ev_size;
	{"}"}
{"}"}


static inline
shm_event * get_event_at_index(char* e1, size_t i1, char* e2, size_t i2, size_t size_event, int element_index) {"{"}
	if (element_index < i1) {"{"}
		return (shm_event *) (e1 + (element_index*size_event));
	{"}"} else {"{"}
		element_index -=i1;
		return (shm_event *) (e2 + (element_index*size_event));
	{"}"}
{"}"}

//arbiter outevent
STREAM_{arbiter_event_source}_out *arbiter_outevent;
{declare_rule_sets(ast[2])}

{print_event_name(stream_types, streams_to_events_map)}
{get_event_at_head()}
{print_buffers_state()}
{build_rule_set_functions(ast[2], streams_to_events_map, stream_types, existing_buffers)}
{arbiter_code(ast[2], components)}

{define_signal_handlers(components["event_source"])}

static void setup_signals() {{
    if (signal(SIGINT, sig_handler) == SIG_ERR) {{
	perror("failed setting SIGINT handler");
    }}

    if (signal(SIGABRT, sig_handler) == SIG_ERR) {{
	perror("failed setting SIGINT handler");
    }}

    if (signal(SIGIOT, sig_handler) == SIG_ERR) {{
	perror("failed setting SIGINT handler");
    }}

    if (signal(SIGSEGV, sig_handler) == SIG_ERR) {{
	perror("failed setting SIGINT handler");
    }}
}}

int main(int argc, char **argv) {"{"}
    setup_signals();

	chosen_streams = (dll_node *) malloc({get_count_events_sources()}); // the maximum size this can have is the total number of event sources
	arbiter_counter = malloc(sizeof(int));
	*arbiter_counter = 10;
	{get_pure_c_code(components, 'startup')}
{initialize_stream_args()}

{event_sources_conn_code(components['event_source'], streams_to_events_map)}
     // activate buffers
{activate_buffers()}
 	monitor_buffer = shm_monitor_buffer_create(sizeof(STREAM_{arbiter_event_source}_out), {TypeChecker.monitor_buffer_size});
 	
 		// init buffer groups
	{init_buffer_groups()}
 	
     // create source-events threads
{activate_threads()}

     // create arbiter thread
     thrd_create(&ARBITER_THREAD, arbiter, 0);
     
 {monitor_code(ast[3], streams_to_events_map, arbiter_event_source)}
 	
{destroy_all()}
	
{get_pure_c_code(components, 'cleanup')}
{"}"}
'''


output_file.write(program)


