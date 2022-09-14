# all the function declared here return a string of C-code
from typing import Type
from utils import *
from parser_indices import *
from type_checker import TypeChecker
from itertools import permutations


class StaticCounter:
    declarations_counter = 0
    calls_counter = 0

    match_expr_counter = 0
    match_expr_calls_counter = 0

def get_pure_c_code(component, token) -> str:
    answer = ""
    if token in component.keys():
        for tree in component[token]:
            assert(tree[0] == "startup" or tree[0] == "cleanup")
            answer += tree[1]
    return answer

def get_globals_code(components, mapping, stream_types) -> str:
    answer = ""
    if "globals" in components.keys():
        globals = components["globals"]
        answer = f"STREAM_{TypeChecker.arbiter_output_type}_out *arbiter_outevent;\n\n"
        for tree in globals:
            assert(tree[0] == "globals")
            answer += get_arb_rule_stmt_list_code(tree[1], mapping, {}, stream_types, TypeChecker.arbiter_output_type)
    return answer

def declare_order_expressions():
    answer = ""

    for (buff_name, data) in TypeChecker.buffer_group_data.items():
        if data["order"] == "round-robin":
            code = "return false;"
        else:
            code = f"return ((STREAM_{data['in_stream']}_ARGS *) args1)->{data['order']} > ((STREAM_{data['in_stream']}_ARGS *) args2)->{data['order']};"
        answer += f'''
bool {buff_name}_ORDER_EXP (void *args1, void *args2) {"{"}
    {code}
{"}"}        
'''
    return answer

def declare_buffer_groups():
    answer = ""

    for (buff_name, data) in TypeChecker.buffer_group_data.items():
        answer += f'''
buffer_group BG_{buff_name};
        '''
    return answer

def init_buffer_groups():
    answer = ""

    for (buff_name, data) in TypeChecker.buffer_group_data.items():
        includes_str = ""
        for i in range(data["arg_includes"]):
            includes_str += f"\tbg_insert(&BG_{buff_name}, EV_SOURCE_{data['includes']}_{i}, BUFFER_{data['includes']}{i},stream_args_{data['includes']}_{i},{buff_name}_ORDER_EXP);\n"
        answer += f'''init_buffer_group(&BG_{buff_name});
{includes_str}        
'''
    return answer

def get_stream_struct_fields(field_declarations):
    if field_declarations is None:
        return ""
    fields = []
    get_parameters_types_field_decl(field_declarations, fields)
    struct_fields = ""
    for field in fields:
        struct_fields += f"\t{field['type']} {field['name']};\n"
    return struct_fields

def stream_arg_structs(stream_types) -> str:
    answer = ""
    for tree in stream_types:
        assert(tree[0] == "stream_type")
        if tree[2] is not None:
            struct_fields = get_stream_struct_fields(tree[2])
            answer += f'''
// args for stream type {tree[1]}
struct _{tree[1]}_ARGS {"{"}
{struct_fields}
{"}"}
typedef struct  _{tree[1]}_ARGS  {tree[1]}_ARGS;
            '''
    return answer
def events_declaration_structs(tree) -> str:
    if tree[0] == "event_list":
        return events_declaration_structs(tree[PLIST_BASE_CASE]) + "\n"+ events_declaration_structs(tree[PLIST_TAIL])
    else:
        assert(tree[0] == "event_decl")
        event_name = tree[PPEVENT_NAME]
        fields = []
        get_parameters_types_field_decl(tree[PPEVENT_PARAMS_LIST], fields)
        struct_fields = ""
        index = 0
        for data in fields:
            is_last = index == len(data)-1
            if not is_last:
                struct_fields += f"\t{data['type']} {data['name']};\n"
            else:
                struct_fields += f"\t{data['type']} {data['name']};"

        return f'''struct _EVENT_{event_name} {"{"}
{struct_fields}
{"}"};
typedef struct _EVENT_{event_name} EVENT_{event_name};'''

def stream_type_args_structs(stream_types) -> str:
    answer = ""
    for tree in stream_types:
        assert(tree[0] == "stream_type")
        stream_name = tree[PPSTREAM_TYPE_NAME]
        stream_arg_fields = get_stream_struct_fields(tree[2])
        if stream_arg_fields != "":
            answer += f'''
    typedef struct _STREAM_{stream_name}_ARGS {'{'}
    {stream_arg_fields}
    {'}'} STREAM_{stream_name}_ARGS;
            '''
    return answer

def instantiate_stream_args():
    answer = ""
    for (stream_name, data) in TypeChecker.event_sources_data.items():
        if len(data['input_stream_args']) > 0:
            stream_type = data["input_stream_type"]
            for i in range(data['copies']):
                answer += f"STREAM_{stream_type}_ARGS *stream_args_{stream_name}_{i};\n"
    return answer

def initialize_stream_args():
    answer = ""

    for (stream_name, data) in TypeChecker.event_sources_data.items():
        if len(data['input_stream_args']) > 0:
            stream_type = data["input_stream_type"]
            for i in range(data['copies']):
                answer += f"stream_args_{stream_name}_{i} = malloc(sizeof(STREAM_{stream_type}_ARGS));\n"

    for (event_source, data) in TypeChecker.event_sources_data.items():
        stream_args = TypeChecker.args_table[data["input_stream_type"]]
        if len(data['input_stream_args']) :
            for i in range(data['copies']):
                for stream_arg, arg_value in zip(stream_args, data["input_stream_args"]):
                    arg_name = stream_arg["name"]
                    answer += f"\tstream_args_{event_source}_{i}->{arg_name} = {arg_value};\n";
    return answer

def stream_type_structs(stream_types) -> str:
    answer = ""
    for tree in stream_types:
        assert(tree[0] == "stream_type")
        stream_name = tree[PPSTREAM_TYPE_NAME]
        union_events = ""
        for name in TypeChecker.stream_types_to_events[stream_name]:
            union_events += f"EVENT_{name} {name};"
        value = f'''// event declarations for stream type {stream_name}
{events_declaration_structs(tree[-1])}

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
        answer += value
    return answer

def declare_event_sources(event_sources):
    event_srcs_names = []
    get_event_sources_names(event_sources, event_srcs_names)
    answer = ""
    for name in event_srcs_names:
        for  i in range(TypeChecker.event_sources_data[name]["copies"]):
            answer += f"shm_stream *EV_SOURCE_{name}_{i};\n"
    return answer

def declare_event_sources_flags(ast):
    assert (ast[0] == "main_program")
    event_srcs_names = []
    get_event_sources_names(ast[PMAIN_PROGRAM_EVENT_SOURCES], event_srcs_names)
    answer = ""
    for name in event_srcs_names:
        answer += f"bool is_{name}_done;\n"
    return answer

def get_count_events_sources():
    total_count = 0
    for (event_source, data) in TypeChecker.event_sources_data.items():
        total_count += data['copies']
    return total_count

def event_sources_conn_code(event_sources) -> str:


    answer = ""
    for event_source in event_sources:
        assert(event_source[0] == "event_source")
        event_src_declaration = event_source[2]
        assert(event_src_declaration[0] == 'event-decl')
        stream_name, args = get_name_with_args(event_src_declaration[1])
        copies = TypeChecker.event_sources_data[stream_name]["copies"]
        
        processor_name = TypeChecker.event_sources_data[stream_name]["processor_name"]

        if processor_name == "forward":
            out_name = TypeChecker.event_sources_data[stream_name]["input_stream_type"]
        else:
            out_name = TypeChecker.stream_processors_data[processor_name]["output_type"]

        connection_kind = TypeChecker.event_sources_data[stream_name]["connection_kind"]
        assert(connection_kind[0] == "conn_kind")
        buff_size = connection_kind[2]
        for i in range(copies):
            name = f"{stream_name}_{i}"
            answer += f"\t// connect to event source {name}\n"
            answer += f"\tEV_SOURCE_{name} = shm_stream_create(\"{name}\", argc, argv);\n"
            answer += f"\tBUFFER_{stream_name}{i} = shm_arbiter_buffer_create(EV_SOURCE_{name},  sizeof(STREAM_{out_name}_out), {buff_size});\n\n"

    return answer

def declare_evt_srcs_threads() -> str:
    answer = ""
    for (event_source, data) in TypeChecker.event_sources_data.items():
        for i in range(data["copies"]):
            name = f"{event_source}_{i}"
            answer += f"thrd_t THREAD_{name};\n"

    return answer


def declare_arbiter_buffers(components, ast) -> str:
    assert (ast[0] == "main_program")
    event_srcs_names = get_event_sources_copies(components["event_source"])

    answer = ""
    for (name, copies) in event_srcs_names:
        if copies == 0:
            answer += f"// Arbiter buffer for event source {name}\n"
            answer += f"shm_arbiter_buffer *BUFFER_{name};\n\n"
        else:
            for i in range(copies):
                answer += f"// Arbiter buffer for event source {name} ({i})\n"
                answer += f"shm_arbiter_buffer *BUFFER_{name}{i};\n\n"
    return answer

def activate_threads() -> str:
    answer = ""
    for (event_source, data) in TypeChecker.event_sources_data.items():
        copies = data["copies"]
        for i in range(copies):
            name = f"{event_source}_{i}"
            answer += f"\tthrd_create(&THREAD_{name}, PERF_LAYER_{event_source},BUFFER_{event_source}{i});\n"


    return answer


def activate_buffers() -> str:
    answer = ""
    for (event_source, data) in TypeChecker.event_sources_data.items():
        for i in range(data["copies"]):
            name = f"{event_source}{i}"
            answer += f"\tshm_arbiter_buffer_set_active(BUFFER_{name}, true);\n"

    return answer


def destroy_buffers() -> str:
    answer = ""
    for (event_source, data) in TypeChecker.event_sources_data.items():
        for i in range(data["copies"]):
            name = f"{event_source}{i}"
            answer += f"\tshm_arbiter_buffer_free(BUFFER_{name});\n"

    return answer

def destroy_streams() -> str:
    answer = ""
    for (event_source, data) in TypeChecker.event_sources_data.items():
        for i in range(data["copies"]):
            name = f"{event_source}_{i}"
            answer += f"\tshm_stream_destroy(EV_SOURCE_{name});\n"

    return answer


def process_performance_match(tree) -> str:
    if tree[0] == 'perf_match1':
        performance_action = tree[PPPERF_MATCH_ACTION]
        if performance_action[0] == "perf_act_drop":
            return "return false;"
        else:

            assert(performance_action[0] == "perf_act_forward")
            return "return true;"
    else:
        assert(tree[0] == 'perf_match2')
        return f'''if ({tree[PPPERF_MATCH_EXPRESSION]}) {"{"}
            {process_performance_match(tree[PPPERF_MATCH_TRUE_PART])}
        {"}"} else {"{"}
            {process_performance_match(tree[PPPERF_MATCH_FALSE_PART])}
        {"}"}
        '''


def build_drop_funcs_conds(tree, stream_name, mapping) -> str:
    if tree[0] == "perf_layer_list":
        return build_drop_funcs_conds(tree[PLIST_BASE_CASE], stream_name, mapping) \
               + build_drop_funcs_conds(tree[PLIST_TAIL], stream_name, mapping)
    else:
        assert(tree[0] == "perf_layer_rule")
        event_name, _ = get_name_with_args(tree[1])
        creates_at_most = tree[2]
        if creates_at_most is not None:
            raise Exception("creates at most not implemented in stream processors")
        return f'''if (e->kind == {mapping[tree[event_name]]["index"]}) {"{"}
        {process_performance_match(tree[-1])}
    {"}"}
        '''


def build_should_keep_funcs(event_sources, mapping) -> str:
    answer = ""
    for tree in event_sources:
        assert(tree[0] == 'event_source')
        ev_src_name = get_event_src_name(tree[2])
        stream_type, _ = get_name_with_args(tree[3])

        event_src_tail = tree[4]
        process_using = event_src_tail[1]
        if process_using is not None:
            name, _ = get_name_with_args(process_using)

        else:
            name = "forward"

        if name.lower() == "forward":
            answer += f'''bool SHOULD_KEEP_{ev_src_name}(shm_stream * s, shm_event * e) {"{"}
    return true;
{"}"}
            '''
        else:
            performance_layer_rule_list = TypeChecker.stream_processors_data[name]["perf_layer_rule_list"]
            answer += f'''bool SHOULD_KEEP_{ev_src_name}(shm_stream * s, shm_event * e) {"{"}
    {build_drop_funcs_conds(performance_layer_rule_list, stream_type, mapping[stream_type])}
    return false;
{"}"}
                        '''
    return answer


def assign_args(event_name, args, list_expressions, level) -> str:
    expressions = []
    get_expressions(list_expressions, expressions)
    answer = ""
    tabs = "\t"*level
    for (arg, expr) in zip(args, expressions):
        answer+=f"{tabs}outevent->cases.{event_name}.{arg[0]} = {expr};\n"
    return answer

def declare_performance_layer_args(event_case: str, mapping_in: Dict[str, Any], ids) -> str:
    to_declare_ids = []
    get_expressions(ids, to_declare_ids)
    assert(len(to_declare_ids) == len(mapping_in['args']))
    answer = ""
    args = mapping_in['args']
    for i in range(len(to_declare_ids)):
        answer += f"{args[i][1]} {to_declare_ids[i]} = inevent->cases.{event_case}.{args[i][0]} ;\n"
    return answer
    

def build_switch_performance_match(tree, mapping_in, mapping_out, level) -> str:
    if tree[0] == 'perf_match1':
        performance_action = tree[PPERF_MATCH_ACTION]
        if performance_action[0] == "perf_act_drop":
            # this shouldn't happen
            return "return 1;"
        else:
            tabs = "\t"*level
            assert(performance_action[0] == "perf_act_forward")
            event_out_name = performance_action[PPPERF_ACTION_FORWARD_EVENT]
            return f'''
{tabs}(outevent->head).kind = {mapping_out[performance_action[PPPERF_ACTION_FORWARD_EVENT]]["index"]};
{tabs}(outevent->head).id = (inevent->head).id;
{declare_performance_layer_args(performance_action[PPPERF_ACTION_FORWARD_EVENT], 
                                mapping_in[performance_action[PPPERF_ACTION_FORWARD_EVENT]], 
                                performance_action[PPPERF_ACTION_FORWARD_EXPRS])}
{assign_args(event_out_name, mapping_out[performance_action[PPPERF_ACTION_FORWARD_EVENT]]["args"], 
             performance_action[PPPERF_ACTION_FORWARD_EXPRS], level)}'''
    else:
        assert(tree[0] == 'perf_match2')
        return f'''
        if ({tree[PPPERF_MATCH_EXPRESSION]}) {"{"}
            {build_switch_performance_match(tree[PPPERF_MATCH_TRUE_PART], mapping_in, mapping_out, level+1)}
        {"}"} else {"{"}
            {build_switch_performance_match(tree[PPPERF_MATCH_FALSE_PART], mapping_in, mapping_out, level+1)}
        {"}"}'''


def get_stream_switch_cases(ast, mapping_in, mapping_out, level) -> str:
    if ast[0] == 'perf_layer_list':
        return get_stream_switch_cases(ast[PLIST_BASE_CASE], mapping_in, mapping_out, level) \
               + get_stream_switch_cases(ast[PLIST_TAIL], mapping_in, mapping_out, level)
    else:
        tabs = "\t"*level
        tabs_plus1 = "\t" * (level+1)
        assert(ast[0] == 'perf_layer_rule')
        return f'''
{tabs}case {mapping_in[ast[PPPERF_LAYER_EVENT]]["index"]}:
{build_switch_performance_match(ast[-1], mapping_in, mapping_out, level=level+1)}
{tabs_plus1}break;'''

def event_sources_thread_funcs(event_sources, mapping) -> str:
    answer = ""
    for tree in event_sources:
        assert(tree[0] == "event_source")
        stream_name = get_event_src_name(tree[2])
        stream_in_name, args_in = get_name_with_args(tree[3])
        stream_tail = tree[-1]
        if stream_tail[1] is not None:
            processor_name, _ = get_name_with_args(stream_tail[1])
        else:
            processor_name = None
        if processor_name is not None and processor_name.lower() != "forward":
            stream_out_name = TypeChecker.stream_processors_data[processor_name]["output_type"]
            perf_layer_list = TypeChecker.stream_processors_data[processor_name]["perf_layer_rule_list"]
            perf_layer_code = f'''
                        switch ((inevent->head).kind) {"{"}
            {get_stream_switch_cases(perf_layer_list, mapping[stream_in_name], mapping[stream_out_name], level=3)}
                        default:
                            printf("Default case executed in thread for event source {stream_name}. Exiting thread...");
                            return 1;
                    {"}"}
                        '''
        else:
            stream_out_name = stream_in_name
            perf_layer_code = f"memcpy(outevent, inevent, sizeof(STREAM_{stream_out_name}_out));"
        answer+= f'''int PERF_LAYER_{stream_name} (shm_arbiter_buffer *buffer) {"{"}
    shm_stream *stream = shm_arbiter_buffer_stream(buffer);   
    STREAM_{stream_in_name}_in *inevent;
    STREAM_{stream_out_name}_out *outevent;   

    // wait for active buffer
    while ((!shm_arbiter_buffer_active(buffer))){"{"}
        sleep_ns(10);
    {"}"}
    while(true) {"{"}
        inevent = stream_filter_fetch(stream, buffer, &SHOULD_KEEP_{stream_name});
        
        if (inevent == NULL) {"{"}
            // no more events
            break;
        {"}"}
        outevent = shm_arbiter_buffer_write_ptr(buffer);
        
        {perf_layer_code}
        shm_arbiter_buffer_write_finish(buffer);
        shm_stream_consume(stream, 1);
    {"}"}  
    atomic_fetch_add(&count_event_streams, -1);   
{"}"}
'''
    return answer


# BEGIN CORRECTNESS LAYER
def declare_rule_sets(tree):
    assert (tree[0] == "arbiter_def")
    rule_set_names = []
    get_rule_set_names(tree[PPARBITER_RULE_SET_LIST], rule_set_names)

    rule_set_declarations = ""
    for name in rule_set_names:
        rule_set_declarations += f"int RULE_SET_{name}();\n"
    return rule_set_declarations

def exists_open_streams():
    return f'''bool exists_open_streams() {"{"}
    return count_event_streams > 0;
{"}"}
    '''
def arbiter_code(tree):
    assert(tree[0] == "arbiter_def")

    rule_set_names = []
    get_rule_set_names(tree[PPARBITER_RULE_SET_LIST], rule_set_names)

    rule_set_invocations = ""
    for name in rule_set_names:
        rule_set_invocations += f"\tRULE_SET_{name}();\n"


    return f'''int arbiter() {"{"}
    while (exists_open_streams()) {"{"}
    {rule_set_invocations}\t{"}"}
    shm_monitor_set_finished(monitor_buffer);
{"}"}
    '''

def rule_set_streams_condition(tree, mapping, stream_types, inner_code="", is_scan=False, context=None):
    # output_stream should be the output type of the event source
    if tree[0] == 'l_buff_match_exp':
        if not is_scan:
            code_tail = rule_set_streams_condition(tree[PLIST_TAIL], mapping, stream_types, inner_code, is_scan, context)
            code_base = rule_set_streams_condition(tree[PLIST_BASE_CASE], mapping, stream_types, code_tail, is_scan, context)
            return code_base
        else:
            return rule_set_streams_condition(tree[PLIST_BASE_CASE], mapping, stream_types, inner_code, is_scan) \
                   + rule_set_streams_condition(tree[PLIST_TAIL], mapping, stream_types, inner_code, is_scan)
    elif tree[0] == 'buff_match_exp':
        event_src_ref = tree[1]
        assert(event_src_ref[0] == 'event_src_ref')
        stream_name = event_src_ref[1]
        out_type = stream_types[stream_name][1]
        if event_src_ref[2] is not None:
            stream_name += f"_{str(event_src_ref[2])}"
        if context is not None:
            if stream_name in context.keys():
                stream_name = context[stream_name]
        if len(tree) == 3:
            if not is_scan:
                if tree[PPBUFFER_MATCH_ARG1] == "nothing":
                    return f'''if (check_n_events(EV_SOURCE_{stream_name}, 0)) {"{"}
                    {inner_code}
                    {"}"}'''
                elif tree[PPBUFFER_MATCH_ARG1] == "done":
                    return f'''if (is_stream_done(EV_SOURCE_{stream_name})) {"{"}
                        {inner_code}
                    {"}"}
                    '''
                else:
                    # check if there are n events
                    return f'''if (check_n_events(EV_SOURCE_{stream_name}, {tree[PPBUFFER_MATCH_ARG1]})) {"{"}
                        {inner_code}
                    {"}"}'''
            return []

        else:
            assert(len(tree) == 4)
            event_kinds = []
            if tree[PPBUFFER_MATCH_ARG1] != "|":
                get_event_kinds(tree[PPBUFFER_MATCH_ARG1], event_kinds, mapping[out_type])

            if tree[PPBUFFER_MATCH_ARG2] != "|":
                get_event_kinds(tree[PPBUFFER_MATCH_ARG2], event_kinds, mapping[out_type])
            if not is_scan:
                StaticCounter.calls_counter+=1
                buffer_name = event_src_ref[1]
                if event_src_ref[2] is not None:
                    buffer_name += str(event_src_ref[2])


                return f'''
                if (are_events_in_head(BUFFER_{buffer_name}, sizeof(STREAM_{out_type}_out), TEMPARR{StaticCounter.calls_counter-1}, {len(event_kinds)})) {"{"}
                    {inner_code}
                    
                {"}"}'''
            else:
                return [event_kinds]
    elif tree[0] == "buff_match_exp-choose":
        if is_scan:
            return []
        choose_order = tree[1]

        buffer_name = tree[3]
        if context is not None:
            if buffer_name in context.keys():
                raise Exception("buffer name is in context created in match fun")
        assert buffer_name in TypeChecker.buffer_group_data.keys()

        temp_binded_streams = []
        get_list_ids(tree[2], temp_binded_streams)

        order = "first"
        if choose_order is not None:
            assert (choose_order[0] == "choose-order")
            order = choose_order[1]
            count_choose = choose_order[2]
            if context is not None:
                if count_choose in context.keys():
                    count_choose = context[count_choose]
        else:
            count_choose = len(temp_binded_streams)
        

        if order == "first":
            choose_statement = f"chosen_streams = bg_get_first_n(&BG_{buffer_name}, {count_choose});\n"
        else:
            choose_statement = f"chosen_streams = bg_get_last_n(&BG_{buffer_name}, {count_choose});\n"

        binded_streams = []
        if context is not None:
            for bs in temp_binded_streams:
                if bs in context.keys():
                    binded_streams.append(context[bs])
        else:
            binded_streams = temp_binded_streams

        assert len(binded_streams) == len(temp_binded_streams)
        for s in binded_streams:
            stream_types[s] = (TypeChecker.buffer_group_data[buffer_name]["in_stream"], TypeChecker.buffer_group_data[buffer_name]["in_stream"])

        stream_type = TypeChecker.buffer_group_data[buffer_name]["in_stream"]

        answer = f'''
            if (chosen_streams != NULL) {"{"}
                free(chosen_streams);
            {"}"}
            bg_update(&BG_{buffer_name}, {buffer_name}_ORDER_EXP);
            {choose_statement}
        '''
        permutation_streams_code = ""
        for perm in permutations(range(0, len(binded_streams))):
            declared_streams = ""
            for (index, name) in zip(perm, binded_streams):
                declared_streams += f"shm_stream *{name} = chosen_streams[{index}]->stream;\n"
                declared_streams += f"shm_arbiter_buffer *BUFFER_{name} = chosen_streams[{index}]->buffer;\n"
                declared_streams += f"STREAM_{stream_type}_ARGS *stream_args_{name} = (STREAM_{stream_type}_ARGS *)chosen_streams[{index}]->args;\n"

            permutation_streams_code += f'''
            {"{"}
                {declared_streams}
                {inner_code}
            {"}"}
            '''
        answer += f'''
            if (chosen_streams != NULL) {'{'}
                {permutation_streams_code}
            {'}'}
                    '''
        return answer
    else:
        assert(tree[0] == "buff_match_exp-args")
        match_fun_name, arg1, arg2 = tree[1], tree[2], tree[3]
        assert match_fun_name in TypeChecker.match_fun_data.keys()
        if context is None:
            context = {}

        if arg1 is not None:
            fun_bind_args = []
            get_list_ids(arg1, fun_bind_args)
            for (original_name, new_name) in zip(TypeChecker.match_fun_data[match_fun_name]["out_args"], fun_bind_args):
                context[original_name] = new_name

        if arg2 is not None:
            new_args = []
            get_list_var_or_int(arg2, new_args)
            for(original_arg, new_arg) in zip(TypeChecker.match_fun_data[match_fun_name]["in_args"], new_args):
                if new_arg in context.keys():
                    context[original_arg] = context[new_arg]
                else:
                    context[original_arg] = new_arg
        return rule_set_streams_condition(TypeChecker.match_fun_data[match_fun_name]["buffer_match_expr"], mapping, stream_types, inner_code, is_scan, context)


def buffer_peeks(binded_args, events_to_retrieve):
    answer = ""

    called_buffers = set()

    for (arg, data) in binded_args.items():
        buffer_name = data[0]
        if data[5] is not None:
            buffer_name += f"{data[5]}"
        if buffer_name not in called_buffers:
            assert(len(data) == 6)
            answer += f'''
                char* e1_{buffer_name}; size_t i1_{buffer_name};
	            char* e2_{buffer_name}; size_t i2_{buffer_name};
	            shm_arbiter_buffer_peek(BUFFER_{buffer_name}, {events_to_retrieve[data[0]]}, (void**)&e1_{buffer_name}, &i1_{buffer_name},(void**) &e2_{buffer_name}, &i2_{buffer_name});
            '''
            called_buffers.add(buffer_name)
    return answer


def construct_arb_rule_outevent(mapping, output_ev_source, output_event, raw_args) -> str:
    local_args = []

    get_expressions(raw_args, local_args)

    answer = f'''arbiter_outevent->head.kind = {mapping[output_ev_source][output_event]["index"]};
    arbiter_outevent->head.id = (*arbiter_counter)++;
    '''
    for (arg, outarg) in zip(local_args, mapping[output_ev_source][output_event]["args"]):
        answer+= f"((STREAM_{output_ev_source}_out *) arbiter_outevent)->cases.{output_event}.{outarg[0]} = {arg};\n"
    return answer

def process_arb_rule_stmt(tree, mapping, output_ev_source) -> str:
    if tree[0] == "switch":
        switch_rule_name = tree[PPARB_RULE_STMT_SWITCH_ARB_RULE]
        return f"RULE_SET_{switch_rule_name}();\n"
    if tree[0] == "yield":
        return f'''
        arbiter_outevent = (STREAM_{TypeChecker.arbiter_output_type}_out *)shm_monitor_buffer_write_ptr(monitor_buffer);
         {construct_arb_rule_outevent(mapping, output_ev_source, 
                                      tree[PPARB_RULE_STMT_YIELD_EVENT], tree[PPARB_RULE_STMT_YIELD_EXPRS])}
         shm_monitor_buffer_write_finish(monitor_buffer);
        '''
    if tree[0] == "drop":
        event_source_ref = tree[PPARB_RULE_STMT_DROP_EV_SOURCE]
        assert(event_source_ref[0] == "event_src_ref")
        event_source_name = event_source_ref[1]
        if event_source_ref[2] is not None:
            event_source_name += f"{event_source_ref[2]}"
        return f"\tshm_arbiter_buffer_drop(BUFFER_{event_source_name}, {tree[PPARB_RULE_STMT_DROP_INT]});\n"
    assert(tree[0] == "field_access")
    target_stream, index, field = tree[1], tree[2], tree[3]
    stream_name = target_stream
    if index is not None:
        stream_name += f"_{index}"
    return f"stream_args_{stream_name}->{field}"




def process_code_stmt_list(tree, mapping, binded_args, stream_types, output_ev_source) -> str:
    assert (tree[0] == "ccode_statement_l")
    if len(tree) == 2:
        return tree[PCODE_STMT_LIST_TOKEN1]

    assert(len(tree) > 1)

    if len(tree) == 3:
        assert(tree[PCODE_STMT_LIST_TOKEN2] == ";")
        return process_arb_rule_stmt(tree[PCODE_STMT_LIST_TOKEN1], mapping, output_ev_source)

    assert(len(tree) == 4)

    if tree[PCODE_STMT_LIST_TOKEN3] == ";":
        return tree[PCODE_STMT_LIST_TOKEN1] + "\n" + process_arb_rule_stmt(tree[PCODE_STMT_LIST_TOKEN2], mapping,
                                                                            output_ev_source)
    else:
        assert(tree[PCODE_STMT_LIST_TOKEN2] == ";")
        return process_arb_rule_stmt(tree[PCODE_STMT_LIST_TOKEN1], mapping, output_ev_source) \
               + "\n" + tree[PCODE_STMT_LIST_TOKEN3]


def get_arb_rule_stmt_list_code(tree, mapping, binded_args, stream_types, output_ev_source) -> str:
    if tree[0] == 'arb_rule_stmt_l':
        return process_code_stmt_list(tree[PLIST_BASE_CASE], mapping, binded_args, stream_types, output_ev_source) + get_arb_rule_stmt_list_code(tree[PLIST_TAIL], mapping,
                                                                                        binded_args, stream_types,
                                                                                        output_ev_source)
    else:
        assert(tree[0] == "ccode_statement_l")
        return process_code_stmt_list(tree, mapping, binded_args, stream_types, output_ev_source)

def define_binded_args(binded_args, stream_types):
    answer = ""
    for (arg, data) in binded_args.items():
        event_source = data[0]
        event = data[1]
        arg_name = data[2]
        arg_type = data[3]
        stream_type_out = stream_types[event_source][1]
        index = data[4]
        stream_index = data[5]
        if stream_index is not None:
            event_source += str(stream_index)
        answer += f"STREAM_{stream_type_out}_out * event_for_{arg} = (STREAM_{stream_type_out}_out *) " \
                  f"get_event_at_index(e1_{event_source}, i1_{event_source}, e2_{event_source}, " \
                  f"i2_{event_source}, sizeof(STREAM_{stream_type_out}_out), {index});\n" \
                  f"{arg_type} {arg} = event_for_{arg}->cases.{event}.{arg_name};\n\n"
    return answer

def declare_arrays(scanned_kinds) -> str:
    answer = ""
    for kinds in scanned_kinds:
        s_kinds = [str(x) for x in kinds]
        answer += f"int TEMPARR{StaticCounter.declarations_counter}[] = {'{'}{','.join(s_kinds)}{'}'};\n"
        StaticCounter.declarations_counter+=1
    return answer

def arbiter_rule_code(tree, mapping, stream_types, output_ev_source) -> str:
    # output_stream should be the output type of the event source
    if tree[0] == "arb_rule_list":
        return arbiter_rule_code(tree[PLIST_BASE_CASE], mapping, stream_types, output_ev_source) + \
               arbiter_rule_code(tree[PLIST_TAIL], mapping, stream_types, output_ev_source)
    else:
        assert(tree[0] == "arbiter_rule1" or tree[0] == "arbiter_rule2")
        if tree[0] == "arbiter_rule1":
            binded_args = dict()
            get_buff_math_binded_args(tree[PPARB_RULE_LIST_BUFF_EXPR], stream_types, mapping, binded_args, TypeChecker.buffer_group_data, TypeChecker.match_fun_data)
            events_to_retrieve = dict()
            get_num_events_to_retrieve(tree[PPARB_RULE_LIST_BUFF_EXPR], events_to_retrieve, TypeChecker.match_fun_data)
            scanned_conditions = rule_set_streams_condition(tree[PPARB_RULE_LIST_BUFF_EXPR], mapping, stream_types, is_scan=True)
            stream_drops_code = ""
            stream_drops = dict()
            get_count_drop_events_from_l_buff(tree[1], stream_drops)
            if len(stream_drops.keys()) != 0:
                assert (len(stream_drops.keys()) > 0)
                for (stream, count) in stream_drops.items():
                    stream_drops_code += f"\tshm_arbiter_buffer_drop(BUFFER_{stream}, {count});\n"
            inner_code = f'''
            if({tree[PPARB_RULE_CONDITION_CODE]}) {"{"}
                {buffer_peeks(binded_args, events_to_retrieve)}
                {define_binded_args(binded_args, stream_types)}
                {get_arb_rule_stmt_list_code(tree[PPARB_RULE_STMT_LIST], mapping, binded_args, stream_types, 
                                                 output_ev_source)}
                {stream_drops_code}
                return 1;
            {"}"}
            '''
            return f'''
            {declare_arrays(scanned_conditions)}
            {rule_set_streams_condition(tree[PPARB_RULE_LIST_BUFF_EXPR], mapping, stream_types, inner_code)}
            '''
        else:
            raise Exception("This should be unreachable!")


def get_code_rule_sets(tree, mapping, stream_types, output_ev_source) -> str:
    if tree[0] == 'arb_rule_set_l':
        return get_code_rule_sets(tree[PLIST_BASE_CASE], mapping, stream_types, output_ev_source) \
               + get_code_rule_sets(tree[PLIST_TAIL], mapping, stream_types, output_ev_source)
    else:
        assert(tree[0] == "arbiter_rule_set")
        rule_set_name = tree[PPARB_RULE_SET_NAME]
        return f'''int RULE_SET_{rule_set_name}() {"{"}
        dll_node *chosen_streams;
    STREAM_{output_ev_source}_out *outevent;   
    {arbiter_rule_code(tree[PPARB_RULE_LIST], mapping, stream_types, output_ev_source)}
    if (chosen_streams != NULL) {"{"}
        free(chosen_streams);
    {"}"}
    
    return 0;
{"}"}
'''

# TODO: free correctly chosen streams
# monitor code

def declare_monitor_args(tree, event_name, event_data, count_tabs) -> str:
    tabs = "\t"*count_tabs
    ids = []
    get_list_ids(tree, ids)
    args = event_data['args']
    assert(len(ids) == len(args))

    answer = ""
    for i in range(len(args)):
        answer += f"{tabs}{args[i][1]} {ids[i]} = received_event->cases.{event_name}.{args[i][0]};\n"
    return answer


def monitor_events_code(tree, stream_name, possible_events, count_tabs) -> str:
    if tree[0] == 'monitor_rule_l':
        return monitor_events_code(tree[PLIST_BASE_CASE], stream_name, possible_events, count_tabs) + \
               monitor_events_code(tree[PLIST_TAIL], stream_name, possible_events, count_tabs)
    else:
        assert(tree[0] == "monitor_rule")
        event = tree[PPMONITOR_RULE_EV_NAME]
        tabs = "\t"*count_tabs
        return f'''
{tabs}if (received_event->head.kind == {possible_events[event]["index"]}) {"{"}
{declare_monitor_args(tree[PPMONITOR_RULE_EV_ARGS], event, possible_events[event], count_tabs+1)}
{tabs}  if ({tree[PPMONITOR_RULE_EXPR]}) {"{"}
{tabs}      {tree[PPMONITOR_RULE_CODE]}
{tabs}  {"}"}
{tabs}{"}"}
        '''


def monitor_code(tree, possible_events, arbiter_event_source) -> str:
    assert(tree[0] == "monitor_def")
    return f'''
    // monitor
    STREAM_{arbiter_event_source}_out * received_event;
    while(true) {"{"}
        received_event = fetch_arbiter_stream(monitor_buffer);
        if (received_event == NULL) {"{"}
            break;
        {"}"}
{monitor_events_code(tree[PPMONITOR_RULE_LIST], arbiter_event_source, possible_events, 2)}
        shm_monitor_buffer_consume(monitor_buffer, 1);
    {"}"}
    '''

# match fun

def build_arbiter_rule(local_tree, mapping, stream_types) -> str:
    # if local_tree[0] == "arbiter_rule1":
    return arbiter_rule_code(local_tree, mapping, stream_types, TypeChecker.arbiter_output_type)
#     else:
#         assert(local_tree[0] == "arbiter_rule2")
#
#         choose_order = local_tree[1]
#         assert(choose_order[0] == "choose-order")
#         count_choose = choose_order[2]
#         buffer_name = local_tree[3]
#         assert buffer_name in TypeChecker.buffer_group_data.keys()
#         if choose_order[1] == "first":
#             choose_statement = f"chosen_streams = bg_get_first_n(&BG_{buffer_name}, {count_choose});"
#         else:
#             assert(choose_order[1] == "last")
#             choose_statement = f"chosen_streams = bg_get_last_n(&BG_{buffer_name}, {count_choose});"
#         binded_streams = []
#         get_list_ids(local_tree[2], binded_streams)
#         declared_streams = ""
#         for (index, name) in enumerate(binded_streams):
#             declared_streams += f"shm_stream *{name} = chosen_streams[{index}]->stream;\n"
#
#         answer = f'''
# void buff_match_exp_{StaticCounter.match_expr_counter}() {"{"}
# {choose_statement}
# if (chosen_streams != NULL) {'{'}
# {declared_streams}
# if ({local_tree[4]}) {"{"}
#     {build_arbiter_rule(local_tree[5], mapping, stream_types)}
# {"}"}
# {"}"}
# {'}'}
#         '''
#     StaticCounter.match_expr_counter += 1
#     return answer

def build_rule_set_functions(tree, mapping, stream_types):

    def local_explore_rule_list(local_tree) -> str:
        if local_tree[0] == "arb_rule_list":
            return local_explore_rule_list(local_tree[1]) + local_explore_rule_list(local_tree[2])
        else:
            assert local_tree[0] == "arbiter_rule1" or local_tree[0] == "arbiter_rule2"
            return build_arbiter_rule(local_tree, mapping, stream_types)

    def local_explore_arb_rule_set_list(local_tree) -> str:
        if local_tree[0] == "arb_rule_set_l":
            return local_explore_arb_rule_set_list(local_tree[1]) + local_explore_arb_rule_set_list(local_tree[2])
        else:
            assert (local_tree[0] == "arbiter_rule_set")
            rule_set_name = local_tree[1]
            return f"int RULE_SET_{rule_set_name}() {'{'}\n" \
                   f"dll_node **chosen_streams; // used for match fun" \
                   f"{local_explore_rule_list(local_tree[2])}" \
                   f"return 0;\n" \
                   f"{'}'}"

    assert (tree[0] == 'arbiter_def')

    arbiter_rule_set_list = tree[2]
    return local_explore_arb_rule_set_list(arbiter_rule_set_list)

def destroy_buffer_groups():
    answer = ""
    for buffer_group in TypeChecker.buffer_group_data.keys():
        answer +=f"\tdestroy_buffer_group(&BG_{buffer_group});\n"
    return answer
