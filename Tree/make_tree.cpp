#include "tree.hpp"
#include <assert.h>



int tree_make_node(Tree **node) {
    assert(node != NULL);

    Tree *new_node = (Tree *) malloc(sizeof(Tree));
    if (new_node == NULL) {
        return TREE_NODE_ALLOC_FAILED;
    }
    DATA(new_node) = data_type_alloc();
    //if (DATA(new_node) == NULL) {
    //    return TREE_NODE_ALLOC_FAILED;
    //}
    LEFT(new_node) = NULL;
    RIGHT(new_node)= NULL;

    *node = new_node;
    return TREE_OK;
}


int tree_free(Tree *tree) {
    if (tree == NULL) return TREE_OK;

    TREE_EXIT_ERR(tree_free(LEFT(tree)));
    data_type_free(DATA(tree));
    TREE_EXIT_ERR(tree_free(RIGHT(tree)));

    free(tree);

    return TREE_OK;
}


Tree *new_node(tree_elem_type_t type, tree_elem_t val, Tree *left, Tree *right) {
    Tree *node = NULL;
    tree_make_node(&node);

    node->type = type;
    node->val = val;
    node->left = left;
    node->right = right;

    return node;
}
