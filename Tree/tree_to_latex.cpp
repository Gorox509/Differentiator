#include "tree.hpp"
#include <assert.h>


#define LATEX_DIR "latex/"
#define LATEX_FILE "tree_dump.tex"


static int tree_latex_print(FILE *fp, Tree *tree);
static int print_operation_in_interposition(FILE *fp, Tree *tree, const char *op_str);
static int print_operation_in_preposition(FILE *fp, Tree *tree, const char *op_str);
static int make_latex_file(const char *filename);
static FILE *open_latex_file(const char *filename);



int tree_to_latex(Tree *tree) {
    if (tree == NULL) return TREE_OK;

    make_latex_file(LATEX_DIR LATEX_FILE);

    FILE *fp = open_latex_file(LATEX_DIR LATEX_FILE);
    if (fp == NULL) return TREE_LATEX_DUMP_CANNOT_OPEN_FILE;

    fprintf(fp, "\\documentclass{article}\n");
    fprintf(fp, "\\begin{document}\n$");
    TREE_EXIT_ERR(tree_latex_print(fp, tree));
    fprintf(fp, "$\n\\end{document}\n");

    fclose(fp);

    system("cd " LATEX_DIR " && ""pdflatex " LATEX_FILE " &> /dev/null ");

    return TREE_LATEX_DUMP_OK;
}


int tree_latex_print(FILE *fp, Tree *tree) {
    assert(fp != NULL);
    assert(tree != NULL);

    switch (TYPE(tree)) {
        case constant:
            fprintf(fp, "%lg", DATA(tree).const_val);
            break;

        case variable:
            fprintf(fp, "%c", DATA(tree).var_code);
            break;

        case operation:
            switch (DATA(tree).op_code) {
                case plus:
                    print_operation_in_interposition(fp, tree, "+");
                    break;
                case minus:
                    print_operation_in_interposition(fp, tree, "-");
                    break;
                case mult:
                    print_operation_in_interposition(fp, tree, "\\cdot");
                    break;
                case divide:
                    print_operation_in_preposition(fp, tree, "\\frac");
                    break;
                case power:
                    print_operation_in_interposition(fp, tree, "^");
                    break;
                case log_op:
                    print_operation_in_preposition(fp, tree, "\\log"); // TODO: idk
                    break;

                case unknown: default: break;
            }
            break;

        default: return TREE_LATEX_DUMP_UNKNOWN_OPERATION;
    }
    return TREE_LATEX_DUMP_OK;
}


int print_operation_in_interposition(FILE *fp, Tree *tree, const char *op_str) {
    assert(fp != NULL);
    assert(tree != NULL);

    fputc('{', fp);
    fprintf(fp, "{\\left(");
    TREE_EXIT_ERR(tree_latex_print(fp, LEFT(tree)));
    fprintf(fp, "\\right)}");
    fprintf(fp, "%s", op_str);
    fprintf(fp, "{\\left(");
    TREE_EXIT_ERR(tree_latex_print(fp, RIGHT(tree)));
    fprintf(fp, "\\right)}");
    fputc('}', fp);

    return TREE_OK;
}


int print_operation_in_preposition(FILE *fp, Tree *tree, const char *op_str) {
    assert(fp != NULL);
    assert(tree != NULL);

    fputc('{', fp);
    fprintf(fp, "%s", op_str);
    fprintf(fp, "{\\left(");
    TREE_EXIT_ERR(tree_latex_print(fp, LEFT(tree)));
    fprintf(fp, "\\right)}");
    fprintf(fp, "{\\left(");
    TREE_EXIT_ERR(tree_latex_print(fp, RIGHT(tree)));
    fprintf(fp, "\\right)}");
    fputc('}', fp);

    return TREE_OK;
}


int make_latex_file(const char *filename) {
    assert(filename != NULL);
    int err = TREE_LATEX_DUMP_OK;

    if (system("mkdir -p " LATEX_DIR)) {
        ON_TREE_DEBUG(fprintf(stderr, "Warning: directory already exists or cannot be created\n"));
        err = TREE_LATEX_DUMP_CANNOT_OPEN_FILE;
    }

    return err;
}


FILE *open_latex_file(const char *filename) {
    assert(filename != NULL);

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: failed to open log file\n");
        return NULL;
    }

    return fp;
}
