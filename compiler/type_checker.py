from typing import Dict, List
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

    @staticmethod
    def clean_checker():
        TypeChecker.symbol_table = dict()
        TypeChecker.args_table = dict()

    @staticmethod
    def add_reserved_keywords():
        for keyword in reserved.keys():
            TypeChecker.symbol_table[keyword] = RESERVED

    @staticmethod
    def symbol_exists(symbol: str) -> bool:
        return symbol in TypeChecker.symbol_table.keys()

    @staticmethod
    def get_symbol_type(symbol: str) -> str:
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
        if len(TypeChecker.args_table[symbol]) != expected_n:
            raise Exception(f"Only {expected_n} arguments provided to function {symbol} that receives {len(TypeChecker.args_table[symbol])} arguments.")

    @staticmethod
    def check_args_are_primitive(symbol: str):
        if not TypeChecker.stream_events_are_primitive[symbol]:
            raise Exception("Calling function that has a non-primitive type parameter")

    @staticmethod
    def insert_event_list(symbol, event_list_tree):
        assert(symbol in TypeChecker.symbol_table.keys())

        TypeChecker.stream_events_are_primitive[symbol] = are_all_events_decl_primitive(event_list_tree)
