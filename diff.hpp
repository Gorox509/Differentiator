#ifndef DIFF_H
#define DIFF_H

#include "Tree/tree.hpp"

Tree *diff(Tree *node);
tree_elem_t evaluate(Tree *tree, tree_elem_t var, bool *has_var);
int reduce(Tree **tree);

#endif
