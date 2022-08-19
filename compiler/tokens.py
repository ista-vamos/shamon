reserved = {
    'if': 'IF',
    'then': 'THEN',
    'else': 'ELSE',
    'stream':"STREAM",
    'type': "TYPE",
    'autodrop': "AUTODROP",
    'infinite': "INFINITE",
    "blocking" : "BLOCKING",
    'drop': "DROP",
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
    "from": "FROM",
    "round": "ROUND",
    "robin": "ROBIN",
    "extends": "EXTENDS",
    "creates": "CREATES",
    "at": "AT",
    "most": "MOST",
    "process": "PROCESS",
    "using": "USING",
    "dynamic": "DYNAMIC",
    "buffer" : "BUFFER",
    "group" : "GROUP",
    "match": "MATCH",
    "fun": "FUN",
    "choose": "CHOOSE",
    "by": "BY",
    "remove": "REMOVE",
    "globals" : "GLOBALS",
    "startup": "STARTUP",
    "cleanup": "CLEANUP"
}

# Token names.
tokens = [
    # data types
     "BOOL", "INT", "ID",
     # operators
     "OP", "BOOL_OP",
     # ccode
     "CCODE_TOKEN",
     "FIELD_ACCESS"
 ] + list(reserved.values())
