#include "tree.hpp"
#include <assert.h>
#include <memory.h>

static int tree_insert_left(Tree *tree, tree_elem_t val);
static int tree_insert_right(Tree *tree, tree_elem_t val);

static void tree_print_data(FILE *fp, Tree *tree);
static void print_constant(FILE *fp, Tree *node);
static void print_operation(FILE *fp, Tree *node);
static void print_variable(FILE *fp, Tree *node);


void tree_error_print(int err) {
    switch (err) {
        case TREE_OK:
        default:
            return;

        case TREE_NODE_ALLOC_FAILED:
            fprintf(stderr, "Error: node allocation failed\n");
            return;

        case TREE_CANT_READ:
            fprintf(stderr, "Error: cant read tree from file\n");
            return;

        case TREE_READ_WRONG_SYNTAX:
            fprintf(stderr, "Error: wrong tree syntax\n");
            return;
    }
}


int tree_insert(Tree **tree, tree_elem_t val) {
    assert(tree != NULL);

    if (*tree == NULL) {
        EXIT_ERR(tree_make_node(tree));
        data_type_copy(&DATA(*tree), &val);
        return TREE_OK;
    }

    int cmp = data_type_cmp(DATA(*tree), val);

    if (LEFT(*tree) == NULL && cmp < 0)
        tree_insert_left(*tree, val);
    else if (RIGHT(*tree) == NULL && cmp >= 0)
        tree_insert_right(*tree, val);
    else if (cmp < 0)
        tree_insert(&LEFT(*tree), val);
    else
        tree_insert(&RIGHT(*tree), val);

    return TREE_OK;
}



int tree_insert_left(Tree *tree, tree_elem_t val) {
    assert(tree != NULL);

    tree_node_t *new_node = NULL;
    EXIT_ERR(tree_make_node(&new_node));
    data_type_copy(&DATA(new_node), &val);

    LEFT(tree) = new_node;

    return TREE_OK;
}


int tree_insert_right(Tree *tree, tree_elem_t val) {
    assert(tree != NULL);

    tree_node_t *new_node = NULL;
    EXIT_ERR(tree_make_node(&new_node));
    data_type_copy(&DATA(new_node), &val);

    RIGHT(tree) = new_node;

    return TREE_OK;
}


void tree_print(FILE *fp, Tree *tree) {
    assert(fp != NULL);
    //if (tree == NULL) return;

    fprintf(fp, "(");

    if (tree != NULL) {
        tree_print(fp, LEFT(tree));
        tree_print_data(fp, tree);
        tree_print(fp, RIGHT(tree));
    }

    fprintf(fp, ")");
}


void tree_print_data(FILE *fp, Tree *tree) {
    assert(fp != NULL);
    assert(tree != NULL);

    switch (TYPE(tree)) {
    case constant:
        print_constant(fp, tree);
        break;
    case operation:
        print_operation(fp, tree);
        break;
    case variable:
        print_variable(fp, tree);
        break;

    default: break;
    }
}

void print_constant(FILE *fp, Tree *node) {
    assert(fp != NULL);
    fprintf(fp, "%lg", DATA(node).val_double);
}

void print_operation(FILE *fp, Tree *node) {
    assert(fp != NULL);
    fprintf(fp, "%s", operation_to_str(node));
}

void print_variable(FILE *fp, Tree *node) {
    assert(fp != NULL);
    fprintf(fp, "x"); //TODO: cringe
}


void tree_write(const char *filename, Tree *tree) {
    assert(filename != NULL);

    FILE *fp = safe_fopen(filename, "w");
    if (fp == NULL) return;

    tree_print(fp, tree);
}


FILE *safe_fopen(const char *filename, const char *mode) {
    assert(filename != NULL);
    assert(mode != NULL);

    FILE *fp = fopen(filename, mode);
        if (fp == NULL) {
            ON_TREE_DEBUG(fprintf(stderr, "Error: cannot open the file\n"));
            return NULL;
        }
    return fp;
}


const char *type_to_str(Tree *node) {
    assert(node != NULL);

    switch (TYPE(node)) {
        case operation: return "OP";
        case variable:  return "VAR";
        case constant:  return "CONST";

        default: return "";
    }
}


const char *operation_to_str(Tree *node) {
    assert(node != NULL);
    assert(TYPE(node) == operation);

    switch (DATA(node).val_int) {
        case plus:  return "+";
        case minus: return "-";
        case mult:  return "*";
        case divide:return "/";
        case power: return "^";
        case log_op:return "log";

        default: return "";
    }
}
