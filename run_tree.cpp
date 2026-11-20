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
    bool has_var = false;
    double x = 0;
    tree_read(argv[1], &tree);

    tree_print(stdout, tree); putchar('\n');

    diff_tree = diff(tree);

    tree_print(stderr, tree); putc('\n', stderr);
    tree_print(stderr, diff_tree); putc('\n', stderr);

    fprintf(stderr, "What x is: ");
    scanf("%lg", &x);

    fprintf(stderr, "%lg\n", evaluate(diff_tree, (tree_elem_t){.val_double = x}, &has_var).val_double);

    tree_write(argv[2], diff_tree);

    tree_to_graphviz(diff_tree);

    tree_free(tree);
    tree_free(diff_tree);

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
