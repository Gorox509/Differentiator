#include "tree.hpp"
#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>



static int make_buffer_for_reading(char **buf, const char *filename);
static int read_file_to_buffer(char *buf, int buf_len, FILE *fp);
static int tree_parse_node(char *buf, Tree **tree, int *parsed);
static int tree_parse_data(char *buf, Tree **tree, int *parsed);
static int str_to_tree_data(Tree **tree, const char str[], int str_len);
static enum operations get_operation_num(const char *str, int str_len);
static int get_var_num(const char *str, int str_len);
static int hash(const char *str, int str_len);
static bool is_all_digits(const char *str, int str_len);
static bool is_operation(const char *str, int str_len);



int tree_read(const char *filename, Tree **tree) {
    assert(filename != NULL);
    assert(tree != NULL);

    FILE *fp = safe_fopen(filename, "r");
    if (fp == NULL) return TREE_CANT_READ;

    char *buf = NULL;
    int buf_len = make_buffer_for_reading(&buf, filename);
    if (buf_len == TREE_CANT_READ) return TREE_CANT_READ;
    read_file_to_buffer(buf, buf_len, fp);

    fclose(fp);

    int parsed = 0;
    TREE_EXIT_ERR(tree_parse_node(buf, tree, &parsed));
    free(buf);

    return TREE_OK;
}


int tree_parse_node(char *buf, Tree **tree, int *parsed) {
    assert(buf != NULL);
    assert(tree != NULL);
    assert(parsed != NULL);

    if (buf[(*parsed)++] != '(') return TREE_READ_WRONG_SYNTAX;

    if (buf[*parsed] == ')') {
        ++(*parsed);
        return TREE_OK;
    }
    if (*tree == NULL) {
        TREE_EXIT_ERR(tree_make_node(tree));
    }

    ON_READ_DEBUG(fprintf(stderr, "scanning left...\n"));
    TREE_EXIT_ERR(tree_parse_node(buf, &LEFT(*tree), parsed));
    ON_READ_DEBUG(fprintf(stderr, "left scanned\n"));

    ON_READ_DEBUG(fprintf(stderr, "scanning data...: "));
    TREE_EXIT_ERR(tree_parse_data(buf, tree, parsed));
    ON_READ_DEBUG(fprintf(stderr, "data scanned\n"));

    ON_READ_DEBUG(fprintf(stderr, "scanning right...\n"));
    TREE_EXIT_ERR(tree_parse_node(buf, &RIGHT(*tree), parsed));
    ON_READ_DEBUG(fprintf(stderr, "right scanned\n"));

    if (buf[(*parsed)++] != ')') return TREE_READ_WRONG_SYNTAX;

    ON_READ_DEBUG(fprintf(stderr, "node successfully read\n"));

    return TREE_OK;
}


int tree_parse_data(char *buf, Tree **tree, int *parsed) {
    assert(buf != NULL);
    assert(tree != NULL);
    assert(parsed != NULL);

    int scanned = 0;
    char str[MAX_STR_LEN] = "";
    sscanf(buf + *parsed, "%[^()]%n", str, &scanned);
    *parsed += scanned;
    ON_READ_DEBUG(fprintf(stderr, "%s\n", str));

    TREE_EXIT_ERR(str_to_tree_data(tree, str, scanned));

    return TREE_OK;
}


int str_to_tree_data(Tree **tree, const char str[], int str_len) {
    assert(tree != NULL);
    assert(str != NULL);

    if (is_all_digits(str, str_len)) {
        TYPE(*tree) = constant;
        DATA(*tree).const_val = strtod(str, NULL); //TODO: to own function
    }
    else if (is_operation(str, str_len)) {
        TYPE(*tree) = operation;
        DATA(*tree).op_code = get_operation_num(str, str_len);
    }
    else {
        TYPE(*tree) = variable;
        DATA(*tree).var_code = get_var_num(str, str_len); //TODO: to parsing context
    }

    return TREE_OK;
}


bool is_all_digits(const char *str, int str_len) {
    assert(str != NULL);
    for (int i = 0; i < str_len; ++i) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}


bool is_operation(const char *str, int str_len) {
    assert(str != NULL);
    assert(str_len > 0);

    if (str[0] == '+' || str[0] == '-' || str[0] == '*' || str[0] == '/' || str[0] == '^' || !strncmp(str, "log", MAX_STR_LEN)) return true; //TODO: cringe

    return false;
}


enum operations get_operation_num(const char *str, int str_len) {
    assert(str != NULL);
    assert(str_len != 0);

    switch (str[0]) { //TODO: cringe pizdec
        case '+': return plus;
        case '-': return minus;
        case '*': return mult;
        case '/': return divide;
        case '^': return power;

        default: break;
    }
    if (!strncmp(str, "log", MAX_STR_LEN)) return log_op;

    return unknown;
}


int get_var_num(const char *str, int str_len) {
    assert(str != NULL);

    return hash(str, str_len);
}

int hash(const char *str, int str_len) {
    assert(str != NULL);

    int sum = 0;
    for (int i = 0; i < str_len; ++i) {
        sum += str[i];
    }
    return sum;
}


int make_buffer_for_reading(char **buf, const char *filename) {
    assert(buf != NULL);
    assert(filename != NULL);

    struct stat filestat = {};
    stat(filename, &filestat);

    *buf = (char *) calloc((size_t) filestat.st_size + 1, sizeof(char));
    if (*buf == NULL) {
        ON_READ_DEBUG(fprintf(stderr, "Error: buffer allocation failed"));
        return TREE_CANT_READ;
    }
    (*buf)[filestat.st_size] = '\0';
    return (int) filestat.st_size + 1;
}


int read_file_to_buffer(char *buf, int buf_len, FILE *fp) {
    assert(buf != NULL);
    assert(fp != NULL);

    fread(buf, (size_t) buf_len, 1, fp);

    return TREE_OK;
}
