from typing import Dict, Set, Optional
from tokens import reserved
from utils import *

# define some types:
VARIABLE = "variable"
RESERVED = "reserved"
# FIELD_NAME = "field_name" # we do not need this type
EVENT_NAME = "event_name"
STREAM_TYPE_NAME = "stream_type_name"
EVENT_SOURCE_NAME = "event_source_name"
ARBITER_RULE_SET = "arbiter_rule_set"
STREAM_PROCESSOR_NAME = "stream_processor_name"
BUFFER_GROUP_NAME = "buffer_group_name"
MATCH_FUN_NAME = "match_fun_name"

'''
TODO: 
- event, stream type, and event source names are unique
- event sources only refer to stream types that use base types
- the arbiter refers to some valid stream type
- on/yield/forward constructions only refer to events and event sources that are 
- valid (including events being part of the stream type of the respective event source/the output stream type of the arbiter), and for events use the right number of arguments
'''

class TypeChecker:

    symbol_table: Dict[str, str] = dict() # maps symbols to types
    args_table: Dict[str, List[str]] = dict() # maps symbol (that represents a function) to a list of the
                                              # types of its arguments
    stream_events_are_primitive: Dict[str, bool] = dict() # maps 'stream type' declaration to the events that are declared inside
    event_sources_types: Dict[str, Tuple[str, str]] = dict() # ev_source_name -> (input type, output type)
    stream_types_to_events: Dict[str, Set[str]] = dict() # maps a stream name to the name of events that can happen
    arbiter_output_type: Optional[str] = None
    logical_copies: Dict[str, int] = dict()
    # in this stream

    @staticmethod
    def clean_checker():
        TypeChecker.symbol_table = dict()
        TypeChecker.args_table = dict()

    @staticmethod
    def get_stream_events(ast):
        if ast[0] == "stream_types":
            TypeChecker.get_stream_events(ast[PLIST_BASE_CASE])
            TypeChecker.get_stream_events(ast[PLIST_TAIL])
        else:
            assert(ast[0] == "stream_type")
            stream_name = ast[PPSTREAM_TYPE_NAME]
            assert( stream_name in TypeChecker.symbol_table.keys())
            names = []
            get_events_names(ast[PPSTREAM_TYPE_EVENT_LIST], names)
            assert(stream_name not in TypeChecker.stream_types_to_events.keys())
            TypeChecker.stream_types_to_events[stream_name] = set(names)


    @staticmethod
    def add_reserved_keywords():
        for keyword in reserved.keys():
            TypeChecker.symbol_table[keyword] = RESERVED

    @staticmethod
    def symbol_exists(symbol: str) -> bool:
        return symbol in TypeChecker.symbol_table.keys()

    @staticmethod
    def get_symbol_type(symbol: str) -> str:
        if symbol == "hole":
            return EVENT_NAME
        return TypeChecker.symbol_table[symbol]

    @staticmethod
    def assert_symbol_type(symbol: str, type_: str):
        if TypeChecker.get_symbol_type(symbol) != type_:
            raise Exception(f"{symbol} is expected to be of type {type_}, instead is {TypeChecker.get_symbol_type(symbol)}")


    @staticmethod
    def insert_symbol(symbol: str, type_: str) -> None:
        if TypeChecker.symbol_exists(symbol):
            raise Exception(f"Symbol {symbol}  of type {TypeChecker.get_symbol_type(symbol)} already exists")
        TypeChecker.symbol_table[symbol] = type_

    @staticmethod
    def is_symbol_in_args_table(symbol):
        return symbol in TypeChecker.args_table.keys()

    @staticmethod
    def insert_into_args_table(symbol: str, symbol_type: str, args_: List[str]) -> None:
        TypeChecker.insert_symbol(symbol,  symbol_type)
        assert(TypeChecker.symbol_exists(symbol))

        assert(not TypeChecker.is_symbol_in_args_table(symbol))
        TypeChecker.args_table[symbol] = args_

    @staticmethod
    def assert_num_args_match(symbol, expected_n):
        if symbol == "hole":
            if expected_n != 1:
                raise Exception("Event hole takes 1 argument.")
        elif len(TypeChecker.args_table[symbol]) != expected_n:
            raise Exception(f"Only {expected_n} arguments provided to function {symbol} that receives {len(TypeChecker.args_table[symbol])} arguments.")

    @staticmethod
    def check_args_are_primitive(symbol: str):
        if not TypeChecker.stream_events_are_primitive[symbol]:
            raise Exception("Calling function that has a non-primitive type parameter")

    @staticmethod
    def insert_event_list(symbol, event_list_tree):
        assert(symbol in TypeChecker.symbol_table.keys())
        TypeChecker.stream_events_are_primitive[symbol] = are_all_events_decl_primitive(event_list_tree)

    @staticmethod
    def is_event_in_stream(stream: str, event_name: str):
        if event_name == "hole":
            return True
        return event_name in TypeChecker.stream_types_to_events[stream]


    @staticmethod
    def check_performance_match(ast, output_type):
        if ast[0] == "perf_match2":
            TypeChecker.check_performance_match(ast[PPPERF_MATCH_TRUE_PART], output_type)
            TypeChecker.check_performance_match(ast[PPPERF_MATCH_FALSE_PART], output_type)
        else:
            assert(ast[0] == "perf_match1")
            perf_action = ast[PPPERF_MATCH_ACTION]
            output_event = perf_action[PPPERF_ACTION_FORWARD_EVENT]
            if not TypeChecker.is_event_in_stream(output_type, output_event):
                raise Exception(f"Event {output_event} does not happen in stream {output_type}.")


    @staticmethod
    def check_perf_layer_list(ast, input_type, output_type):
        if ast[0] == "perf_layer_list":
            TypeChecker.check_perf_layer_list(ast[PLIST_BASE_CASE], input_type, output_type)
            TypeChecker.check_perf_layer_list(ast[PLIST_TAIL], input_type, output_type)
        else:
            assert(ast[0] == "perf_layer_rule")
            if not TypeChecker.is_event_in_stream(input_type, ast[PPPERF_LAYER_EVENT]):
                raise Exception(f"event {ast[PPPERF_LAYER_EVENT]} does not happen in stream {input_type}.")
            # we already check right number of arguments at parser.py in p_performance_layer_rule

            # check output types
            TypeChecker.check_performance_match(ast[PPPERF_LAYER_PERF_MATCH], output_type)

    @staticmethod
    def check_event_sources_types(ast):
        if ast[0] == "event_sources":
            TypeChecker.check_event_sources_types(ast[PLIST_BASE_CASE])
            TypeChecker.check_event_sources_types(ast[PLIST_TAIL])
        else:
            assert(ast[0] == "event_source")
            event_source = ast[PPEVENT_SOURCE_NAME]
            input_type = ast[PPEVENT_SOURCE_INPUT_TYPE]
            output_type = ast[PPEVENT_SOURCE_OUTPUT_TYPE]
            TypeChecker.event_sources_types[event_source] = (input_type, output_type)
            TypeChecker.check_perf_layer_list(ast[PPEVENT_SOURCE_PERF_LAYER_LIST],
                                              input_type, output_type)

    @staticmethod
    def check_list_buff_exprs(ast):
        if ast[0] == "l_buff_match_exp":
            TypeChecker.check_list_buff_exprs(ast[PLIST_BASE_CASE])
            TypeChecker.check_list_buff_exprs(ast[PLIST_TAIL])
        else:
            assert(ast[0] == "buff_match_exp")
            event_source = ast[PPBUFFER_MATCH_EV_NAME]
            for i in range(1, len(ast)):
                TypeChecker.check_event_calls(ast[i], event_source)

    @staticmethod
    def is_event_in_event_source(event_source, event, is_input=True):
        types = TypeChecker.event_sources_types[event_source]
        if is_input:
            stream = types[0]
        else:
            stream = types[1]
        return TypeChecker.is_event_in_stream(stream, event)

    @staticmethod
    def check_event_calls(ast, stream_name):
        if ast[0] == "list_ev_calls":
            event = ast[PPLIST_EV_CALL_EV_NAME]
            if not TypeChecker.is_event_in_event_source(stream_name, event):
                raise Exception(f"Event source {stream_name} does not considers event {event}")
            TypeChecker.check_event_calls(ast[PPLIST_EV_CALL_TAIL], stream_name)
        elif ast[0] == "ev_call":
            event = ast[PPLIST_EV_CALL_EV_NAME]
            if not TypeChecker.is_event_in_event_source(stream_name, event):
                raise Exception(f"Event source {stream_name} does not considers event {event}")

    @staticmethod
    def check_arb_rule_stmt_list(ast, output_type):
        if ast[0] == "arb_rule_stmt_l":
            TypeChecker.check_arb_rule_stmt_list(ast[PLIST_BASE_CASE], output_type)
            TypeChecker.check_arb_rule_stmt_list(ast[PLIST_TAIL], output_type)
        else:
            assert(ast[0] == "ccode_statement_l")
            for i in range(1, len(ast)):
                if len(ast[i]) == 0:
                    continue
                if ast[i][0] == "yield":
                    if not TypeChecker.is_event_in_stream(output_type, ast[i][PPARB_RULE_STMT_YIELD_EVENT]):
                        raise Exception(f"Event {ast[i][PPARB_RULE_STMT_YIELD_EVENT]} does not happen in stream "
                                        f"{output_type}")
                elif ast[i][0] == "switch":
                    TypeChecker.assert_symbol_type(ast[i][PPARB_RULE_STMT_SWITCH_ARB_RULE], ARBITER_RULE_SET )




    @staticmethod
    def check_arbiter_rule_list(ast, output_type):
        if ast[0] == "arb_rule_list":
            TypeChecker.check_arbiter_rule_list(ast[PLIST_BASE_CASE], output_type)
            TypeChecker.check_arbiter_rule_list(ast[PLIST_TAIL], output_type)
        else:
            assert(ast[0] == "arbiter_rule")
            TypeChecker.check_list_buff_exprs(ast[PPARB_RULE_LIST_BUFF_EXPR])
            TypeChecker.check_arb_rule_stmt_list(ast[PPARB_RULE_STMT_LIST], output_type)


    @staticmethod
    def check_rule_set_list(ast, output_type):
        if ast[0] == "arb_rule_set_l":
            TypeChecker.check_rule_set_list(ast[PLIST_BASE_CASE], output_type)
            TypeChecker.check_rule_set_list(ast[PLIST_TAIL], output_type)
        else:
            assert(ast[0] == "arbiter_rule_set")
            TypeChecker.check_arbiter_rule_list(ast[PPARB_RULE_LIST], output_type)

    @staticmethod
    def check_arbiter(ast):
        assert(ast[0] == "arbiter_def")
        output_type = ast[PPARBITER_OUTPUT_TYPE]
        TypeChecker.arbiter_output_type = output_type
        TypeChecker.check_rule_set_list(ast[PPARBITER_RULE_SET_LIST], output_type)


    @staticmethod
    def check_monitor_rule_list(ast):
        if ast[0] == "monitor_rule_l":
            TypeChecker.check_monitor_rule_list(ast[PLIST_BASE_CASE], ast[PLIST_TAIL])
        else:
            assert(ast[0] == "monitor_rule")
            event_name = ast[PPMONITOR_RULE_EV_NAME]
            if not TypeChecker.is_event_in_stream(TypeChecker.arbiter_output_type, event_name):
                raise Exception(f"Arbiter outputs stream of type {TypeChecker.arbiter_output_type}. "
                                f"It does not consider event {event_name}.")

    @staticmethod
    def check_monitor(ast):
        assert(ast[0] == "monitor_def")
        TypeChecker.check_monitor_rule_list(ast[PPMONITOR_RULE_LIST])
