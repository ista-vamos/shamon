# all the function declared here return a string of C-code
from utils import *

def events_declaration_structs(tree) -> str:
    if tree[0] == "event_list":
        return events_declaration_structs(tree[1]) + "\n"+ events_declaration_structs(tree[2])
    else:
        assert(tree[0] == "event_decl")
        event_name = tree[1]
        names = []
        get_parameters_names_field_decl(tree[2], names)
        field_types = []
        get_parameters_types_field_decl(tree[2], field_types)
        struct_fields = ""

        for (index, (name, f_type)) in enumerate(zip(names, field_types)):
            is_last = index == len(names)-1
            if not is_last:
                struct_fields += f"\t{f_type[1]} {name};\n"
            else:
                struct_fields += f"\t{f_type[1]} {name};"

        return f'''struct _EVENT_{event_name} {"{"}
{struct_fields}
{"}"};
typedef struct _EVENT_{event_name} EVENT_{event_name};'''

def event_stream_structs(tree) -> str:
    if tree[0] == "stream_types":
        return event_stream_structs(tree[1]) +"\n" + event_stream_structs(tree[2])
    else:
        assert(tree[0] == "stream_type")
        stream_name = tree[1]

        events_names = []
        get_events_names(tree[2], events_names)
        union_events = ""
        for name in events_names:
            union_events += f"EVENT_{name} {name};"
        value = f'''// event declarations for stream type {stream_name}
{events_declaration_structs(tree[2])}

// input stream for stream type {stream_name}
struct _STREAM_{stream_name}_in {"{"}
    shm_event head;
    union {"{"}
        {union_events}
    {"}"}cases;
{"}"};
typedef struct _STREAM_{stream_name}_in STREAM_{stream_name}_in;

// output stream for stream type {stream_name}
struct _STREAM_{stream_name}_out {"{"}
    shm_event head;
    union {"{"}
        EVENT_hole hole;
        {union_events}
    {"}"}cases;
{"}"};
typedef struct _STREAM_{stream_name}_out STREAM_{stream_name}_out;
        '''
        return value


def event_sources_conn_code(ast) -> str:
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)

    buffer_sizes = []
    get_buffer_sizes(ast[2], buffer_sizes)

    out_names = []
    get_out_names(ast[2], out_names)

    answer = ""
    for (name, buff_size, out_name) in zip(event_srcs_names, buffer_sizes, out_names):
        answer += f"\t// connect to event source {name}\n"
        answer += f"\tshm_stream *EV_SOURCE_{name} = shm_stream_create(\"{name}\", argc, argv);\n"
        answer += f"\tBUFFER_{name} = shm_arbiter_buffer_create(EV_SOURCE_{name},  sizeof(STREAM_{out_name}_out), {buff_size});\n\n"

    return answer

def declare_evt_srcs_threads(ast) -> str:
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)

    answer = ""
    for name in event_srcs_names:
        answer += f"thrd_t THREAD_{name};\n"

    return answer


def declare_arbiter_buffers(ast) -> str:
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)

    answer = ""
    for name in event_srcs_names:
        answer += f"// Arbiter buffer for event source {name}\n"
        answer += f"shm_arbiter_buffer *BUFFER_{name};\n\n"

    return answer

def activate_threads(ast) -> str:
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)

    answer = ""
    for name in event_srcs_names:
        answer += f"\tthrd_create(&THREAD_{name}, PERF_LAYER_{name});\n"

    return answer


def activate_buffers(ast) -> str:
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)

    answer = ""
    for name in event_srcs_names:
        answer += f"\tshm_arbiter_buffer_set_active(BUFFER_{name}, true);\n"

    return answer


def destroy_buffers(ast) -> str:
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)

    answer = ""
    for name in event_srcs_names:
        answer += f"\tshm_arbiter_buffer_free(BUFFER_{name});\n"

    return answer

def destroy_streams(ast) -> str:
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)

    answer = ""
    for name in event_srcs_names:
        answer += f"\tshm_stream_destroy(EV_SOURCE_{name});\n"

    return answer


def process_performance_match(tree) -> str:
    if tree[0] == 'perf_match1':
        performance_action = tree[1]
        if performance_action[0] == "perf_act_drop":
            return "return false;"
        else:

            assert(performance_action[0] == "perf_act_forward")
            return "return true;"
    else:
        assert(tree[0] == 'perf_match2')
        return f'''if ({tree[1]}) {"{"}
            {process_performance_match(tree[2])}
        {"}"} else {"{"}
            {process_performance_match(tree[3])}
        {"}"}
        '''


def build_drop_funcs_conds(tree, stream_name, mapping) -> str:
    if tree[0] == "perf_layer_list":
        return build_drop_funcs_conds(tree[1], stream_name, mapping) \
               + build_drop_funcs_conds(tree[2], stream_name, mapping)
    else:
        assert(tree[0] == "perf_layer_rule")
        return f'''if (e->kind == {mapping[tree[1]]["index"]}) {"{"}
        {process_performance_match(tree[3])}
    {"}"}
        '''


def build_should_keep_funcs(tree, mapping) -> str:
    if tree[0] == 'event_sources':
        return build_should_keep_funcs(tree[1], mapping) + build_should_keep_funcs(tree[2], mapping)
    else:
        assert(tree[0] == 'event_source')
        ev_src_name = tree[1]
        stream_type = tree[2]
        performance_layer_rule_list = tree[5]
        return f'''bool SHOULD_KEEP_{ev_src_name}(shm_stream * s, shm_event * e) {"{"}
    {build_drop_funcs_conds(performance_layer_rule_list, stream_type, mapping[stream_type])}
    return false;
{"}"}
'''

def get_stream_switch_cases(ast) -> str:
    if ast[0] == 'perf_layer_list':
        return get_stream_switch_cases(ast[1]) + get_stream_switch_cases(ast[2])
    else:
        assert(ast[0] == 'perf_layer_rule')
        # TODO

def event_sources_thread_funcs(tree, mapping) -> str:
    if tree[0] == "event_sources":
        return event_sources_thread_funcs(tree[1], mapping) +"\n" + event_sources_thread_funcs(tree[2], mapping)
    else:
        assert(tree[0] == "event_source")
        stream_name = tree[1]
        stream_in_name = tree[2]
        stream_out_name = tree[4]
        value = f'''int PERF_LAYER_{stream_name} (void *arg) {"{"}
    shm_arbiter_buffer *buffer = BUFFER_{stream_name};
    shm_stream *stream = shm_arbiter_buffer_stream(buffer);   

    STREAM_{stream_in_name}_in *inevent;
    STREAM_{stream_out_name}_out *outevent;   

    // wait for active buffer
    while ((!shm_arbiter_buffer_active(buffer))){"{"}
        sleep_ns(10);
    {"}"}
    
    while(true) {"{"}
        inevent = stream_filter_fetch(stream,buffer,&SHOULD_KEEP_{stream_name});
        if (inevent == NULL) {"{"}
            break;
        {"}"}
        outevent = shm_arbiter_buffer_write_ptr(buffer);
        
        switch ((inevent->head).kind) {"{"}
{get_stream_switch_cases(tree[5])}
            default:
                printf("Default case execute in thread for event source {stream_name}. Exiting thread...");
                return 1;
        {"}"}
        shm_arbiter_buffer_write_finish(b);
        shm_stream_consume(stream, 1);
    {"}"}     
{"}"}
'''
        return value