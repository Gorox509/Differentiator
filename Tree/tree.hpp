#ifndef TREE_H
#define TREE_H

#include <stdlib.h>
#include <stdio.h>

#define MAX_STR_LEN 256

enum tree_elem_type_t {
    operation,
    variable,
    constant,
};

enum operations : int {
    unknown,

    plus,
    minus,
    divide,
    mult,
};

union tree_elem_t {
    double val_double;
    int val_int;
};

typedef struct tree_node_t {
    enum tree_elem_type_t type;
    union tree_elem_t val;

    tree_node_t *left;
    tree_node_t *right;
} Tree;


#define DATA(tree)  (tree)->val
#define TYPE(tree)  (tree)->type
#define LEFT(tree)  (tree)->left
#define RIGHT(tree) (tree)->right


#define TREE_OK                 (+0)
#define TREE_NODE_ALLOC_FAILED  (-1)
#define TREE_CANT_READ          (-2)
#define TREE_READ_WRONG_SYNTAX  (-3)

#define TREE_GRAPHVIZ_OK                    (+0)
#define TREE_GRAPHVIZ_FAILED_TO_MAKE_DIR    (-1)
#define TREE_GRAPHVIZ_FAILED_TO_OPEN_FILE   (-2)


#define EXIT_ERR(...)  { int err = TREE_OK; if ((err = __VA_ARGS__) != TREE_OK) { tree_error_print(err); exit(err); } }
#define CHECK_ERR(...) { int err = TREE_OK; if ((err = __VA_ARGS__) != TREE_OK) return err; }


#ifdef READ_DEBUG
#define ON_READ_DEBUG(...) __VA_ARGS__
#else
#define ON_READ_DEBUG(...)
#endif

#ifdef TREE_DEBUG
#define ON_TREE_DEBUG(...) __VA_ARGS__
#else
#define ON_TREE_DEBUG(...)
#endif



int data_type_free(tree_elem_t);
int data_type_cmp(tree_elem_t, tree_elem_t);
tree_elem_t data_type_alloc();
int data_type_copy(tree_elem_t *, const tree_elem_t *);

FILE *safe_fopen(const char *filename, const char *mode);

void tree_error_print(int err);
int tree_make_node(Tree **node);
int tree_free(Tree *tree);
int tree_insert(Tree **tree, tree_elem_t val);
void tree_print(FILE *fp, Tree *tree);
void tree_write(const char *filename, Tree *tree);
int tree_read(const char *filename, Tree **tree);

int tree_to_graphviz(Tree *tree);

#endif
