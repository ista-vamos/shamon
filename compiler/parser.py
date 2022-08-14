from lexer import MyLexer
import ply.lex as lex
from ply.yacc import yacc
from tokens import *
from type_checker import *
from utils import *
from parser_indices import *


# this is the entry point
def p_main_program(p):
    '''
    main_program : stream_type_list event_source_list arbiter_definition monitor_definition
    '''
    p[0] = ('main_program', p[PMAIN_PROGRAM_STREAM_TYPES], p[PMAIN_PROGRAM_EVENT_SOURCES], p[PMAIN_PROGRAM_ARBITER],
            p[PMAIN_PROGRAM_MONITOR])


# BEGIN event streams
def p_stream_type_list(p):
    '''
    stream_type_list : stream_type
                     | stream_type stream_type_list
    '''
    if len(p) == 2:
        p[0] = p[PLIST_BASE_CASE]
    else:
        p[0] = ('stream_types', p[PLIST_BASE_CASE], p[PLIST_TAIL])


def p_stream_type(p):
    '''
    stream_type : STREAM TYPE ID '{' event_list '}'
    '''
    TypeChecker.insert_symbol(p[PSTREAM_TYPE_NAME], STREAM_TYPE_NAME)
    TypeChecker.insert_event_list(p[PSTREAM_TYPE_NAME], p[PSTREAM_TYPE_EVENT_LIST])
    p[0] = ("stream_type", p[PSTREAM_TYPE_NAME], p[PSTREAM_TYPE_EVENT_LIST])


def p_event_declaration_list(p):
    '''
    event_list : event_decl ';'
               | event_decl ';' event_list
    '''
    if len(p) == 3:
        p[0] = p[PLIST_BASE_CASE]
    else:
        assert (len(p) == 4)
        p[0] = ("event_list", p[PLIST_BASE_CASE], p[PLIST_TAIL_WITH_SEP])


def p_event_declaration(p):
    '''
    event_decl : ID '(' list_field_decl ')'
    '''

    # Type checker
    params = []
    get_parameters_types_field_decl(p[PEVENT_PARAMS_LIST], params)
    TypeChecker.insert_into_args_table(p[PEVENT_NAME], EVENT_NAME, params)

    p[0] = ('event_decl', p[PEVENT_NAME], p[PEVENT_PARAMS_LIST])


def p_field_declaration_list(p):
    '''
    list_field_decl : field_decl
                    | field_decl ',' list_field_decl
    '''
    if len(p) == 2:
        p[0] = p[PLIST_BASE_CASE]
    else:
        assert (len(p) == 4)
        p[0] = ("list_field_decl", p[PLIST_BASE_CASE], p[PLIST_TAIL_WITH_SEP])


def p_field_declaration(p):
    '''
    field_decl : ID ':' type
    '''
    p[0] = ("field_decl", p[PFIELD_NAME], p[PFIELD_TYPE])

# END event streams

# BEGIN performance layer specifications

def p_event_source_list(p):
    '''
    event_source_list : event_source
                      | event_source event_source_list
    '''

    if len(p) == 2:
        p[0] = p[PLIST_BASE_CASE]
    else:
        p[0] = ('event_sources', p[PLIST_BASE_CASE], p[PLIST_TAIL])

def p_event_source(p):
    '''
    event_source : EVENT SOURCE ID ':' ID right_arrow connection_kind ID '{' perf_layer_rule_list '}'
    '''

    # EVENT SOURCE ID : ID right_arrow connection_kind ID { perf_layer_rule_list }
    #   1     2     3 4  5      6           7          8  9           10

    p[0] = ("event_source", p[PEVENT_SOURCE_NAME], p[PEVENT_SOURCE_INPUT_TYPE], p[PEVENT_CONN_KIND],
            p[PEVENT_SOURCE_OUTPUT_TYPE], p[PEVENT_SOURCE_PERF_LAYER_LIST])

    TypeChecker.insert_symbol(p[PEVENT_SOURCE_NAME], EVENT_SOURCE_NAME)
    TypeChecker.assert_symbol_type(p[PEVENT_SOURCE_INPUT_TYPE], STREAM_TYPE_NAME)
    TypeChecker.assert_symbol_type(p[PEVENT_SOURCE_OUTPUT_TYPE], STREAM_TYPE_NAME)
    TypeChecker.check_args_are_primitive(p[PEVENT_SOURCE_OUTPUT_TYPE])
    TypeChecker.check_args_are_primitive(p[PEVENT_SOURCE_INPUT_TYPE])

def p_right_arrow(p):
    '''
    right_arrow : '-' '>'
    '''
    p[0] = ('right_arrow', '->')

def p_perf_layer_rule_list(p):
    '''
    perf_layer_rule_list : perf_layer_rule ';'
                         | perf_layer_rule ';' perf_layer_rule_list
    '''
    if len(p) == 3:
        p[0] = p[PLIST_BASE_CASE]
    else:
        assert(len(p) == 4)
        p[0] = ("perf_layer_list", p[PLIST_BASE_CASE], p[PLIST_TAIL_WITH_SEP])


def p_performance_layer_rule(p):
    '''
    perf_layer_rule : ON ID '(' listids ')' performance_match
    '''

    # ON ID ( listids ) performance_match
    #  1  2  3    4   5          6


    TypeChecker.assert_symbol_type(p[PPERF_LAYER_EVENT], EVENT_NAME)
    length_listids = get_count_list_ids(p[PPERF_LAYER_EV_ARGS])
    TypeChecker.assert_num_args_match(p[PPERF_LAYER_EVENT], length_listids)

    p[0] = ("perf_layer_rule", p[PPERF_LAYER_EVENT], p[PPERF_LAYER_EV_ARGS], p[PPERF_LAYER_PERF_MATCH])


def p_performance_match (p):
    '''
    performance_match : performance_action
                      | IF '(' expression ')' THEN performance_match ELSE performance_match
    '''
    if len(p) == 2:
        p[0] = ("perf_match1",p[PPERF_MATCH_ACTION])
    else:
        # IF ( expression ) THEN performance_match ELSE performance_match
        # 1  2      3     4   5          6           7          8
        assert(len(p) == 8)
        p[0] = ("perf_match2", p[PPERF_MATCH_EXPRESSION], p[PPERF_MATCH_TRUE_PART], p[PPERF_MATCH_FALSE_PART])


def p_performance_action(p):
    '''
    performance_action : DROP
                       | FORWARD ID '(' expression_list ')'
    '''
    if len(p) == 2:
        p[0] = ("perf_act_drop",p[PPERF_ACTION_DROP])
    else:
        TypeChecker.assert_symbol_type(p[PPERF_ACTION_FORWARD_EVENT], EVENT_NAME)
        p[0] = ("perf_act_forward", p[PPERF_ACTION_FORWARD_EVENT], p[PPERF_ACTION_FORWARD_EXPRS])


def p_connection_kind(p):
    '''
    connection_kind : AUTODROP '(' INT ')'
                    | BLOCKING '(' INT ')'
                    | INFINITE
    '''
    if len(p) == 0:
        p[0] = ('conn_kind', p[PCONN_KIND_NAME])
    else:
        p[0] = ('conn_kind', p[PCONN_KIND_NAME], p[PCONN_KIND_INT])
# END performance layer specifications

# BEGIN arbiter Specification
def p_arbiter_definition(p):
    '''
    arbiter_definition : ARBITER ':' ID '{' arbiter_rule_set_list '}'
    '''

    TypeChecker.assert_symbol_type(p[ARBITER_OUTPUT_TYPE], STREAM_TYPE_NAME)
    p[0] = ("arbiter_def", p[ARBITER_OUTPUT_TYPE], p[ARBITER_RULE_SET_LIST])


def p_arbiter_rule_set_list(p):
    '''
    arbiter_rule_set_list : arbiter_rule_set
                          | arbiter_rule_set arbiter_rule_set_list
    '''
    if len(p) == 2:
        p[0] = p[PLIST_BASE_CASE]
    else:
        p[0] = ("arb_rule_set_l", p[PLIST_BASE_CASE], p[PLIST_TAIL])


def p_arbiter_rule_set(p):
    '''
    arbiter_rule_set : RULE SET ID '{' arbiter_rule_list '}'
    '''
    TypeChecker.insert_symbol(p[PARB_RULE_SET_NAME], ARBITER_RULE_SET)

    p[0] = ("arbiter_rule_set", p[PARB_RULE_SET_NAME], p[PARB_RULE_LIST])


def p_arbiter_rule_list(p):
    '''
    arbiter_rule_list : arbiter_rule
                      | arbiter_rule arbiter_rule_list
    '''
    if len(p) == 2:
        p[0] = p[PLIST_BASE_CASE]
    else:
        p[0] = ("arb_rule_list", p[PLIST_BASE_CASE], p[PLIST_TAIL])


def p_arbiter_rule(p):
    '''
    arbiter_rule : ON list_buff_match_exp WHERE expression arbiter_rule_stmt_list
    '''

    # ON list_buff_match_exp WHERE pure_foreign_code arbiter_rule_stmt_list
    # 1           2            3           4                    5
    p[0] = ("arbiter_rule", p[PARB_RULE_LIST_BUFF_EXPR], p[PARB_RULE_CONDITION_CODE], p[PARB_RULE_STMT_LIST])


def p_list_buff_match_exp(p):
    '''
    list_buff_match_exp : buffer_match_exp
                        | buffer_match_exp ',' list_buff_match_exp
    '''
    if len(p) == 2:
        p[0] = p[PLIST_BASE_CASE]
    else:
        assert(len(p) == 4)
        p[0] = ("l_buff_match_exp", p[PLIST_BASE_CASE], p[PLIST_TAIL_WITH_SEP])


def p_buffer_match_exp(p):
    '''
    buffer_match_exp : ID ':' INT
                     | ID ':' NOTHING
                     | ID ':' DONE
                     | ID ':' '|' list_event_calls
                     | ID ':' list_event_calls '|'
                     | ID ':' list_event_calls '|' list_event_calls
    '''

    TypeChecker.assert_symbol_type(p[PBUFFER_MATCH_EV_NAME], EVENT_SOURCE_NAME)
    if len(p) == 4:
        p[0] = ("buff_match_exp", p[PBUFFER_MATCH_EV_NAME], p[PBUFFER_MATCH_ARG1])
    elif len(p) == 5:
        p[0] = ("buff_match_exp", p[PBUFFER_MATCH_EV_NAME], p[PBUFFER_MATCH_ARG1], p[PBUFFER_MATCH_ARG2])
    else:
        p[0] = ("buff_match_exp", p[PBUFFER_MATCH_EV_NAME], p[PBUFFER_MATCH_ARG1], p[PBUFFER_MATCH_ARG3])


def p_list_event_calls(p):
    '''
    list_event_calls : ID '(' listids ')'
                     | ID '(' listids  ')' list_event_calls
    '''

    # TODO: what is E^H
    # TypeChecker.assert_symbol_type(p[1], EVENT_NAME)
    # list_ids_length = get_count_list_ids(p[3])
    # TypeChecker.assert_num_args_match(p[1], list_ids_length)

    if len(p) == 5:
        p[0] = ("ev_call", p[PLIST_EV_CALL_EV_NAME], p[PLIST_EV_CALL_EV_PARAMS])
    else:
        p[0] = ("list_ev_calls", p[PLIST_EV_CALL_EV_NAME], p[PLIST_EV_CALL_EV_PARAMS], p[PLIST_EV_CALL_TAIL])




def p_arbiter_rule_stmt_list(p):
    '''
    arbiter_rule_stmt_list : ccode_statement_list
                           | ccode_statement_list arbiter_rule_stmt_list
    '''

    if len(p) == 2:
        p[0] = p[PLIST_BASE_CASE]
    else:
        p[0] = ("arb_rule_stmt_l", p[PLIST_BASE_CASE], p[PLIST_TAIL])


def p_ccode_statement_list(p):
    '''
    ccode_statement_list : CCODE_TOKEN
                         | CCODE_TOKEN arbiter_rule_stmt ';'
                         | arbiter_rule_stmt ';'
                         | arbiter_rule_stmt ';' CCODE_TOKEN
                         |
    '''

    if len(p) == 1:
        #
        p[0] = ("ccode_statement_l", "")
    elif len(p) == 2:
        # CCODE_TOKEN
        p[0] = ("ccode_statement_l", p[PCODE_STMT_LIST_TOKEN1])
    elif len(p) == 3:
        # arbiter_rule_stmt ';'
        p[0] = ("ccode_statement_l", p[PCODE_STMT_LIST_TOKEN1], p[PCODE_STMT_LIST_TOKEN2])
    else:
        assert(len(p) == 4)
        # CCODE_TOKEN arbiter_rule_stmt ';'
        # arbiter_rule_stmt ';' CCODE_TOKEN
        p[0] = ("ccode_statement_l", p[PCODE_STMT_LIST_TOKEN1], p[PCODE_STMT_LIST_TOKEN2], p[PCODE_STMT_LIST_TOKEN3])


def p_arbiter_rule_stmt(p):
    '''
    arbiter_rule_stmt : YIELD ID '(' expression_list ')'
                      | SWITCH TO arbiter_rule
                      | DROP INT FROM ID
    '''

    if len(p) == 6:
        assert(p[1] == "yield")
        p[0] = ("yield", p[PARB_RULE_STMT_YIELD_EVENT], p[PARB_RULE_STMT_YIELD_EXPRS])
        TypeChecker.assert_symbol_type(p[PARB_RULE_STMT_YIELD_EVENT], EVENT_NAME)
        count_expr_list = get_count_list_expr(p[PARB_RULE_STMT_YIELD_EXPRS])
        TypeChecker.assert_num_args_match(p[PARB_RULE_STMT_YIELD_EVENT], count_expr_list)
    elif len(p) == 5:
        p[0] = ("drop", p[PARB_RULE_STMT_DROP_INT], p[PARB_RULE_STMT_DROP_EV_SOURCE])

        TypeChecker.assert_symbol_type(p[PARB_RULE_STMT_DROP_EV_SOURCE], EVENT_SOURCE_NAME)
    else:
        assert(p[1] == "switch")
        # TODO: should I check something here?
        p[0] = ("switch", p[PARB_RULE_STMT_SWITCH_ARB_RULE])


# END arbiter Specification

# BEGIN monitor Specification

def p_monitor_definition(p):
    '''
    monitor_definition : MONITOR '{' monitor_rule_list '}'
    '''
    p[0] = ("monitor_def", p[PMONITOR_RULE_LIST])

def p_monitor_rule_list(p):
    '''
    monitor_rule_list : monitor_rule
                      | monitor_rule monitor_rule_list
    '''

    if len(p) == 2:
        p[0] = p[PLIST_BASE_CASE]
    else:
        p[0] = ("monitor_rule_l", p[PLIST_BASE_CASE], p[PLIST_TAIL])

def p_monitor_rule(p):
    '''
    monitor_rule : ON ID '(' listids ')' WHERE expression CCODE_TOKEN
    '''

    # ON ID ( listids ) WHERE expression list_statement
    #  1 2  3    4    5   6       7            8

    TypeChecker.assert_symbol_type(p[PMONITOR_RULE_EV_NAME], EVENT_NAME)
    TypeChecker.assert_num_args_match(p[PMONITOR_RULE_EV_NAME], get_count_list_ids(p[PMONITOR_RULE_EV_ARGS]))
    p[0] = ("monitor_rule", p[PMONITOR_RULE_EV_NAME], p[PMONITOR_RULE_EV_ARGS], p[PMONITOR_RULE_EXPR],
            p[PMONITOR_RULE_CODE])

# END monitor Specification

def p_type(p):
    '''
    type : ID
         | type '[' ']'
    '''
    if len(p) == 2:
        p[0] = ("type", p[PTYPE_NAME])
    else:
        assert (len(p) == 4)
        p[0] = ('array', p[PTYPE_NAME])


def p_expression_list(p):
    '''
    expression_list : expression
                    | expression ',' expression_list
    '''

    if len(p) == 2:
        p[0] = ("expr",p[PLIST_BASE_CASE])
    else:
        assert(len(p) == 4)
        p[0] = ("expr_list", p[PLIST_BASE_CASE], p[PLIST_TAIL_WITH_SEP])


def p_expression(p):
    '''
    expression : ID
               | INT
               | BOOL
               | CCODE_TOKEN
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        raise Exception("this should not happen")
        # assert(len(p) == 4)
        # p[0] = ('binop', p[2], p[1], p[3])

def p_arithmetic_op(p):
    '''
    arithmetic_op : OP
                  | '-'
    '''
    p[0] = p[1]

def p_listids(p):
    '''
    listids : ID
            | ID ',' listids
    '''
    if len(p) == 2:
        p[0] = ("ID",p[PLIST_BASE_CASE])
    elif len(p) == 4:
        p[0] = ("listids", p[PLIST_BASE_CASE], p[PLIST_TAIL_WITH_SEP])
    else:
        assert(False)


# public interface
def parse_program(s: str):
    L = MyLexer()
    lexer = lex.lex(object=L)
    parser = yacc(debug=None)

    # Parse an expression
    ast = parser.parse(s)
    return ast

