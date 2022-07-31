def get_parameters_types_field_decl(tree, params):
    assert(len(tree) == 3)
    if tree[0] == 'list_field_decl':
        get_parameters_types_field_decl(tree[1], params)
        get_parameters_types_field_decl(tree[2], params)
    else:
        assert(tree[0] == 'field_decl')
        params.append(tree[2])
