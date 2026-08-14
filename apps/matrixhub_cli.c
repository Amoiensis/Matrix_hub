/*
 * Matrix Hub script mode.
 *
 * This file adds a small MATLAB-like runner without changing the core
 * matrix API. It intentionally supports a compact grammar for demos and
 * teaching: matrix literals, variables, basic arithmetic, selected matrix
 * functions, and optional debug snapshots.
 */

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#endif

#include "matrix.h"

#define MH_MAX_NAME 64
#define MH_MAX_VARS 128
#define MH_MAX_ARGS 8
#define MH_MAX_LINE 2048
#define MH_HISTORY_SIZE 64
#define MH_ERR_SIZE 256
#define MH_MAX_RANGE_ITEMS 100000

typedef enum {
    TOK_EOF = 0,
    TOK_EOL,
    TOK_IDENT,
    TOK_NUMBER,
    TOK_ASSIGN,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_TRANSPOSE,
    TOK_EQ,
    TOK_NE,
    TOK_LT,
    TOK_LE,
    TOK_GT,
    TOK_GE,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_COMMA,
    TOK_COLON,
    TOK_SEMICOLON,
    TOK_INVALID
} TokenType;

typedef struct {
    TokenType type;
    MATRIX_TYPE number;
    char text[MH_MAX_NAME];
    int line;
    int column;
    size_t start;
    size_t end;
} Token;

typedef struct {
    const char *src;
    size_t pos;
    int line;
    int column;
} Lexer;

typedef enum {
    VAL_NONE = 0,
    VAL_SCALAR,
    VAL_MATRIX
} ValueType;

/* Value owns VAL_MATRIX pointers. Use value_clone() when reading from Env and
 * value_free() on every temporary path to keep Matrix ownership explicit. */
typedef struct {
    ValueType type;
    MATRIX_TYPE scalar;
    Matrix *matrix;
} Value;

typedef struct {
    char name[MH_MAX_NAME];
    Value value;
} Variable;

typedef struct {
    Variable vars[MH_MAX_VARS];
    int count;
    FILE *debug;
} Env;

typedef struct {
    char *items[MH_HISTORY_SIZE];
    int count;
} ReplHistory;

typedef struct {
    Lexer lexer;
    Token current;
    Env *env;
    int had_error;
    char error[MH_ERR_SIZE];
} Parser;

static Value parse_expression(Parser *parser);
static int run_script(const char *source, Env *env);
static int parse_statement(Parser *parser);

static Value value_none(void) {
    Value value;
    value.type = VAL_NONE;
    value.scalar = 0;
    value.matrix = NULL;
    return value;
}

static Value value_scalar(MATRIX_TYPE scalar) {
    Value value = value_none();
    value.type = VAL_SCALAR;
    value.scalar = scalar;
    return value;
}

static Value value_matrix(Matrix *matrix) {
    Value value = value_none();
    value.type = VAL_MATRIX;
    value.matrix = matrix;
    return value;
}

static void value_free(Value *value) {
    if ((value != NULL) && (value->type == VAL_MATRIX) && (value->matrix != NULL)) {
        M_free(value->matrix);
    }
    if (value != NULL) {
        *value = value_none();
    }
}

static Value value_clone(Value value) {
    if (value.type == VAL_MATRIX) {
        return value_matrix(Matrix_copy(value.matrix));
    }
    if (value.type == VAL_SCALAR) {
        return value_scalar(value.scalar);
    }
    return value_none();
}

static int is_identifier_start(int ch) {
    return isalpha(ch) || (ch == '_');
}

static int is_identifier_part(int ch) {
    return isalnum(ch) || (ch == '_');
}

static void lexer_init(Lexer *lexer, const char *src) {
    lexer->src = src;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->column = 1;
}

static int lexer_peek_char(Lexer *lexer) {
    return lexer->src[lexer->pos];
}

static int lexer_peek_next_char(Lexer *lexer) {
    int ch = lexer->src[lexer->pos];
    if (ch == '\0') {
        return '\0';
    }
    return lexer->src[lexer->pos + 1];
}

static int lexer_advance_char(Lexer *lexer) {
    int ch = lexer->src[lexer->pos];
    if (ch == '\0') {
        return ch;
    }
    lexer->pos++;
    if (ch == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    return ch;
}

static Token make_token(TokenType type, Lexer *lexer) {
    Token token;
    token.type = type;
    token.number = 0;
    token.text[0] = '\0';
    token.line = lexer->line;
    token.column = lexer->column;
    token.start = lexer->pos;
    token.end = lexer->pos;
    return token;
}

static Token lexer_next_token(Lexer *lexer) {
    for (;;) {
        int ch = lexer_peek_char(lexer);
        if ((ch == ' ') || (ch == '\t') || (ch == '\r')) {
            lexer_advance_char(lexer);
            continue;
        }
        if (ch == '%') {
            while ((lexer_peek_char(lexer) != '\0') && (lexer_peek_char(lexer) != '\n')) {
                lexer_advance_char(lexer);
            }
            continue;
        }
        break;
    }

    int ch = lexer_peek_char(lexer);
    Token token = make_token(TOK_INVALID, lexer);
    if (ch == '\0') {
        token.type = TOK_EOF;
        return token;
    }
    if (ch == '\n') {
        lexer_advance_char(lexer);
        token.type = TOK_EOL;
        return token;
    }
    if (is_identifier_start(ch)) {
        size_t start = lexer->pos;
        int start_column = lexer->column;
        while (is_identifier_part(lexer_peek_char(lexer))) {
            lexer_advance_char(lexer);
        }
        size_t len = lexer->pos - start;
        if (len >= sizeof(token.text)) {
            len = sizeof(token.text) - 1;
        }
        memcpy(token.text, lexer->src + start, len);
        token.text[len] = '\0';
        token.type = TOK_IDENT;
        token.column = start_column;
        token.start = start;
        token.end = lexer->pos;
        return token;
    }
    if (isdigit(ch) || ((ch == '.') && isdigit(lexer_peek_next_char(lexer)))) {
        char *end_ptr = NULL;
        size_t start = lexer->pos;
        int start_column = lexer->column;
        errno = 0;
        token.number = (MATRIX_TYPE)strtod(lexer->src + lexer->pos, &end_ptr);
        if ((end_ptr == lexer->src + lexer->pos) || (errno == ERANGE)) {
            token.type = TOK_INVALID;
            return token;
        }
        while (lexer->src + lexer->pos < end_ptr) {
            lexer_advance_char(lexer);
        }
        size_t len = lexer->pos - start;
        if (len >= sizeof(token.text)) {
            len = sizeof(token.text) - 1;
        }
        memcpy(token.text, lexer->src + start, len);
        token.text[len] = '\0';
        token.type = TOK_NUMBER;
        token.column = start_column;
        token.start = start;
        token.end = lexer->pos;
        return token;
    }

    lexer_advance_char(lexer);
    switch (ch) {
        case '=':
            if (lexer_peek_char(lexer) == '=') {
                lexer_advance_char(lexer);
                token.type = TOK_EQ;
            } else {
                token.type = TOK_ASSIGN;
            }
            break;
        case '!':
            if (lexer_peek_char(lexer) == '=') {
                lexer_advance_char(lexer);
                token.type = TOK_NE;
            } else {
                token.text[0] = (char)ch;
                token.text[1] = '\0';
                token.type = TOK_INVALID;
            }
            break;
        case '<':
            if (lexer_peek_char(lexer) == '=') {
                lexer_advance_char(lexer);
                token.type = TOK_LE;
            } else {
                token.type = TOK_LT;
            }
            break;
        case '>':
            if (lexer_peek_char(lexer) == '=') {
                lexer_advance_char(lexer);
                token.type = TOK_GE;
            } else {
                token.type = TOK_GT;
            }
            break;
        case '+': token.type = TOK_PLUS; break;
        case '-': token.type = TOK_MINUS; break;
        case '*': token.type = TOK_STAR; break;
        case '/': token.type = TOK_SLASH; break;
        case '\'': token.type = TOK_TRANSPOSE; break;
        case '(': token.type = TOK_LPAREN; break;
        case ')': token.type = TOK_RPAREN; break;
        case '[': token.type = TOK_LBRACKET; break;
        case ']': token.type = TOK_RBRACKET; break;
        case ',': token.type = TOK_COMMA; break;
        case ':': token.type = TOK_COLON; break;
        case ';': token.type = TOK_SEMICOLON; break;
        default:
            token.text[0] = (char)ch;
            token.text[1] = '\0';
            token.type = TOK_INVALID;
            break;
    }
    token.end = lexer->pos;
    return token;
}

static void parser_set_error(Parser *parser, const char *message) {
    if (!parser->had_error) {
        snprintf(parser->error, sizeof(parser->error), "line %d:%d: %s",
                 parser->current.line, parser->current.column, message);
        parser->had_error = 1;
    }
}

static void parser_advance(Parser *parser) {
    parser->current = lexer_next_token(&parser->lexer);
}

static Token parser_peek(Parser *parser) {
    Lexer copy = parser->lexer;
    return lexer_next_token(&copy);
}

static int parser_match(Parser *parser, TokenType type) {
    if (parser->current.type == type) {
        parser_advance(parser);
        return 1;
    }
    return 0;
}

static int parser_expect(Parser *parser, TokenType type, const char *message) {
    if (parser_match(parser, type)) {
        return 1;
    }
    parser_set_error(parser, message);
    return 0;
}

static void skip_statement_separators(Parser *parser) {
    while ((parser->current.type == TOK_EOL) || (parser->current.type == TOK_SEMICOLON)) {
        parser_advance(parser);
    }
}

static int env_find(Env *env, const char *name) {
    int i;
    for (i = 0; i < env->count; i++) {
        if (strcmp(env->vars[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int builtin_constant(const char *name, MATRIX_TYPE *value) {
    if (strcmp(name, "INF") == 0) {
        *value = INF;
        return 1;
    }
    if (strcmp(name, "FRO") == 0) {
        *value = FRO;
        return 1;
    }
    if (strcmp(name, "ROW") == 0) {
        *value = _ROW_;
        return 1;
    }
    if (strcmp(name, "COLUMN") == 0) {
        *value = _COLUMN_;
        return 1;
    }
    if (strcmp(name, "END") == 0) {
        *value = _END_;
        return 1;
    }
    if (strcmp(name, "HEAD") == 0) {
        *value = _HEAD_;
        return 1;
    }
    if (strcmp(name, "SVD") == 0) {
        *value = _SVD_;
        return 1;
    }
    if (strcmp(name, "INV_L") == 0) {
        *value = _INV_L_;
        return 1;
    }
    if (strcmp(name, "INV_R") == 0) {
        *value = _INV_R_;
        return 1;
    }
    if (strcmp(name, "MUL") == 0) {
        *value = _MUL_;
        return 1;
    }
    if (strcmp(name, "DIV") == 0) {
        *value = _DIV_;
        return 1;
    }
    if (strcmp(name, "AND") == 0) {
        *value = _AND_;
        return 1;
    }
    if (strcmp(name, "OR") == 0) {
        *value = _OR_;
        return 1;
    }
    if (strcmp(name, "NOT") == 0) {
        *value = _NOT_;
        return 1;
    }
    return 0;
}

static Value env_get(Env *env, const char *name, Parser *parser) {
    MATRIX_TYPE constant_value = 0;
    if (builtin_constant(name, &constant_value)) {
        return value_scalar(constant_value);
    }
    int index = env_find(env, name);
    if (index < 0) {
        char message[MH_ERR_SIZE];
        snprintf(message, sizeof(message), "unknown variable or function '%s'", name);
        parser_set_error(parser, message);
        return value_none();
    }
    return value_clone(env->vars[index].value);
}

static int env_set(Env *env, const char *name, Value value, Parser *parser) {
    int index = env_find(env, name);
    if (index < 0) {
        if (env->count >= MH_MAX_VARS) {
            parser_set_error(parser, "too many variables");
            value_free(&value);
            return 0;
        }
        index = env->count++;
        snprintf(env->vars[index].name, sizeof(env->vars[index].name), "%s", name);
        env->vars[index].value = value_none();
    }
    value_free(&env->vars[index].value);
    env->vars[index].value = value;
    return 1;
}

static void env_clear(Env *env) {
    int i;
    for (i = 0; i < env->count; i++) {
        value_free(&env->vars[i].value);
        env->vars[i].name[0] = '\0';
    }
    env->count = 0;
}

static int env_delete(Env *env, const char *name) {
    int index = env_find(env, name);
    int i;
    if (index < 0) {
        return 0;
    }
    value_free(&env->vars[index].value);
    for (i = index; i + 1 < env->count; i++) {
        env->vars[i] = env->vars[i + 1];
    }
    env->count--;
    if (env->count >= 0) {
        env->vars[env->count].name[0] = '\0';
        env->vars[env->count].value = value_none();
    }
    return 1;
}

static void env_print_variables(Env *env) {
    int i;
    if (env->count <= 0) {
        printf("No variables.\n");
        return;
    }
    printf("%-20s %-8s %s\n", "Name", "Type", "Size");
    printf("%-20s %-8s %s\n", "----", "----", "----");
    for (i = 0; i < env->count; i++) {
        Value value = env->vars[i].value;
        if ((value.type == VAL_MATRIX) && (value.matrix != NULL)) {
            printf("%-20s %-8s %dx%d\n", env->vars[i].name, "matrix",
                   value.matrix->row, value.matrix->column);
        } else if (value.type == VAL_SCALAR) {
            printf("%-20s %-8s 1x1\n", env->vars[i].name, "scalar");
        } else {
            printf("%-20s %-8s -\n", env->vars[i].name, "none");
        }
    }
}

static void clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

static int topic_is_script_help(const char *topic) {
    return (topic == NULL) || (topic[0] == '\0') ||
           (strcmp(topic, "script") == 0) ||
           (strcmp(topic, "script_mode") == 0) ||
           (strcmp(topic, "functions") == 0);
}

static int topic_has_txt_suffix(const char *topic) {
    size_t len = strlen(topic);
    return (len >= 4) && (strcmp(topic + len - 4, ".txt") == 0);
}

static int print_help_path(const char *path) {
    FILE *fp = fopen(path, "rb");
    char buffer[512];
    size_t nread;
    int last_char = '\n';
    if (fp == NULL) {
        return 0;
    }
    while ((nread = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        fwrite(buffer, 1, nread, stdout);
        last_char = buffer[nread - 1];
    }
    fclose(fp);
    if (last_char != '\n') {
        printf("\n");
    }
    return 1;
}

static int try_print_help_file(const char *dir, const char *topic) {
    char path[512];
    int written;

    written = snprintf(path, sizeof(path), "%s/%s", dir, topic);
    if ((written > 0) && ((size_t)written < sizeof(path)) && print_help_path(path)) {
        return 1;
    }

    if (!topic_has_txt_suffix(topic)) {
        written = snprintf(path, sizeof(path), "%s/%s.txt", dir, topic);
        if ((written > 0) && ((size_t)written < sizeof(path)) && print_help_path(path)) {
            return 1;
        }
    }
    return 0;
}

static int print_help_file_by_topic(const char *topic) {
    /* Keep command/function help in text files so topics can be updated
     * without touching the parser. */
    const char *dirs[] = {
        "help",
        "../help",
        "Matrix_hub/help"
    };
    int i;

    for (i = 0; i < (int)(sizeof(dirs) / sizeof(dirs[0])); i++) {
        if (try_print_help_file(dirs[i], topic)) {
            return 1;
        }
    }
    return 0;
}

typedef struct {
    const char *topic;
    const char *file_topic;
} HelpAlias;

static const char *help_alias_topic(const char *topic) {
    static const HelpAlias aliases[] = {
        {"help", "script_commands"}, {"vars", "script_commands"}, {"who", "script_commands"},
        {"clear", "script_commands"}, {"clean", "script_commands"}, {"del", "script_commands"},
        {"quit", "script_commands"}, {"exit", "script_commands"},
        {"solver", "solver_mode"}, {"lp", "solver_mode"}, {"mip", "solver_mode"},
        {"library", "c_library"}, {"c_library", "c_library"}, {"plugin", "c_library"},
        {"for", "script_functions"}, {"if", "script_functions"}, {"elif", "script_functions"},
        {"else", "script_functions"}, {"slice", "script_functions"}, {"slicing", "script_functions"},
        {"range", "script_functions"},
        {"shape", "script_functions"}, {"size", "script_functions"}, {"diag", "script_functions"},
        {"qr", "script_functions"}, {"svd", "script_functions"}, {"eigen", "script_functions"},
        {"eig", "script_functions"}, {"eigen_max", "script_functions"}, {"complex", "script_functions"},
        {"print", "M_print"}, {"eye", "M_I"}, {"zeros", "Matrix_gen"}, {"ones", "M_Ones"},
        {"hilbert", "Hilbert"}, {"inv", "M_Inverse"}, {"inverse", "M_Inverse"},
        {"pinv", "M_pinv"}, {"transpose", "M_T"}, {"trans", "M_T"}, {"det", "M_det"},
        {"tr", "M_tr"}, {"trace", "M_tr"}, {"norm", "M_norm"}, {"cond", "M_cond"},
        {"rank", "M_rank"}, {"abs", "M_abs"}, {"sum", "M_sum"}, {"min", "M_min"},
        {"max", "M_max"}, {"cut", "M_Cut"}, {"full", "M_full"}, {"find", "M_find"},
        {"logic_equal", "M_logic_equal"}, {"pmul", "M_pmuldiv"}, {"pdiv", "M_pmuldiv"},
        {"logic", "M_logic"}, {"numul_m", "M_numul_m"}, {"swap", "M_Swap"},
        {"sample", "M_Sample"}, {"matfull", "M_matFull"}, {"setval", "M_setval"},
        {"uptri", "M_Uptri_"}, {"uptri_trans", "M_Uptri_"}, {"lowtri", "M_Lowtri_"},
        {"lowtri_trans", "M_Lowtri_"}, {"diatri", "M_Diatri_"}, {"dia_inv", "M_Dia_Inv"},
        {"householder", "householder"}, {"hessenberg", "M_householder"},
        {"qr_q", "M_QR"}, {"qr_r", "M_QR"}, {"svd_u", "M_SVD"}, {"svd_s", "M_SVD"},
        {"svd_v", "M_SVD"}, {"eigen_val", "M_eigen"}, {"eigen_vec", "M_eigen"},
        {"eigen_max_val", "M_eigen_max"}, {"eigen_max_vec", "M_eigen_max"}
    };
    int i;

    for (i = 0; i < (int)(sizeof(aliases) / sizeof(aliases[0])); i++) {
        if (strcmp(aliases[i].topic, topic) == 0) {
            return aliases[i].file_topic;
        }
    }
    return NULL;
}

static void print_script_help_fallback(void) {
    printf("MatrixHub script help\n");
    printf("Commands: help [topic], help(topic), vars, who, clear, clean, del <name>, quit, exit\n");
    printf("Functions: print, eye, zeros, ones, hilbert, inv, pinv, transpose, det,\n");
    printf("           tr, norm, cond, rank, shape, size, abs, sum, min, max, diag,\n");
    printf("           cut, full, find, logic_equal, pmul, pdiv, logic, swap, sample,\n");
    printf("           uptri, lowtri, diatri, dia_inv, householder, hessenberg,\n");
    printf("           qr, svd, eigen, eig, eigen_max, eigen_val, eigen_vec,\n");
    printf("           eigen_max_val, eigen_max_vec, qr_q, qr_r, svd_u, svd_s,\n");
    printf("           svd_v\n");
}

static void print_help_topic(const char *topic) {
    const char *help_topic = NULL;
    int used_alias = 0;

    if (topic_is_script_help(topic)) {
        help_topic = "script_mode";
    } else {
        help_topic = help_alias_topic(topic);
        if (help_topic != NULL) {
            used_alias = 1;
        } else {
            help_topic = topic;
        }
    }

    if (print_help_file_by_topic(help_topic)) {
        return;
    }
    if (used_alias && print_help_file_by_topic(topic)) {
        return;
    }
    if (!topic_is_script_help(topic)) {
        printf("[WARNING] help topic '%s' was not found.\n\n", topic);
    }
    print_script_help_fallback();
}

static int finish_command_statement(Parser *parser) {
    int consumed_semicolon = parser_match(parser, TOK_SEMICOLON);
    if (consumed_semicolon) {
        skip_statement_separators(parser);
        return parser->current.type != TOK_EOF;
    }
    if ((parser->current.type != TOK_EOL) && (parser->current.type != TOK_EOF) &&
        (parser->current.type != TOK_SEMICOLON)) {
        parser_set_error(parser, "expected statement separator");
        return 0;
    }
    skip_statement_separators(parser);
    return parser->current.type != TOK_EOF;
}

static void matrix_print_file(FILE *out, Matrix *matrix) {
    int i, j;
    if (matrix == NULL) {
        fprintf(out, "(null)\n");
        return;
    }
    fprintf(out, "[");
    for (i = 0; i < matrix->row; i++) {
        if (i > 0) {
            fprintf(out, " ");
        }
        for (j = 0; j < matrix->column; j++) {
            fprintf(out, "%.10g", matrix->data[i * matrix->column + j]);
            if (j + 1 < matrix->column) {
                fprintf(out, " ");
            }
        }
        if (i + 1 < matrix->row) {
            fprintf(out, ";\n");
        }
    }
    fprintf(out, "]\n");
}

static void value_print(FILE *out, const char *name, Value value) {
    if ((name != NULL) && (name[0] != '\0')) {
        fprintf(out, "%s = ", name);
    }
    if (value.type == VAL_SCALAR) {
        fprintf(out, "%.10g\n", value.scalar);
    } else if (value.type == VAL_MATRIX) {
        if ((name != NULL) && (name[0] != '\0')) {
            fprintf(out, "\n");
        }
        matrix_print_file(out, value.matrix);
    }
}

static void debug_assignment(Env *env, const char *name, Value value) {
    if (env->debug == NULL) {
        return;
    }
    fprintf(env->debug, "# %s\n", name);
    value_print(env->debug, name, value);
    fprintf(env->debug, "\n");
    fflush(env->debug);
}

static Matrix *matrix_diag_from_vector(Matrix *vector) {
    int i, length;
    Matrix *result;
    if (vector == NULL) {
        return NULL;
    }
    if ((vector->row != 1) && (vector->column != 1)) {
        return NULL;
    }
    length = (vector->row > vector->column) ? vector->row : vector->column;
    result = M_Zeros(length, length);
    if (result == NULL) {
        return NULL;
    }
    for (i = 0; i < length; i++) {
        result->data[i * length + i] = vector->data[i];
    }
    return result;
}

static Matrix *matrix_shape(Matrix *matrix) {
    MATRIX_TYPE data[2];
    if (matrix == NULL) {
        return NULL;
    }
    data[0] = matrix->row;
    data[1] = matrix->column;
    return Matrix_gen(1, 2, data);
}

static int value_to_int(Value value, int *out) {
    if (value.type != VAL_SCALAR) {
        return 0;
    }
    *out = (int)value.scalar;
    return fabs(value.scalar - (MATRIX_TYPE)(*out)) <= _APPROXIMATELY_ZERO_;
}

static int require_arg_count(Parser *parser, const char *name, int actual, int expected) {
    if (actual != expected) {
        char message[MH_ERR_SIZE];
        snprintf(message, sizeof(message), "%s expects %d argument(s)", name, expected);
        parser_set_error(parser, message);
        return 0;
    }
    return 1;
}

static int require_matrix_arg(Parser *parser, const char *name, Value value) {
    if (value.type != VAL_MATRIX) {
        char message[MH_ERR_SIZE];
        snprintf(message, sizeof(message), "%s expects a matrix argument", name);
        parser_set_error(parser, message);
        return 0;
    }
    return 1;
}

static int require_scalar_arg(Parser *parser, const char *name, Value value) {
    if (value.type != VAL_SCALAR) {
        char message[MH_ERR_SIZE];
        snprintf(message, sizeof(message), "%s expects a scalar argument", name);
        parser_set_error(parser, message);
        return 0;
    }
    return 1;
}

static void free_args(Value *args, int arg_count) {
    int i;
    for (i = 0; i < arg_count; i++) {
        value_free(&args[i]);
    }
}

static int parse_call_args(Parser *parser, Value *args, int *arg_count) {
    *arg_count = 0;
    parser_expect(parser, TOK_LPAREN, "expected '(' after function name");
    if (parser->had_error) {
        return 0;
    }
    if (parser_match(parser, TOK_RPAREN)) {
        return 1;
    }
    while (!parser->had_error) {
        if (*arg_count >= MH_MAX_ARGS) {
            parser_set_error(parser, "too many function arguments");
            return 0;
        }
        args[*arg_count] = parse_expression(parser);
        (*arg_count)++;
        if (parser_match(parser, TOK_COMMA)) {
            continue;
        }
        return parser_expect(parser, TOK_RPAREN, "expected ')'");
    }
    return 0;
}

static int value_is_scalar_like(Value value, MATRIX_TYPE *scalar) {
    /* Treat 1x1 matrices as scalars in expressions. This matches common
     * matrix-calculator behavior while preserving full matrix multiplication. */
    if (value.type == VAL_SCALAR) {
        *scalar = value.scalar;
        return 1;
    }
    if ((value.type == VAL_MATRIX) && (value.matrix != NULL) &&
        (value.matrix->row == 1) && (value.matrix->column == 1)) {
        *scalar = value.matrix->data[0];
        return 1;
    }
    return 0;
}

static Matrix *matrix_add_scalar(Matrix *matrix, MATRIX_TYPE scalar) {
    Matrix *result = Matrix_copy(matrix);
    int i, size = result->row * result->column;
    for (i = 0; i < size; i++) {
        result->data[i] += scalar;
    }
    return result;
}

static Value apply_binary(Parser *parser, TokenType op, Value left, Value right) {
    Value result = value_none();
    MATRIX_TYPE left_scalar = 0;
    MATRIX_TYPE right_scalar = 0;
    int left_scalar_like = value_is_scalar_like(left, &left_scalar);
    int right_scalar_like = value_is_scalar_like(right, &right_scalar);

    if (left_scalar_like && right_scalar_like) {
        if ((left.type == VAL_MATRIX) && (right.type == VAL_MATRIX) &&
            ((op == TOK_PLUS) || (op == TOK_MINUS) || (op == TOK_STAR))) {
            if (op == TOK_PLUS) {
                result = value_matrix(M_add_sub(1, left.matrix, -1, right.matrix));
            } else if (op == TOK_MINUS) {
                result = value_matrix(M_add_sub(1, left.matrix, 1, right.matrix));
            } else {
                result = value_matrix(M_mul(left.matrix, right.matrix));
            }
        } else if (op == TOK_PLUS) {
            result = value_scalar(left_scalar + right_scalar);
        } else if (op == TOK_MINUS) {
            result = value_scalar(left_scalar - right_scalar);
        } else if (op == TOK_STAR) {
            result = value_scalar(left_scalar * right_scalar);
        } else if (op == TOK_SLASH) {
            result = value_scalar(left_scalar / right_scalar);
        }
        value_free(&left);
        value_free(&right);
        return result;
    }

    if ((left.type == VAL_MATRIX) && right_scalar_like) {
        Matrix *matrix = left.matrix;
        left.matrix = NULL;
        if (op == TOK_PLUS) {
            result = value_matrix(matrix_add_scalar(matrix, right_scalar));
            M_free(matrix);
        } else if (op == TOK_MINUS) {
            result = value_matrix(matrix_add_scalar(matrix, -right_scalar));
            M_free(matrix);
        } else if (op == TOK_STAR) {
            M_numul(matrix, right_scalar);
            result = value_matrix(matrix);
        } else if (op == TOK_SLASH) {
            M_numul(matrix, 1 / right_scalar);
            result = value_matrix(matrix);
        }
        value_free(&left);
        value_free(&right);
        return result;
    }

    if (left_scalar_like && (right.type == VAL_MATRIX)) {
        Matrix *matrix = right.matrix;
        right.matrix = NULL;
        if (op == TOK_PLUS) {
            result = value_matrix(matrix_add_scalar(matrix, left_scalar));
            M_free(matrix);
        } else if (op == TOK_MINUS) {
            M_numul(matrix, -1);
            result = value_matrix(matrix_add_scalar(matrix, left_scalar));
            M_free(matrix);
        } else if (op == TOK_STAR) {
            M_numul(matrix, left_scalar);
            result = value_matrix(matrix);
        } else {
            M_free(matrix);
            parser_set_error(parser, "scalar divided by matrix is not supported");
        }
        value_free(&left);
        value_free(&right);
        return result;
    }

    if ((left.type == VAL_MATRIX) && (right.type == VAL_MATRIX)) {
        if (op == TOK_PLUS) {
            result = value_matrix(M_add_sub(1, left.matrix, -1, right.matrix));
        } else if (op == TOK_MINUS) {
            result = value_matrix(M_add_sub(1, left.matrix, 1, right.matrix));
        } else if (op == TOK_STAR) {
            result = value_matrix(M_mul(left.matrix, right.matrix));
        } else {
            parser_set_error(parser, "matrix division is not supported");
        }
        value_free(&left);
        value_free(&right);
        return result;
    }

    parser_set_error(parser, "unsupported operand types");
    value_free(&left);
    value_free(&right);
    return value_none();
}

static int append_matrix_literal_value(Parser *parser, MATRIX_TYPE **data,
                                       int *capacity, int *count,
                                       MATRIX_TYPE value) {
    if (*count >= *capacity) {
        int new_capacity = (*capacity == 0) ? 16 : (*capacity * 2);
        MATRIX_TYPE *new_data = (MATRIX_TYPE *)realloc(*data, sizeof(MATRIX_TYPE) * new_capacity);
        if (new_data == NULL) {
            free(*data);
            *data = NULL;
            parser_set_error(parser, "out of memory while reading matrix literal");
            return 0;
        }
        *data = new_data;
        *capacity = new_capacity;
    }
    (*data)[(*count)++] = value;
    return 1;
}

static int parse_matrix_literal_number(Parser *parser, MATRIX_TYPE *number) {
    MATRIX_TYPE sign = 1;
    if (parser_match(parser, TOK_MINUS)) {
        sign = -1;
    } else {
        parser_match(parser, TOK_PLUS);
    }
    if (parser->current.type != TOK_NUMBER) {
        parser_set_error(parser, "matrix literals currently accept numeric values only");
        return 0;
    }
    *number = sign * parser->current.number;
    parser_advance(parser);
    return 1;
}

static int append_matrix_literal_range(Parser *parser, MATRIX_TYPE **data,
                                       int *capacity, int *count,
                                       MATRIX_TYPE start, MATRIX_TYPE step,
                                       MATRIX_TYPE end, int *added) {
    MATRIX_TYPE value = start;
    MATRIX_TYPE eps = fabs(step) * 1e-10 + 1e-12;
    int item_count = 0;

    if (fabs(step) <= 1e-12) {
        parser_set_error(parser, "matrix literal range step cannot be zero");
        return 0;
    }

    if (step > 0) {
        while (value <= end + eps) {
            if (item_count >= MH_MAX_RANGE_ITEMS) {
                parser_set_error(parser, "matrix literal range has too many values");
                return 0;
            }
            if (!append_matrix_literal_value(parser, data, capacity, count, value)) {
                return 0;
            }
            value += step;
            item_count++;
        }
    } else {
        while (value >= end - eps) {
            if (item_count >= MH_MAX_RANGE_ITEMS) {
                parser_set_error(parser, "matrix literal range has too many values");
                return 0;
            }
            if (!append_matrix_literal_value(parser, data, capacity, count, value)) {
                return 0;
            }
            value += step;
            item_count++;
        }
    }

    if (item_count == 0) {
        parser_set_error(parser, "matrix literal range produced no values");
        return 0;
    }
    *added = item_count;
    return 1;
}

static Value parse_matrix_literal(Parser *parser) {
    /* Matrix literals accept numeric rows and simple ranges:
     * [1 2; 3 4], [2:6], and [1:0.5:3]. Rows must remain rectangular. */
    MATRIX_TYPE *data = NULL;
    int capacity = 0;
    int count = 0;
    int rows = 0;
    int expected_cols = -1;
    int current_cols = 0;

    parser_expect(parser, TOK_LBRACKET, "expected '['");
    while (!parser->had_error && (parser->current.type != TOK_RBRACKET) &&
           (parser->current.type != TOK_EOF)) {
        if ((parser->current.type == TOK_SEMICOLON) || (parser->current.type == TOK_EOL)) {
            if (current_cols > 0) {
                if (expected_cols < 0) {
                    expected_cols = current_cols;
                } else if (expected_cols != current_cols) {
                    parser_set_error(parser, "matrix literal rows have different lengths");
                    break;
                }
                rows++;
                current_cols = 0;
            }
            parser_advance(parser);
            continue;
        }
        if (parser_match(parser, TOK_COMMA)) {
            continue;
        }

        MATRIX_TYPE first = 0;
        if (!parse_matrix_literal_number(parser, &first)) {
            break;
        }
        if (parser_match(parser, TOK_COLON)) {
            MATRIX_TYPE second = 0;
            MATRIX_TYPE step = 1;
            MATRIX_TYPE end = 0;
            int added = 0;
            if (!parse_matrix_literal_number(parser, &second)) {
                break;
            }
            if (parser_match(parser, TOK_COLON)) {
                step = second;
                if (!parse_matrix_literal_number(parser, &end)) {
                    break;
                }
            } else {
                end = second;
            }
            if (!append_matrix_literal_range(parser, &data, &capacity, &count,
                                             first, step, end, &added)) {
                break;
            }
            current_cols += added;
        } else {
            if (!append_matrix_literal_value(parser, &data, &capacity, &count, first)) {
                return value_none();
            }
            current_cols++;
        }
    }

    if (!parser_expect(parser, TOK_RBRACKET, "expected ']'")) {
        free(data);
        return value_none();
    }

    if (current_cols > 0) {
        if (expected_cols < 0) {
            expected_cols = current_cols;
        } else if (expected_cols != current_cols) {
            free(data);
            parser_set_error(parser, "matrix literal rows have different lengths");
            return value_none();
        }
        rows++;
    }

    if ((rows <= 0) || (expected_cols <= 0)) {
        free(data);
        parser_set_error(parser, "empty matrix literals are not supported");
        return value_none();
    }

    Matrix *matrix = Matrix_gen(rows, expected_cols, data);
    free(data);
    return value_matrix(matrix);
}

static Value parse_index_access(Parser *parser, const char *name) {
    Value args[MH_MAX_ARGS];
    Matrix *matrix = NULL;
    int argc = 0;
    int index = env_find(parser->env, name);
    int i;
    int row = 0;
    int column = 0;

    for (i = 0; i < MH_MAX_ARGS; i++) {
        args[i] = value_none();
    }
    if (index < 0) {
        parser_set_error(parser, "unknown indexed variable");
        return value_none();
    }
    if (parser->env->vars[index].value.type != VAL_MATRIX) {
        parser_set_error(parser, "indexing requires a matrix or vector variable");
        return value_none();
    }
    matrix = parser->env->vars[index].value.matrix;

    if (!parse_call_args(parser, args, &argc)) {
        free_args(args, argc);
        return value_none();
    }
    if ((argc != 1) && (argc != 2)) {
        parser_set_error(parser, "indexing expects one or two indices");
        free_args(args, argc);
        return value_none();
    }
    if (!value_to_int(args[0], &row)) {
        parser_set_error(parser, "index values must be integers");
        free_args(args, argc);
        return value_none();
    }
    if (argc == 1) {
        if ((matrix->row != 1) && (matrix->column != 1)) {
            parser_set_error(parser, "single-index access requires a row or column vector");
            free_args(args, argc);
            return value_none();
        }
        if (row < 1 || row > matrix->row * matrix->column) {
            parser_set_error(parser, "vector index out of range");
            free_args(args, argc);
            return value_none();
        }
        free_args(args, argc);
        return value_scalar(matrix->data[row - 1]);
    }

    if (!value_to_int(args[1], &column)) {
        parser_set_error(parser, "index values must be integers");
        free_args(args, argc);
        return value_none();
    }
    if ((row < 1) || (row > matrix->row) || (column < 1) || (column > matrix->column)) {
        parser_set_error(parser, "matrix index out of range");
        free_args(args, argc);
        return value_none();
    }
    free_args(args, argc);
    return value_scalar(matrix->data[(row - 1) * matrix->column + (column - 1)]);
}

typedef struct {
    int all;
    int single;
    int start;
    int step;
    int end;
} SliceSpec;

/* Bracket slicing is a postfix operation on matrix values, for example
 * A[:,1], A[1:2,2:3], or v[2:4]. Indices are 1-based like MATLAB. */
static SliceSpec slice_spec_single(int index) {
    SliceSpec spec;
    spec.all = 0;
    spec.single = 1;
    spec.start = index;
    spec.step = 1;
    spec.end = index;
    return spec;
}

static int parse_slice_int(Parser *parser, int *out) {
    Value value = parse_expression(parser);
    int ok = 0;
    if (!parser->had_error) {
        ok = value_to_int(value, out);
        if (!ok) {
            parser_set_error(parser, "slice bounds must be integers");
        }
    }
    value_free(&value);
    return ok && !parser->had_error;
}

static int parse_slice_spec(Parser *parser, SliceSpec *spec) {
    int first = 0;
    int second = 0;

    spec->all = 0;
    spec->single = 0;
    spec->start = 1;
    spec->step = 1;
    spec->end = 1;

    if (parser_match(parser, TOK_COLON)) {
        spec->all = 1;
        return 1;
    }

    if (!parse_slice_int(parser, &first)) {
        return 0;
    }
    if (!parser_match(parser, TOK_COLON)) {
        *spec = slice_spec_single(first);
        return 1;
    }

    if (!parse_slice_int(parser, &second)) {
        return 0;
    }
    if (parser_match(parser, TOK_COLON)) {
        int last = 0;
        if (!parse_slice_int(parser, &last)) {
            return 0;
        }
        spec->start = first;
        spec->step = second;
        spec->end = last;
    } else {
        spec->start = first;
        spec->step = 1;
        spec->end = second;
    }
    return 1;
}

static int normalize_slice_spec(Parser *parser, SliceSpec *spec, int limit, int *count) {
    int cursor;
    int n = 0;

    if (limit <= 0) {
        parser_set_error(parser, "cannot slice an empty dimension");
        return 0;
    }
    if (spec->all) {
        spec->single = 0;
        spec->start = 1;
        spec->step = 1;
        spec->end = limit;
    }
    if (spec->step == 0) {
        parser_set_error(parser, "slice step cannot be zero");
        return 0;
    }

    for (cursor = spec->start;
         (spec->step > 0) ? (cursor <= spec->end) : (cursor >= spec->end);
         cursor += spec->step) {
        if ((cursor < 1) || (cursor > limit)) {
            parser_set_error(parser, "slice index out of range");
            return 0;
        }
        n++;
        if (n > limit) {
            parser_set_error(parser, "slice range has too many values");
            return 0;
        }
    }
    if (n <= 0) {
        parser_set_error(parser, "slice range produced no values");
        return 0;
    }
    *count = n;
    return 1;
}

static Value apply_matrix_slice(Parser *parser, Value source) {
    SliceSpec row_spec;
    SliceSpec col_spec;
    Matrix *matrix = NULL;
    MATRIX_TYPE *data = NULL;
    Matrix *result = NULL;
    int row_count = 0;
    int col_count = 0;
    int i, j;

    if (source.type != VAL_MATRIX || source.matrix == NULL) {
        value_free(&source);
        parser_set_error(parser, "slice requires a matrix or vector");
        return value_none();
    }
    matrix = source.matrix;

    parser_expect(parser, TOK_LBRACKET, "expected '[' for slice");
    if (parser->had_error) {
        value_free(&source);
        return value_none();
    }
    if (!parse_slice_spec(parser, &row_spec)) {
        value_free(&source);
        return value_none();
    }
    if (parser_match(parser, TOK_COMMA)) {
        if (!parse_slice_spec(parser, &col_spec)) {
            value_free(&source);
            return value_none();
        }
    } else {
        if ((matrix->row != 1) && (matrix->column != 1)) {
            value_free(&source);
            parser_set_error(parser, "single-dimension slice requires a row or column vector");
            return value_none();
        }
        if (matrix->row == 1) {
            SliceSpec vector_spec = row_spec;
            row_spec = slice_spec_single(1);
            col_spec = vector_spec;
        } else {
            col_spec = slice_spec_single(1);
        }
    }
    if (!parser_expect(parser, TOK_RBRACKET, "expected ']' after slice")) {
        value_free(&source);
        return value_none();
    }

    if (!normalize_slice_spec(parser, &row_spec, matrix->row, &row_count) ||
        !normalize_slice_spec(parser, &col_spec, matrix->column, &col_count)) {
        value_free(&source);
        return value_none();
    }

    if (row_spec.single && col_spec.single) {
        MATRIX_TYPE scalar = matrix->data[(row_spec.start - 1) * matrix->column + (col_spec.start - 1)];
        value_free(&source);
        return value_scalar(scalar);
    }

    data = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * row_count * col_count);
    if (data == NULL) {
        value_free(&source);
        parser_set_error(parser, "out of memory while slicing matrix");
        return value_none();
    }

    for (i = 0; i < row_count; i++) {
        int row = row_spec.start + i * row_spec.step;
        for (j = 0; j < col_count; j++) {
            int col = col_spec.start + j * col_spec.step;
            data[i * col_count + j] = matrix->data[(row - 1) * matrix->column + (col - 1)];
        }
    }

    result = Matrix_gen(row_count, col_count, data);
    free(data);
    value_free(&source);
    return value_matrix(result);
}

static Value parse_help_function_call(Parser *parser) {
    char topic[MH_MAX_NAME];

    snprintf(topic, sizeof(topic), "script_mode");
    if (!parser_expect(parser, TOK_LPAREN, "expected '(' after help")) {
        return value_none();
    }
    if (!parser_match(parser, TOK_RPAREN)) {
        if (parser->current.type != TOK_IDENT) {
            parser_set_error(parser, "help(...) expects a command or function name");
            return value_none();
        }
        snprintf(topic, sizeof(topic), "%s", parser->current.text);
        parser_advance(parser);
        if (!parser_expect(parser, TOK_RPAREN, "expected ')' after help topic")) {
            return value_none();
        }
    }
    print_help_topic(topic);
    return value_none();
}

static Value parse_function_call(Parser *parser, const char *name) {
    /* This dispatcher maps script-level names onto the existing MatrixHub C API.
     * Multi-output routines are handled separately so old single-output aliases
     * such as qr_q(A) and eigen_val(A) continue to work. */
    Value args[MH_MAX_ARGS];
    int argc = 0;
    int i;

    if (strcmp(name, "help") == 0) {
        return parse_help_function_call(parser);
    }

    for (i = 0; i < MH_MAX_ARGS; i++) {
        args[i] = value_none();
    }
    if (!parse_call_args(parser, args, &argc)) {
        free_args(args, argc);
        return value_none();
    }

    if (strcmp(name, "print") == 0) {
        if (require_arg_count(parser, name, argc, 1)) {
            value_print(stdout, NULL, args[0]);
        }
        free_args(args, argc);
        return value_none();
    }

    if ((strcmp(name, "eye") == 0) || (strcmp(name, "hilbert") == 0)) {
        int n = 0;
        if (!require_arg_count(parser, name, argc, 1) || !value_to_int(args[0], &n) || (n <= 0)) {
            parser_set_error(parser, "eye/hilbert require a positive integer");
            free_args(args, argc);
            return value_none();
        }
        free_args(args, argc);
        return value_matrix((strcmp(name, "eye") == 0) ? M_I(n) : Hilbert(n));
    }

    if ((strcmp(name, "zeros") == 0) || (strcmp(name, "ones") == 0)) {
        int row = 0, column = 0;
        if (!require_arg_count(parser, name, argc, 2) ||
            !value_to_int(args[0], &row) || !value_to_int(args[1], &column) ||
            (row <= 0) || (column <= 0)) {
            parser_set_error(parser, "zeros/ones require positive integer dimensions");
            free_args(args, argc);
            return value_none();
        }
        free_args(args, argc);
        return value_matrix((strcmp(name, "zeros") == 0) ? M_Zeros(row, column) : M_Ones(row, column));
    }

    if ((strcmp(name, "shape") == 0) || (strcmp(name, "size") == 0)) {
        Matrix *matrix = NULL;
        int dim = 0;
        if ((argc != 1) && (argc != 2)) {
            parser_set_error(parser, "shape/size expect one or two arguments");
            free_args(args, argc);
            return value_none();
        }
        if (!require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        if (argc == 2) {
            if (!value_to_int(args[1], &dim) || ((dim != 1) && (dim != 2))) {
                parser_set_error(parser, "size dimension must be 1 or 2");
                free_args(args, argc);
                return value_none();
            }
            {
                MATRIX_TYPE result = (dim == 1) ? args[0].matrix->row : args[0].matrix->column;
                free_args(args, argc);
                return value_scalar(result);
            }
        }
        matrix = matrix_shape(args[0].matrix);
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if ((strcmp(name, "inv") == 0) || (strcmp(name, "inverse") == 0) ||
        (strcmp(name, "transpose") == 0) || (strcmp(name, "trans") == 0) ||
        (strcmp(name, "abs") == 0) || (strcmp(name, "sum") == 0) ||
        (strcmp(name, "min") == 0) || (strcmp(name, "max") == 0) ||
        (strcmp(name, "dia_inv") == 0) || (strcmp(name, "householder") == 0) ||
        (strcmp(name, "hessenberg") == 0) || (strcmp(name, "eigen_val") == 0)) {
        Matrix *matrix = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        if ((strcmp(name, "inv") == 0) || (strcmp(name, "inverse") == 0)) {
            matrix = M_Inverse(args[0].matrix);
        } else if ((strcmp(name, "transpose") == 0) || (strcmp(name, "trans") == 0)) {
            matrix = M_T(args[0].matrix);
        } else if (strcmp(name, "abs") == 0) {
            matrix = M_abs(args[0].matrix);
        } else if (strcmp(name, "sum") == 0) {
            matrix = M_sum(args[0].matrix);
        } else if (strcmp(name, "min") == 0) {
            matrix = M_min(args[0].matrix);
        } else if (strcmp(name, "max") == 0) {
            matrix = M_max(args[0].matrix);
        } else if (strcmp(name, "dia_inv") == 0) {
            matrix = M_Dia_Inv(args[0].matrix);
        } else if (strcmp(name, "householder") == 0) {
            matrix = householder(args[0].matrix);
        } else if (strcmp(name, "hessenberg") == 0) {
            matrix = M_householder(args[0].matrix);
        } else {
            matrix = M_eigen_val(args[0].matrix);
        }
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if ((strcmp(name, "det") == 0) || (strcmp(name, "tr") == 0) ||
        (strcmp(name, "trace") == 0) || (strcmp(name, "rank") == 0) ||
        (strcmp(name, "eigen_max_val") == 0)) {
        MATRIX_TYPE result = 0;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        if (strcmp(name, "det") == 0) {
            result = M_det(args[0].matrix);
        } else if ((strcmp(name, "tr") == 0) || (strcmp(name, "trace") == 0)) {
            result = M_tr(args[0].matrix);
        } else if (strcmp(name, "rank") == 0) {
            result = M_rank(args[0].matrix);
        } else {
            M_eigen_struct *eigen = M_eigen_max(args[0].matrix);
            if (eigen != NULL) {
                result = eigen->eigen_value;
                M_free(eigen->eigen_matrix);
                free(eigen);
            }
        }
        free_args(args, argc);
        return value_scalar(result);
    }

    if ((strcmp(name, "norm") == 0) || (strcmp(name, "cond") == 0)) {
        int setting = 0;
        MATRIX_TYPE result = 0;
        if (!require_arg_count(parser, name, argc, 2) ||
            !require_matrix_arg(parser, name, args[0]) ||
            !value_to_int(args[1], &setting)) {
            parser_set_error(parser, "norm/cond require a matrix and integer setting");
            free_args(args, argc);
            return value_none();
        }
        result = (strcmp(name, "norm") == 0) ? M_norm(args[0].matrix, setting) : M_cond(args[0].matrix, setting);
        free_args(args, argc);
        return value_scalar(result);
    }

    if (strcmp(name, "diag") == 0) {
        Matrix *matrix = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        matrix = matrix_diag_from_vector(args[0].matrix);
        if (matrix == NULL) {
            parser_set_error(parser, "diag(v) requires a row or column vector");
        }
        free_args(args, argc);
        return parser->had_error ? value_none() : value_matrix(matrix);
    }

    if ((strcmp(name, "cut") == 0) || (strcmp(name, "full") == 0)) {
        int a = 0, b = 0, c = 0, d = 0;
        Matrix *matrix = NULL;
        int expected = (strcmp(name, "cut") == 0) ? 5 : 6;
        if (!require_arg_count(parser, name, argc, expected) || !require_matrix_arg(parser, name, args[0]) ||
            !value_to_int(args[1], &a) || !value_to_int(args[2], &b) ||
            !value_to_int(args[3], &c) || !value_to_int(args[4], &d)) {
            parser_set_error(parser, "cut/full argument type error");
            free_args(args, argc);
            return value_none();
        }
        if (strcmp(name, "cut") == 0) {
            matrix = M_Cut(args[0].matrix, a, b, c, d);
        } else {
            if (!require_scalar_arg(parser, name, args[5])) {
                free_args(args, argc);
                return value_none();
            }
            matrix = M_full(args[0].matrix, a, b, c, d, args[5].scalar);
        }
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if ((strcmp(name, "find") == 0) || (strcmp(name, "logic_equal") == 0)) {
        Matrix *matrix = NULL;
        if (!require_arg_count(parser, name, argc, 2) || !require_matrix_arg(parser, name, args[0]) ||
            !require_scalar_arg(parser, name, args[1])) {
            free_args(args, argc);
            return value_none();
        }
        matrix = (strcmp(name, "find") == 0) ?
                 M_find(args[0].matrix, args[1].scalar) :
                 M_logic_equal(args[0].matrix, args[1].scalar);
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if ((strcmp(name, "pmul") == 0) || (strcmp(name, "pdiv") == 0) ||
        (strcmp(name, "logic") == 0) || (strcmp(name, "numul_m") == 0)) {
        Matrix *matrix = NULL;
        if (!require_matrix_arg(parser, name, args[0]) || !require_matrix_arg(parser, name, args[1])) {
            free_args(args, argc);
            return value_none();
        }
        if ((strcmp(name, "pmul") == 0) || (strcmp(name, "pdiv") == 0)) {
            if (!require_arg_count(parser, name, argc, 2)) {
                free_args(args, argc);
                return value_none();
            }
            matrix = M_pmuldiv(args[0].matrix, args[1].matrix, (strcmp(name, "pmul") == 0) ? _MUL_ : _DIV_);
        } else if (strcmp(name, "logic") == 0) {
            int op = 0;
            if (!require_arg_count(parser, name, argc, 3) || !value_to_int(args[2], &op)) {
                parser_set_error(parser, "logic(A, B, op) requires integer op");
                free_args(args, argc);
                return value_none();
            }
            matrix = M_logic(args[0].matrix, args[1].matrix, op);
        } else {
            if (!require_arg_count(parser, name, argc, 2)) {
                free_args(args, argc);
                return value_none();
            }
            matrix = args[0].matrix;
            args[0].matrix = NULL;
            M_numul_m(matrix, args[1].matrix);
        }
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if ((strcmp(name, "pinv") == 0) || (strcmp(name, "swap") == 0) ||
        (strcmp(name, "sample") == 0)) {
        int a = 0, b = 0, mode = _SVD_;
        Matrix *matrix = NULL;
        if (!require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        if (strcmp(name, "pinv") == 0) {
            if ((argc != 1) && (argc != 2)) {
                parser_set_error(parser, "pinv(A[, method]) expects one or two arguments");
                free_args(args, argc);
                return value_none();
            }
            if ((argc == 2) && !value_to_int(args[1], &mode)) {
                parser_set_error(parser, "pinv method must be an integer");
                free_args(args, argc);
                return value_none();
            }
            matrix = M_pinv(args[0].matrix, mode);
        } else if (strcmp(name, "swap") == 0) {
            if (!require_arg_count(parser, name, argc, 4) ||
                !value_to_int(args[1], &a) || !value_to_int(args[2], &b) || !value_to_int(args[3], &mode)) {
                parser_set_error(parser, "swap(A, line1, line2, mode) argument type error");
                free_args(args, argc);
                return value_none();
            }
            matrix = args[0].matrix;
            args[0].matrix = NULL;
            M_Swap(matrix, a, b, mode);
        } else {
            if (!require_arg_count(parser, name, argc, 3) ||
                !require_matrix_arg(parser, name, args[1]) || !value_to_int(args[2], &mode)) {
                parser_set_error(parser, "sample(A, index, mode) argument type error");
                free_args(args, argc);
                return value_none();
            }
            matrix = M_Sample(args[0].matrix, args[1].matrix, mode);
        }
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if ((strcmp(name, "matfull") == 0) || (strcmp(name, "setval") == 0)) {
        int a = 0, b = 0, mode = 0;
        Matrix *matrix = NULL;
        if (!require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        if (strcmp(name, "matfull") == 0) {
            if (!require_arg_count(parser, name, argc, 4) ||
                !value_to_int(args[1], &a) || !value_to_int(args[2], &b) ||
                !require_matrix_arg(parser, name, args[3])) {
                parser_set_error(parser, "matfull(A, row, col, B) argument type error");
                free_args(args, argc);
                return value_none();
            }
            matrix = args[0].matrix;
            args[0].matrix = NULL;
            M_matFull(matrix, a, b, args[3].matrix);
        } else {
            if (!require_arg_count(parser, name, argc, 4) ||
                !require_matrix_arg(parser, name, args[1]) ||
                !require_matrix_arg(parser, name, args[2]) ||
                !value_to_int(args[3], &mode)) {
                parser_set_error(parser, "setval(A, V, order, mode) argument type error");
                free_args(args, argc);
                return value_none();
            }
            matrix = args[0].matrix;
            args[0].matrix = NULL;
            M_setval(matrix, args[1].matrix, args[2].matrix, mode);
        }
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if ((strcmp(name, "uptri") == 0) || (strcmp(name, "uptri_trans") == 0)) {
        Uptri_struct *uptri = NULL;
        Matrix *matrix = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        uptri = M_Uptri_(args[0].matrix);
        if (strcmp(name, "uptri") == 0) {
            matrix = uptri->Uptri_matrix;
            M_free(uptri->trans_matrix);
        } else {
            matrix = uptri->trans_matrix;
            M_free(uptri->Uptri_matrix);
        }
        free(uptri);
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if ((strcmp(name, "lowtri") == 0) || (strcmp(name, "lowtri_trans") == 0)) {
        Lowtri_struct *lowtri = NULL;
        Matrix *matrix = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        lowtri = M_Lowtri_(args[0].matrix);
        if (strcmp(name, "lowtri") == 0) {
            matrix = lowtri->Lowtri_matrix;
            M_free(lowtri->trans_matrix);
        } else {
            matrix = lowtri->trans_matrix;
            M_free(lowtri->Lowtri_matrix);
        }
        free(lowtri);
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if (strcmp(name, "diatri") == 0) {
        Dia_struct *dia = NULL;
        Matrix *matrix = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        dia = M_Diatri_(args[0].matrix);
        matrix = dia->Diatri_matrix;
        M_free(dia->trans_leftmatrix);
        M_free(dia->trans_rightmatrix);
        free(dia);
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if ((strcmp(name, "qr_q") == 0) || (strcmp(name, "qr_r") == 0)) {
        Matrix **qr = NULL;
        Matrix *matrix = NULL;
        enum { q = 0, r = 1 };
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        qr = M_QR(args[0].matrix);
        matrix = (strcmp(name, "qr_q") == 0) ? qr[q] : qr[r];
        if (strcmp(name, "qr_q") == 0) {
            M_free(qr[r]);
        } else {
            M_free(qr[q]);
        }
        free(qr);
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if ((strcmp(name, "svd_u") == 0) || (strcmp(name, "svd_s") == 0) || (strcmp(name, "svd_v") == 0)) {
        Matrix **svd = NULL;
        Matrix *matrix = NULL;
        enum { U = 0, Dia = 1, V = 2 };
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        svd = M_SVD(args[0].matrix);
        if (strcmp(name, "svd_u") == 0) {
            matrix = svd[U];
            M_free(svd[Dia]);
            M_free(svd[V]);
        } else if (strcmp(name, "svd_s") == 0) {
            matrix = svd[Dia];
            M_free(svd[U]);
            M_free(svd[V]);
        } else {
            matrix = svd[V];
            M_free(svd[U]);
            M_free(svd[Dia]);
        }
        free(svd);
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if (strcmp(name, "eigen_max_vec") == 0) {
        M_eigen_struct *eigen = NULL;
        Matrix *matrix = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        eigen = M_eigen_max(args[0].matrix);
        if (eigen != NULL) {
            matrix = eigen->eigen_matrix;
            free(eigen);
        }
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if (strcmp(name, "eigen_vec") == 0) {
        Matrix **eigen = NULL;
        Matrix *matrix = NULL;
        enum { val = 0, vec = 1 };
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return value_none();
        }
        eigen = M_eigen(args[0].matrix);
        matrix = eigen[vec];
        M_free(eigen[val]);
        free(eigen);
        free_args(args, argc);
        return value_matrix(matrix);
    }

    if (strcmp(name, "complex") == 0) {
        free_args(args, argc);
        parser_set_error(parser, "complex numbers are reserved for a future matrix type");
        return value_none();
    }

    if ((strcmp(name, "qr") == 0) || (strcmp(name, "svd") == 0) ||
        (strcmp(name, "eigen") == 0) || (strcmp(name, "eig") == 0) ||
        (strcmp(name, "eigen_max") == 0)) {
        free_args(args, argc);
        parser_set_error(parser, "function returns multiple outputs; use [a, b] = func(...)");
        return value_none();
    }

    free_args(args, argc);
    parser_set_error(parser, "unknown function");
    return value_none();
}

static Value parse_primary(Parser *parser) {
    if (parser->current.type == TOK_NUMBER) {
        MATRIX_TYPE number = parser->current.number;
        parser_advance(parser);
        return value_scalar(number);
    }
    if (parser->current.type == TOK_IDENT) {
        char name[MH_MAX_NAME];
        Token next;
        snprintf(name, sizeof(name), "%s", parser->current.text);
        parser_advance(parser);
        next = parser->current;
        if (next.type == TOK_LPAREN) {
            if (env_find(parser->env, name) >= 0) {
                return parse_index_access(parser, name);
            }
            return parse_function_call(parser, name);
        }
        return env_get(parser->env, name, parser);
    }
    if (parser->current.type == TOK_LBRACKET) {
        return parse_matrix_literal(parser);
    }
    if (parser_match(parser, TOK_LPAREN)) {
        Value value = parse_expression(parser);
        parser_expect(parser, TOK_RPAREN, "expected ')'");
        return value;
    }
    parser_set_error(parser, "expected expression");
    return value_none();
}

static Value parse_factor(Parser *parser) {
    if (parser_match(parser, TOK_MINUS)) {
        Value value = parse_factor(parser);
        if (value.type == VAL_SCALAR) {
            value.scalar = -value.scalar;
        } else if (value.type == VAL_MATRIX) {
            M_numul(value.matrix, -1);
        }
        return value;
    }
    if (parser_match(parser, TOK_PLUS)) {
        return parse_factor(parser);
    }

    Value value = parse_primary(parser);
    while (!parser->had_error) {
        if (parser_match(parser, TOK_TRANSPOSE)) {
            if (value.type != VAL_MATRIX) {
                value_free(&value);
                parser_set_error(parser, "transpose operator requires a matrix");
                return value_none();
            }
            Matrix *transposed = M_T(value.matrix);
            value_free(&value);
            value = value_matrix(transposed);
            continue;
        }
        if (parser->current.type == TOK_LBRACKET) {
            value = apply_matrix_slice(parser, value);
            continue;
        }
        break;
    }
    return value;
}

static Value parse_term(Parser *parser) {
    Value left = parse_factor(parser);
    while (!parser->had_error &&
           ((parser->current.type == TOK_STAR) || (parser->current.type == TOK_SLASH))) {
        TokenType op = parser->current.type;
        parser_advance(parser);
        Value right = parse_factor(parser);
        left = apply_binary(parser, op, left, right);
    }
    return left;
}

static Value parse_additive(Parser *parser) {
    Value left = parse_term(parser);
    while (!parser->had_error &&
           ((parser->current.type == TOK_PLUS) || (parser->current.type == TOK_MINUS))) {
        TokenType op = parser->current.type;
        parser_advance(parser);
        Value right = parse_term(parser);
        left = apply_binary(parser, op, left, right);
    }
    return left;
}

static int token_is_comparison(TokenType type) {
    return (type == TOK_EQ) || (type == TOK_NE) || (type == TOK_LT) ||
           (type == TOK_LE) || (type == TOK_GT) || (type == TOK_GE);
}

static Value apply_comparison(Parser *parser, TokenType op, Value left, Value right) {
    MATRIX_TYPE left_scalar = 0;
    MATRIX_TYPE right_scalar = 0;
    MATRIX_TYPE eps = _APPROXIMATELY_ZERO_;
    int result = 0;

    if (!value_is_scalar_like(left, &left_scalar) || !value_is_scalar_like(right, &right_scalar)) {
        value_free(&left);
        value_free(&right);
        parser_set_error(parser, "comparison requires scalar values");
        return value_none();
    }

    if (op == TOK_EQ) {
        result = fabs(left_scalar - right_scalar) <= eps;
    } else if (op == TOK_NE) {
        result = fabs(left_scalar - right_scalar) > eps;
    } else if (op == TOK_LT) {
        result = left_scalar < right_scalar;
    } else if (op == TOK_LE) {
        result = (left_scalar < right_scalar) || (fabs(left_scalar - right_scalar) <= eps);
    } else if (op == TOK_GT) {
        result = left_scalar > right_scalar;
    } else {
        result = (left_scalar > right_scalar) || (fabs(left_scalar - right_scalar) <= eps);
    }

    value_free(&left);
    value_free(&right);
    return value_scalar(result ? 1 : 0);
}

static Value parse_expression(Parser *parser) {
    Value left = parse_additive(parser);
    while (!parser->had_error && token_is_comparison(parser->current.type)) {
        TokenType op = parser->current.type;
        parser_advance(parser);
        Value right = parse_additive(parser);
        left = apply_comparison(parser, op, left, right);
    }
    return left;
}

static int token_is_ident(Token token, const char *text) {
    return (token.type == TOK_IDENT) && (strcmp(token.text, text) == 0);
}

static int token_starts_block(Token token) {
    return token_is_ident(token, "for") || token_is_ident(token, "if");
}

static void lexer_restart_at_token(Lexer *lexer, Token token) {
    lexer->pos = token.start;
    lexer->line = token.line;
    lexer->column = token.column;
}

static int value_to_bool(Parser *parser, Value value, int *out) {
    MATRIX_TYPE scalar = 0;
    if (!value_is_scalar_like(value, &scalar)) {
        parser_set_error(parser, "condition requires a scalar or 1x1 matrix");
        return 0;
    }
    *out = fabs(scalar) > _APPROXIMATELY_ZERO_;
    return 1;
}

static int find_for_body(Parser *parser, size_t body_start, size_t *body_end, Lexer *after_end) {
    /* Blocks are scanned with lexer offsets instead of building an AST. This
     * keeps the runner small while still allowing nested for/if bodies. */
    Lexer scan = parser->lexer;
    int depth = 0;
    scan.pos = body_start;
    scan.line = parser->current.line;
    scan.column = parser->current.column;

    for (;;) {
        Token token = lexer_next_token(&scan);
        if (token.type == TOK_EOF) {
            parser_set_error(parser, "for loop missing matching end");
            return 0;
        }
        if (token_starts_block(token)) {
            depth++;
        } else if (token_is_ident(token, "end")) {
            if (depth == 0) {
                *body_end = token.start;
                *after_end = scan;
                return 1;
            }
            depth--;
        }
    }
}

static int run_for_statement(Parser *parser) {
    char loop_name[MH_MAX_NAME];
    Value start_value = value_none();
    Value second_value = value_none();
    Value end_value = value_none();
    MATRIX_TYPE start = 0;
    MATRIX_TYPE step = 1;
    MATRIX_TYPE stop = 0;
    MATRIX_TYPE cursor = 0;
    size_t body_start = 0;
    size_t body_end = 0;
    size_t body_len = 0;
    char *body = NULL;
    Lexer after_end;
    int iterations = 0;
    int status = 0;

    parser_advance(parser); /* for */
    if (parser->current.type != TOK_IDENT) {
        parser_set_error(parser, "for loop requires a variable name");
        return 0;
    }
    snprintf(loop_name, sizeof(loop_name), "%s", parser->current.text);
    parser_advance(parser);
    if (!parser_expect(parser, TOK_ASSIGN, "expected '=' in for loop")) {
        return 0;
    }

    start_value = parse_expression(parser);
    if (!parser_expect(parser, TOK_COLON, "expected ':' in for range")) {
        value_free(&start_value);
        return 0;
    }
    second_value = parse_expression(parser);
    if (parser_match(parser, TOK_COLON)) {
        end_value = parse_expression(parser);
        if (!require_scalar_arg(parser, "for", start_value) ||
            !require_scalar_arg(parser, "for", second_value) ||
            !require_scalar_arg(parser, "for", end_value)) {
            value_free(&start_value);
            value_free(&second_value);
            value_free(&end_value);
            return 0;
        }
        start = start_value.scalar;
        step = second_value.scalar;
        stop = end_value.scalar;
    } else {
        if (!require_scalar_arg(parser, "for", start_value) ||
            !require_scalar_arg(parser, "for", second_value)) {
            value_free(&start_value);
            value_free(&second_value);
            return 0;
        }
        start = start_value.scalar;
        stop = second_value.scalar;
        step = (start <= stop) ? 1 : -1;
    }
    value_free(&start_value);
    value_free(&second_value);
    value_free(&end_value);

    if (fabs(step) <= _APPROXIMATELY_ZERO_) {
        parser_set_error(parser, "for loop step cannot be zero");
        return 0;
    }
    if ((parser->current.type != TOK_EOL) && (parser->current.type != TOK_SEMICOLON)) {
        parser_expect(parser, TOK_EOF, "expected statement separator after for header");
        return 0;
    }
    skip_statement_separators(parser);
    body_start = parser->current.start;
    if (!find_for_body(parser, body_start, &body_end, &after_end)) {
        return 0;
    }

    body_len = body_end - body_start;
    body = (char *)malloc(body_len + 1);
    if (body == NULL) {
        parser_set_error(parser, "out of memory while preparing for body");
        return 0;
    }
    memcpy(body, parser->lexer.src + body_start, body_len);
    body[body_len] = '\0';

    for (cursor = start;
         (step > 0) ? (cursor <= stop + _APPROXIMATELY_ZERO_) : (cursor >= stop - _APPROXIMATELY_ZERO_);
         cursor += step) {
        Value loop_value = value_scalar(cursor);
        if (!env_set(parser->env, loop_name, loop_value, parser)) {
            free(body);
            return 0;
        }
        {
            int index = env_find(parser->env, loop_name);
            if (index >= 0) {
                debug_assignment(parser->env, loop_name, parser->env->vars[index].value);
            }
        }
        status = run_script(body, parser->env);
        if (status != 0) {
            parser_set_error(parser, "for loop body failed");
            free(body);
            return 0;
        }
        iterations++;
        if (iterations > _MAX_LOOP_NUM_) {
            parser_set_error(parser, "for loop exceeded maximum iteration count");
            free(body);
            return 0;
        }
    }

    free(body);
    parser->lexer = after_end;
    parser_advance(parser);
    skip_statement_separators(parser);
    return parser->current.type != TOK_EOF;
}

static void parser_take_nested_error(Parser *parser, Parser *nested) {
    if (nested->had_error && !parser->had_error) {
        snprintf(parser->error, sizeof(parser->error), "%s", nested->error);
        parser->had_error = 1;
    }
}

static int run_source_slice(Parser *parser, size_t start, size_t end, const char *context) {
    size_t body_len = end - start;
    char *body = NULL;
    int status;

    if (body_len == 0) {
        return 1;
    }
    body = (char *)malloc(body_len + 1);
    if (body == NULL) {
        parser_set_error(parser, "out of memory while preparing block body");
        return 0;
    }
    memcpy(body, parser->lexer.src + start, body_len);
    body[body_len] = '\0';
    status = run_script(body, parser->env);
    free(body);
    if (status != 0) {
        parser_set_error(parser, context);
        return 0;
    }
    return 1;
}

static int parse_elif_header(Parser *parser, Lexer header, int *truth,
                             Lexer *body_scan, size_t *body_start) {
    Parser nested;
    Value condition = value_none();

    nested.lexer = header;
    nested.env = parser->env;
    nested.had_error = 0;
    nested.error[0] = '\0';
    parser_advance(&nested);

    condition = parse_expression(&nested);
    if (nested.had_error) {
        value_free(&condition);
        parser_take_nested_error(parser, &nested);
        return 0;
    }
    if (!value_to_bool(&nested, condition, truth)) {
        value_free(&condition);
        parser_take_nested_error(parser, &nested);
        return 0;
    }
    value_free(&condition);

    if ((nested.current.type != TOK_EOL) && (nested.current.type != TOK_SEMICOLON)) {
        parser_set_error(parser, "elif condition must end at a statement separator");
        return 0;
    }
    skip_statement_separators(&nested);
    *body_start = nested.current.start;
    *body_scan = nested.lexer;
    lexer_restart_at_token(body_scan, nested.current);
    return 1;
}

static int skip_elif_header(Parser *parser, Lexer header, Lexer *body_scan, size_t *body_start) {
    Parser nested;

    nested.lexer = header;
    nested.env = parser->env;
    nested.had_error = 0;
    nested.error[0] = '\0';
    parser_advance(&nested);

    while ((nested.current.type != TOK_EOL) && (nested.current.type != TOK_SEMICOLON) &&
           (nested.current.type != TOK_EOF)) {
        parser_advance(&nested);
    }
    if (nested.current.type == TOK_EOF) {
        parser_set_error(parser, "elif condition must end at a statement separator");
        return 0;
    }
    skip_statement_separators(&nested);
    *body_start = nested.current.start;
    *body_scan = nested.lexer;
    lexer_restart_at_token(body_scan, nested.current);
    return 1;
}

static int parse_else_header(Parser *parser, Lexer header, Lexer *body_scan, size_t *body_start) {
    Parser nested;

    nested.lexer = header;
    nested.env = parser->env;
    nested.had_error = 0;
    nested.error[0] = '\0';
    parser_advance(&nested);

    if ((nested.current.type != TOK_EOL) && (nested.current.type != TOK_SEMICOLON)) {
        parser_set_error(parser, "else does not take a condition");
        return 0;
    }
    skip_statement_separators(&nested);
    *body_start = nested.current.start;
    *body_scan = nested.lexer;
    lexer_restart_at_token(body_scan, nested.current);
    return 1;
}

static int run_if_statement(Parser *parser) {
    /* if/elif/else selects exactly one source slice, then executes it through
     * run_script() so nested statements reuse the same parser and environment. */
    Value condition = value_none();
    int truth = 0;
    int matched = 0;
    int selected = 0;
    int executed = 0;
    int else_seen = 0;
    int depth = 0;
    size_t selected_start = 0;
    Lexer scan;

    parser_advance(parser); /* if */
    condition = parse_expression(parser);
    if (parser->had_error) {
        value_free(&condition);
        return 0;
    }
    if (!value_to_bool(parser, condition, &truth)) {
        value_free(&condition);
        return 0;
    }
    value_free(&condition);

    if ((parser->current.type != TOK_EOL) && (parser->current.type != TOK_SEMICOLON)) {
        parser_set_error(parser, "if condition must end at a statement separator");
        return 0;
    }
    skip_statement_separators(parser);
    if (truth) {
        matched = 1;
        selected = 1;
        selected_start = parser->current.start;
    }

    scan = parser->lexer;
    lexer_restart_at_token(&scan, parser->current);
    for (;;) {
        Token token = lexer_next_token(&scan);
        if (token.type == TOK_EOF) {
            parser_set_error(parser, "if block missing matching end");
            return 0;
        }
        if (token_starts_block(token)) {
            depth++;
            continue;
        }
        if (token_is_ident(token, "end")) {
            if (depth == 0) {
                if (selected && !executed) {
                    if (!run_source_slice(parser, selected_start, token.start, "if branch failed")) {
                        return 0;
                    }
                }
                parser->lexer = scan;
                parser_advance(parser);
                skip_statement_separators(parser);
                return parser->current.type != TOK_EOF;
            }
            depth--;
            continue;
        }
        if ((depth == 0) && token_is_ident(token, "elif")) {
            Lexer branch_scan;
            size_t branch_start = 0;
            int branch_truth = 0;

            if (else_seen) {
                parser_set_error(parser, "elif cannot appear after else");
                return 0;
            }
            if (selected && !executed) {
                if (!run_source_slice(parser, selected_start, token.start, "if branch failed")) {
                    return 0;
                }
                executed = 1;
            }
            if (matched) {
                if (!skip_elif_header(parser, scan, &branch_scan, &branch_start)) {
                    return 0;
                }
            } else {
                if (!parse_elif_header(parser, scan, &branch_truth, &branch_scan, &branch_start)) {
                    return 0;
                }
            }
            if (!matched && branch_truth) {
                matched = 1;
                selected = 1;
                selected_start = branch_start;
            } else {
                selected = 0;
            }
            scan = branch_scan;
            continue;
        }
        if ((depth == 0) && token_is_ident(token, "else")) {
            Lexer branch_scan;
            size_t branch_start = 0;

            if (else_seen) {
                parser_set_error(parser, "duplicate else in if block");
                return 0;
            }
            if (selected && !executed) {
                if (!run_source_slice(parser, selected_start, token.start, "if branch failed")) {
                    return 0;
                }
                executed = 1;
            }
            else_seen = 1;
            if (!parse_else_header(parser, scan, &branch_scan, &branch_start)) {
                return 0;
            }
            if (!matched) {
                matched = 1;
                selected = 1;
                selected_start = branch_start;
            } else {
                selected = 0;
            }
            scan = branch_scan;
        }
    }
}

static int hessenberg_multi_output(Matrix *source, Matrix **transform, Matrix **hessenberg) {
    /* The original C helper returns the Hessenberg matrix. Script mode also
     * exposes the accumulated transform so [Q, H] = hessenberg(A) is available. */
    Matrix *ri = NULL;
    Matrix *q = NULL;
    int i, j, k, dim;

    *transform = NULL;
    *hessenberg = NULL;
    if ((source == NULL) || (source->row != source->column)) {
        return 0;
    }

    dim = source->row;
    ri = Matrix_copy(source);
    q = M_I(dim);
    if ((ri == NULL) || (q == NULL)) {
        M_free(ri);
        M_free(q);
        return 0;
    }

    for (i = 1; i < dim; i++) {
        Matrix *x = M_Cut(ri, i + 1, _END_, i, i);
        Matrix *hi = householder(x);
        Matrix *qi = M_I(dim);
        Matrix *temp = NULL;
        M_free(x);
        if ((hi == NULL) || (qi == NULL)) {
            M_free(hi);
            M_free(qi);
            M_free(q);
            M_free(ri);
            return 0;
        }
        for (j = 0; j < dim - i; j++) {
            for (k = 0; k < dim - i; k++) {
                qi->data[(j + i) * dim + (k + i)] = hi->data[j * (dim - i) + k];
            }
        }
        M_free(hi);

        temp = q;
        q = M_mul(qi, temp);
        M_free(temp);

        temp = ri;
        ri = M_mul(qi, ri);
        M_free(temp);

        temp = ri;
        ri = M_mul(ri, qi);
        M_free(temp);
        M_free(qi);

        if ((q == NULL) || (ri == NULL)) {
            M_free(q);
            M_free(ri);
            return 0;
        }
    }

    *transform = q;
    *hessenberg = ri;
    return 1;
}

static int parse_multi_output_call(Parser *parser, const char *name, Value *outputs, int *output_count) {
    /* MatrixHub decomposition structs own their Matrix* fields. After assigning
     * fields into outputs[], only the light wrapper struct is freed. */
    Value args[MH_MAX_ARGS];
    int argc = 0;
    int i;

    for (i = 0; i < MH_MAX_ARGS; i++) {
        args[i] = value_none();
        outputs[i] = value_none();
    }
    *output_count = 0;

    if (!parse_call_args(parser, args, &argc)) {
        free_args(args, argc);
        return 0;
    }

    if ((strcmp(name, "uptri") == 0) || (strcmp(name, "uptri_trans") == 0)) {
        Uptri_struct *uptri = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return 0;
        }
        uptri = M_Uptri_(args[0].matrix);
        if (uptri == NULL) {
            parser_set_error(parser, "uptri failed");
            free_args(args, argc);
            return 0;
        }
        outputs[0] = value_matrix(uptri->trans_matrix);
        outputs[1] = value_matrix(uptri->Uptri_matrix);
        *output_count = 2;
        free(uptri);
        free_args(args, argc);
        return 1;
    }

    if ((strcmp(name, "lowtri") == 0) || (strcmp(name, "lowtri_trans") == 0)) {
        Lowtri_struct *lowtri = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return 0;
        }
        lowtri = M_Lowtri_(args[0].matrix);
        if (lowtri == NULL) {
            parser_set_error(parser, "lowtri failed");
            free_args(args, argc);
            return 0;
        }
        outputs[0] = value_matrix(lowtri->trans_matrix);
        outputs[1] = value_matrix(lowtri->Lowtri_matrix);
        *output_count = 2;
        free(lowtri);
        free_args(args, argc);
        return 1;
    }

    if (strcmp(name, "diatri") == 0) {
        Dia_struct *dia = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return 0;
        }
        dia = M_Diatri_(args[0].matrix);
        if (dia == NULL) {
            parser_set_error(parser, "diatri failed");
            free_args(args, argc);
            return 0;
        }
        outputs[0] = value_matrix(dia->trans_leftmatrix);
        outputs[1] = value_matrix(dia->Diatri_matrix);
        outputs[2] = value_matrix(dia->trans_rightmatrix);
        *output_count = 3;
        free(dia);
        free_args(args, argc);
        return 1;
    }

    if (strcmp(name, "householder") == 0) {
        Matrix *transform = NULL;
        Matrix *result = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return 0;
        }
        transform = householder(args[0].matrix);
        if (transform == NULL) {
            parser_set_error(parser, "householder failed");
            free_args(args, argc);
            return 0;
        }
        result = M_mul(transform, args[0].matrix);
        if (result == NULL) {
            M_free(transform);
            parser_set_error(parser, "householder transformed result failed");
            free_args(args, argc);
            return 0;
        }
        outputs[0] = value_matrix(transform);
        outputs[1] = value_matrix(result);
        *output_count = 2;
        free_args(args, argc);
        return 1;
    }

    if (strcmp(name, "hessenberg") == 0) {
        Matrix *transform = NULL;
        Matrix *result = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return 0;
        }
        if (!hessenberg_multi_output(args[0].matrix, &transform, &result)) {
            parser_set_error(parser, "hessenberg failed");
            free_args(args, argc);
            return 0;
        }
        outputs[0] = value_matrix(transform);
        outputs[1] = value_matrix(result);
        *output_count = 2;
        free_args(args, argc);
        return 1;
    }

    if (strcmp(name, "qr") == 0) {
        Matrix **qr = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return 0;
        }
        qr = M_QR(args[0].matrix);
        if (qr == NULL) {
            parser_set_error(parser, "qr failed");
            free_args(args, argc);
            return 0;
        }
        outputs[0] = value_matrix(qr[0]);
        outputs[1] = value_matrix(qr[1]);
        *output_count = 2;
        free(qr);
        free_args(args, argc);
        return 1;
    }

    if (strcmp(name, "svd") == 0) {
        Matrix **svd = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return 0;
        }
        svd = M_SVD(args[0].matrix);
        if (svd == NULL) {
            parser_set_error(parser, "svd failed");
            free_args(args, argc);
            return 0;
        }
        outputs[0] = value_matrix(svd[0]);
        outputs[1] = value_matrix(svd[1]);
        outputs[2] = value_matrix(svd[2]);
        *output_count = 3;
        free(svd);
        free_args(args, argc);
        return 1;
    }

    if ((strcmp(name, "eigen") == 0) || (strcmp(name, "eig") == 0)) {
        Matrix **eigen = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return 0;
        }
        eigen = M_eigen(args[0].matrix);
        if (eigen == NULL) {
            parser_set_error(parser, "eigen failed");
            free_args(args, argc);
            return 0;
        }
        outputs[0] = value_matrix(eigen[0]);
        outputs[1] = value_matrix(eigen[1]);
        *output_count = 2;
        free(eigen);
        free_args(args, argc);
        return 1;
    }

    if (strcmp(name, "eigen_max") == 0) {
        M_eigen_struct *eigen = NULL;
        if (!require_arg_count(parser, name, argc, 1) || !require_matrix_arg(parser, name, args[0])) {
            free_args(args, argc);
            return 0;
        }
        eigen = M_eigen_max(args[0].matrix);
        if (eigen == NULL) {
            parser_set_error(parser, "eigen_max failed");
            free_args(args, argc);
            return 0;
        }
        outputs[0] = value_scalar(eigen->eigen_value);
        outputs[1] = value_matrix(eigen->eigen_matrix);
        *output_count = 2;
        free(eigen);
        free_args(args, argc);
        return 1;
    }

    free_args(args, argc);
    parser_set_error(parser, "unknown multi-output function");
    return 0;
}

static int parse_multi_output_statement(Parser *parser) {
    /* Parse MATLAB-style result lists such as [Q, R] = qr(A). A trailing
     * semicolon suppresses printing but still records debug snapshots. */
    char names[MH_MAX_ARGS][MH_MAX_NAME];
    char function_name[MH_MAX_NAME];
    Value outputs[MH_MAX_ARGS];
    int requested = 0;
    int actual = 0;
    int suppress_output = 0;
    int i;

    for (i = 0; i < MH_MAX_ARGS; i++) {
        names[i][0] = '\0';
        outputs[i] = value_none();
    }

    parser_expect(parser, TOK_LBRACKET, "expected '[' for output list");
    if (parser->had_error) {
        return 0;
    }
    for (;;) {
        if (requested >= MH_MAX_ARGS) {
            parser_set_error(parser, "too many output variables");
            return 0;
        }
        if (parser->current.type != TOK_IDENT) {
            parser_set_error(parser, "output list requires variable names");
            return 0;
        }
        snprintf(names[requested], sizeof(names[requested]), "%s", parser->current.text);
        requested++;
        parser_advance(parser);
        if (parser_match(parser, TOK_COMMA)) {
            continue;
        }
        break;
    }
    if (!parser_expect(parser, TOK_RBRACKET, "expected ']' after output list") ||
        !parser_expect(parser, TOK_ASSIGN, "expected '=' after output list")) {
        return 0;
    }
    if (parser->current.type != TOK_IDENT) {
        parser_set_error(parser, "multi-output assignment requires a function name");
        return 0;
    }
    snprintf(function_name, sizeof(function_name), "%s", parser->current.text);
    parser_advance(parser);
    if (!parse_multi_output_call(parser, function_name, outputs, &actual)) {
        free_args(outputs, actual);
        return 0;
    }
    if (actual != requested) {
        char message[MH_ERR_SIZE];
        snprintf(message, sizeof(message), "%s returns %d output(s), but %d name(s) were provided",
                 function_name, actual, requested);
        parser_set_error(parser, message);
        free_args(outputs, actual);
        return 0;
    }

    if (parser_match(parser, TOK_SEMICOLON)) {
        suppress_output = 1;
    }
    if ((parser->current.type != TOK_EOL) && (parser->current.type != TOK_EOF) &&
        (parser->current.type != TOK_SEMICOLON)) {
        parser_set_error(parser, "expected statement separator");
        free_args(outputs, actual);
        return 0;
    }

    for (i = 0; i < actual; i++) {
        if (!env_set(parser->env, names[i], outputs[i], parser)) {
            outputs[i] = value_none();
            free_args(outputs + i + 1, actual - i - 1);
            return 0;
        }
        outputs[i] = value_none();
        {
            int index = env_find(parser->env, names[i]);
            if (index >= 0) {
                debug_assignment(parser->env, names[i], parser->env->vars[index].value);
                if (!suppress_output) {
                    value_print(stdout, names[i], parser->env->vars[index].value);
                }
            }
        }
    }

    skip_statement_separators(parser);
    return parser->current.type != TOK_EOF;
}

static int parse_statement(Parser *parser) {
    char assign_name[MH_MAX_NAME];
    int is_assignment = 0;
    int suppress_output = 0;
    int consumed_semicolon = 0;
    Value value;

    assign_name[0] = '\0';
    skip_statement_separators(parser);
    if (parser->current.type == TOK_EOF) {
        return 0;
    }
    if (token_is_ident(parser->current, "help")) {
        Token next = parser_peek(parser);
        if (next.type != TOK_LPAREN) {
            char topic[MH_MAX_NAME];
            snprintf(topic, sizeof(topic), "script_mode");
            parser_advance(parser);
            if (parser->current.type == TOK_IDENT) {
                snprintf(topic, sizeof(topic), "%s", parser->current.text);
                parser_advance(parser);
            }
            print_help_topic(topic);
            return finish_command_statement(parser);
        }
    }
    if (token_is_ident(parser->current, "clear")) {
        parser_advance(parser);
        clear_screen();
        return finish_command_statement(parser);
    }
    if (token_is_ident(parser->current, "clean")) {
        parser_advance(parser);
        env_clear(parser->env);
        return finish_command_statement(parser);
    }
    if (token_is_ident(parser->current, "vars") || token_is_ident(parser->current, "who")) {
        parser_advance(parser);
        env_print_variables(parser->env);
        return finish_command_statement(parser);
    }
    if (token_is_ident(parser->current, "del")) {
        char name[MH_MAX_NAME];
        parser_advance(parser);
        if (parser->current.type != TOK_IDENT) {
            parser_set_error(parser, "del requires a variable name");
            return 0;
        }
        snprintf(name, sizeof(name), "%s", parser->current.text);
        parser_advance(parser);
        if (!env_delete(parser->env, name)) {
            printf("[WARNING] variable '%s' does not exist.\n", name);
        }
        return finish_command_statement(parser);
    }
    if (token_is_ident(parser->current, "for")) {
        return run_for_statement(parser);
    }
    if (token_is_ident(parser->current, "if")) {
        return run_if_statement(parser);
    }
    if (token_is_ident(parser->current, "elif") || token_is_ident(parser->current, "else")) {
        parser_set_error(parser, "unexpected if branch keyword");
        return 0;
    }
    if (token_is_ident(parser->current, "end")) {
        parser_set_error(parser, "unexpected end");
        return 0;
    }
    if (parser->current.type == TOK_LBRACKET) {
        Token next = parser_peek(parser);
        if (next.type == TOK_IDENT) {
            return parse_multi_output_statement(parser);
        }
    }

    if (parser->current.type == TOK_IDENT) {
        Token next = parser_peek(parser);
        if (next.type == TOK_ASSIGN) {
            snprintf(assign_name, sizeof(assign_name), "%s", parser->current.text);
            parser_advance(parser);
            parser_advance(parser);
            is_assignment = 1;
        }
    }

    value = parse_expression(parser);
    if (parser->had_error) {
        value_free(&value);
        return 0;
    }

    if (parser_match(parser, TOK_SEMICOLON)) {
        suppress_output = 1;
        consumed_semicolon = 1;
    }
    if (!consumed_semicolon &&
        (parser->current.type != TOK_EOL) && (parser->current.type != TOK_EOF) &&
        (parser->current.type != TOK_SEMICOLON)) {
        value_free(&value);
        parser_set_error(parser, "expected statement separator");
        return 0;
    }

    if (is_assignment) {
        if (!env_set(parser->env, assign_name, value, parser)) {
            return 0;
        }
        int index = env_find(parser->env, assign_name);
        if (index >= 0) {
            debug_assignment(parser->env, assign_name, parser->env->vars[index].value);
            if (!suppress_output) {
                value_print(stdout, assign_name, parser->env->vars[index].value);
            }
        }
    } else {
        if (!suppress_output && (value.type != VAL_NONE)) {
            value_print(stdout, NULL, value);
        }
        value_free(&value);
    }

    skip_statement_separators(parser);
    return parser->current.type != TOK_EOF;
}

static int run_script(const char *source, Env *env) {
    /* A script is interpreted statement by statement. The parser keeps only one
     * active error so diagnostics point at the first failing location. */
    Parser parser;
    lexer_init(&parser.lexer, source);
    parser.env = env;
    parser.had_error = 0;
    parser.error[0] = '\0';
    parser_advance(&parser);

    while (!parser.had_error && (parser.current.type != TOK_EOF)) {
        parse_statement(&parser);
    }

    if (parser.had_error) {
        fprintf(stderr, "matrix script error: %s\n", parser.error);
        return 1;
    }
    return 0;
}

static char *read_file(const char *path) {
    FILE *fp = fopen(path, "rb");
    char *buffer;
    long size;
    if (fp == NULL) {
        return NULL;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }
    size = ftell(fp);
    if (size < 0) {
        fclose(fp);
        return NULL;
    }
    rewind(fp);
    buffer = (char *)malloc((size_t)size + 1);
    if (buffer == NULL) {
        fclose(fp);
        return NULL;
    }
    if (fread(buffer, 1, (size_t)size, fp) != (size_t)size) {
        free(buffer);
        fclose(fp);
        return NULL;
    }
    buffer[size] = '\0';
    fclose(fp);
    return buffer;
}

static int ends_with(const char *text, const char *suffix) {
    size_t text_len = strlen(text);
    size_t suffix_len = strlen(suffix);
    if (text_len < suffix_len) {
        return 0;
    }
    return strcmp(text + text_len - suffix_len, suffix) == 0;
}

static void env_free(Env *env) {
    env_clear(env);
}

static int source_for_balance(const char *source) {
    Lexer lexer;
    int balance = 0;
    lexer_init(&lexer, source);
    for (;;) {
        Token token = lexer_next_token(&lexer);
        if (token.type == TOK_EOF) {
            break;
        }
        if (token_starts_block(token)) {
            balance++;
        } else if (token_is_ident(token, "end")) {
            balance--;
        }
    }
    return balance;
}

static int is_repl_exit(const char *line) {
    while ((*line == ' ') || (*line == '\t')) {
        line++;
    }
    return (strncmp(line, "quit", 4) == 0 || strncmp(line, "exit", 4) == 0) &&
           ((line[4] == '\0') || (line[4] == '\n') || isspace((unsigned char)line[4]));
}

static int append_line(char **buffer, size_t *capacity, size_t *length, const char *line) {
    size_t line_len = strlen(line);
    if (*length + line_len + 1 > *capacity) {
        size_t next_capacity = (*capacity == 0) ? 4096 : (*capacity * 2);
        char *next_buffer = NULL;
        while (*length + line_len + 1 > next_capacity) {
            next_capacity *= 2;
        }
        next_buffer = (char *)realloc(*buffer, next_capacity);
        if (next_buffer == NULL) {
            return 0;
        }
        *buffer = next_buffer;
        *capacity = next_capacity;
    }
    memcpy(*buffer + *length, line, line_len);
    *length += line_len;
    (*buffer)[*length] = '\0';
    return 1;
}

static char *mh_strdup(const char *text) {
    size_t len = strlen(text);
    char *copy = (char *)malloc(len + 1);
    if (copy != NULL) {
        memcpy(copy, text, len + 1);
    }
    return copy;
}

static void history_add(ReplHistory *history, const char *line) {
    char *copy;
    if ((line == NULL) || (line[0] == '\0') || (line[0] == '\n')) {
        return;
    }
    copy = mh_strdup(line);
    if (copy == NULL) {
        return;
    }
    if (history->count == MH_HISTORY_SIZE) {
        free(history->items[0]);
        memmove(history->items, history->items + 1, sizeof(history->items[0]) * (MH_HISTORY_SIZE - 1));
        history->count--;
    }
    history->items[history->count++] = copy;
}

static void history_free(ReplHistory *history) {
    int i;
    for (i = 0; i < history->count; i++) {
        free(history->items[i]);
    }
    history->count = 0;
}

static void redraw_input(const char *prompt, const char *line, size_t len, size_t cursor) {
    size_t move_left = len - cursor;
    printf("\r%s%s\033[K", prompt, line);
    while (move_left > 0) {
        printf("\033[D");
        move_left--;
    }
    fflush(stdout);
}

static void load_history_line(char *line, size_t capacity, size_t *len, size_t *cursor, const char *history_line) {
    *len = strlen(history_line);
    if (*len >= capacity) {
        *len = capacity - 1;
    }
    memcpy(line, history_line, *len);
    line[*len] = '\0';
    *cursor = *len;
}

static int read_plain_line(const char *prompt, char *line, size_t capacity) {
    printf("%s", prompt);
    fflush(stdout);
    return fgets(line, (int)capacity, stdin) != NULL;
}

static int read_repl_line(const char *prompt, ReplHistory *history, char *line, size_t capacity) {
#ifdef _WIN32
    (void)history;
    return read_plain_line(prompt, line, capacity);
#else
    /* On POSIX terminals raw mode gives in-line editing and arrow-key history.
     * Piped input and unsupported terminals fall back to fgets(). */
    struct termios old_term;
    struct termios new_term;
    size_t len = 0;
    size_t cursor = 0;
    int history_index = history->count;
    char ch;

    if (!isatty(STDIN_FILENO) || (tcgetattr(STDIN_FILENO, &old_term) != 0)) {
        return read_plain_line(prompt, line, capacity);
    }

    new_term = old_term;
    new_term.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    new_term.c_cc[VMIN] = 1;
    new_term.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term) != 0) {
        return read_plain_line(prompt, line, capacity);
    }

    line[0] = '\0';
    printf("%s", prompt);
    fflush(stdout);

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if ((ch == '\r') || (ch == '\n')) {
            line[len] = '\0';
            history_add(history, line);
            if (len + 1 < capacity) {
                line[len++] = '\n';
            }
            line[len] = '\0';
            printf("\n");
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
            return 1;
        }
        if (ch == 4) { /* Ctrl-D */
            if (len == 0) {
                printf("\n");
                tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
                return 0;
            }
            continue;
        }
        if ((ch == 127) || (ch == 8)) {
            if (cursor > 0) {
                memmove(line + cursor - 1, line + cursor, len - cursor + 1);
                cursor--;
                len--;
                redraw_input(prompt, line, len, cursor);
            }
            continue;
        }
        if (ch == '\033') {
            char seq1 = '\0';
            char seq2 = '\0';
            if (read(STDIN_FILENO, &seq1, 1) != 1 || read(STDIN_FILENO, &seq2, 1) != 1) {
                continue;
            }
            if (seq1 == '[') {
                if (seq2 == 'A') { /* up */
                    if ((history->count > 0) && (history_index > 0)) {
                        history_index--;
                        load_history_line(line, capacity, &len, &cursor, history->items[history_index]);
                        redraw_input(prompt, line, len, cursor);
                    }
                } else if (seq2 == 'B') { /* down */
                    if (history_index + 1 < history->count) {
                        history_index++;
                        load_history_line(line, capacity, &len, &cursor, history->items[history_index]);
                    } else {
                        history_index = history->count;
                        len = 0;
                        cursor = 0;
                        line[0] = '\0';
                    }
                    redraw_input(prompt, line, len, cursor);
                } else if (seq2 == 'C') { /* right */
                    if (cursor < len) {
                        cursor++;
                        printf("\033[C");
                        fflush(stdout);
                    }
                } else if (seq2 == 'D') { /* left */
                    if (cursor > 0) {
                        cursor--;
                        printf("\033[D");
                        fflush(stdout);
                    }
                } else if (seq2 == '3') { /* delete */
                    char seq3 = '\0';
                    if ((read(STDIN_FILENO, &seq3, 1) == 1) && (seq3 == '~') && (cursor < len)) {
                        memmove(line + cursor, line + cursor + 1, len - cursor);
                        len--;
                        redraw_input(prompt, line, len, cursor);
                    }
                }
            }
            continue;
        }
        if (isprint((unsigned char)ch) && (len + 1 < capacity)) {
            memmove(line + cursor + 1, line + cursor, len - cursor + 1);
            line[cursor] = ch;
            cursor++;
            len++;
            redraw_input(prompt, line, len, cursor);
        }
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
    return 0;
#endif
}

static void print_repl_banner(void) {
    printf("\\ \\     __  ___     __      _       __ __     __\n");
    printf(" \\ \\   /  |/  /__ _/ /_____(_)_ __ / // /_ __/ /\n");
    printf(" / /  / /|_/ / _ `/ __/ __/ /\\ \\ // _  / // / _ \\\n");
    printf("/ /  /_/  /_/\\_,_/\\__/_/ /_//_\\_\\/_//_/\\_,_/_.__/\n");
    printf("MatrixHub v2.0 script mode\n");
    printf("Project: https://github.com/Amoiensis/Matrix_hub\n");
    printf("Type quit or exit to leave.\n\n");
}

static int run_repl(Env *env) {
    /* REPL input is accumulated until for/if/end balance returns to zero, then
     * the buffered chunk is executed as a normal script. */
    char line[MH_MAX_LINE];
    char *buffer = NULL;
    size_t capacity = 0;
    size_t length = 0;
    int status = 0;
    const char *prompt = ">> ";
    ReplHistory history;

    memset(&history, 0, sizeof(history));
    print_repl_banner();
    while (read_repl_line(prompt, &history, line, sizeof(line))) {
        int balance;
        if ((length == 0) && is_repl_exit(line)) {
            break;
        }
        if (!append_line(&buffer, &capacity, &length, line)) {
            fprintf(stderr, "matrix script error: out of memory in REPL\n");
            status = 1;
            break;
        }
        balance = source_for_balance(buffer);
        if (balance > 0) {
            prompt = ".. ";
            continue;
        }
        status = run_script(buffer, env);
        length = 0;
        if (buffer != NULL) {
            buffer[0] = '\0';
        }
        prompt = ">> ";
    }
    free(buffer);
    history_free(&history);
    printf("\n");
    return status;
}

static void usage(const char *argv0) {
    printf("Usage: %s [--debug [debug.txt]] [script.m]\n", argv0);
    printf("Run without script.m to enter interactive REPL mode.\n");
    printf("Supported examples: A=[1 2;3 4]; B=inv(A); print(A*B);\n");
    printf("Inside REPL or script mode, use help or help M_mul for function help.\n");
}

int main(int argc, char **argv) {
    const char *script_path = NULL;
    const char *debug_path = NULL;
    char *source;
    Env env;
    int i;
    int status;

    memset(&env, 0, sizeof(env));
    for (i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[i], "--debug") == 0) {
            debug_path = "matrix_hub_debug.txt";
            if ((i + 2 < argc) && !ends_with(argv[i + 1], ".m")) {
                debug_path = argv[++i];
            }
            continue;
        }
        script_path = argv[i];
    }

    if (debug_path != NULL) {
        env.debug = fopen(debug_path, "w");
        if (env.debug == NULL) {
            fprintf(stderr, "failed to open debug file: %s\n", debug_path);
            return 1;
        }
    }

    if (script_path == NULL) {
        status = run_repl(&env);
        env_free(&env);
        if (env.debug != NULL) {
            fclose(env.debug);
        }
        return status;
    }

    source = read_file(script_path);
    if (source == NULL) {
        fprintf(stderr, "failed to read script: %s\n", script_path);
        if (env.debug != NULL) {
            fclose(env.debug);
        }
        return 1;
    }

    status = run_script(source, &env);
    free(source);
    env_free(&env);
    if (env.debug != NULL) {
        fclose(env.debug);
    }
    return status;
}
