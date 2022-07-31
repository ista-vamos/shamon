from typing import Dict, List
from tokens import reserved

# define some types:
VARIABLE = "variable"
RESERVED = "reserved"
# FIELD_NAME = "field_name" # we do not need this type
EVENT_NAME = "event_name"
STREAM_TYPE_NAME = "stream_type_name"

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
        return TypeChecker.get_symbol_type(symbol) == type_


    @staticmethod
    def insert_symbol(symbol: str, type_: str) -> None:
        if TypeChecker.symbol_exists(symbol):
            raise Exception(f"Symbol {symbol}  of type {TypeChecker.get_symbol_type(symbol)} already exists")
        TypeChecker.symbol_table[symbol] = type_