from lexer import MyLexer
import ply.lex as lex
from ply.yacc import yacc
from tokens import *


# this is the entry point
def p_main_program(p):
    '''
    main_program : stream_type_list event_source_list arbiter_definition monitor_definition
    '''
    p[0] = ('main_program', p[1], p[2], p[3], p[4])


# BEGIN event streams
def p_stream_type_list(p):
    '''
    stream_type_list : stream_type
                     | stream_type stream_type_list
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ('stream_types', p[1], p[2])


def p_stream_type(p):
    '''
    stream_type : STREAM TYPE ID '{' event_list '}'
    '''

    p[0] = ("stream_type", p[3], p[5])


def p_event_declaration_list(p):
    '''
    event_list : event_decl ';'
               | event_decl ';' event_list
    '''
    if len(p) == 3:
        p[0] = p[1]
    else:
        assert (len(p) == 4)
        p[0] = ("event_list", p[1], p[3])


def p_event_declaration(p):
    '''
    event_decl : ID '(' list_field_decl ')'
    '''
    p[0] = ('event_decl', p[1], p[3])


def p_field_declaration_list(p):
    '''
    list_field_decl : field_decl
                    | field_decl ',' list_field_decl
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        assert (len(p) == 4)
        p[0] = ("list_field_decl", p[1], p[3])


def p_field_declaration(p):
    '''
    field_decl : ID ':' type
    '''
    p[0] = ("field_decl", p[1], p[3])

# END event streams

# BEGIN performance layer specifications

def p_event_source_list(p):
    '''
    event_source_list : event_source
                      | event_source event_source_list
    '''

    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ('event_sources', p[1], p[2])

def p_event_source(p):
    '''
    event_source : EVENT SOURCE ID ':' ID right_arrow connection_kind ID '{' perf_layer_rule_list '}'
    '''

    # EVENT SOURCE ID : ID right_arrow connection_kind ID { perf_layer_rule_list }
    #   1     2     3 4  5      6           7          8  9           10
    p[0] = ("event_source", p[3], p[5], p[7], p[8], p[10])

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
        p[0] = p[1]
    else:
        assert(len(p) == 4)
        p[0] = ("perf_layer_list", p[1], p[3])


def p_performance_layer_rule(p):
    '''
    perf_layer_rule : ON ID '(' listids ')' performance_match
    '''

    # ON ID ( listids ) performance_match
    #  1  2  3    4   5          6

    p[0] = ("perf_layer_rule", p[2], p[4], p[6])


def p_performance_match (p):
    '''
    performance_match : performance_action
                      | IF '(' expression ')' THEN performance_match ELSE performance_match
    '''
    if len(p) == 2:
        p[0] = ("perf_match1",p[1])
    else:
        # IF ( expression ) THEN performance_match ELSE performance_match
        # 1  2      3     4   5          6           7          8
        assert(len(p) == 8)
        p[0] = ("perf_match2", p[3], p[6], p[8])


def p_performance_action(p):
    '''
    performance_action : DROP
                       | FORWARD ID '(' expression_list ')'
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ("perf_act_forward", p[2], p[4])


def p_connection_kind(p):
    '''
    connection_kind : AUTODROP '(' INT ')'
                    | BLOCKING '(' INT ')'
                    | INFINITE
    '''
    if len(p) == 0:
        p[0] = p[1]
    else:
        p[0] = ('CONN_KIND2', p[1], p[3])
# END performance layer specifications

# BEGIN arbiter Specification
def p_arbiter_definition(p):
    '''
    arbiter_definition : ARBITER ':' ID '{' arbiter_rule_set_list '}'
    '''
    p[0] = ("arbiter_def", p[3], p[5])


def p_arbiter_rule_set_list(p):
    '''
    arbiter_rule_set_list : arbiter_rule_set
                          | arbiter_rule_set arbiter_rule_set_list
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ("arb_rule_set_l", p[1], p[3])


def p_arbiter_rule_set(p):
    '''
    arbiter_rule_set : RULE SET ID '{' arbiter_rule_list '}'
    '''

    p[0] = ("arbiter_rule_set", p[3], p[5])


def p_arbiter_rule_list(p):
    '''
    arbiter_rule_list : arbiter_rule
                      | arbiter_rule arbiter_rule_list
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ("arb_rule_list", p[1], p[2])


def p_arbiter_rule(p):
    '''
    arbiter_rule : ON list_buff_match_exp WHERE expression arbiter_rule_stmt_list
    '''

    # ON list_buff_match_exp WHERE pure_foreign_code arbiter_rule_stmt_list
    # 1           2            3           4                    5
    p[0] = ("arbiter_rule", p[2], p[4], p[5])


def p_list_buff_match_exp(p):
    '''
    list_buff_match_exp : buffer_match_exp
                        | buffer_match_exp ',' list_buff_match_exp
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        assert(len(p) == 4)
        p[0] = ("l_buff_match_exp", p[1], p[3])


def p_buffer_match_exp(p):
    '''
    buffer_match_exp : ID ':' INT
                     | ID ':' NOTHING
                     | ID ':' DONE
                     | ID ':' '|' list_event_calls
                     | ID ':' list_event_calls '|'
                     | ID ':' list_event_calls '|' list_event_calls
    '''
    if len(p) == 4:
        p[0] = ("buff_match_exp", p[1], p[3])
    elif len(p) == 5:
        p[0] = ("buff_match_exp", p[1], p[3], p[4])
    else:
        p[0] = ("buff_match_exp", p[1], p[3], p[5])


def p_list_event_calls(p):
    '''
    list_event_calls : ID '(' listids ')'
                     | ID '(' listids  ')' list_event_calls
    '''

    if len(p) == 5:
        p[0] = ("list_ev_calls", p[1], p[3])
    elif len(p) == 1:
        p[0] = ("empty_list_ev_calls", "")
    else:
        p[0] = ("list_ev_calls", p[1], p[3], p[6])


def p_arbiter_rule_stmt_list(p):
    '''
    arbiter_rule_stmt_list : ccode_statement_list
                           | ccode_statement_list arbiter_rule_stmt_list
    '''

    if len(p) == 2:
        p[0] = ("arb_rule_stmt_l", p[1])
    else:
        p[0] = ("arb_rule_stmt_l", p[1], p[2])


def p_ccode_statement_list(p):
    '''
    ccode_statement_list : CCODE_TOKEN
                         | CCODE_TOKEN arbiter_rule_stmt ';'
                         | arbiter_rule_stmt ';'
                         | arbiter_rule_stmt ';' CCODE_TOKEN
                         |
    '''

    if len(p) == 1:
        p[0] = ("ccode_statement_l", "")
    elif len(p) == 2:
        p[0] = ("ccode_statement_l", p[1])
    elif len(p) == 3:
        p[0] = ("ccode_statement_l", p[1], p[2])
    else:
        p[0] = ("ccode_statement_l", p[1], p[2], p[3])
def p_arbiter_rule_stmt(p):
    '''
    arbiter_rule_stmt : YIELD ID '(' expression_list ')'
                      | SWITCH TO arbiter_rule
                      | DROP INT FROM ID
    '''

    # TODO: like s, without return, but with drop n from S

    if len(p) == 6:
        assert(p[1] == "yield")
        p[0] = ("arb_rule_stmt", p[2], p[4])
    elif len(p) == 5:
        p[0] = ("arb_rule_stmt", p[2], p[4])
    else:
        assert(p[1] == "switch")
        p[0] = ("arb_rule_stmt", p[3])


# END arbiter Specification

# BEGIN monitor Specification

def p_monitor_definition(p):
    '''
    monitor_definition : MONITOR '{' monitor_rule_list '}'
    '''
    p[0] = ("monitor_def", p[3])

def p_monitor_rule_list(p):
    '''
    monitor_rule_list : monitor_rule
                      | monitor_rule monitor_rule_list
    '''

    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ("monitor_rule_l", p[1], p[3])

def p_monitor_rule(p):
    '''
    monitor_rule : ON ID '(' listids ')' WHERE expression CCODE_TOKEN
    '''

    # ON ID ( listids ) WHERE expression list_statement
    #  1 2  3    4    5   6       7            8
    p[0] = ("monitor_rule", p[4], p[7], p[8])

# END monitor Specification

def p_type(p):
    '''
    type : ID
         | type '[' ']'
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        assert (len(p) == 4)
        p[0] = ('array', p[1], [2])


def p_expression_list(p):
    '''
    expression_list : expression
                    | expression ',' expression_list
    '''

    if len(p) == 2:
        p[0] = p[1]
    else:
        assert(len(p) == 4)
        p[0] = ("expr_list", p[1], p[3])


def p_expression(p):
    '''
    expression : CCODE_TOKEN
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        assert(len(p) == 4)
        p[0] = ('binop', p[2], p[1], p[3])

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
        p[0] = p[1]
    elif len(p) == 4:
        p[0] = ("listids", p[1], p[3])
    else:
        assert(False)


# public interface
def parse_program(s: str):
    L = MyLexer()
    lexer = lex.lex(object=L)
    parser = yacc(debug=None)

    # Parse an expression
    ast = parser.parse(s)
    print(ast)
    return ast

