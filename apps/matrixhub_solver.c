/*
 * MatrixHub solver plugin example:
 * Read a small LP-format model file and solve it with the LP or MIP plugin.
 *
 * Supported LP-format subset:
 *   Maximize / Minimize
 *   Subject To rows with <=, >=, =
 *   Bounds with 0 <= x <= ub, x <= ub, x >= 0
 *   Generals / Integers / Binary sections
 *
 * The parser stays compact and targets small LP/MIP models used by the
 * lightweight solver workflow.
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "matrix.h"
#include "solver_plugin/plugin_LP_Sover.h"
#include "solver_plugin/plugin_MIP_Solver.h"

#define EX_MAX_VARS 8192
#define EX_MAX_ROWS 8192
#define EX_INITIAL_VARS 128
#define EX_INITIAL_ROWS 128
#define EX_NAME_LEN 64
#define EX_LINE_LEN 512
#define EX_ROW_BUFFER_LEN 262144
#define EX_PATH_LEN 1024
#define EX_INF 1e50

typedef struct _example_solver_options {
    int progress_interval;
    const char *output_dir;
    LP_Options lp_options;
    MIP_Options mip_options;
} ExampleSolverOptions;

typedef struct _lp_file_model {
    int var_count;
    int var_capacity;
    int row_count;
    int row_capacity;
    int minimize;
    int parse_failed;
    int line_no;
    char error[256];
    char (*var_names)[EX_NAME_LEN];
    MATRIX_TYPE *objective;
    MATRIX_TYPE *rows;
    MATRIX_TYPE *rhs;
    char *sense;
    MATRIX_TYPE *lower;
    MATRIX_TYPE *upper;
    int *is_integer;
} LPFileModel;

enum {
    SEC_NONE = 0,
    SEC_OBJECTIVE,
    SEC_ROWS,
    SEC_BOUNDS,
    SEC_GENERALS,
    SEC_BINARY
};

static int model_init(LPFileModel *model);
static void model_free(LPFileModel *model);
static void model_set_error(LPFileModel *model, const char *format, ...);
static int model_ensure_var_capacity(LPFileModel *model, int needed);
static int model_ensure_row_capacity(LPFileModel *model, int needed);
static MATRIX_TYPE *model_row(LPFileModel *model, int row);
static char *trim_space(char *text);
static void strip_comment(char *text);
static char *find_comparator(char *line);
static int append_row_buffer(LPFileModel *model, char *buffer,
                             size_t buffer_size, const char *line);
static int flush_row_buffer(LPFileModel *model, char *buffer);
static int detect_section(const char *line);
static int get_var_index(LPFileModel *model, const char *name, int create);
static int parse_linear_expression(LPFileModel *model, const char *expr,
                                   int row_index, int objective_mode,
                                   int create_vars);
static int parse_objective_line(LPFileModel *model, char *line);
static int parse_constraint_line(LPFileModel *model, char *line);
static int parse_bound_line(LPFileModel *model, char *line);
static int parse_var_list(LPFileModel *model, char *line, int binary);
static int parse_lp_file(const char *path, LPFileModel *model);
static int check_zero_lower_bounds(LPFileModel *model);
static int count_le_rows(LPFileModel *model);
static int count_integer_vars(LPFileModel *model);
static int count_negative_rhs(Matrix *b);
static int add_model_le_row(Matrix *A, Matrix *b, int row, int total_cols,
                            LPFileModel *model, int source_row,
                            MATRIX_TYPE scale, MATRIX_TYPE rhs);
static int build_mip_matrices(LPFileModel *model, Matrix **A, Matrix **b,
                              Matrix **c, Matrix **integer_mask);
static int build_lp_standard_form(LPFileModel *model, Matrix **A, Matrix **b,
                                  Matrix **c);
static Matrix *extract_lp_original_solution(M_LP_struct *lp, int n);
static const char *lp_status_name(int status);
static const char *mip_status_name(int status);
static void print_solution(LPFileModel *model, Matrix *x, MATRIX_TYPE value,
                           int minimize);
static void print_solution_with_status(LPFileModel *model, Matrix *x,
                                       MATRIX_TYPE value, int minimize,
                                       const char *status_label);
static void print_solution_values(LPFileModel *model, Matrix *x,
                                  MATRIX_TYPE value, int minimize);
static int write_lp_report(const char *output_dir, const char *input_path,
                           LPFileModel *model, Matrix *A, Matrix *b, Matrix *c,
                           const LP_Options *options, M_LP_struct *lp,
                           Matrix *x);
static int write_mip_report(const char *output_dir, const char *input_path,
                            LPFileModel *model, Matrix *A, Matrix *b, Matrix *c,
                            const MIP_Options *options, M_MIP_struct *mip);
static int solve_lp_file(const char *path, const LP_Options *lp_options,
                         const char *output_dir);
static int solve_mip_file(const char *path, const MIP_Options *mip_options,
                          const char *output_dir);
static void example_options_init(ExampleSolverOptions *options);
static int option_matches(const char *arg, const char *name,
                          const char **value);
static int option_value(int argc, char **argv, int *index,
                        const char *name, const char **value);
static int parse_positive_int(const char *name, const char *text, int *out);
static int parse_nonnegative_int(const char *name, const char *text, int *out);
static int parse_positive_double(const char *name, const char *text,
                                 MATRIX_TYPE *out);
static int parse_nonnegative_double(const char *name, const char *text,
                                    MATRIX_TYPE *out);
static int value_looks_integer(const char *text);
static int parse_cut_option(const char *text, MIP_Options *options);
static int parse_method_option(const char *text, MIP_Options *options);
static int parse_heuristic_option(const char *text, MIP_Options *options);
static int parse_branch_option(const char *text, MIP_Options *options);
static int parse_node_select_option(const char *text, MIP_Options *options);
static int parse_bound_mode_option(const char *text, MIP_Options *options);
static int parse_cli_options(int argc, char **argv, const char **mode,
                             const char **path, ExampleSolverOptions *options);
static void print_solver_banner(void);
static void print_usage(const char *program);
static void write_solver_banner(FILE *out);
static int ensure_output_dir(const char *output_dir);
static void sanitize_model_name(const char *input_path, char *name,
                                size_t name_size);
static FILE *open_report_file(const char *output_dir, const char *input_path,
                              const char *mode, char *report_path,
                              size_t report_path_size);
static void report_solution_values(FILE *out, LPFileModel *model, Matrix *x,
                                   MATRIX_TYPE value, int minimize);

static int model_init(LPFileModel *model) {
    int i;
    memset(model, 0, sizeof(LPFileModel));
    model->var_capacity = EX_INITIAL_VARS;
    model->row_capacity = EX_INITIAL_ROWS;
    model->var_names = (char (*)[EX_NAME_LEN])calloc(
        (size_t)model->var_capacity, sizeof(*model->var_names));
    model->objective = (MATRIX_TYPE *)calloc((size_t)model->var_capacity,
                                             sizeof(MATRIX_TYPE));
    model->rows = (MATRIX_TYPE *)calloc(
        (size_t)model->row_capacity * (size_t)model->var_capacity,
        sizeof(MATRIX_TYPE));
    model->rhs = (MATRIX_TYPE *)calloc((size_t)model->row_capacity,
                                       sizeof(MATRIX_TYPE));
    model->sense = (char *)calloc((size_t)model->row_capacity, sizeof(char));
    model->lower = (MATRIX_TYPE *)calloc((size_t)model->var_capacity,
                                         sizeof(MATRIX_TYPE));
    model->upper = (MATRIX_TYPE *)malloc((size_t)model->var_capacity *
                                         sizeof(MATRIX_TYPE));
    model->is_integer = (int *)calloc((size_t)model->var_capacity, sizeof(int));
    if ((model->var_names == NULL) || (model->objective == NULL) ||
        (model->rows == NULL) || (model->rhs == NULL) ||
        (model->sense == NULL) || (model->lower == NULL) ||
        (model->upper == NULL) || (model->is_integer == NULL)) {
        model_free(model);
        model->parse_failed = 1;
        snprintf(model->error, sizeof(model->error),
                 "out of memory while initializing parser");
        return 0;
    }
    for (i = 0; i < model->var_capacity; i++) {
        model->lower[i] = 0;
        model->upper[i] = EX_INF;
    }
    return 1;
}

static void model_free(LPFileModel *model) {
    if (model == NULL) {
        return;
    }
    free(model->var_names);
    free(model->objective);
    free(model->rows);
    free(model->rhs);
    free(model->sense);
    free(model->lower);
    free(model->upper);
    free(model->is_integer);
    memset(model, 0, sizeof(*model));
}

static void model_set_error(LPFileModel *model, const char *format, ...) {
    va_list args;
    if ((model == NULL) || model->parse_failed) {
        return;
    }
    model->parse_failed = 1;
    va_start(args, format);
    vsnprintf(model->error, sizeof(model->error), format, args);
    va_end(args);
}

static int model_ensure_var_capacity(LPFileModel *model, int needed) {
    int i, row;
    int old_capacity;
    int new_capacity;
    char (*new_names)[EX_NAME_LEN] = NULL;
    MATRIX_TYPE *new_objective = NULL;
    MATRIX_TYPE *new_rows = NULL;
    MATRIX_TYPE *new_lower = NULL;
    MATRIX_TYPE *new_upper = NULL;
    int *new_integer = NULL;
    if (needed <= model->var_capacity) {
        return 1;
    }
    if (needed > EX_MAX_VARS) {
        model_set_error(model,
                        "variable limit exceeded (%d > %d); increase EX_MAX_VARS for larger models",
                        needed, EX_MAX_VARS);
        return 0;
    }
    old_capacity = model->var_capacity;
    new_capacity = old_capacity;
    while (new_capacity < needed) {
        new_capacity *= 2;
    }
    if (new_capacity > EX_MAX_VARS) {
        new_capacity = EX_MAX_VARS;
    }
    new_names = (char (*)[EX_NAME_LEN])calloc((size_t)new_capacity,
                                              sizeof(*new_names));
    new_objective = (MATRIX_TYPE *)calloc((size_t)new_capacity,
                                          sizeof(MATRIX_TYPE));
    new_rows = (MATRIX_TYPE *)calloc(
        (size_t)model->row_capacity * (size_t)new_capacity,
        sizeof(MATRIX_TYPE));
    new_lower = (MATRIX_TYPE *)calloc((size_t)new_capacity,
                                      sizeof(MATRIX_TYPE));
    new_upper = (MATRIX_TYPE *)malloc((size_t)new_capacity *
                                      sizeof(MATRIX_TYPE));
    new_integer = (int *)calloc((size_t)new_capacity, sizeof(int));
    if ((new_names == NULL) || (new_objective == NULL) ||
        (new_rows == NULL) || (new_lower == NULL) ||
        (new_upper == NULL) || (new_integer == NULL)) {
        free(new_names);
        free(new_objective);
        free(new_rows);
        free(new_lower);
        free(new_upper);
        free(new_integer);
        model_set_error(model, "out of memory while growing variable storage");
        return 0;
    }
    memcpy(new_names, model->var_names,
           (size_t)old_capacity * sizeof(*new_names));
    memcpy(new_objective, model->objective,
           (size_t)old_capacity * sizeof(MATRIX_TYPE));
    memcpy(new_lower, model->lower,
           (size_t)old_capacity * sizeof(MATRIX_TYPE));
    memcpy(new_upper, model->upper,
           (size_t)old_capacity * sizeof(MATRIX_TYPE));
    memcpy(new_integer, model->is_integer,
           (size_t)old_capacity * sizeof(int));
    for (i = old_capacity; i < new_capacity; i++) {
        new_lower[i] = 0;
        new_upper[i] = EX_INF;
    }
    for (row = 0; row < model->row_count; row++) {
        memcpy(new_rows + (size_t)row * (size_t)new_capacity,
               model->rows + (size_t)row * (size_t)old_capacity,
               (size_t)old_capacity * sizeof(MATRIX_TYPE));
    }
    free(model->var_names);
    free(model->objective);
    free(model->rows);
    free(model->lower);
    free(model->upper);
    free(model->is_integer);
    model->var_names = new_names;
    model->objective = new_objective;
    model->rows = new_rows;
    model->lower = new_lower;
    model->upper = new_upper;
    model->is_integer = new_integer;
    model->var_capacity = new_capacity;
    return 1;
}

static int model_ensure_row_capacity(LPFileModel *model, int needed) {
    int new_capacity;
    int row;
    MATRIX_TYPE *new_rows = NULL;
    MATRIX_TYPE *new_rhs = NULL;
    char *new_sense = NULL;
    if (needed <= model->row_capacity) {
        return 1;
    }
    if (needed > EX_MAX_ROWS) {
        model_set_error(model,
                        "row limit exceeded (%d > %d); increase EX_MAX_ROWS for larger models",
                        needed, EX_MAX_ROWS);
        return 0;
    }
    new_capacity = model->row_capacity;
    while (new_capacity < needed) {
        new_capacity *= 2;
    }
    if (new_capacity > EX_MAX_ROWS) {
        new_capacity = EX_MAX_ROWS;
    }
    new_rows = (MATRIX_TYPE *)calloc(
        (size_t)new_capacity * (size_t)model->var_capacity,
        sizeof(MATRIX_TYPE));
    new_rhs = (MATRIX_TYPE *)calloc((size_t)new_capacity,
                                    sizeof(MATRIX_TYPE));
    new_sense = (char *)calloc((size_t)new_capacity, sizeof(char));
    if ((new_rows == NULL) || (new_rhs == NULL) || (new_sense == NULL)) {
        free(new_rows);
        free(new_rhs);
        free(new_sense);
        model_set_error(model, "out of memory while growing row storage");
        return 0;
    }
    for (row = 0; row < model->row_count; row++) {
        memcpy(new_rows + (size_t)row * (size_t)model->var_capacity,
               model->rows + (size_t)row * (size_t)model->var_capacity,
               (size_t)model->var_capacity * sizeof(MATRIX_TYPE));
    }
    memcpy(new_rhs, model->rhs,
           (size_t)model->row_count * sizeof(MATRIX_TYPE));
    memcpy(new_sense, model->sense, (size_t)model->row_count * sizeof(char));
    free(model->rows);
    free(model->rhs);
    free(model->sense);
    model->rows = new_rows;
    model->rhs = new_rhs;
    model->sense = new_sense;
    model->row_capacity = new_capacity;
    return 1;
}

static MATRIX_TYPE *model_row(LPFileModel *model, int row) {
    return model->rows + (size_t)row * (size_t)model->var_capacity;
}

static char *trim_space(char *text) {
    char *end;
    while (isspace((unsigned char)*text)) {
        text++;
    }
    if (*text == '\0') {
        return text;
    }
    end = text + strlen(text) - 1;
    while ((end > text) && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return text;
}

static void strip_comment(char *text) {
    char *comment = strchr(text, '\\');
    if (comment != NULL) {
        *comment = '\0';
    }
}

static char *find_comparator(char *line) {
    char *le = strstr(line, "<=");
    char *ge = strstr(line, ">=");
    char *eq = strchr(line, '=');
    char *cmp = NULL;
    if (le != NULL) {
        cmp = le;
    }
    if ((ge != NULL) && ((cmp == NULL) || (ge < cmp))) {
        cmp = ge;
    }
    if ((eq != NULL) && ((cmp == NULL) || (eq < cmp))) {
        cmp = eq;
    }
    return cmp;
}

static int append_row_buffer(LPFileModel *model, char *buffer,
                             size_t buffer_size, const char *line) {
    size_t have = strlen(buffer);
    size_t add = strlen(line);
    size_t separator = (have > 0) ? 1U : 0U;
    if (have + separator + add + 1U > buffer_size) {
        model_set_error(model, "row text exceeds parser buffer (%u bytes)",
                        (unsigned)buffer_size);
        return 0;
    }
    if (separator != 0U) {
        strcat(buffer, " ");
    }
    strcat(buffer, line);
    return 1;
}

static int flush_row_buffer(LPFileModel *model, char *buffer) {
    char *row = trim_space(buffer);
    if (*row == '\0') {
        buffer[0] = '\0';
        return 1;
    }
    if (find_comparator(row) == NULL) {
        model_set_error(model, "unfinished constraint row: %.80s", row);
        return 0;
    }
    if (!parse_constraint_line(model, row)) {
        if (!model->parse_failed) {
            model_set_error(model, "cannot parse constraint row: %.80s", row);
        }
        return 0;
    }
    buffer[0] = '\0';
    return 1;
}

static int detect_section(const char *line) {
    char key[EX_NAME_LEN];
    int i;
    if (sscanf(line, "%63s", key) != 1) {
        return SEC_NONE;
    }
    for (i = 0; key[i] != '\0'; i++) {
        key[i] = (char)tolower((unsigned char)key[i]);
    }
    if ((strcmp(key, "max") == 0) || (strcmp(key, "maximize") == 0) ||
        (strcmp(key, "maximum") == 0)) {
        return SEC_OBJECTIVE;
    }
    if ((strcmp(key, "min") == 0) || (strcmp(key, "minimize") == 0) ||
        (strcmp(key, "minimum") == 0)) {
        return -SEC_OBJECTIVE;
    }
    if ((strcmp(key, "subject") == 0) || (strcmp(key, "such") == 0) ||
        (strcmp(key, "st") == 0) || (strcmp(key, "s.t.") == 0)) {
        return SEC_ROWS;
    }
    if ((strcmp(key, "bounds") == 0) || (strcmp(key, "bound") == 0)) {
        return SEC_BOUNDS;
    }
    if ((strcmp(key, "generals") == 0) || (strcmp(key, "general") == 0) ||
        (strcmp(key, "integers") == 0) || (strcmp(key, "integer") == 0)) {
        return SEC_GENERALS;
    }
    if ((strcmp(key, "binaries") == 0) || (strcmp(key, "binary") == 0)) {
        return SEC_BINARY;
    }
    if (strcmp(key, "end") == 0) {
        return 99;
    }
    return SEC_NONE;
}

static int is_var_char(int ch) {
    return isalnum((unsigned char)ch) || (ch == '_') || (ch == '.');
}

static int get_var_index(LPFileModel *model, const char *name, int create) {
    int i;
    for (i = 0; i < model->var_count; i++) {
        if (strcmp(model->var_names[i], name) == 0) {
            return i;
        }
    }
    if (!create) {
        return -1;
    }
    if (!model_ensure_var_capacity(model, model->var_count + 1)) {
        return -1;
    }
    i = model->var_count++;
    snprintf(model->var_names[i], EX_NAME_LEN, "%s", name);
    model->lower[i] = 0;
    model->upper[i] = EX_INF;
    model->is_integer[i] = 0;
    return i;
}

static int parse_linear_expression(LPFileModel *model, const char *expr,
                                   int row_index, int objective_mode,
                                   int create_vars) {
    const char *p = expr;
    int sign = 1;
    while (*p != '\0') {
        MATRIX_TYPE coef = 1;
        char *number_end;
        char name[EX_NAME_LEN];
        int len = 0;
        int too_long = 0;
        int index;

        while (isspace((unsigned char)*p)) {
            p++;
        }
        if (*p == '+') {
            sign = 1;
            p++;
        } else if (*p == '-') {
            sign = -1;
            p++;
        }
        while (isspace((unsigned char)*p)) {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        if (isdigit((unsigned char)*p) || (*p == '.')) {
            coef = (MATRIX_TYPE)strtod(p, &number_end);
            if (number_end == p) {
                model_set_error(model, "cannot parse number near: %.40s", p);
                return 0;
            }
            p = number_end;
            while (isspace((unsigned char)*p)) {
                p++;
            }
            if (*p == '*') {
                p++;
            }
            while (isspace((unsigned char)*p)) {
                p++;
            }
        }
        if (!isalpha((unsigned char)*p) && (*p != '_')) {
            model_set_error(model, "expected variable name near: %.40s", p);
            return 0;
        }
        while (is_var_char((unsigned char)*p)) {
            if (len < EX_NAME_LEN - 1) {
                name[len++] = *p;
            } else {
                too_long = 1;
            }
            p++;
        }
        name[len] = '\0';
        if (too_long) {
            model_set_error(model, "variable name is too long near: %.40s", name);
            return 0;
        }
        index = get_var_index(model, name, create_vars);
        if (index < 0) {
            if (!model->parse_failed) {
                model_set_error(model, "unknown variable: %s", name);
            }
            return 0;
        }
        if (objective_mode) {
            model->objective[index] += sign * coef;
        } else {
            model_row(model, row_index)[index] += sign * coef;
        }
        sign = 1;
    }
    return 1;
}

static char *skip_optional_label(char *line) {
    char *colon = strchr(line, ':');
    char *cmp = find_comparator(line);
    if ((colon != NULL) && ((cmp == NULL) || (colon < cmp))) {
        return trim_space(colon + 1);
    }
    return trim_space(line);
}

static int parse_objective_line(LPFileModel *model, char *line) {
    char *expr = skip_optional_label(line);
    if (!parse_linear_expression(model, expr, -1, 1, 1)) {
        return 0;
    }
    return 1;
}

static int parse_constraint_line(LPFileModel *model, char *line) {
    char *expr = skip_optional_label(line);
    char *cmp = find_comparator(expr);
    char sense = '<';
    int cmp_len = 2;
    MATRIX_TYPE rhs;
    char *rhs_end;
    MATRIX_TYPE *row_data;
    if (cmp == NULL) {
        model_set_error(model, "missing constraint comparator: %.80s", expr);
        return 0;
    }
    if (strncmp(cmp, ">=", 2) == 0) {
        sense = '>';
    } else if (*cmp == '=') {
        sense = '=';
        cmp_len = 1;
    }
    if (!model_ensure_row_capacity(model, model->row_count + 1)) {
        return 0;
    }
    *cmp = '\0';
    rhs = (MATRIX_TYPE)strtod(cmp + cmp_len, &rhs_end);
    if (rhs_end == cmp + cmp_len) {
        model_set_error(model, "cannot parse RHS near: %.40s", cmp + cmp_len);
        return 0;
    }
    row_data = model_row(model, model->row_count);
    memset(row_data, 0, (size_t)model->var_capacity * sizeof(MATRIX_TYPE));
    if (!parse_linear_expression(model, expr, model->row_count, 0, 1)) {
        return 0;
    }
    model->rhs[model->row_count] = rhs;
    model->sense[model->row_count] = sense;
    model->row_count++;
    return 1;
}

static void clean_var_name(char *name) {
    char *p = name;
    while (*p != '\0') {
        if (!is_var_char((unsigned char)*p)) {
            *p = '\0';
            return;
        }
        p++;
    }
}

static int parse_bound_line(LPFileModel *model, char *line) {
    char var[EX_NAME_LEN];
    MATRIX_TYPE lb, ub;
    int index;
    char lower_line[EX_LINE_LEN];
    int i;
    snprintf(lower_line, sizeof(lower_line), "%s", line);
    for (i = 0; lower_line[i] != '\0'; i++) {
        lower_line[i] = (char)tolower((unsigned char)lower_line[i]);
    }
    if (strstr(lower_line, "free") != NULL) {
        model_set_error(model, "free variables are not supported: %.80s", line);
        return 0;
    }
    if (sscanf(line, "%lf <= %63s <= %lf", &lb, var, &ub) == 3) {
        clean_var_name(var);
        index = get_var_index(model, var, 1);
        if (index < 0) {
            return 0;
        }
        model->lower[index] = lb;
        model->upper[index] = ub;
        return 1;
    }
    if (sscanf(line, "%63s <= %lf", var, &ub) == 2) {
        clean_var_name(var);
        index = get_var_index(model, var, 1);
        if (index < 0) {
            return 0;
        }
        model->upper[index] = ub;
        return 1;
    }
    if (sscanf(line, "%63s >= %lf", var, &lb) == 2) {
        clean_var_name(var);
        index = get_var_index(model, var, 1);
        if (index < 0) {
            return 0;
        }
        model->lower[index] = lb;
        return 1;
    }
    if (sscanf(line, "%lf <= %63s", &lb, var) == 2) {
        clean_var_name(var);
        index = get_var_index(model, var, 1);
        if (index < 0) {
            return 0;
        }
        model->lower[index] = lb;
        return 1;
    }
    if (sscanf(line, "%63s = %lf", var, &ub) == 2) {
        clean_var_name(var);
        index = get_var_index(model, var, 1);
        if (index < 0) {
            return 0;
        }
        model->lower[index] = ub;
        model->upper[index] = ub;
        return 1;
    }
    model_set_error(model, "cannot parse bound line: %.80s", line);
    return 0;
}

static int parse_var_list(LPFileModel *model, char *line, int binary) {
    char *token = strtok(line, " \t\r\n");
    while (token != NULL) {
        int index;
        clean_var_name(token);
        if (token[0] != '\0') {
            index = get_var_index(model, token, 1);
            if (index < 0) {
                if (!model->parse_failed) {
                    model_set_error(model, "cannot register variable: %s", token);
                }
                return 0;
            }
            model->is_integer[index] = 1;
            if (binary) {
                model->lower[index] = 0;
                model->upper[index] = 1;
            }
        }
        token = strtok(NULL, " \t\r\n");
    }
    return 1;
}

static int parse_lp_file(const char *path, LPFileModel *model) {
    FILE *fp;
    char line_buffer[EX_LINE_LEN];
    char *row_buffer = NULL;
    int section = SEC_NONE;
    int ok = 1;
    if (!model_init(model)) {
        printf("[example parser] %s\n", model->error);
        return 0;
    }
    fp = fopen(path, "r");
    if (fp == NULL) {
        printf("[example parser] cannot open file: %s\n", path);
        return 0;
    }
    row_buffer = (char *)calloc(EX_ROW_BUFFER_LEN, sizeof(char));
    if (row_buffer == NULL) {
        model_set_error(model, "out of memory while allocating row buffer");
        fclose(fp);
        printf("[example parser] parse failed: %s\n", model->error);
        return 0;
    }
    while (fgets(line_buffer, sizeof(line_buffer), fp) != NULL) {
        char *line;
        int detected;
        model->line_no++;
        strip_comment(line_buffer);
        line = trim_space(line_buffer);
        if (*line == '\0') {
            continue;
        }
        detected = detect_section(line);
        if (detected == 99) {
            if ((section == SEC_ROWS) && !flush_row_buffer(model, row_buffer)) {
                ok = 0;
            }
            break;
        }
        if (detected != SEC_NONE) {
            if ((section == SEC_ROWS) && !flush_row_buffer(model, row_buffer)) {
                ok = 0;
                break;
            }
            if (detected == -SEC_OBJECTIVE) {
                model->minimize = 1;
                section = SEC_OBJECTIVE;
            } else {
                section = detected;
            }
            continue;
        }
        if (section == SEC_OBJECTIVE) {
            ok = parse_objective_line(model, line);
        } else if (section == SEC_ROWS) {
            ok = append_row_buffer(model, row_buffer, EX_ROW_BUFFER_LEN, line);
            if (ok && (find_comparator(row_buffer) != NULL)) {
                ok = flush_row_buffer(model, row_buffer);
            }
        } else if (section == SEC_BOUNDS) {
            ok = parse_bound_line(model, line);
        } else if (section == SEC_GENERALS) {
            ok = parse_var_list(model, line, 0);
        } else if (section == SEC_BINARY) {
            ok = parse_var_list(model, line, 1);
        } else {
            model_set_error(model, "content outside a supported LP section: %.80s", line);
            ok = 0;
        }
        if (!ok || model->parse_failed) {
            ok = 0;
            break;
        }
    }
    if (ok && (section == SEC_ROWS) && (row_buffer[0] != '\0')) {
        ok = flush_row_buffer(model, row_buffer);
    }
    fclose(fp);
    free(row_buffer);
    if (!ok || model->parse_failed) {
        printf("[example parser] parse failed at line %d: %s\n",
               model->line_no,
               (model->error[0] != '\0') ? model->error : "unknown parser error");
        return 0;
    }
    if ((model->var_count <= 0) || (model->row_count <= 0)) {
        printf("[example parser] model has no variables or constraints: vars=%d rows=%d\n",
               model->var_count, model->row_count);
        return 0;
    }
    return 1;
}

static int check_zero_lower_bounds(LPFileModel *model) {
    int i;
    for (i = 0; i < model->var_count; i++) {
        if (fabs(model->lower[i]) > 1e-12) {
            printf("[example parser] only zero lower bounds are supported: %s >= %.10g\n",
                   model->var_names[i], model->lower[i]);
            return 0;
        }
    }
    return 1;
}

static int count_le_rows(LPFileModel *model) {
    int i;
    int count = 0;
    for (i = 0; i < model->row_count; i++) {
        count += (model->sense[i] == '=') ? 2 : 1;
    }
    for (i = 0; i < model->var_count; i++) {
        if (model->upper[i] < EX_INF / 2) {
            count++;
        }
    }
    return count;
}

static int count_integer_vars(LPFileModel *model) {
    int i;
    int count = 0;
    for (i = 0; i < model->var_count; i++) {
        if (model->is_integer[i]) {
            count++;
        }
    }
    return count;
}

static int count_negative_rhs(Matrix *b) {
    int i;
    int count = 0;
    if (b == NULL) {
        return 0;
    }
    for (i = 0; i < b->row * b->column; i++) {
        if (b->data[i] < -1e-12) {
            count++;
        }
    }
    return count;
}

static int add_model_le_row(Matrix *A, Matrix *b, int row, int total_cols,
                            LPFileModel *model, int source_row,
                            MATRIX_TYPE scale, MATRIX_TYPE rhs) {
    const MATRIX_TYPE *coefs = model_row(model, source_row);
    int col;
    for (col = 0; col < model->var_count; col++) {
        A->data[row * total_cols + col] = scale * coefs[col];
    }
    b->data[row] = rhs;
    return 1;
}

static int build_mip_matrices(LPFileModel *model, Matrix **A, Matrix **b,
                              Matrix **c, Matrix **integer_mask) {
    int n = model->var_count;
    int row_count;
    int row = 0;
    int i, col;
    MATRIX_TYPE obj_sign = model->minimize ? -1 : 1;
    if (!check_zero_lower_bounds(model)) {
        return 0;
    }
    row_count = count_le_rows(model);
    *A = M_Zeros(row_count, n);
    *b = M_Zeros(row_count, 1);
    *c = M_Zeros(1, n);
    *integer_mask = M_Zeros(1, n);
    if ((*A == NULL) || (*b == NULL) || (*c == NULL) || (*integer_mask == NULL)) {
        return 0;
    }
    for (col = 0; col < n; col++) {
        (*c)->data[col] = obj_sign * model->objective[col];
        (*integer_mask)->data[col] = model->is_integer[col] ? 1 : 0;
    }
    for (i = 0; i < model->row_count; i++) {
        if (model->sense[i] == '<') {
            add_model_le_row(*A, *b, row++, n, model, i, 1, model->rhs[i]);
        } else if (model->sense[i] == '>') {
            add_model_le_row(*A, *b, row++, n, model, i, -1, -model->rhs[i]);
        } else {
            add_model_le_row(*A, *b, row++, n, model, i, 1, model->rhs[i]);
            add_model_le_row(*A, *b, row++, n, model, i, -1, -model->rhs[i]);
        }
    }
    for (col = 0; col < n; col++) {
        if (model->upper[col] < EX_INF / 2) {
            (*A)->data[row * n + col] = 1;
            (*b)->data[row] = model->upper[col];
            row++;
        }
    }
    return 1;
}

static int build_lp_standard_form(LPFileModel *model, Matrix **A, Matrix **b,
                                  Matrix **c) {
    int n = model->var_count;
    int le_rows;
    int total_cols;
    int row = 0;
    int i, col;
    MATRIX_TYPE obj_sign = model->minimize ? -1 : 1;
    if (!check_zero_lower_bounds(model)) {
        return 0;
    }
    le_rows = count_le_rows(model);
    total_cols = n + le_rows;
    *A = M_Zeros(le_rows, total_cols);
    *b = M_Zeros(le_rows, 1);
    *c = M_Zeros(1, total_cols);
    if ((*A == NULL) || (*b == NULL) || (*c == NULL)) {
        return 0;
    }
    for (col = 0; col < n; col++) {
        (*c)->data[col] = obj_sign * model->objective[col];
    }
    for (i = 0; i < model->row_count; i++) {
        MATRIX_TYPE rhs;
        MATRIX_TYPE scale;
        int repeats = (model->sense[i] == '=') ? 2 : 1;
        int pass;
        for (pass = 0; pass < repeats; pass++) {
            rhs = model->rhs[i];
            scale = 1;
            if ((model->sense[i] == '>') || ((model->sense[i] == '=') && (pass == 1))) {
                rhs = -rhs;
                scale = -1;
            }
            add_model_le_row(*A, *b, row, total_cols, model, i, scale, rhs);
            (*A)->data[row * total_cols + n + row] = 1;
            row++;
        }
    }
    for (col = 0; col < n; col++) {
        if (model->upper[col] < EX_INF / 2) {
            (*A)->data[row * total_cols + col] = 1;
            (*b)->data[row] = model->upper[col];
            (*A)->data[row * total_cols + n + row] = 1;
            row++;
        }
    }
    return 1;
}

static Matrix *extract_lp_original_solution(M_LP_struct *lp, int n) {
    Matrix *x = M_Zeros(n, 1);
    int row;
    if (x == NULL) {
        return NULL;
    }
    for (row = 0; row < lp->_matrix_base->column; row++) {
        int basis_index = (int)lp->_matrix_base->data[row];
        if ((basis_index >= 0) && (basis_index < n)) {
            x->data[basis_index] = lp->_matrix_b->data[row];
        }
    }
    return x;
}

static const char *lp_status_name(int status) {
    if (status == LP_STATUS_OPTIMAL) {
        return "OPTIMAL";
    }
    if (status == LP_STATUS_UNBOUNDED) {
        return "UNBOUNDED";
    }
    if (status == LP_STATUS_INFEASIBLE) {
        return "INFEASIBLE";
    }
    if (status == LP_STATUS_ITER_LIMIT) {
        return "ITER_LIMIT";
    }
    if (status == LP_STATUS_INPUT_ERROR) {
        return "INPUT_ERROR";
    }
    if (status == LP_STATUS_TIME_LIMIT) {
        return "TIME_LIMIT";
    }
    if (status == LP_STATUS_GAP_LIMIT) {
        return "GAP_LIMIT";
    }
    return "UNKNOWN";
}

static const char *mip_status_name(int status) {
    if (status == MIP_STATUS_OPTIMAL) {
        return "OPTIMAL";
    }
    if (status == MIP_STATUS_INFEASIBLE) {
        return "INFEASIBLE";
    }
    if (status == MIP_STATUS_NODE_LIMIT) {
        return "NODE_LIMIT";
    }
    if (status == MIP_STATUS_UNBOUNDED) {
        return "UNBOUNDED";
    }
    if (status == MIP_STATUS_INPUT_ERROR) {
        return "INPUT_ERROR";
    }
    if (status == MIP_STATUS_TIME_LIMIT) {
        return "TIME_LIMIT";
    }
    if (status == MIP_STATUS_GAP_LIMIT) {
        return "GAP_LIMIT";
    }
    return "UNKNOWN";
}

static void print_solution(LPFileModel *model, Matrix *x, MATRIX_TYPE value,
                           int minimize) {
    print_solution_with_status(model, x, value, minimize, "OPTIMAL");
}

static void print_solution_with_status(LPFileModel *model, Matrix *x,
                                       MATRIX_TYPE value, int minimize,
                                       const char *status_label) {
    printf("status = %s\n", status_label);
    print_solution_values(model, x, value, minimize);
}

static void print_solution_values(LPFileModel *model, Matrix *x,
                                  MATRIX_TYPE value, int minimize) {
    int i;
    MATRIX_TYPE display_value = minimize ? -value : value;
    printf("objective = %.10g\n", display_value);
    for (i = 0; i < model->var_count; i++) {
        printf("%s = %.10g\n", model->var_names[i], x->data[i]);
    }
}

static void write_solver_banner(FILE *out) {
    fprintf(out, "\\ \\     __  ___     __      _       __ __     __\n");
    fprintf(out, " \\ \\   /  |/  /__ _/ /_____(_)_ __ / // /_ __/ /\n");
    fprintf(out, " / /  / /|_/ / _ `/ __/ __/ /\\ \\ // _  / // / _ \\\n");
    fprintf(out, "/ /  /_/  /_/\\_,_/\\__/_/ /_//_\\_\\/_//_/\\_,_/_.__/\n");
    fprintf(out, "MatrixHub v2.0 solver mode\n");
    fprintf(out, "Project: https://github.com/Amoiensis/Matrix_hub\n\n");
}

static int ensure_output_dir(const char *output_dir) {
    struct stat st;
    if ((output_dir == NULL) || (output_dir[0] == '\0')) {
        return 0;
    }
    if (stat(output_dir, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    if (errno != ENOENT) {
        return 0;
    }
#ifdef _WIN32
    if ((mkdir(output_dir) != 0) && (errno != EEXIST)) {
#else
    if ((mkdir(output_dir, 0775) != 0) && (errno != EEXIST)) {
#endif
        return 0;
    }
    return (stat(output_dir, &st) == 0) && S_ISDIR(st.st_mode);
}

static void sanitize_model_name(const char *input_path, char *name,
                                size_t name_size) {
    const char *base;
    const char *dot;
    size_t i, len, out = 0;
    if (name_size == 0) {
        return;
    }
    base = strrchr(input_path, '/');
    base = (base == NULL) ? input_path : base + 1;
    dot = strrchr(base, '.');
    len = ((dot != NULL) && (dot > base)) ? (size_t)(dot - base) : strlen(base);
    for (i = 0; (i < len) && (out + 1 < name_size); i++) {
        unsigned char ch = (unsigned char)base[i];
        if (isalnum(ch) || (ch == '_') || (ch == '-')) {
            name[out++] = (char)ch;
        } else {
            name[out++] = '_';
        }
    }
    if (out == 0) {
        snprintf(name, name_size, "model");
    } else {
        name[out] = '\0';
    }
}

static FILE *open_report_file(const char *output_dir, const char *input_path,
                              const char *mode, char *report_path,
                              size_t report_path_size) {
    char model_name[EX_NAME_LEN];
    FILE *out;
    int written;
    if (output_dir == NULL) {
        return NULL;
    }
    if (!ensure_output_dir(output_dir)) {
        printf("[example report] warning: could not create or open output directory: %s\n",
               output_dir);
        return NULL;
    }
    sanitize_model_name(input_path, model_name, sizeof(model_name));
    written = snprintf(report_path, report_path_size, "%s%s%s_%s_result.txt",
                       output_dir,
                       (output_dir[strlen(output_dir) - 1] == '/') ? "" : "/",
                       model_name, mode);
    if ((written < 0) || ((size_t)written >= report_path_size)) {
        printf("[example report] warning: report path is too long under: %s\n",
               output_dir);
        return NULL;
    }
    out = fopen(report_path, "w");
    if (out == NULL) {
        printf("[example report] warning: could not write report file: %s\n",
               report_path);
    }
    return out;
}

static void report_solution_values(FILE *out, LPFileModel *model, Matrix *x,
                                   MATRIX_TYPE value, int minimize) {
    int i;
    MATRIX_TYPE display_value;
    if (x == NULL) {
        fprintf(out, "solution = unavailable\n");
        return;
    }
    display_value = minimize ? -value : value;
    fprintf(out, "objective = %.10g\n", display_value);
    for (i = 0; i < model->var_count; i++) {
        fprintf(out, "%s = %.10g\n", model->var_names[i], x->data[i]);
    }
}

static int write_lp_report(const char *output_dir, const char *input_path,
                           LPFileModel *model, Matrix *A, Matrix *b, Matrix *c,
                           const LP_Options *options, M_LP_struct *lp,
                           Matrix *x) {
    char report_path[EX_PATH_LEN];
    FILE *out;
    int status;
    (void)b;
    (void)c;
    if (output_dir == NULL) {
        return 1;
    }
    out = open_report_file(output_dir, input_path, "lp",
                           report_path, sizeof(report_path));
    if (out == NULL) {
        return 0;
    }
    status = (lp == NULL) ? LP_STATUS_INPUT_ERROR : lp->OPT_STATUS;
    write_solver_banner(out);
    fprintf(out, "Solve Summary\n");
    fprintf(out, "-------------\n");
    fprintf(out, "mode = LP\n");
    fprintf(out, "model_file = %s\n", input_path);
    fprintf(out, "sense = %s\n", model->minimize ? "minimize" : "maximize");
    fprintf(out, "status = %s (%d)\n", lp_status_name(status), status);
    fprintf(out, "variables = %d\n", model->var_count);
    fprintf(out, "constraints = %d\n", model->row_count);
    fprintf(out, "standard_form_rows = %d\n", (A == NULL) ? 0 : A->row);
    fprintf(out, "standard_form_cols = %d\n", (A == NULL) ? 0 : A->column);
    fprintf(out, "iterations = %d\n", (lp == NULL) ? 0 : lp->iter_num);
    fprintf(out, "time_limit = %.10g\n", options->time_limit_seconds);
    fprintf(out, "gap_limit = %.10g\n", options->gap_limit);
    fprintf(out, "progress_interval = %d\n", options->progress_interval);
    fprintf(out, "\nSolution\n");
    fprintf(out, "--------\n");
    report_solution_values(out, model, x,
                           (lp == NULL) ? 0 : lp->values_opt,
                           model->minimize);
    fclose(out);
    printf("[example report] wrote %s\n", report_path);
    return 1;
}

static int write_mip_report(const char *output_dir, const char *input_path,
                            LPFileModel *model, Matrix *A, Matrix *b, Matrix *c,
                            const MIP_Options *options, M_MIP_struct *mip) {
    char report_path[EX_PATH_LEN];
    FILE *out;
    int status;
    (void)b;
    (void)c;
    if (output_dir == NULL) {
        return 1;
    }
    out = open_report_file(output_dir, input_path, "mip",
                           report_path, sizeof(report_path));
    if (out == NULL) {
        return 0;
    }
    status = (mip == NULL) ? MIP_STATUS_INPUT_ERROR : mip->OPT_STATUS;
    write_solver_banner(out);
    fprintf(out, "Solve Summary\n");
    fprintf(out, "-------------\n");
    fprintf(out, "mode = MIP\n");
    fprintf(out, "model_file = %s\n", input_path);
    fprintf(out, "sense = %s\n", model->minimize ? "minimize" : "maximize");
    fprintf(out, "status = %s (%d)\n", mip_status_name(status), status);
    fprintf(out, "variables = %d\n", model->var_count);
    fprintf(out, "constraints = %d\n", model->row_count);
    fprintf(out, "integer_variables = %d\n", count_integer_vars(model));
    fprintf(out, "inequality_form_rows = %d\n", (A == NULL) ? 0 : A->row);
    fprintf(out, "inequality_form_cols = %d\n", (A == NULL) ? 0 : A->column);
    fprintf(out, "nodes = %d\n", (mip == NULL) ? 0 : mip->node_num);
    fprintf(out, "root_cuts = %d\n", (mip == NULL) ? 0 : mip->cut_num);
    fprintf(out, "node_limit = %d\n", options->node_limit);
    fprintf(out, "open_node_limit = %d\n", options->open_node_limit);
    fprintf(out, "time_limit = %.10g\n", options->time_limit_seconds);
    fprintf(out, "gap_limit = %.10g\n", options->gap_limit);
    fprintf(out, "progress_interval = %d\n", options->progress_interval);
    fprintf(out, "\nSolution\n");
    fprintf(out, "--------\n");
    if ((mip != NULL) && (mip->_matrix_x != NULL) &&
        (mip->OPT_STATUS != MIP_STATUS_OPTIMAL)) {
        fprintf(out, "incumbent_status = FEASIBLE\n");
    }
    report_solution_values(out, model,
                           (mip == NULL) ? NULL : mip->_matrix_x,
                           (mip == NULL) ? 0 : mip->values_opt,
                           model->minimize);
    fclose(out);
    printf("[example report] wrote %s\n", report_path);
    return 1;
}

static void example_options_init(ExampleSolverOptions *options) {
    options->progress_interval = 0;
    options->output_dir = NULL;
    options->lp_options = LP_Default_Options();
    options->mip_options = MIP_Default_Options();
}

static int option_matches(const char *arg, const char *name,
                          const char **value) {
    size_t len = strlen(name);
    if (strncmp(arg, name, len) != 0) {
        return 0;
    }
    if (arg[len] == '=') {
        *value = arg + len + 1;
        return 1;
    }
    if (arg[len] == '\0') {
        *value = NULL;
        return 1;
    }
    return 0;
}

static int option_value(int argc, char **argv, int *index,
                        const char *name, const char **value) {
    const char *matched_value = NULL;
    if (!option_matches(argv[*index], name, &matched_value)) {
        return 0;
    }
    if (matched_value != NULL) {
        *value = matched_value;
        return 1;
    }
    if ((*index + 1 >= argc) || (strncmp(argv[*index + 1], "--", 2) == 0)) {
        printf("[example parser] missing value for %s\n", name);
        return -1;
    }
    *index += 1;
    *value = argv[*index];
    return 1;
}

static int parse_positive_int(const char *name, const char *text, int *out) {
    char *end = NULL;
    long value;
    errno = 0;
    value = strtol(text, &end, 10);
    if ((errno != 0) || (end == text) || (*end != '\0') ||
        (value <= 0) || (value > INT_MAX)) {
        printf("[example parser] %s expects a positive integer, got: %s\n",
               name, text);
        return 0;
    }
    *out = (int)value;
    return 1;
}

static int parse_nonnegative_int(const char *name, const char *text, int *out) {
    char *end = NULL;
    long value;
    errno = 0;
    value = strtol(text, &end, 10);
    if ((errno != 0) || (end == text) || (*end != '\0') ||
        (value < 0) || (value > INT_MAX)) {
        printf("[example parser] %s expects a nonnegative integer, got: %s\n",
               name, text);
        return 0;
    }
    *out = (int)value;
    return 1;
}

static int parse_positive_double(const char *name, const char *text,
                                 MATRIX_TYPE *out) {
    char *end = NULL;
    double value;
    errno = 0;
    value = strtod(text, &end);
    if ((errno != 0) || (end == text) || (*end != '\0') ||
        !isfinite(value) || (value <= 0)) {
        printf("[example parser] %s expects a positive number, got: %s\n",
               name, text);
        return 0;
    }
    *out = (MATRIX_TYPE)value;
    return 1;
}

static int parse_nonnegative_double(const char *name, const char *text,
                                    MATRIX_TYPE *out) {
    char *end = NULL;
    double value;
    errno = 0;
    value = strtod(text, &end);
    if ((errno != 0) || (end == text) || (*end != '\0') ||
        !isfinite(value) || (value < 0)) {
        printf("[example parser] %s expects a nonnegative number, got: %s\n",
               name, text);
        return 0;
    }
    *out = (MATRIX_TYPE)value;
    return 1;
}

static int value_looks_integer(const char *text) {
    const char *p = text;
    if ((*p == '+') || (*p == '-')) {
        p++;
    }
    if (!isdigit((unsigned char)*p)) {
        return 0;
    }
    while (isdigit((unsigned char)*p)) {
        p++;
    }
    return *p == '\0';
}

static int parse_cut_option(const char *text, MIP_Options *options) {
    int rounds;
    if ((strcmp(text, "off") == 0) || (strcmp(text, "false") == 0) ||
        (strcmp(text, "no") == 0) || (strcmp(text, "0") == 0)) {
        options->method = MIP_METHOD_BRANCH_BOUND;
        options->root_cut_rounds = 0;
        return 1;
    }
    if ((strcmp(text, "on") == 0) || (strcmp(text, "true") == 0) ||
        (strcmp(text, "yes") == 0)) {
        options->method = MIP_METHOD_CUT_BRANCH_BOUND;
        options->root_cut_rounds = MIP_DEFAULT_ROOT_CUT_ROUNDS;
        return 1;
    }
    if (!parse_nonnegative_int("--cuts", text, &rounds)) {
        return 0;
    }
    options->root_cut_rounds = rounds;
    options->method = (rounds > 0) ? MIP_METHOD_CUT_BRANCH_BOUND :
        MIP_METHOD_BRANCH_BOUND;
    return 1;
}

static int parse_method_option(const char *text, MIP_Options *options) {
    if ((strcmp(text, "bb") == 0) || (strcmp(text, "branch-bound") == 0) ||
        (strcmp(text, "branch_and_bound") == 0)) {
        options->method = MIP_METHOD_BRANCH_BOUND;
        options->root_cut_rounds = 0;
        return 1;
    }
    if ((strcmp(text, "cut-bb") == 0) || (strcmp(text, "cut_bb") == 0) ||
        (strcmp(text, "cut") == 0) || (strcmp(text, "cuts") == 0)) {
        options->method = MIP_METHOD_CUT_BRANCH_BOUND;
        if (options->root_cut_rounds <= 0) {
            options->root_cut_rounds = MIP_DEFAULT_ROOT_CUT_ROUNDS;
        }
        return 1;
    }
    printf("[example parser] --method expects bb or cut-bb, got: %s\n", text);
    return 0;
}

static int parse_heuristic_option(const char *text, MIP_Options *options) {
    if ((strcmp(text, "off") == 0) || (strcmp(text, "false") == 0) ||
        (strcmp(text, "no") == 0) || (strcmp(text, "0") == 0)) {
        options->heuristic_frequency = 0;
        return 1;
    }
    if ((strcmp(text, "on") == 0) || (strcmp(text, "true") == 0) ||
        (strcmp(text, "yes") == 0)) {
        options->heuristic_frequency = MIP_DEFAULT_HEURISTIC_FREQUENCY;
        return 1;
    }
    if (strcmp(text, "root") == 0) {
        options->heuristic_frequency = INT_MAX;
        return 1;
    }
    return parse_positive_int("--heuristic", text,
                              &options->heuristic_frequency);
}

static int parse_branch_option(const char *text, MIP_Options *options) {
    if ((strcmp(text, "first") == 0) ||
        (strcmp(text, "first-fractional") == 0)) {
        options->branch_rule = MIP_BRANCH_FIRST_FRACTIONAL;
        return 1;
    }
    if ((strcmp(text, "most") == 0) ||
        (strcmp(text, "most-fractional") == 0)) {
        options->branch_rule = MIP_BRANCH_MOST_FRACTIONAL;
        return 1;
    }
    printf("[example parser] --branch expects first or most, got: %s\n", text);
    return 0;
}

static int parse_node_select_option(const char *text, MIP_Options *options) {
    if ((strcmp(text, "dfs") == 0) || (strcmp(text, "depth") == 0) ||
        (strcmp(text, "depth-first") == 0)) {
        options->node_selection = MIP_NODE_DEPTH_FIRST;
        return 1;
    }
    if ((strcmp(text, "best") == 0) || (strcmp(text, "best-bound") == 0) ||
        (strcmp(text, "bound") == 0)) {
        options->node_selection = MIP_NODE_BEST_BOUND;
        return 1;
    }
    printf("[example parser] --node-select expects dfs or best-bound, got: %s\n",
           text);
    return 0;
}

static int parse_bound_mode_option(const char *text, MIP_Options *options) {
    if ((strcmp(text, "parent") == 0) || (strcmp(text, "loose") == 0) ||
        (strcmp(text, "fast") == 0)) {
        options->bound_mode = MIP_BOUND_PARENT;
        return 1;
    }
    if ((strcmp(text, "tight") == 0) || (strcmp(text, "child") == 0) ||
        (strcmp(text, "exact") == 0)) {
        options->bound_mode = MIP_BOUND_TIGHT;
        return 1;
    }
    printf("[example parser] --bound-mode expects parent or tight, got: %s\n",
           text);
    return 0;
}

static int parse_cli_options(int argc, char **argv, const char **mode,
                             const char **path, ExampleSolverOptions *options) {
    int i;
    example_options_init(options);
    *mode = NULL;
    *path = NULL;
    for (i = 1; i < argc; i++) {
        const char *arg = argv[i];
        const char *value = NULL;
        int matched;

        if (option_matches(arg, "--progress", &value)) {
            if (value == NULL) {
                if ((i + 1 < argc) && value_looks_integer(argv[i + 1])) {
                    value = argv[++i];
                    if (!parse_positive_int("--progress", value,
                                            &options->progress_interval)) {
                        return 0;
                    }
                } else {
                    options->progress_interval = 10;
                }
            } else if (!parse_positive_int("--progress", value,
                                           &options->progress_interval)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--output-dir", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            options->output_dir = value;
            continue;
        }

        matched = option_value(argc, argv, &i, "--time-limit", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            MATRIX_TYPE seconds;
            if (!parse_nonnegative_double("--time-limit", value, &seconds)) {
                return 0;
            }
            options->lp_options.time_limit_seconds = seconds;
            options->mip_options.time_limit_seconds = seconds;
            continue;
        }

        matched = option_value(argc, argv, &i, "--max-time", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            MATRIX_TYPE seconds;
            if (!parse_nonnegative_double("--max-time", value, &seconds)) {
                return 0;
            }
            options->lp_options.time_limit_seconds = seconds;
            options->mip_options.time_limit_seconds = seconds;
            continue;
        }

        matched = option_value(argc, argv, &i, "--time-limit-sec", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            MATRIX_TYPE seconds;
            if (!parse_nonnegative_double("--time-limit-sec", value, &seconds)) {
                return 0;
            }
            options->lp_options.time_limit_seconds = seconds;
            options->mip_options.time_limit_seconds = seconds;
            continue;
        }

        matched = option_value(argc, argv, &i, "--gap-limit", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            MATRIX_TYPE gap;
            if (!parse_nonnegative_double("--gap-limit", value, &gap)) {
                return 0;
            }
            options->lp_options.gap_limit = gap;
            options->mip_options.gap_limit = gap;
            continue;
        }

        matched = option_value(argc, argv, &i, "--max-gap", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            MATRIX_TYPE gap;
            if (!parse_nonnegative_double("--max-gap", value, &gap)) {
                return 0;
            }
            options->lp_options.gap_limit = gap;
            options->mip_options.gap_limit = gap;
            continue;
        }

        matched = option_value(argc, argv, &i, "--node-limit", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_positive_int("--node-limit", value,
                                    &options->mip_options.node_limit)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--max-nodes", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_positive_int("--max-nodes", value,
                                    &options->mip_options.node_limit)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--open-limit", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_positive_int("--open-limit", value,
                                    &options->mip_options.open_node_limit)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--open-node-limit", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_positive_int("--open-node-limit", value,
                                    &options->mip_options.open_node_limit)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--cuts", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_cut_option(value, &options->mip_options)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--cut-rounds", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_cut_option(value, &options->mip_options)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--method", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_method_option(value, &options->mip_options)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--mip-method", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_method_option(value, &options->mip_options)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--feas-tol", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_positive_double("--feas-tol", value,
                                       &options->mip_options.feasibility_tol)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--mip-tol", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_positive_double("--mip-tol", value,
                                       &options->mip_options.feasibility_tol)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--int-tol", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_positive_double("--int-tol", value,
                                       &options->mip_options.integer_tol)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--integer-tol", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_positive_double("--integer-tol", value,
                                       &options->mip_options.integer_tol)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--heuristic", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_heuristic_option(value, &options->mip_options)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--heuristic-frequency", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_heuristic_option(value, &options->mip_options)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--repair-limit", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_nonnegative_int("--repair-limit", value,
                                       &options->mip_options.repair_limit)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--branch", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_branch_option(value, &options->mip_options)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--node-select", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_node_select_option(value, &options->mip_options)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--bound-mode", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_bound_mode_option(value, &options->mip_options)) {
                return 0;
            }
            continue;
        }

        matched = option_value(argc, argv, &i, "--bound", &value);
        if (matched < 0) {
            return 0;
        }
        if (matched > 0) {
            if (!parse_bound_mode_option(value, &options->mip_options)) {
                return 0;
            }
            continue;
        }

        if (strncmp(arg, "--", 2) == 0) {
            printf("[example parser] unknown option: %s\n", arg);
            return 0;
        }
        if (*mode == NULL) {
            *mode = arg;
        } else if (*path == NULL) {
            *path = arg;
        } else {
            printf("[example parser] unexpected argument: %s\n", arg);
            return 0;
        }
    }
    options->lp_options.progress_interval = options->progress_interval;
    options->mip_options.progress_interval = options->progress_interval;
    return (*mode != NULL) && (*path != NULL);
}

static int solve_lp_file(const char *path, const LP_Options *lp_options,
                         const char *output_dir) {
    LPFileModel model;
    Matrix *A = NULL, *b = NULL, *c = NULL;
    Matrix *x = NULL;
    M_LP_struct *lp = NULL;
    LP_Options options = (lp_options != NULL) ?
        *lp_options : LP_Default_Options();
    int progress_interval = options.progress_interval;
    if (!parse_lp_file(path, &model)) {
        model_free(&model);
        return 1;
    }
    if (progress_interval > 0) {
        printf("[example parser] parsed vars=%d rows=%d integer=%d\n",
               model.var_count, model.row_count, count_integer_vars(&model));
    }
    if (!build_lp_standard_form(&model, &A, &b, &c)) {
        M_free(A);
        M_free(b);
        M_free(c);
        model_free(&model);
        return 1;
    }
    if (progress_interval > 0) {
        printf("[example parser] standard form rows=%d cols=%d\n", A->row, A->column);
        printf("[example solver] lp options: method=revised-simplex time_limit=%.10g gap_limit=%.10g progress=%d\n",
               options.time_limit_seconds, options.gap_limit,
               options.progress_interval);
    }
    options.method = LP_METHOD_REVISED_SIMPLEX;
    lp = LP_Solver_With_Options(A, b, c, &options);
    if ((lp == NULL) ||
        ((lp->OPT_STATUS != LP_STATUS_OPTIMAL) &&
         (lp->OPT_STATUS != LP_STATUS_GAP_LIMIT))) {
        int status = (lp == NULL) ? LP_STATUS_INPUT_ERROR : lp->OPT_STATUS;
        printf("status = %s (%d)\n", lp_status_name(status), status);
        write_lp_report(output_dir, path, &model, A, b, c, &options, lp, NULL);
        LP_free(lp);
        M_free(A);
        M_free(b);
        M_free(c);
        model_free(&model);
        return 1;
    }
    x = extract_lp_original_solution(lp, model.var_count);
    if (x == NULL) {
        printf("status = %s (%d)\n",
               lp_status_name(LP_STATUS_INPUT_ERROR), LP_STATUS_INPUT_ERROR);
        write_lp_report(output_dir, path, &model, A, b, c, &options, lp, NULL);
        LP_free(lp);
        M_free(A);
        M_free(b);
        M_free(c);
        model_free(&model);
        return 1;
    }
    if (lp->OPT_STATUS == LP_STATUS_GAP_LIMIT) {
        printf("status = %s (%d)\n",
               lp_status_name(lp->OPT_STATUS), lp->OPT_STATUS);
        print_solution_values(&model, x, lp->values_opt, model.minimize);
    } else {
        print_solution(&model, x, lp->values_opt, model.minimize);
    }
    write_lp_report(output_dir, path, &model, A, b, c, &options, lp, x);
    M_free(x);
    LP_free(lp);
    M_free(A);
    M_free(b);
    M_free(c);
    model_free(&model);
    return 0;
}

static int solve_mip_file(const char *path, const MIP_Options *mip_options,
                          const char *output_dir) {
    LPFileModel model;
    Matrix *A = NULL, *b = NULL, *c = NULL, *integer_mask = NULL;
    M_MIP_struct *mip = NULL;
    MIP_Options options = (mip_options != NULL) ?
        *mip_options : MIP_Default_Options();
    int progress_interval = options.progress_interval;
    int negative_rhs;
    if (!parse_lp_file(path, &model)) {
        model_free(&model);
        return 1;
    }
    if (progress_interval > 0) {
        printf("[example parser] parsed vars=%d rows=%d integer=%d\n",
               model.var_count, model.row_count, count_integer_vars(&model));
    }
    options.objective_display_sign = model.minimize ? -1 : 1;
    if (!build_mip_matrices(&model, &A, &b, &c, &integer_mask)) {
        M_free(A);
        M_free(b);
        M_free(c);
        M_free(integer_mask);
        model_free(&model);
        return 1;
    }
    if (progress_interval > 0) {
        printf("[example parser] inequality form rows=%d cols=%d\n", A->row, A->column);
        printf("[example solver] mip options: node_limit=%d open_limit=%d time_limit=%.10g gap_limit=%.10g method=%s cuts=%d heuristic=%d repair=%d branch=%s node_select=%s bound_mode=%s feas_tol=%.3g int_tol=%.3g\n",
               options.node_limit, options.open_node_limit,
               options.time_limit_seconds, options.gap_limit,
               (options.method == MIP_METHOD_BRANCH_BOUND) ? "bb" : "cut-bb",
               options.root_cut_rounds, options.heuristic_frequency,
               options.repair_limit,
               (options.branch_rule == MIP_BRANCH_FIRST_FRACTIONAL) ?
                   "first" : "most",
               (options.node_selection == MIP_NODE_BEST_BOUND) ?
                   "best-bound" : "dfs",
               (options.bound_mode == MIP_BOUND_TIGHT) ? "tight" : "parent",
               options.feasibility_tol,
               options.integer_tol);
        negative_rhs = count_negative_rhs(b);
        if (negative_rhs > 0) {
            printf("[example solver] note: %d rows have negative RHS; LP Phase-I will search for a feasible basis, which can be slow for dense benchmark-sized MIPs.\n",
                   negative_rhs);
        }
    }
    mip = MIP_Solver_With_Options(A, b, c, integer_mask, &options);
    if ((mip == NULL) || (mip->OPT_STATUS != MIP_STATUS_OPTIMAL)) {
        int status = (mip == NULL) ? MIP_STATUS_INPUT_ERROR : mip->OPT_STATUS;
        int gap_accepted = (mip != NULL) &&
            (mip->OPT_STATUS == MIP_STATUS_GAP_LIMIT) &&
            (mip->_matrix_x != NULL);
        printf("status = %s (%d)\n", mip_status_name(status), status);
        if ((mip != NULL) && (mip->_matrix_x != NULL)) {
            printf("incumbent_status = FEASIBLE\n");
            print_solution_values(&model, mip->_matrix_x, mip->values_opt,
                                  model.minimize);
            printf("nodes = %d\n", mip->node_num);
            printf("root_cuts = %d\n", mip->cut_num);
        }
        write_mip_report(output_dir, path, &model, A, b, c, &options, mip);
        MIP_free(mip);
        M_free(A);
        M_free(b);
        M_free(c);
        M_free(integer_mask);
        model_free(&model);
        return gap_accepted ? 0 : 1;
    }
    print_solution(&model, mip->_matrix_x, mip->values_opt, model.minimize);
    printf("nodes = %d\n", mip->node_num);
    printf("root_cuts = %d\n", mip->cut_num);
    write_mip_report(output_dir, path, &model, A, b, c, &options, mip);
    MIP_free(mip);
    M_free(A);
    M_free(b);
    M_free(c);
    M_free(integer_mask);
    model_free(&model);
    return 0;
}

static void print_solver_banner(void) {
    write_solver_banner(stdout);
}

static void print_usage(const char *program) {
    printf("Usage:\n");
    printf("  %s lp  path/to/model.lp [options]\n", program);
    printf("  %s mip path/to/model.lp [options]\n", program);
    printf("\nOptions:\n");
    printf("  --progress[=N]          Print progress every N iterations/nodes (default N=10).\n");
    printf("  --output-dir DIR        Write a human-readable result report into DIR.\n");
    printf("  --time-limit SECONDS    Max solve time; LP checks per iteration, MIP per node.\n");
    printf("  --gap-limit VALUE       Stop when LP/MIP gap is within VALUE.\n");
    printf("  --node-limit N          Maximum processed MIP nodes (default %d).\n",
           MIP_DEFAULT_NODE_LIMIT);
    printf("  --max-nodes N           Alias of --node-limit.\n");
    printf("  --open-limit N          Maximum queued open MIP nodes (default %d).\n",
           MIP_DEFAULT_OPEN_NODE_LIMIT);
    printf("  --method bb|cut-bb      Branch-and-bound only, or root cuts + B&B.\n");
    printf("  --cuts off|N            Disable root cuts or set root cut rounds.\n");
    printf("  --heuristic off|root|N  Run primal heuristic at root and every N nodes.\n");
    printf("  --repair-limit N        Max repair steps inside the primal heuristic.\n");
    printf("  --branch first|most     Branch on first or most fractional integer var.\n");
    printf("  --node-select dfs|best-bound\n");
    printf("                           Select open nodes by depth-first or best bound.\n");
    printf("  --bound-mode parent|tight\n");
    printf("                           Parent bound is faster; tight solves child LP before queuing.\n");
    printf("  --feas-tol VALUE        Override scale-relative feasibility tolerance.\n");
    printf("  --int-tol VALUE         Override integer acceptance tolerance.\n");
}

int main(int argc, char **argv) {
    const char *mode = NULL;
    const char *path = NULL;
    ExampleSolverOptions options;
    int i;
    print_solver_banner();
    for (i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
            print_usage(argv[0]);
            return 0;
        }
    }
    if ((argc < 3) || !parse_cli_options(argc, argv, &mode, &path, &options)) {
        print_usage(argv[0]);
        return 1;
    }
    if (strcmp(mode, "lp") == 0) {
        return solve_lp_file(path, &options.lp_options, options.output_dir);
    }
    if (strcmp(mode, "mip") == 0) {
        return solve_mip_file(path, &options.mip_options, options.output_dir);
    }
    print_usage(argv[0]);
    return 1;
}
