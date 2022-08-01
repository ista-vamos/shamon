def get_parameters_types_field_decl(tree, params):
    assert(len(tree) == 3)
    if tree[0] == 'list_field_decl':
        get_parameters_types_field_decl(tree[1], params)
        get_parameters_types_field_decl(tree[2], params)
    else:
        assert(tree[0] == 'field_decl')
        params.append(tree[2])

def get_count_list_ids(tree):
    if tree[0] == 'listids':
        return 1 + get_count_list_ids(tree[2])
    else:
        assert(tree[0] == 'ID')
        return 1


def get_count_list_expr(tree):
    if tree[0] == 'expr_list':
        return 1 + get_count_list_expr(tree[2])
    else:
        assert(tree[0] == 'expr')
        return 1

def is_primitive_type(type_ : str):
    answer = type_ == "int" or type_ == "bool" or type_ =="string" or type_ == "float"
    answer = answer or type_ == "double"
    return answer

def is_type_primitive(tree):
    if tree[0] == 'type':
        return is_primitive_type(tree[1])
    else:
        assert(tree[0] == "array")
        return is_type_primitive(tree[1])

def are_all_events_decl_primitive(tree):
    if tree[0] == 'event_list':
        return are_all_events_decl_primitive(tree[1]) and are_all_events_decl_primitive(tree[2])
    else:
        assert(tree[0] == 'event_decl')
        params = []
        get_parameters_types_field_decl(tree[2], params)
        for param in params:
            if not is_type_primitive(param):
                return False
        return True

