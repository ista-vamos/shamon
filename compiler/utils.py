# general utils
from typing import List, Tuple


def get_count_list_ids(tree):
    if tree[0] == 'listids':
        return 1 + get_count_list_ids(tree[2])
    else:
        assert(tree[0] == 'ID')
        return 1


def get_count_list_expr(tree):
    if tree[0] == 'expr_list':
        return 1 + get_count_list_expr(tree[2])
    else:
        assert(tree[0] == 'expr')
        return 1

def is_primitive_type(type_ : str):
    answer = type_ == "int" or type_ == "bool" or type_ =="string" or type_ == "float"
    answer = answer or type_ == "double"
    return answer

def is_type_primitive(tree):
    if tree[0] == 'type':
        return is_primitive_type(tree[1])
    else:
        assert(tree[0] == "array")
        return is_type_primitive(tree[1])


# event streams utils

def get_events_names(tree, names) -> None:
    if tree[0] == "event_list":
        get_event_sources_names(tree[1], names)
        get_event_sources_names(tree[2], names)
    else:
        assert (tree[0] == "event_decl")
        names.append(tree[1])

def get_event_args(tree, event_args: List[Tuple[str, str]]):
    if tree[0] == "list_field_decl":
        get_event_args(tree[1], event_args)
        get_event_args(tree[2], event_args)
    else:
        assert(tree[0] == "field_decl")
        event_args.append((tree[1], tree[2][1]))


def get_events_data(tree, events_data) -> None:
    if tree[0] == "event_list":
        get_events_data(tree[1], events_data)
        get_events_data(tree[2], events_data)
    else:
        assert (tree[0] == "event_decl")
        event_args: List[Tuple[str, str]] = [] # list consists of a tuple (name_arg, type_arg)
        get_event_args(tree[2], event_args)

        data = {
            "name": tree[1],
            "args": event_args
        }
        events_data.append(data)


def get_stream_to_events_mapping(tree, mapping) -> None:
    if tree[0] == "stream_types":
        get_stream_to_events_mapping(tree[1], mapping)
        get_stream_to_events_mapping(tree[2], mapping)
    else:
        assert(tree[0] == "stream_type")
        events_data = []
        get_events_data(tree[2], events_data)
        assert(tree[1] not in mapping.keys())
        mapping_events = {}
        for (index, data) in enumerate(events_data):
            data.update({'index': index+2})
            mapping_events[data['name']] = data
        mapping[tree[1]] = mapping_events



def get_parameters_types_field_decl(tree, params):
    assert (len(tree) == 3)
    if tree[0] == 'list_field_decl':
        get_parameters_types_field_decl(tree[1], params)
        get_parameters_types_field_decl(tree[2], params)
    else:
        assert (tree[0] == 'field_decl')
        params.append(tree[2])


def get_parameters_names_field_decl(tree, params):
    assert (len(tree) == 3)
    if tree[0] == 'list_field_decl':

        get_parameters_names_field_decl(tree[1], params)
        get_parameters_names_field_decl(tree[2], params)
    else:
        assert (tree[0] == 'field_decl')
        params.append(tree[1])


def are_all_events_decl_primitive(tree):
    if tree[0] == 'event_list':
        return are_all_events_decl_primitive(tree[1]) and are_all_events_decl_primitive(tree[2])
    else:
        assert (tree[0] == 'event_decl')
        params = []
        get_parameters_types_field_decl(tree[2], params)
        for param in params:
            if not is_type_primitive(param):
                return False
        return True


# Performance Layer utils
def get_event_sources_names(tree, names) -> None:
    if tree[0] == 'event_sources':
        get_event_sources_names(tree[1], names)
        get_event_sources_names(tree[2], names)
    else:
        assert(tree[0] == 'event_source')
        names.append(tree[1])

def get_buffer_sizes(tree, buff_sizes) -> None:
    if tree[0] == 'event_sources':
        get_buffer_sizes(tree[1], buff_sizes)
        get_buffer_sizes(tree[2], buff_sizes)
    else:
        assert(tree[0] == 'event_source')
        connection_kind = tree[3]
        assert(connection_kind[0] == 'conn_kind')
        if connection_kind[1] == 'infinite' or connection_kind[1] == 'blocking':
            raise Exception("Connection kind infinite and blocking not implemented!")
        buff_sizes.append(connection_kind[2])

def get_out_names(tree, out_names) -> None:
    if tree[0] == 'event_sources':
        get_out_names(tree[1], out_names)
        get_out_names(tree[2], out_names)
    else:
        assert(tree[0] == 'event_source')
        out_names.append(tree[4])


