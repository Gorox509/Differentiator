#include <assert.h>
#include <math.h>
#include "diff.hpp"
#include "Tree/tree.hpp"

#define VAR_TO_DIFF_BY 'x'


static Tree *copy(Tree *node);
static tree_elem_t apply_operation(int opcode, tree_elem_t val1, tree_elem_t val2);


#define _CL copy(LEFT(node))
#define _CR copy(RIGHT(node)) //TODO: C(self)
#define _DL diff(LEFT(node))
#define _DR diff(RIGHT(node))
#define _NUM(val) new_node(constant, (tree_elem_t){val}, NULL, NULL);
#define _ADD(l_tree, r_tree) new_node(operation, (tree_elem_t){.val_int = plus}, l_tree, r_tree)
#define _MULT(l_tree, r_tree)new_node(operation, (tree_elem_t){.val_int = mult}, l_tree, r_tree)
#define _SUB(l_tree, r_tree) new_node(operation, (tree_elem_t){.val_int = minus}, l_tree, r_tree)
#define _DIV(l_tree, r_tree) new_node(operation, (tree_elem_t){.val_int = divide}, l_tree, r_tree)


Tree *diff(Tree *node) {
    assert(node != NULL);

    switch (TYPE(node)) {
        case constant:
            return _NUM(0);

        case variable:
            if (DATA(node).val_int == VAR_TO_DIFF_BY) return _NUM(1);
            return _NUM(0);

        case operation:
            switch (DATA(node).val_int) {
                case plus:  return _ADD(_DL, _DR);
                case minus: return _SUB(_DL, _DR);
                case mult:  return _ADD(_MULT(_DL, _CR), _MULT(_CL, _DR));
                case divide:return _DIV(_SUB(_MULT(_DL, _CR), _MULT(_DR, _CL)), _MULT(_CR, _CR));

                default:    return NULL;
            }

        default: return NULL;
    }
}


tree_elem_t evaluate(Tree *tree, tree_elem_t var, bool *has_var) {
    assert(tree != NULL);
    assert(has_var != NULL);

    switch (TYPE(tree)) {
        case constant:
            return (tree_elem_t){DATA(tree).val_double};

        case variable:
            *has_var = true;
            return var; //TODO: only for x

        case operation: {
            bool l_has_var = false, r_has_var = false;
            tree_elem_t l_val = evaluate(LEFT(tree), var, &l_has_var);
            tree_elem_t r_val = evaluate(RIGHT(tree), var, &r_has_var);

            if (l_has_var || r_has_var) {
                *has_var = true;
            }
            return apply_operation(DATA(tree).val_int, l_val, r_val);
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
        case plus:  return {val1.val_double + val2.val_double};
        case minus: return {val1.val_double - val2.val_double};
        case mult:  return {val1.val_double * val2.val_double};
        case divide:return {val1.val_double / val2.val_double};

        default:    return {NAN};
    }
}
