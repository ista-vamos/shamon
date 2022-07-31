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