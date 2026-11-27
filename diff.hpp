#ifndef DIFF_H
#define DIFF_H

#include "Tree/tree.hpp"

Tree *diff(Tree *node);
tree_elem_t evaluate(Tree *tree, tree_elem_t var, bool *has_var);
Tree *copy(Tree *node);
int reduce(Tree **tree);

tree_elem_t apply_operation(int opcode, tree_elem_t val1, tree_elem_t val2);


#define E_CONST 2.718281828

#define EPSILON 1e-6


#define _C  copy(node)
#define _CL copy(LEFT(node))
#define _CR copy(RIGHT(node))
#define _DL diff(LEFT(node))
#define _DR diff(RIGHT(node))
#define _NUM(val) new_node(constant, (tree_elem_t){val}, NULL, NULL)
#define _ADD(l_tree, r_tree) new_node(operation, (tree_elem_t){.op_code = plus}, l_tree, r_tree)
#define _MULT(l_tree, r_tree)new_node(operation, (tree_elem_t){.op_code = mult}, l_tree, r_tree)
#define _SUB(l_tree, r_tree) new_node(operation, (tree_elem_t){.op_code = minus}, l_tree, r_tree)
#define _DIV(l_tree, r_tree) new_node(operation, (tree_elem_t){.op_code = divide}, l_tree, r_tree)
#define _LOG(l_tree, r_tree) new_node(operation, (tree_elem_t){.op_code = log_op}, l_tree, r_tree)

#endif
