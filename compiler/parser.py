from lexer import MyLexer
import ply.lex as lex
from ply.yacc import yacc
from tokens import *


# this is the entry point
def p_main_program(p):
    '''
    main_program : stream_type_list event_source_list
    '''
    p[0] = ('main_program', p[1], p[2])


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
    #  1  2       3               4

    p[0] = ("perf_layer_rule", p[2], p[3], p[4])


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
        p[1] = ('CONN_KIND2', p[1], p[3])
# END performance layer specifications


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
    expression : ID
               | INT
               | BOOL
               | expression arithmetic_op expression
               | expression BOOL_OP expression
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
    else:
        assert(len(p) == 4)
        p[0] = ("listids", p[1], p[3])

# public interface
def parse_program(s: str):
    L = MyLexer()
    lexer = lex.lex(object=L)
    parser = yacc(debug=None)

    # Parse an expression
    ast = parser.parse(s)
    print(ast)
    return ast