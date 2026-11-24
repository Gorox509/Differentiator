#include <assert.h>
#include <math.h>
#include "diff.hpp"
#include "Tree/tree.hpp"

#define VAR_TO_DIFF_BY 'x'

#define E_CONST 2.718281828

#define EPSILON 1e-6

static Tree *copy(Tree *node);
static int try_reduce_const_tree(Tree **tree, bool *has_changes);
static int try_reduce_neutral_tree(Tree **tree, bool *has_changes);
static int try_reduce_right_1   (Tree **tree, bool *has_changes);
static int try_reduce_left_1    (Tree **tree, bool *has_changes);
static int try_reduce_0         (Tree **tree, bool *has_changes);
static tree_elem_t apply_operation(int opcode, tree_elem_t val1, tree_elem_t val2);
static bool are_doubles_equal(double val1, double val2);

#define _C  copy(node)
#define _CL copy(LEFT(node))
#define _CR copy(RIGHT(node))
#define _DL diff(LEFT(node))
#define _DR diff(RIGHT(node))
#define _NUM(val) new_node(constant, (tree_elem_t){val}, NULL, NULL)
#define _ADD(l_tree, r_tree) new_node(operation, (tree_elem_t){.val_int = plus}, l_tree, r_tree)
#define _MULT(l_tree, r_tree)new_node(operation, (tree_elem_t){.val_int = mult}, l_tree, r_tree)
#define _SUB(l_tree, r_tree) new_node(operation, (tree_elem_t){.val_int = minus}, l_tree, r_tree)
#define _DIV(l_tree, r_tree) new_node(operation, (tree_elem_t){.val_int = divide}, l_tree, r_tree)
#define _LOG(l_tree, r_tree) new_node(operation, (tree_elem_t){.val_int = log_op}, l_tree, r_tree)


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
                case power: return _MULT(_C, _ADD(_MULT(_DIV(_CR, _CL), _DL), _MULT(_DR, _LOG(_CL, _NUM(E_CONST)))));
                case log_op:return _DIV(_SUB(_DIV(_MULT(_DL, _LOG(_CR, _NUM(E_CONST))), _CL), _DIV(_MULT(_LOG(_CL, _NUM(E_CONST)), _DR), _CR)), _MULT(_CR, _CR)); //TODO: broken

                default:    return NULL;
            }

        default: return NULL;
    }
}


tree_elem_t evaluate(Tree *tree, tree_elem_t var, bool *has_var) {
    assert(has_var != NULL);

    if (tree == NULL) return {NAN};

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


int reduce(Tree **tree) {
    assert(tree != NULL);

    bool const_changed = false, neutral_changed = false;

    do {
        const_changed = neutral_changed = false;

        try_reduce_const_tree(tree, &const_changed);
        try_reduce_neutral_tree(tree, &neutral_changed);

    } while (const_changed || neutral_changed);

    return TREE_OK;
}


int try_reduce_const_tree(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    assert(has_changes != NULL);

    if (*tree == NULL) return TREE_OK;

    bool left_not_const = true, right_not_const = true;
    tree_elem_t val_l = {}, val_r = {};

    if (val_l = evaluate(LEFT(*tree), {NAN}, &left_not_const),
        val_r = evaluate(RIGHT(*tree),{NAN}, &right_not_const),
        !left_not_const && !right_not_const)
    {
        DATA(*tree) = (tree_elem_t) {val_l.val_double + val_r.val_double}; // if evaluation can happen then val_l and val_r are some constants
        *has_changes = true;
        tree_free(LEFT(*tree));
        tree_free(RIGHT(*tree));
    }

    if (LEFT(*tree) != NULL) try_reduce_const_tree(&LEFT(*tree), has_changes);
    if (RIGHT(*tree) != NULL)try_reduce_const_tree(&RIGHT(*tree),has_changes);

    return TREE_OK;
}


int try_reduce_neutral_tree(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    if (*tree == NULL) return TREE_OK;

    if (TYPE(*tree) != operation) {
        return TREE_OK;
    }

    if (TYPE(LEFT(*tree)) == constant && TYPE(RIGHT(*tree)) == constant) try_reduce_const_tree(tree, has_changes);

    if (LEFT(*tree) != NULL && RIGHT(*tree) != NULL) {
        try_reduce_left_1(tree, has_changes);
        try_reduce_right_1(tree, has_changes);
        try_reduce_0(tree, has_changes);
    }

    if (LEFT(*tree) != NULL) try_reduce_neutral_tree(&LEFT(*tree), has_changes);
    if (RIGHT(*tree) != NULL)try_reduce_neutral_tree(&RIGHT(*tree),has_changes);

    return TREE_OK;
}


int try_reduce_left_1(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    assert(*tree != NULL);

    assert(LEFT(*tree) != NULL);
    assert(RIGHT(*tree) != NULL);

    if ((TYPE(LEFT(*tree)) == constant && TYPE(RIGHT(*tree)) != constant) &&
        (are_doubles_equal(DATA(LEFT(*tree)).val_double, 1)) &&
        ((DATA(*tree).val_int == mult) || DATA(*tree).val_int == divide))
    {
        Tree *old_tree = *tree;
        *tree = copy(RIGHT(*tree));
        tree_free(old_tree);
        *has_changes = true;
    }

    return TREE_OK;
}


int try_reduce_right_1(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    assert(*tree != NULL);

    assert(LEFT(*tree) != NULL);
    assert(RIGHT(*tree) != NULL);

    if ((TYPE(RIGHT(*tree)) == constant && TYPE(LEFT(*tree)) != constant) &&
        (are_doubles_equal(DATA(RIGHT(*tree)).val_double, 1)) &&
        ((DATA(*tree).val_int == mult) || DATA(*tree).val_int == divide))
    {
        Tree *old_tree = *tree;
        *tree = copy(LEFT(*tree));
        tree_free(old_tree);
        *has_changes = true;
    }

    return TREE_OK;
}


int try_reduce_0(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    assert(*tree != NULL);

    assert(LEFT(*tree) != NULL);
    assert(RIGHT(*tree) != NULL);

    if (((TYPE(LEFT(*tree)) == constant && are_doubles_equal(DATA(LEFT(*tree)).val_double, 0)) ||
        (TYPE(RIGHT(*tree)) == constant && are_doubles_equal(DATA(RIGHT(*tree)).val_double, 0))) &&
        (DATA(*tree).val_int == mult))
    {
        Tree *old_tree = *tree;
        *tree = _NUM(0);
        tree_free(old_tree);
        *has_changes = true;
    }
    else if ((TYPE(LEFT(*tree)) == constant && are_doubles_equal(DATA(LEFT(*tree)).val_double, 0)) &&
        (DATA(*tree).val_int == plus || DATA(*tree).val_int == minus))
    {
        Tree *old_tree = *tree;
        *tree = copy(RIGHT(*tree));
        tree_free(old_tree);
        *has_changes = true;
    }
    else if ((TYPE(RIGHT(*tree)) == constant && are_doubles_equal(DATA(RIGHT(*tree)).val_double, 0)) &&
        (DATA(*tree).val_int == plus || DATA(*tree).val_int == minus))
    {
        Tree *old_tree = *tree;
        *tree = copy(LEFT(*tree));
        tree_free(old_tree);
        *has_changes = true;
    }

    return TREE_OK;
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
        case power: return {pow(val1.val_double, val2.val_double)};
        case log_op:return {log(val1.val_double) / log(val2.val_double)};

        default:    return {NAN};
    }
}



bool are_doubles_equal(double val1, double val2) {
    if (fabs(val1 - val2) < EPSILON) return true;
    return false;
}
