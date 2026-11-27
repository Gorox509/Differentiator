#include "Tree/tree.hpp"
#include <string.h>
#include <assert.h>

static int do_program(int argc, char *argv[]);

#include "diff.hpp"

int main(int argc, char *argv[]) {
    do_program(argc, argv);

    return 0;
}


int do_program(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: <read_file> <write_file>\n");
        return 1;
    }

    Tree *tree = NULL;
    Tree *diff_tree = NULL;
    Tree *double_diff_tree = NULL;
    bool has_var = false;
    double x = 0;
    tree_read(argv[1], &tree);

    reduce(&tree);

    diff_tree = diff(tree);

    reduce(&diff_tree);

    double_diff_tree = diff(diff_tree);

    reduce(&double_diff_tree);

    fprintf(stdout, "What x is: ");
    scanf("%lg", &x);

    fprintf(stdout, "Second derivative at x=%lg: %lg\n", x, evaluate(double_diff_tree, (tree_elem_t){.const_val = x}, &has_var).const_val);

    tree_write(argv[2], double_diff_tree);

    tree_to_graphviz(double_diff_tree);
    tree_to_latex(double_diff_tree);

    tree_free(tree);
    tree_free(diff_tree);
    tree_free(double_diff_tree);

    return 0;
}


int data_type_free(tree_elem_t data) {
    //free(data);
    return TREE_OK;
}

int data_type_cmp(tree_elem_t val1, tree_elem_t val2) {
    //return -strcmp(val1, val2);
    //return val2 - val1;

    return 0;
}

tree_elem_t data_type_alloc() {
    //return (char *) malloc(sizeof(char) * MAX_STR_LEN);
    return {0};
}

int data_type_copy(tree_elem_t *dest, const tree_elem_t *src) {
    assert(dest != NULL);
    assert(src != NULL);

    //memcpy(*dest, *src, sizeof(char) * MAX_STR_LEN);

    *dest = *src;
    return 0;
}
