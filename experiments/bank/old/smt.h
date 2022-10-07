#include <z3.h>

Z3_context mk_context_custom(Z3_config cfg, Z3_error_handler err);
Z3_context mk_context();

Z3_solver mk_solver(Z3_context ctx);
void del_solver(Z3_context ctx, Z3_solver s);