#include "Tree/tree.hpp"
#include "diff.hpp"
#include <assert.h>
#include <math.h>


static bool are_doubles_equal(double val1, double val2);
static int try_reduce_const_tree(Tree **tree, bool *has_changes);
static int try_reduce_neutral_tree(Tree **tree, bool *has_changes);
static int try_reduce_plus(Tree **tree, bool *has_changes);
static int try_reduce_minus(Tree **tree, bool *has_changes);
static int try_reduce_mult(Tree **tree, bool *has_changes);
static int try_reduce_divide(Tree **tree, bool *has_changes);
static int try_reduce_power(Tree **tree, bool *has_changes);
static int try_reduce_log(Tree **tree, bool *has_changes);


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
    if (TYPE(*tree) != operation) return TREE_OK;

    Tree *old_tree = *tree;

    bool left_not_const = false, right_not_const = false;
    tree_elem_t val_l = {}, val_r = {};

    if (val_l = evaluate(LEFT(*tree), {NAN}, &left_not_const), //TODO: to try_...
        val_r = evaluate(RIGHT(*tree),{NAN}, &right_not_const),
        !left_not_const && !right_not_const)
    {
        *tree = _NUM(apply_operation(DATA(*tree).op_code, val_l, val_r)); // if evaluation can happen then val_l and val_r are some constants
        *has_changes = true;
        tree_free(old_tree);
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
        switch (DATA(*tree).op_code) {
            case plus:
                try_reduce_plus(tree, has_changes);
                break;
            case minus:
                try_reduce_minus(tree, has_changes);
                break;
            case mult:
                try_reduce_mult(tree, has_changes);
                break;
            case divide:
                try_reduce_divide(tree, has_changes);
                break;
            case power:
                try_reduce_power(tree, has_changes);
                break;
            case log_op:
                try_reduce_log(tree, has_changes);
                break;

            case unknown: default: break;
        }
    }

    if (LEFT(*tree) != NULL) try_reduce_neutral_tree(&LEFT(*tree), has_changes);
    if (RIGHT(*tree) != NULL)try_reduce_neutral_tree(&RIGHT(*tree),has_changes);

    return TREE_OK;
}


int try_reduce_plus(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    assert(*tree != NULL);
    assert(LEFT(*tree) != NULL);
    assert(RIGHT(*tree) != NULL);
    assert(DATA(*tree).op_code == plus);

    Tree *old_tree = *tree;
    bool changed = false;

    if (TYPE(LEFT(*tree)) == constant &&
        are_doubles_equal(DATA(LEFT(*tree)).const_val, 0))
    {
        *tree = copy(RIGHT(*tree));
        changed = true;

    }
    else if (TYPE(RIGHT(*tree)) == constant &&
        are_doubles_equal(DATA(RIGHT(*tree)).const_val, 0))
    {
        *tree = copy(LEFT(*tree));
        changed = true;
    }

    if (changed) {
            tree_free(old_tree);
            *has_changes = true;
        }

    return TREE_OK;
}


int try_reduce_minus(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    assert(*tree != NULL);
    assert(LEFT(*tree) != NULL);
    assert(RIGHT(*tree) != NULL);
    assert(DATA(*tree).op_code == minus);

    Tree *old_tree = *tree;
    bool changed = false;

    if (TYPE(RIGHT(*tree)) == constant &&
        are_doubles_equal(DATA(RIGHT(*tree)).const_val, 0))
    {
        *tree = copy(LEFT(*tree));
        changed = true;
    }

    if (changed) {
        tree_free(old_tree);
        *has_changes = true;
    }

    return TREE_OK;
}


int try_reduce_mult(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    assert(*tree != NULL);
    assert(LEFT(*tree) != NULL);
    assert(RIGHT(*tree) != NULL);
    assert(DATA(*tree).op_code == mult);

    Tree *old_tree = *tree;
    bool changed = false;

    if (TYPE(LEFT(*tree)) == constant &&
        are_doubles_equal(DATA(LEFT(*tree)).const_val, 0))
    {
        *tree = _NUM(0);
        changed = true;
    }
    else if (TYPE(RIGHT(*tree)) == constant &&
        are_doubles_equal(DATA(RIGHT(*tree)).const_val, 0))
    {
        *tree = _NUM(0);
        changed = true;
    }

    else if (TYPE(RIGHT(*tree)) == constant &&
        are_doubles_equal(DATA(RIGHT(*tree)).const_val, 1))
    {
        *tree = copy(LEFT(*tree));
        changed = true;
    }
    else if (TYPE(LEFT(*tree)) == constant &&
        are_doubles_equal(DATA(LEFT(*tree)).const_val, 1))
    {
        *tree = copy(RIGHT(*tree));
        changed = true;
    }

    if (changed) {
        tree_free(old_tree);
        *has_changes = true;
    }

    return TREE_OK;
}


int try_reduce_divide(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    assert(*tree != NULL);
    assert(LEFT(*tree) != NULL);
    assert(RIGHT(*tree) != NULL);
    assert(DATA(*tree).op_code == divide);

    Tree *old_tree = *tree;
    bool changed = false;

    if (TYPE(LEFT(*tree)) == constant &&
        are_doubles_equal(DATA(LEFT(*tree)).const_val, 0))
    {
        *tree = _NUM(0);
        changed = true;
    }
    else if (TYPE(RIGHT(*tree)) == constant &&
        are_doubles_equal(DATA(RIGHT(*tree)).const_val, 1))
    {
        *tree = copy(LEFT(*tree));
        changed = true;
    }

    if (changed) {
        tree_free(old_tree);
        *has_changes = true;
    }

    return TREE_OK;
}


int try_reduce_power(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    assert(*tree != NULL);
    assert(LEFT(*tree) != NULL);
    assert(RIGHT(*tree) != NULL);
    assert(DATA(*tree).op_code == power);

    Tree *old_tree = *tree;
    bool changed = false;

    if (TYPE(RIGHT(*tree)) == constant &&
        are_doubles_equal(DATA(RIGHT(*tree)).const_val, 0))
    {
        *tree = _NUM(1);
        changed = true;
    }

    else if (TYPE(RIGHT(*tree)) == constant &&
        are_doubles_equal(DATA(RIGHT(*tree)).const_val, 1))
    {
        *tree = copy(LEFT(*tree));
        changed = true;
    }
    else if (TYPE(LEFT(*tree)) == constant &&
        are_doubles_equal(DATA(LEFT(*tree)).const_val, 1))
    {
        *tree = _NUM(1);
        changed = true;
    }
    else if (TYPE(LEFT(*tree)) == constant &&
        are_doubles_equal(DATA(LEFT(*tree)).const_val, 0))
    {
        *tree = _NUM(0);
        changed = true;
    }

    if (changed) {
        tree_free(old_tree);
        *has_changes = true;
    }

    return TREE_OK;
}


int try_reduce_log(Tree **tree, bool *has_changes) {
    assert(tree != NULL);
    assert(*tree != NULL);
    assert(LEFT(*tree) != NULL);
    assert(RIGHT(*tree) != NULL);
    assert(DATA(*tree).op_code == log_op);

    Tree *old_tree = *tree;
    bool changed = false;

    if (TYPE(RIGHT(*tree)) == constant &&
        are_doubles_equal(DATA(LEFT(*tree)).const_val, 1))
    {
        *tree = _NUM(0);
        changed = true;
    }

    if (changed) {
        tree_free(old_tree);
        *has_changes = true;
    }

    return TREE_OK;
}


bool are_doubles_equal(double val1, double val2) {
    if (fabs(val1 - val2) < EPSILON) return true;
    return false;
}
