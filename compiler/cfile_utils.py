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

def declare_event_sources(ast):
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)
    answer = ""
    for name in event_srcs_names:
        answer += f"shm_stream *EV_SOURCE_{name};\n"
    return answer

def declare_event_sources_flags(ast):
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)
    answer = ""
    for name in event_srcs_names:
        answer += f"bool is_{name}_done;\n"
    return answer

def get_count_events_sources(ast):
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)
    return len(event_srcs_names)

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
        answer += f"\tEV_SOURCE_{name} = shm_stream_create(\"{name}\", argc, argv);\n"
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


def assign_args(event_name, args, list_expressions, level) -> str:
    expressions = []
    get_expressions(list_expressions, expressions)
    answer = ""
    tabs = "\t"*level
    for (arg, expr) in zip(args, expressions):
        answer+=f"{tabs}outevent->cases.{event_name}.{arg[0]} = {expr};\n"
    return answer


def build_switch_performance_match(tree, mapping_in, mapping_out, level) -> str:
    if tree[0] == 'perf_match1':
        performance_action = tree[1]
        if performance_action[0] == "perf_act_drop":
            # this shouldn't happen
            return "return 1;"
        else:
            tabs = "\t"*level
            assert(performance_action[0] == "perf_act_forward")
            event_out_name = performance_action[1]
            return f'''
{tabs}(outevent->head).kind = {mapping_out[performance_action[1]]["index"]};
{tabs}(outevent->head).id = (inevent->head).id;
{assign_args(event_out_name, mapping_out[performance_action[1]]["args"], performance_action[2], level)}'''
    else:
        assert(tree[0] == 'perf_match2')
        return f'''
        if ({tree[1]}) {"{"}
            {build_switch_performance_match(tree[2], mapping_in, mapping_out, level+1)}
        {"}"} else {"{"}
            {build_switch_performance_match(tree[3], mapping_in, mapping_out, level+1)}
        {"}"}'''


def get_stream_switch_cases(ast, mapping_in, mapping_out, level) -> str:
    if ast[0] == 'perf_layer_list':
        return get_stream_switch_cases(ast[1], mapping_in, mapping_out, level) \
               + get_stream_switch_cases(ast[2], mapping_in, mapping_out, level)
    else:
        tabs = "\t"*level
        tabs_plus1 = "\t" * (level+1)
        assert(ast[0] == 'perf_layer_rule')
        return f'''
{tabs}case {mapping_in[ast[1]]["index"]}:
{build_switch_performance_match(ast[3], mapping_in, mapping_out, level=level+1)}
{tabs_plus1}break;'''

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
    
    is_{stream_name}_done = 0;
    while(true) {"{"}
        inevent = stream_filter_fetch(stream, buffer, &SHOULD_KEEP_{stream_name});
        
        if (inevent == NULL) {"{"}
            // no more events
            is_{stream_name}_done = 1;
            break;
        {"}"}
        outevent = shm_arbiter_buffer_write_ptr(buffer);
        
        switch ((inevent->head).kind) {"{"}
{get_stream_switch_cases(tree[5], mapping[stream_in_name], mapping[stream_out_name], level=3)}
            default:
                printf("Default case executed in thread for event source {stream_name}. Exiting thread...");
                return 1;
        {"}"}
        shm_arbiter_buffer_write_finish(b);
        shm_stream_consume(stream, 1);
    {"}"}     
{"}"}
'''
        return value


# BEGIN CORRECTNESS LAYER
def declare_rule_sets(tree):
    assert (tree[0] == "arbiter_def")
    rule_set_names = []
    get_rule_set_names(tree[2], rule_set_names)

    rule_set_declarations = ""
    for name in rule_set_names:
        rule_set_declarations += f"int RULE_SET_{name}();\n"
    return rule_set_declarations

def exists_open_streams(tree):
    event_srcs_names = []
    get_event_sources_names(tree[2], event_srcs_names)
    code = ""
    for name in event_srcs_names:
        code += f"\tc += is_{name}_done;\n"

    return f'''bool exists_open_streams() {"{"}
    int c = 0;
{code}\treturn c < count_event_streams;
{"}"}
    '''
def arbiter_code(tree):
    assert(tree[0] == "arbiter_def")

    rule_set_names = []
    get_rule_set_names(tree[2], rule_set_names)

    rule_set_invocations = ""
    rule_set_declarations = ""
    for name in rule_set_names:
        rule_set_invocations += f"\tRULE_SET_{name}();\n"
        rule_set_declarations += f"int RULE_SET_{name}();"


    return f'''int arbiter() {"{"}

    while (exists_open_stream()) {"{"}
    {rule_set_invocations}\t{"}"}
{"}"}
    '''

def rule_set_streams_condition(tree, mapping, stream_types):
    # output_stream should be the output type of the event source
    if tree[0] == 'l_buff_match_exp':
        return rule_set_streams_condition(tree[1], mapping, stream_types)+ " && " + \
               rule_set_streams_condition(tree[2], mapping, stream_types)
    else:
        assert(tree[0] == 'buff_match_exp')
        stream_name = tree[1]
        if len(tree) == 3:

            if tree[2] == "nothing":
                return f"check_n_events(EV_SOURCE_{stream_name}, 0)"
            elif tree[2] == "done":
                return f"is_{stream_name}_done"
            else:
                # check if there are n events
                return f"check_n_events(EV_SOURCE_{stream_name}, {tree[2]})"

        else:
            assert(len(tree) == 4)
            event_kinds = []
            out_type = stream_types[stream_name][1]
            if tree[2] != "|":
                get_event_kinds(tree[2], event_kinds, mapping[out_type])

            if tree[3] != "|":
                get_event_kinds(tree[3], event_kinds, mapping[out_type])
            kinds = ",".join([str(x) for x in event_kinds])
            return f"are_events_in_head(EV_SOURCE_{stream_name}, BUFFER_{stream_name}, sizeof(STREAM_{out_type}_out), [{kinds}], {len(event_kinds)})"


def process_arb_rule_stmt(tree, mapping, stream_types, output_ev_source) -> str:
    if tree[0] == "switch":
        switch_rule_name = tree[1]
        return f"RULE_SET_{switch_rule_name}();\n"
    if tree[0] == "yield":
        return ""
    assert(tree[0] == "drop")
    return f"shm_arbiter_buffer_drop(BUFFER_{tree[2]}, {tree[1]});\n"


def process_code_stmt_list(tree, mapping, stream_types, output_ev_source) -> str:
    assert (tree[0] == "ccode_statement_l")

    if len(tree) == 2:
        return tree[1]

    assert(len(tree) > 1)

    if len(tree) == 3:
        assert(tree[2] == ";")
        return process_arb_rule_stmt(tree[1], mapping, stream_types, output_ev_source)

    assert(len(tree) == 4)

    if tree[3] == ";":
        return tree[1] + "\n" + process_arb_rule_stmt(tree[2], mapping, stream_types, output_ev_source)
    else:
        assert(tree[2] == ";")
        return process_arb_rule_stmt(tree[1], mapping, stream_types, output_ev_source) + "\n" + tree[3]


def get_arb_rule_stmt_list_code(tree, mapping, stream_types, output_ev_source) -> str:
    if tree[0] == 'arb_rule_stmt_l':
        return process_code_stmt_list(tree[1], mapping, stream_types, output_ev_source) + \
               get_arb_rule_stmt_list_code(tree[2], mapping, stream_types, output_ev_source)
    else:
        assert(tree[0] == "ccode_statement_l")
        return process_code_stmt_list(tree, mapping, stream_types, output_ev_source)


def arbiter_rule_code(tree, mapping, stream_types, output_ev_source) -> str:
    # output_stream should be the output type of the event source
    if tree[0] == "arb_rule_list":
        return arbiter_rule_code(tree[1], mapping, stream_types, output_ev_source) + \
               arbiter_rule_code(tree[2], mapping, stream_types, output_ev_source)
    else:
        assert(tree[0] == "arbiter_rule")
        return f'''
    if ({rule_set_streams_condition(tree[1], mapping, stream_types)}) {"{"}
        if({tree[2]}) {"{"}
            {get_arb_rule_stmt_list_code(tree[3], mapping, stream_types, output_ev_source)}
        {"}"}
    {"}"}
        '''


def get_code_rule_sets(tree, mapping, stream_types, output_ev_source) -> str:
    if tree[0] == 'arb_rule_set_l':
        return get_code_rule_sets(tree[1], mapping, stream_types, output_ev_source) \
               + get_code_rule_sets(tree[2], mapping, stream_types, output_ev_source)
    else:
        assert(tree[0] == "arbiter_rule_set")
        rule_set_name = tree[1]
        return f'''int RULE_SET_{rule_set_name}() {"{"}
    {arbiter_rule_code(tree[2], mapping, stream_types, output_ev_source)}
{"}"}
'''

def build_rule_set_functions(tree, mapping, stream_types):
    assert(tree[0] == "arbiter_def")
    output_stream = tree[1]

    return get_code_rule_sets(tree[2], mapping, stream_types, output_stream)

# monitor code

def monitor_events_code(tree, possible_events, count_tabs) -> str:
    if tree[0] == 'monitor_rule_l':
        return monitor_events_code(tree[1], possible_events, count_tabs) + \
               monitor_events_code(tree[2], possible_events, count_tabs)
    else:
        assert(tree[0] == "monitor_rule")
        event = tree[1]
        tabs = "\t"*count_tabs
        return f'''
{tabs}if (received_event->head.kind == {possible_events[event]["index"]}) {"{"}
{tabs}  if ({tree[3]}) {"{"}
{tabs}      {tree[4]}
{tabs}  {"}"}
{tabs}{"}"}
        '''


def monitor_code(tree, possible_events) -> str:
    return f'''
    // monitor
    while(true) {"{"}
{monitor_events_code(tree[1], possible_events, 2)}
    {"}"}
    '''

