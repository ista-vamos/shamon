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

def declare_arbiter_buffers(ast) -> str:
    event_srcs_names = []
    get_event_sources_names(ast[2], event_srcs_names)

    answer = ""
    for name in event_srcs_names:
        answer += f"// Arbiter buffer for event source {name}\n"
        answer += f"shm_arbiter_buffer *BUFFER_{name};\n\n"

    return answer