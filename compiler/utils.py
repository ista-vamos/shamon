# general utils
from typing import List, Tuple, Dict, Any, Optional, Set
from parser_indices import *


def get_components_dict(tree, answer):
    if tree[0] == "components":
        get_components_dict(tree[1], answer)
        get_components_dict(tree[2], answer)
    else:
        name = tree[0]
        if name not in answer.keys():
            answer[name] = []

        answer[name].append(tree)
    return answer

def get_name_with_args(tree):
    if tree[0] == "name-with-args":
        args = []
        if tree[2][0] == 'listids':
            get_list_ids(tree[2], args)
        else:
            get_expressions(tree[2], args)
        return tree[1], args
    else:
        return tree, []

def get_name_args_count(tree):
    if tree[0] == "name-with-args":
        args = []
        if tree[2][0] == 'listids':
            get_list_ids(tree[2], args)
        else:
            get_expressions(tree[2], args)
        return tree[1], len(args)
    else:
        return tree, 0

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

def get_list_var_or_int(tree, result):
    if tree[0] == "list_var_or_integer":
        get_list_ids(tree[1], result)
        get_list_ids(tree[2], result)
    else:
        result.append(tree)

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

def is_type_primitive(tree) -> bool:
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


def get_stream_to_events_mapping(stream_types) -> Dict[str, Any]:
    mapping = dict()
    for tree in stream_types:
        assert(tree[0] == "stream_type")
        assert(len(tree) == 5)
        events_data = []
        get_events_data(tree[-1], events_data)
        assert(tree[PPSTREAM_TYPE_NAME] not in mapping.keys())
        mapping_events = {}
        for (index, data) in enumerate(events_data):
            data.update({'index': index+2})
            mapping_events[data['name']] = data
        mapping_events['hole'] = {'index': 1, 'args':[('n', 'int')]}
        mapping[tree[PPSTREAM_TYPE_NAME]] = mapping_events
    return mapping

def get_stream_types(event_sources) -> Dict[str, Any]:
    mapping = dict()
    for tree in event_sources:
        assert(tree[0] == "event_source")
        assert(len(tree) == 5)
        event_decl = tree[2]
        assert(event_decl[0] == "event-decl")
        event_source_name = event_decl[1]
        assert(event_source_name not in mapping.keys())
        event_source_tail = tree[-1]
        input_type, _ = get_name_with_args(tree[-2])
        assert(event_source_tail[0] == 'ev-source-tail')
        if event_source_tail[1] == None:
            output_type = input_type
        else:
            output_type, _ = get_name_with_args(event_source_tail[1])
            if output_type.lower() == "forward":
                output_type = input_type
        mapping[event_source_name] = (input_type, output_type)
    return mapping



def get_parameters_types_field_decl(tree, params):
    assert (len(tree) == 3)
    if tree[0] == 'list_field_decl':
        get_parameters_types_field_decl(tree[PLIST_BASE_CASE], params)
        get_parameters_types_field_decl(tree[PLIST_TAIL], params)
    else:
        assert (tree[0] == 'field_decl')
        params.append({"name": tree[1], "type": tree[PPFIELD_TYPE][1], "is_primitive" : is_type_primitive(tree[PPFIELD_TYPE])})


def get_parameters_names_field_decl(tree, params):
    assert (len(tree) == 3)
    if tree[0] == 'list_field_decl':
        get_parameters_names_field_decl(tree[PLIST_BASE_CASE], params)
        get_parameters_names_field_decl(tree[PLIST_TAIL], params)
    else:
        assert (tree[0] == 'field_decl')
        params.append(tree[PPFIELD_NAME])

def get_event_src_name(tree) -> str:
    assert(tree[0] == "event-decl")
    name, _ = get_name_with_args(tree[1])
    return name

def are_all_events_decl_primitive(tree) -> bool:
    if tree[0] == 'event_list':
        return are_all_events_decl_primitive(tree[PLIST_BASE_CASE]) and are_all_events_decl_primitive(tree[PLIST_TAIL])
    else:
        assert (tree[0] == 'event_decl')
        params = []
        get_parameters_types_field_decl(tree[PPEVENT_PARAMS_LIST], params)
        for param in params:
            if not param["is_primitive"]:
                return False
        return True


# Performance Layer utils
def get_event_sources_names(event_sources, names) -> None:
    for tree in event_sources:
        assert(tree[0] == 'event_source')
        event_src_declaration = tree[2]
        assert(event_src_declaration[0] == "event-decl")
        name, _ = get_name_with_args(event_src_declaration[1])
        names.append(name)

def get_event_sources_copies(event_sources):
    result = []
    for tree in event_sources:
        assert (tree[0] == 'event_source')
        event_src_declaration = tree[2]
        copies = 0
        assert (event_src_declaration[0] == "event-decl")
        name, _ = get_name_with_args(event_src_declaration[1])
        if event_src_declaration[2] is not None:
            copies = int(event_src_declaration[2])
        result.append((name, copies))
    return result

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

def get_count_events_from_list_calls(tree) -> int:
    assert(tree[0] != "|")
    if tree[0] == "list_ev_calls":
        return 1 + get_count_events_from_list_calls(tree[PPLIST_EV_CALL_TAIL])
    else:
        assert(tree[0] == "ev_call")
        return 1

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

def get_parameters_names(tree, stream_name, mapping, binded_args, index=0, stream_index=None):
    if tree[0] == 'list_ev_calls':
        ids = []
        get_list_ids(tree[PPLIST_EV_CALL_EV_PARAMS], ids)
        assert(len(ids) == len(mapping[tree[PPLIST_EV_CALL_EV_NAME]]['args']))
        for (arg_bind, arg) in zip(mapping[tree[PPLIST_EV_CALL_EV_NAME]]['args'], ids):
            binded_args[arg_bind] = (stream_name, arg[0], arg[1], index, stream_index)
        get_parameters_names(tree[PPLIST_EV_CALL_TAIL], stream_name, mapping, binded_args, index+1)
    else:
        assert(tree[0] == 'ev_call')
        ids = []
        get_list_ids(tree[PPLIST_EV_CALL_EV_PARAMS], ids)
        assert (len(ids) == len(mapping[tree[PPLIST_EV_CALL_EV_NAME]]['args']))
        for (arg_bind, arg) in zip(mapping[tree[PPLIST_EV_CALL_EV_NAME]]['args'], ids):
            binded_args[arg] = (stream_name, tree[PPLIST_EV_CALL_EV_NAME], arg_bind[0], arg_bind[1], index, stream_index)



def get_buff_math_binded_args(tree, stream_types, mapping, binded_args, buffer_group_data, match_fun_data) -> None:
    if tree[0] == 'l_buff_match_exp':
        get_buff_math_binded_args(tree[PLIST_BASE_CASE], stream_types, mapping, binded_args, buffer_group_data, match_fun_data)
        get_buff_math_binded_args(tree[PLIST_TAIL], stream_types, mapping, binded_args, buffer_group_data, match_fun_data)
    else:
        if tree[0] == "buff_match_exp":
            event_src_ref = tree[PPBUFFER_MATCH_EV_NAME]
            assert(event_src_ref[0] == "event_src_ref")
            event_source_name =  event_src_ref[1]
            stream_type = stream_types[event_source_name][1]
            stream_index = event_src_ref[2]
            if len(tree) > 3:
                for i in range(2, len(tree)):
                    if tree[i] != '|':
                        get_parameters_names(tree[i], event_source_name, mapping[stream_type], binded_args, stream_index=stream_index)
        elif tree[0] == "buff_match_exp-choose":
            buffer_name = tree[3]
            binded_streams = []
            get_list_ids(tree[2], binded_streams)
            for s in binded_streams:
                stream_types[s] = (buffer_group_data[buffer_name]["input_stream_type"],
                                   buffer_group_data[buffer_name]["input_stream_type"])
        else:
            assert(tree[0] == "buff_match_exp-args")
            match_fun_name, arg1, arg2 = tree[1], tree[2], tree[3]
            assert match_fun_name in match_fun_data.keys()
            if arg1 is not None:
                fun_bind_args = []
                get_list_ids(arg1, fun_bind_args)
                for (arg, t) in zip(fun_bind_args, match_fun_data[match_fun_name]["stream_types"]):
                    stream_types[arg] = (t, t)


def get_events_count(tree):
    if tree[0] == 'list_ev_calls':
        return 1 + get_events_count(tree[PPLIST_EV_CALL_TAIL])
    else:
        assert(tree[0] == 'ev_call')
        return 1

def get_num_events_to_retrieve(tree, events_to_retrieve, match_fun_data) -> None:
    if tree[0] == 'l_buff_match_exp':
        get_num_events_to_retrieve(tree[PLIST_BASE_CASE], events_to_retrieve, match_fun_data)
        get_num_events_to_retrieve(tree[PLIST_TAIL], events_to_retrieve, match_fun_data)
    else:
        if tree[0] == "buff_match_exp":
            event_src_ref = tree[PPBUFFER_MATCH_EV_NAME]
            assert(event_src_ref[0] == "event_src_ref")
            event_source_name = event_src_ref[1]
            if len(tree) > 3:
                for i in range(2, len(tree)):
                    if tree[i] != '|':
                        count = get_events_count(tree[i])
                        events_to_retrieve[event_source_name] = count
        else:
            if tree[0] == "buff_match_exp-args":
                match_fun_name = tree[1]
                get_num_events_to_retrieve(match_fun_data[match_fun_name]["buffer_match_expr"], events_to_retrieve,
                                           match_fun_data)



def get_count_drop_events_from_l_buff(tree, answer):
    if tree[0] == "l_buff_match_exp":
        get_count_drop_events_from_l_buff(tree[1], answer)
        get_count_drop_events_from_l_buff(tree[2], answer)
    else:
        if tree[0] == "buff_match_exp":
            event_src_ref = tree[PPBUFFER_MATCH_EV_NAME]
            assert (event_src_ref[0] == "event_src_ref")
            event_source_name = event_src_ref[1]
            stream_index = event_src_ref[2]
            count = 0
            if len(tree) > 3:
                for i in range(2, len(tree)):

                    if tree[i] == '|':
                        break # only drop events that are behind |
                    count += get_count_events_from_list_calls(tree[i])
                if count > 0:
                    if stream_index is not None:
                        event_source_name += str(stream_index)
                    assert(event_source_name not in answer.keys())
                    answer[event_source_name] = count
        else:
            assert(tree[0] == "buff_match_exp-choose" or tree[0] == "buff_match_exp-args")

def get_existing_buffers(type_checker) -> List[str]:
    '''
    :param type_checker:  TypeChecker object (cannot import it in this file because of recursive imports)
    :return:
    '''
    answer = []
    for (event_source, data) in type_checker.event_sources_data.items():
        for i in range(data["copies"]):
            name = f"{event_source}{i}"
            answer.append(name)

    return answer

def insert_in_result(buffer_name, count, result, existing_buffers):
    assert(count > -1)

    if buffer_name in existing_buffers:
        if buffer_name in result.keys():
            result[buffer_name] = max(result[buffer_name], count)
        else:
            result[buffer_name] = count

def local_get_buffer_peeks(local_tree, type_checker, result, existing_buffers):
    if local_tree[0] ==  "l_buff_match_exp":
        local_get_buffer_peeks(local_tree[1], type_checker, result, existing_buffers)
        local_get_buffer_peeks(local_tree[2], type_checker, result, existing_buffers)
    else:
        if local_tree[0] == "buff_match_exp-args":
            local_get_buffer_peeks(type_checker.match_fun_data[local_tree[1]]["buffer_match_expr"])
        elif local_tree[0] == "buff_match_exp-choose":
            pass
        else:
            assert(local_tree[0] == "buff_match_exp")
            if len(local_tree) == 3:
                if local_tree[-1] == "done":
                    # event_src_ref ':' DONE
                    pass
                else:
                    assert(local_tree[-1] == "nothing")
                    # event_src_ref ':' NOTHING
                    event_src_ref = local_tree[1]
                    event_src_name = event_src_ref[1]
                    if event_src_ref[2] is not None:
                        event_src_name += str(event_src_ref[2])

                    insert_in_result(event_src_name, 0, result, existing_buffers)
            else:
                assert(len(local_tree) == 4)
                event_src_ref = local_tree[1]
                event_src_name = event_src_ref[1]
                if event_src_ref[2] is not None:
                    event_src_name += str(event_src_ref[2])
                local_count = 0
                for list_event_calls in local_tree[2:]:
                    if list_event_calls != "|":
                        local_count += get_count_events_from_list_calls(list_event_calls)

                insert_in_result(event_src_name, local_count, result, existing_buffers)

def get_buffers_and_peeks(tree, result, type_checker, existing_buffers):
    '''
    :param tree: tree of arbiter_rules of a rule set
    :param result: dictionary that maps a buffer_name to the number of events that it needs to process
    :param type_checker: TypeChecker object (cannot import it in this file because of recursive imports)
    :param existing_buffers: these are the buffers (NOT buffer groups) explicitly created through 'event source' command
    (not as an product of a choose expression)
    :return:
    '''
    # MAIN CODE of this function
    if tree[0] == "arb_rule_list":
        get_buffers_and_peeks(tree[1], result, type_checker, existing_buffers)
        get_buffers_and_peeks(tree[2], result, type_checker, existing_buffers)
    else:
        if tree[0] == "arbiter_rule1":

            list_buff_match = tree[1]
            local_get_buffer_peeks(list_buff_match, type_checker, result, existing_buffers)
        else:
            assert(tree[0] == "arbiter_rule2")
            get_buffers_and_peeks(tree[-1], result, type_checker, existing_buffers)
