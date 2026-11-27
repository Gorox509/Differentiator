#include <assert.h>
#include <math.h>
#include "diff.hpp"
#include "Tree/tree.hpp"

#define VAR_TO_DIFF_BY 'x'


Tree *diff(Tree *node) {
    assert(node != NULL);

    switch (TYPE(node)) {
        case constant:
            return _NUM(0);

        case variable:
            if (DATA(node).var_code == VAR_TO_DIFF_BY) return _NUM(1);
            return _NUM(0);

        case operation:
            switch (DATA(node).op_code) {
                case plus:  return _ADD(_DL, _DR);
                case minus: return _SUB(_DL, _DR);
                case mult:  return _ADD(_MULT(_DL, _CR), _MULT(_CL, _DR));
                case divide:return _DIV(_SUB(_MULT(_DL, _CR), _MULT(_DR, _CL)), _MULT(_CR, _CR));
                case power: return _MULT(_C, _ADD(_MULT(_DIV(_CR, _CL), _DL), _MULT(_DR, _LOG(_CL, _NUM(E_CONST)))));
                case log_op:return _DIV(_SUB(_DIV(_MULT(_DL, _LOG(_CR, _NUM(E_CONST))), _CL), _DIV(_MULT(_LOG(_CL, _NUM(E_CONST)), _DR), _CR)), _MULT(_CR, _CR)); //TODO: broken

                case unknown: default:  return NULL;
            }

        default: return NULL;
    }
}


tree_elem_t evaluate(Tree *tree, tree_elem_t var, bool *has_var) {
    assert(has_var != NULL);

    if (tree == NULL) return {NAN};

    switch (TYPE(tree)) {
        case constant:
            return (tree_elem_t){DATA(tree).const_val};

        case variable:
            *has_var = true;
            return var; //TODO: only for x now

        case operation: {
            bool l_has_var = false, r_has_var = false;
            tree_elem_t l_val = evaluate(LEFT(tree), var, &l_has_var);
            tree_elem_t r_val = evaluate(RIGHT(tree), var, &r_has_var);

            *has_var = l_has_var || r_has_var;
            return apply_operation(DATA(tree).op_code, l_val, r_val);
        }

        default:
            *has_var = true;
            return {0};
    }
}


Tree *copy(Tree *node) {
    if (node == NULL) return NULL;

    Tree *new_node = (Tree *) malloc(sizeof(Tree));
    if (new_node == NULL) {
        fprintf(stderr, "Error: allocation failed\n");
        return NULL;
    }

    new_node->type = node->type;
    new_node->val = node->val;

    if (node->left != NULL) new_node->left = copy(node->left);
    else new_node->left = NULL;

    if (node->right != NULL) new_node->right = copy(node->right);
    else new_node->right = NULL;

    return new_node;
}



tree_elem_t apply_operation(int opcode, tree_elem_t val1, tree_elem_t val2) {
    switch (opcode) {
        case plus:  return {val1.const_val + val2.const_val};
        case minus: return {val1.const_val - val2.const_val};
        case mult:  return {val1.const_val * val2.const_val};
        case divide:return {val1.const_val / val2.const_val};
        case power: return {pow(val1.const_val, val2.const_val)};
        case log_op:return {log(val1.const_val) / log(val2.const_val)};

        default:    return {NAN};
    }
}
