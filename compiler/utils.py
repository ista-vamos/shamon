# general utils
from typing import List, Tuple
from parser_indices import *


def get_count_list_ids(tree):
    if tree[0] == 'listids':
        return 1 + get_count_list_ids(tree[PLIST_TAIL])
    else:
        assert(tree[0] == 'ID')
        return 1

def get_list_ids(tree, ids):
    if tree[0] == 'listids':
        get_list_ids(tree[PLIST_BASE_CASE], ids)
        get_list_ids(tree[PLIST_TAIL], ids)
    else:
        if type(tree) == str:
            ids.append(tree)
        else:
            assert(tree[0] == 'ID')
            ids.append(tree[PLIST_DATA])

def get_count_list_expr(tree):
    if tree[0] == 'expr_list':
        return 1 + get_count_list_expr(tree[PLIST_TAIL])
    else:
        assert(tree[0] == 'expr')
        return 1

def get_expressions(tree, result):
    if tree[0] == 'expr_list':
        get_expressions(tree[PLIST_BASE_CASE], result)
        get_expressions(tree[PLIST_TAIL], result)
    else:
        if tree[0] == "expr":
            result.append(tree[PLIST_DATA])
        else:
            result.append(tree)

def is_primitive_type(type_ : str):
    answer = type_ == "int" or type_ == "bool" or type_ =="string" or type_ == "float"
    answer = answer or type_ == "double"
    return answer

def is_type_primitive(tree):
    if tree[0] == 'type':
        return is_primitive_type(tree[PTYPE_DATA])
    else:
        assert(tree[0] == "array")
        return is_type_primitive(tree[PTYPE_DATA])


# event streams utils

def get_events_names(tree, names) -> None:
    if tree[0] == "event_list":
        get_event_sources_names(tree[PLIST_BASE_CASE], names)
        get_event_sources_names(tree[PLIST_TAIL], names)
    else:
        assert (tree[0] == "event_decl")
        names.append(tree[PLIST_DATA])

def get_event_args(tree, event_args: List[Tuple[str, str]]):
    if tree[0] == "list_field_decl":
        get_event_args(tree[PLIST_BASE_CASE], event_args)
        get_event_args(tree[PLIST_TAIL], event_args)
    else:
        assert(tree[0] == "field_decl")
        event_args.append((tree[PPFIELD_NAME], tree[PPFIELD_TYPE][PTYPE_DATA]))


def get_events_data(tree, events_data) -> None:
    if tree[0] == "event_list":
        get_events_data(tree[PLIST_BASE_CASE], events_data)
        get_events_data(tree[PLIST_TAIL], events_data)
    else:
        assert (tree[0] == "event_decl")
        event_args: List[Tuple[str, str]] = [] # list consists of a tuple (name_arg, type_arg)
        get_event_args(tree[PPEVENT_PARAMS_LIST], event_args)

        data = {
            "name": tree[PPEVENT_NAME],
            "args": event_args
        }
        events_data.append(data)


def get_stream_to_events_mapping(tree, mapping) -> None:
    if tree[0] == "stream_types":
        get_stream_to_events_mapping(tree[PLIST_BASE_CASE], mapping)
        get_stream_to_events_mapping(tree[PLIST_TAIL], mapping)
    else:
        assert(tree[0] == "stream_type")
        events_data = []
        get_events_data(tree[PPSTREAM_TYPE_EVENT_LIST], events_data)
        assert(tree[PPSTREAM_TYPE_NAME] not in mapping.keys())
        mapping_events = {}
        for (index, data) in enumerate(events_data):
            data.update({'index': index+2})
            mapping_events[data['name']] = data
        mapping_events['hole'] = {'index': 1, 'args':[('n', 'int')]}
        mapping[tree[PPSTREAM_TYPE_NAME]] = mapping_events

def get_stream_types(tree, mapping) -> None:
    if tree[0] == "event_sources":
        get_stream_types(tree[PLIST_BASE_CASE], mapping)
        get_stream_types(tree[PLIST_TAIL], mapping)
    else:
        assert(tree[0] == "event_source")
        assert(tree[PPEVENT_SOURCE_NAME] not in mapping.keys())
        mapping[tree[PPEVENT_SOURCE_NAME]] = (tree[PPEVENT_SOURCE_INPUT_TYPE], tree[PPEVENT_SOURCE_OUTPUT_TYPE])


def get_parameters_types_field_decl(tree, params):
    assert (len(tree) == 3)
    if tree[0] == 'list_field_decl':
        get_parameters_types_field_decl(tree[PLIST_BASE_CASE], params)
        get_parameters_types_field_decl(tree[PLIST_TAIL], params)
    else:
        assert (tree[0] == 'field_decl')
        params.append(tree[PPFIELD_TYPE])


def get_parameters_names_field_decl(tree, params):
    assert (len(tree) == 3)
    if tree[0] == 'list_field_decl':
        get_parameters_names_field_decl(tree[PLIST_BASE_CASE], params)
        get_parameters_names_field_decl(tree[PLIST_TAIL], params)
    else:
        assert (tree[0] == 'field_decl')
        params.append(tree[PPFIELD_NAME])


def are_all_events_decl_primitive(tree):
    if tree[0] == 'event_list':
        return are_all_events_decl_primitive(tree[PLIST_BASE_CASE]) and are_all_events_decl_primitive(tree[PLIST_TAIL])
    else:
        assert (tree[0] == 'event_decl')
        params = []
        get_parameters_types_field_decl(tree[PPEVENT_PARAMS_LIST], params)
        for param in params:
            if not is_type_primitive(param):
                return False
        return True


# Performance Layer utils
def get_event_sources_names(tree, names) -> None:
    if tree[0] == 'event_sources':
        get_event_sources_names(tree[PLIST_BASE_CASE], names)
        get_event_sources_names(tree[PLIST_TAIL], names)
    else:
        assert(tree[0] == 'event_source')
        names.append(tree[PPEVENT_SOURCE_NAME])

def get_buffer_sizes(tree, buff_sizes) -> None:
    if tree[0] == 'event_sources':
        get_buffer_sizes(tree[PLIST_BASE_CASE], buff_sizes)
        get_buffer_sizes(tree[PLIST_TAIL], buff_sizes)
    else:
        assert(tree[0] == 'event_source')
        connection_kind = tree[PPEVENT_CONN_KIND]
        assert(connection_kind[0] == 'conn_kind')
        if connection_kind[PPCONN_KIND_NAME] == 'infinite' or connection_kind[PPCONN_KIND_NAME] == 'blocking':
            raise Exception("Connection kind infinite and blocking not implemented!")
        buff_sizes.append(connection_kind[PPCONN_KIND_INT])

def get_out_names(tree, out_names) -> None:
    if tree[0] == 'event_sources':
        get_out_names(tree[PLIST_BASE_CASE], out_names)
        get_out_names(tree[PLIST_TAIL], out_names)
    else:
        assert(tree[0] == 'event_source')
        out_names.append(tree[PPEVENT_SOURCE_OUTPUT_TYPE])


def get_rule_set_names(tree, names) -> None:
    if tree[0] == 'arb_rule_set_l':
        get_rule_set_names(tree[PLIST_BASE_CASE], names)
        get_rule_set_names(tree[PLIST_TAIL], names)
    else:
        assert(tree[0] == 'arbiter_rule_set')
        names.append(tree[PPARB_RULE_SET_NAME])

def get_event_kinds(tree, kinds, mapping) -> None:
    assert(tree[0] != "|")

    if tree[0] == "list_ev_calls":
        kinds.append(mapping[tree[PPLIST_EV_CALL_EV_NAME]]["index"])
        get_event_kinds(tree[PPLIST_EV_CALL_TAIL], kinds, mapping)
    else:
        assert(tree[0] == "ev_call")
        if tree[PPLIST_EV_CALL_EV_NAME] == "hole":
            kinds.append(1)
        else:
            kinds.append(mapping[tree[PPLIST_EV_CALL_EV_NAME]]["index"])

def get_arbiter_event_source(tree) -> str:
    assert(tree[0] == 'arbiter_def')
    return tree[PPARBITER_OUTPUT_TYPE]

def get_parameters_names(tree, stream_name, mapping, binded_args, index=0):
    if tree[0] == 'list_ev_calls':
        ids = []
        get_list_ids(tree[PPLIST_EV_CALL_EV_PARAMS], ids)
        assert(len(ids) == len(mapping[tree[PPLIST_EV_CALL_EV_NAME]]['args']))
        for (arg_bind, arg) in zip(mapping[tree[PPLIST_EV_CALL_EV_NAME]]['args'], ids):
            binded_args[arg_bind] = (stream_name, arg[0], arg[1], index)
        get_parameters_names(tree[PPLIST_EV_CALL_TAIL], stream_name, mapping, binded_args, index+1)
    else:
        assert(tree[0] == 'ev_call')
        ids = []
        get_list_ids(tree[PPLIST_EV_CALL_EV_PARAMS], ids)
        assert (len(ids) == len(mapping[tree[PPLIST_EV_CALL_EV_NAME]]['args']))
        for (arg_bind, arg) in zip(mapping[tree[PPLIST_EV_CALL_EV_NAME]]['args'], ids):
            binded_args[arg] = (stream_name, tree[PPLIST_EV_CALL_EV_NAME], arg_bind[0], arg_bind[1], index)



def get_buff_math_binded_args(tree, stream_types, mapping, binded_args) -> None:
    if tree[0] == 'l_buff_match_exp':
        get_buff_math_binded_args(tree[PLIST_BASE_CASE], stream_types, mapping, binded_args)
        get_buff_math_binded_args(tree[PLIST_TAIL], stream_types, mapping, binded_args)
    else:
        assert(tree[0] == 'buff_match_exp')
        event_source_name = tree[PPBUFFER_MATCH_EV_NAME]
        stream_type = stream_types[event_source_name][1]
        if len(tree) > 3:
            for i in range(2, len(tree)):
                if tree[i] != '|':
                    get_parameters_names(tree[i], event_source_name, mapping[stream_type], binded_args)

def get_events_count(tree):
    if tree[0] == 'list_ev_calls':
        return 1 + get_events_count(tree[PPLIST_EV_CALL_TAIL])
    else:
        assert(tree[0] == 'ev_call')
        return 1

def get_num_events_to_retrieve(tree, events_to_retrieve) -> None:
    if tree[0] == 'l_buff_match_exp':
        get_num_events_to_retrieve(tree[PLIST_BASE_CASE], events_to_retrieve)
        get_num_events_to_retrieve(tree[PLIST_TAIL], events_to_retrieve)
    else:
        assert(tree[0] == 'buff_match_exp')
        event_source_name = tree[PPBUFFER_MATCH_EV_NAME]
        if len(tree) > 3:
            for i in range(2, len(tree)):
                if tree[i] != '|':
                    count = get_events_count(tree[i])
                    events_to_retrieve[event_source_name] = count
