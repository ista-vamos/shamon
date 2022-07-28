literals = ['[', ']', '{', '}', '(', ')', ':', ';', '=', ',', '>', '-']


def t_2dots(self, t):
    r"^:$"
    t.type = ':'
    return t


def t_comma(self, t):
    r"^,$"
    t.type = ','
    return t


def t_equal(self, t):
    r"^=$"
    t.type = '='
    return t


def t_semicol(self, t):
    r"^;$"
    t.type = ";"
    return t


def t_lcurly(self, t):
    r"^\{$"
    t.type = '{'  # Set token type to the expected literal
    return t


def t_rcurly(self, t):
    r"^\}$"
    t.type = '}'  # Set token type to the expected literal
    return t


def t_lbracket(self, t):
    r"^\[$"
    t.type = '['  # Set token type to the expected literal
    return t


def t_rbracket(self, t):
    r"^\]$"
    t.type = ']'  # Set token type to the expected literal
    return t


def t_lparenthesis(self, t):
    r"^\($"
    t.type = '('  # Set token type to the expected literal
    return t


def t_rparenthesis(self, t):
    r"^\)$"
    t.type = ')'  # Set token type to the expected literal
    return t


reserved = {
    'if': 'IF',
    'then': 'THEN',
    'else': 'ELSE',
    'stream': "STREAM",
    'type': "TYPE",
    'conn_kind_keyword': "CONN_KIND_KEYWORD",
    'conn_kind_keyword2': "CONN_KIND_KEYWORD2",
    'drop': "DROP",
    'autodrop': "AUTODROP",
    'infinite': "INFINITE",
    "blocking" : "BLOCKING",
    'forward': "FORWARD",
    "on": "ON",
    "done": "DONE",
    "nothing": "NOTHING",
    "yield": "YIELD",
    "switch": "SWITCH",
    "to": "TO",
    "where": "WHERE",
    "rule": "RULE",
    "set": "SET",
    "arbiter": "ARBITER",
    "monitor": "MONITOR",
    "event": "EVENT",
    "source": "SOURCE",
    "true": "TRUE",
    "false": "FALSE"
}

# Token names.
tokens = [
    # data types
    "BYTE", "BOOL", "INT", "STRING", "ID",
    # operators
    "OP", "BOOL_OP",
] + list(reserved.values())

# Regular expression rules for simple tokens
t_BYTE = r"[0|1]{8}b"
# t_BOOL = r"(true|false)"
t_STRING = r"\"[a-zA-Z0-9]*\""  # TODO: I am not completely sure what kind of string me want to accept.
# Maybe this is enough?
t_OP = r"(\+|\-|\*|\/|\^)"  # arithmetic operators
t_BOOL_OP = r"(and|or)"  # boolean operators


# A regular expression rule with some action code
# Note addition of self parameter since we're in a class
def t_INT(self, t):
    r'((\-?|\+?)([1-9][0-9]*)|0)'
    t.value = int(t.value)
    return t


def t_ID(self, t):
    r'[a-zA-Z_][a-zA-Z_0-9]*'
    # for variable names and keywords
    t.type = self.reserved.get(t.value, 'ID')  # Check for reserved words
    return t

# # Error handling rule
# def p_error(t):
#     print(f"Illegal character {t.value[0]}, {t.type} ")
#     t.lexer.skip(1)