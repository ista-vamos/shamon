import ply.lex as lex
from ply.yacc import yacc

literals = ['[', ']','{', '}', '(', ')', ':', ';', '=', ',']

def t_2dots(t):
    r"^:$"
    t.type = ':'
    return t

def t_comma(t):
    r"^,$"
    t.type = ','
    return t

def t_equal(t):
    r"^=$"
    t.type = '='
    return t

def t_semicol(t):
    r"^;$"
    t.type = ";"
    return t

def t_lcurly(t):
    r"^\{$"
    t.type = '{'  # Set token type to the expected literal
    return t


def t_rcurly(t):
    r"^\}$"
    t.type = '}'  # Set token type to the expected literal
    return t


def t_lbracket(t):
    r"^\[$"
    t.type = '['  # Set token type to the expected literal
    return t


def t_rbracket(t):
    r"^\]$"
    t.type = ']'  # Set token type to the expected literal
    return t

def t_lparenthesis(t):
    r"^\($"
    t.type = '('  # Set token type to the expected literal
    return t


def t_rparenthesis(t):
    r"^\)$"
    t.type = ')'  # Set token type to the expected literal
    return t

# define tokens
tokens = (
          # KEYWORDS
          "IF", "THEN", "ELSE", "STREAM",
          "TYPE", "CONN_KIND_KEYWORD",
          "CONN_KIND_KEYWORD2", "DROP", "FORWARD",
          "ON", "RIGHT_ARROW", "DONE", "NOTHING",
          "YIELD", "SWITCH", "TO", "WHERE",
          "RULE","SET", "ARBITER", "MONITOR",
          "EVENT", "SOURCE",
          # data types
          "BYTE", "BOOL", "INT", "STRING","ID",
          # operators
          "OP", "BOOL_OP",
        )

# BEGIN token regex
t_BYTE = r"^[0|1]{8}b$"
t_BOOL = r"^(true|false)$"
# A function can be used if there is an associated action.
# Write the matching regex in the docstring.
def t_INT(t):
    r'^(-?|\+?)([1-9][0-9]+)|0$'
    t.value = int(t.value)
    return t

t_ignore = ' \t'

# Ignored token with an action associated with it
def t_ignore_newline(t):
    r'\n+'
    t.lexer.lineno += t.value.count('\n')

# Error handler for illegal characters
def t_error(t):
    print(f'Illegal character {t.value[0]!r}')
    t.lexer.skip(1)

t_STRING = r"^\"[a-zA-Z0-9]*\"$"  # TODO: I am not completely sure what kind of string me want to accept.
                                    # Maybe this is enough?
t_ID = r"[a-zA-Z][a-zA-Z_0-9]*"  # for variable names and keywords
t_OP = r"^(\+|-|\*|\/|\^)$"  # arithmetic operators
t_BOOL_OP = r"^(and|or)"  # boolean operators

t_IF = r"^if$"
t_THEN = r"^then$"
t_ELSE = r"^else$"
t_STREAM = r"^stream$"
t_TYPE = r"^type$"
t_CONN_KIND_KEYWORD = r"^infinite$"
t_CONN_KIND_KEYWORD2 = r"^(autodrop|blocking)$"
t_DROP = r"^drop$"
t_FORWARD = r"^forward$"
t_ON = r"^on$"
t_RIGHT_ARROW = r"^->$"
t_DONE = r"^done$"
t_NOTHING = r"^nothing$"
t_YIELD= r"^yield$"
t_SWITCH= r"^switch$"
t_TO = r"^to$"
t_WHERE = r"^where$"
t_RULE = r"^rule$"
t_SET = r"^set$"
t_ARBITER = r"^arbiter$"
t_MONITOR = r"^monitor$"
t_EVENT = r"^event$"
t_SOURCE = r"^source$"


# Parser
# Type Construct
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


def p_type(p):
    '''
    type : ID
         | type '[' ']'
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        assert(len(p) == 4)
        p[0] = ('array', p[1], [2])

# Expression construct

# def p_grouped_expression(p):
#     '''
#     factor : '(' expression ')'
#     '''
#     assert(len(p) == 4)
#     p[0] = ('grouped', p[2])


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
               | expression OP expression
               | expression BOOL_OP expression
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        assert(len(p) == 4)
        p[0] = ('binop', p[2], p[1], p[3])



# Statement construct

def p_list_statement(p):
    '''
    list_statement : statement
                   | statement ',' list_statement
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        assert(len(p) == 4)
        p[0] = ('list_statement', p[1], p[3])


def p_statement(p):
    '''
    statement : '{' list_statement '}'
              | ID ':' type ';'
              | ID '=' expression ';'
              | expression ';'
              | IF '(' expression ')' THEN statement ELSE statement
    '''
    if p[1] == "{":
        # {list_statement}
        assert(len(p) == 4)
        p[0] = ('list_s', p[2])
    elif p[2] == ":":
        # var : type;
        assert (len(p) == 5)
        p[0] = ('decl', p[1], p[3])
    elif p[2] == '=':
        # var = expression;
        assert(len(p) == 5)
        p[0] = ('assignment', p[1], p[3])
    elif len(p) == 2:
        # expression;
        assert(len(p) == 3)
        p[0] = p[1]
    else:
        assert(p[1] == "if")
        # IF(expression) statement ELSE statement
        # 1 2     3    4     5       6      7
        p[0] = ("ifelse", p[3], p[5], p[7])


# BEGIN event streams

def p_field_declaration(p):
    '''
    field_decl : ID ':' type
    '''
    p[0] = ("field_decl", p[1], p[3])

def p_field_declaration_list(p):
    '''
    list_field_decl : field_decl
                    | field_decl ',' list_field_decl
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        assert(len(p) == 4)
        p[0] = ("list_field_decl", p[1], p[3])

def p_event_declaration(p):
    '''
    event_decl : ID '(' list_field_decl ')'
    '''
    p[0] = ('event_decl', p[1], p[3])

def p_event_declaration_list(p):
    '''
    event_list : event_decl
               | event_decl ',' event_list
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        assert(len(p) == 4)
        p[0] = ("event_list", p[1], p[3])


def p_stream_type(p):
    '''
    stream_type : STREAM TYPE ID '{' event_list '}'
    '''

    p[0] = ("stream_type", p[3], p[5])

# END event streams

# BEGIN performance layer specification

def p_connection_kind(p):
    '''
    connection_kind : CONN_KIND_KEYWORD2 '(' INT ')'
                    | CONN_KIND_KEYWORD
    '''
    if len(p) == 0:
        p[0] = p[1]
    else:
        p[1] = ('CONN_KIND2', p[1], p[3])


def p_performance_action(p):
    '''
    performance_action : DROP
                       | FORWARD ID '(' expression_list ')'
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ("perf_act_forward", p[2], p[4])


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


def p_performance_layer_rule(p):
    '''
    perf_layer_rule : ON ID '(' listids ')' performance_match
    '''

    # ON ID ( listids ) performance_match
    #  1  2       3               4

    p[0] = ("perf_layer_rule", p[2], p[3], p[4])


def p_perf_layer_rule_list(p):
    '''
    perf_layer_rule_list : perf_layer_rule
                         | perf_layer_rule ',' perf_layer_rule_list
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        assert(len(p) == 4)
        p[0] = ("perf_layer_list", p[1], p[3])


def p_event_source(p):
    '''
    event_source : EVENT SOURCE ID ':' ID RIGHT_ARROW connection_kind '{' perf_layer_rule_list '}'
    '''

    # EVENT SOURCE ID : ID RIGHT_ARROW connection_kind { perf_layer_rule_list }
    #   1     2     3 4  5      6           7          8          9           10
    p[0] = ("event_source", p[3], p[5], p[7], p[9])

# END performance layer specification


# BEGIN arbiter specification

def p_list_event_calls(p):
    '''
    list_event_calls : ID '(' listids ')'
                     | ID '(' listids  ')' ',' list_event_calls
    '''

    if len(p) == 5:
        p[0] = ("list_ev_calls", p[1], p[3])
    else:
        p[0] = ("list_ev_calls", p[1], p[3], p[6])


def p_buffer_match_exp(p):
    '''
    buffer_match_exp : ID ':' INT
                     | ID ':' NOTHING
                     | ID ':' DONE
                     | ID ':' list_event_calls '[' list_event_calls ']'
    '''
    if len(p) == 4:
        p[0] = ("buff_match_exp", p[1], p[3])
    else:
        p[0] = ("buff_match_exp", p[1], p[3], p[5])


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


def p_arbiter_rule_stmt(p):
    '''
    arbiter_rule_stmt : YIELD ID '(' expression_list ')'
                      | SWITCH TO arbiter_rule
    '''

    # TODO: like s, without return, but with drop n from S

    if len(p) == 6:
        assert(p[1] == "yield")
        p[0] = ("arb_rule_stmt", p[2], p[4])
    else:
        assert(p[1] == "switch")
        p[0] = ("arb_rule_stmt", p[3])


def p_arbiter_rule_stmt_list(p):
    '''
    arbiter_rule_stmt_list : arbiter_rule_stmt
                           | arbiter_rule_stmt ',' arbiter_rule_stmt_list
    '''

    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ("arb_rule_stmt_l", p[1], p[3])


def p_arbiter_rule(p):
    '''
    arbiter_rule : ON list_buff_match_exp WHERE performance_match  '{' arbiter_rule_stmt_list '}'
    '''

    # ON list_buff_match_exp WHERE performance_match  { arbiter_rule_stmt_list }
    # 1           2            3           4          5            6           7
    p[0] = ("arbiter_rule", p[2], p[4], p[6])


def p_arbiter_rule_list(p):
    '''
    arbiter_rule_list : arbiter_rule
                      | arbiter_rule ',' arbiter_rule_list
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ("arb_rule_list", p[1], p[3])


def p_arbiter_rule_set(p):
    '''
    arbiter_rule_set : RULE SET arbiter_rule '{' arbiter_rule_list '}'
    '''

    p[0] = ("arbiter_rule_set", p[3], p[5])


def p_arbiter_rule_set_list(p):
    '''
    arbiter_rule_set_list : arbiter_rule_set
                          | arbiter_rule_set ',' arbiter_rule_set_list
    '''
    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ("arb_rule_set_l", p[1], p[3])


def p_arbiter_definition(p):
    '''
    arbiter_definition : ARBITER ':' ID '{' arbiter_rule_set_list '}'
    '''
    p[0] = ("arbiter_def", p[3], p[5])
# END arbiter specification


# BEGIN monitor specification
def p_monitor_rule(p):
    '''
    monitor_rule : ON ID '(' listids ')' WHERE expression '{' list_statement '}'
    '''

    # ON ID ( listids ) WHERE expression { list_statement }
    #  1  2 3    4     5   6       7      8      9         10
    p[0] = ("monitor_rule", p[4], p[7], p[9])


def p_monitor_rule_list(p):
    '''
    monitor_rule_list : monitor_rule
                      | monitor_rule ',' monitor_rule_list
    '''

    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = ("monitor_rule_l", p[1], p[3])


def p_monitor_definition(p):
    '''
    monitor_definition : MONITOR '{' monitor_rule_list '}'
    '''
    p[0] = ("monitor_def", p[3])

# END monitor specification
lexer = lex.lex()
lexer.input('stream type Primes{Prime(n : int,p : int);}')
# Tokenize
while True:
    tok = lexer.token()
    if not tok:
        break  # No more input
    print(tok)
# Build the parser
parser = yacc()

# Parse an expression
# ast = parser.parse('stream type Primes{Prime(n : int,p : int);}')
# print(ast)