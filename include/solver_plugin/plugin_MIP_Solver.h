/*
\ \     __  ___     __      _       __ __     __
 \ \   /  |/  /__ _/ /_____(_)_ __ / // /_ __/ /
 / /  / /|_/ / _ `/ __/ __/ /\ \ // _  / // / _ \
/ /  /_/  /_/\_,_/\__/_/ /_//_\_\/_//_/\_,_/_.__/
* [INFORMATION]
    MATRIX_HUB
    AUTHOR: Xiping.Yu
    GITHUB: https://github.com/Amoiensis/Matrix_hub
    VERSION: 2.0.0
    CASE: Matrix Solver Plugin (C)
    DETAILS: Teaching MIP solver plugin with simple cuts and branch-and-bound.
    LICENSE: Apache-2.0
*/

#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#if !defined(_WIN32)
#include <sys/time.h>
#endif
#include "../matrix.h"
#include "./plugin_LP_Sover.h"

#define MIP_STATUS_OPTIMAL 0
#define MIP_STATUS_INFEASIBLE 1
#define MIP_STATUS_NODE_LIMIT 2
#define MIP_STATUS_UNBOUNDED 3
#define MIP_STATUS_INPUT_ERROR 4
#define MIP_STATUS_TIME_LIMIT 5
#define MIP_STATUS_GAP_LIMIT 6

#define MIP_METHOD_BRANCH_BOUND 1
#define MIP_METHOD_CUT_BRANCH_BOUND 2

#define MIP_BRANCH_FIRST_FRACTIONAL 1
#define MIP_BRANCH_MOST_FRACTIONAL 2

#define MIP_NODE_DEPTH_FIRST 1
#define MIP_NODE_BEST_BOUND 2

#define MIP_BOUND_PARENT 1
#define MIP_BOUND_TIGHT 2

#define MIP_DEFAULT_NODE_LIMIT 1024
#define MIP_DEFAULT_OPEN_NODE_LIMIT 1024
#define MIP_DEFAULT_ROOT_CUT_ROUNDS 1
#define MIP_DEFAULT_HEURISTIC_FREQUENCY 100
#define MIP_MAX_NODES MIP_DEFAULT_NODE_LIMIT
#define MIP_INF 1e100

typedef struct _matrix_MIP_result_struct {
    /* Store MIP solver result.
     * _matrix_x is the best integer solution for the original variables.
     * node_num and cut_num are kept visible for progress reporting and debugging. */
    Matrix *_matrix_x;
    Matrix *_matrix_c;
    Matrix *_matrix_integer;
    int OPT_STATUS;
    MATRIX_TYPE values_opt;
    int node_num;
    int cut_num;
} M_MIP_struct;

typedef struct _mip_node_struct {
    MATRIX_TYPE *lower;
    MATRIX_TYPE *upper;
    Matrix *relax_x;
    MATRIX_TYPE bound;
    int depth;
    int has_relaxation;
} MIP_Node;

typedef struct _mip_options_struct {
    /* Runtime controls for the lightweight MIP solver.
     * Nonpositive limits are disabled; nonpositive tolerances use defaults. */
    int method;
    int node_limit;
    int open_node_limit;
    int progress_interval;
    int root_cut_rounds;
    int heuristic_frequency;
    int repair_limit;
    int branch_rule;
    int node_selection;
    int bound_mode;
    MATRIX_TYPE feasibility_tol;
    MATRIX_TYPE integer_tol;
    MATRIX_TYPE objective_display_sign;
    MATRIX_TYPE time_limit_seconds;
    MATRIX_TYPE gap_limit;
} MIP_Options;

MIP_Options MIP_Default_Options(void);
M_MIP_struct *MIP_Solver(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                         Matrix *integer_mask, int MIP_method);
M_MIP_struct *MIP_Solver_With_Progress(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                       Matrix *integer_mask, int MIP_method,
                                       int progress_interval);
M_MIP_struct *MIP_Solver_With_Options(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                      Matrix *integer_mask,
                                      const MIP_Options *options);
int MIP_free(M_MIP_struct *mip);

static MIP_Options MIP_normalized_options(const MIP_Options *options);
static double MIP_now_seconds(void);
static int MIP_time_limit_reached(double start_seconds,
                                  MATRIX_TYPE time_limit_seconds);
static MATRIX_TYPE MIP_tolerance(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c);
static MATRIX_TYPE MIP_b_get(Matrix *mat_b, int index);
static MATRIX_TYPE MIP_c_get(Matrix *mat_c, int index);
static int MIP_is_integer_var(Matrix *integer_mask, int index, int n);
static int MIP_validate_input(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                              Matrix *integer_mask);
static MIP_Node MIP_make_node(int n);
static void MIP_free_node(MIP_Node *node);
static int MIP_copy_bounds(MIP_Node *dst, MIP_Node *src, int n);
static int MIP_apply_bound_cuts(Matrix *mat_A, Matrix *mat_b, Matrix *integer_mask,
                                MATRIX_TYPE *upper, MATRIX_TYPE tol);
static int MIP_build_lp_relaxation(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                   const MATRIX_TYPE *lower, const MATRIX_TYPE *upper,
                                   MATRIX_TYPE tol,
                                   Matrix **lp_A, Matrix **lp_b, Matrix **lp_c,
                                   MATRIX_TYPE *objective_shift);
static int MIP_solve_relaxation(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                const MATRIX_TYPE *lower, const MATRIX_TYPE *upper,
                                MATRIX_TYPE tol, int lp_progress_interval,
                                Matrix **solution,
                                MATRIX_TYPE *objective, int *status);
static int MIP_prepare_child_node(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                  Matrix *integer_mask, MIP_Node *child,
                                  MATRIX_TYPE tol, MATRIX_TYPE integer_tol,
                                  int branch_rule, Matrix **best_x,
                                  MATRIX_TYPE *best_value, int *status,
                                  int *numerical_skip_count,
                                  MATRIX_TYPE *skipped_bound);
static int MIP_find_branch_var(Matrix *x, Matrix *integer_mask,
                               MATRIX_TYPE tol, int branch_rule);
static MIP_Node MIP_pop_node(MIP_Node *stack, int *stack_size, int node_selection);
static MATRIX_TYPE MIP_solution_objective(Matrix *mat_c, Matrix *x);
static int MIP_solution_feasible(Matrix *mat_A, Matrix *mat_b, Matrix *x,
                                 Matrix *integer_mask, const MATRIX_TYPE *lower,
                                 const MATRIX_TYPE *upper, MATRIX_TYPE tol,
                                 MATRIX_TYPE integer_tol);
static void MIP_polish_solution(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                Matrix *integer_mask, const MATRIX_TYPE *lower,
                                const MATRIX_TYPE *upper, Matrix *x,
                                MATRIX_TYPE tol, MATRIX_TYPE integer_tol,
                                int polish_limit);
static Matrix *MIP_round_repair_heuristic(Matrix *mat_A, Matrix *mat_b,
                                          Matrix *mat_c, Matrix *integer_mask,
                                          const MATRIX_TYPE *lower,
                                          const MATRIX_TYPE *upper,
                                          Matrix *x_relax,
                                          MATRIX_TYPE tol,
                                          MATRIX_TYPE integer_tol,
                                          int repair_limit,
                                          MATRIX_TYPE *objective);
static Matrix *MIP_cover_greedy_heuristic(Matrix *mat_A, Matrix *mat_b,
                                          Matrix *mat_c, Matrix *integer_mask,
                                          const MATRIX_TYPE *lower,
                                          const MATRIX_TYPE *upper,
                                          MATRIX_TYPE tol,
                                          MATRIX_TYPE integer_tol,
                                          int repair_limit,
                                          MATRIX_TYPE *objective);
static MATRIX_TYPE MIP_stack_bound(MIP_Node *stack, int stack_size);
static int MIP_bound_is_finite(MATRIX_TYPE bound);
static MATRIX_TYPE MIP_relative_gap(int has_incumbent,
                                    MATRIX_TYPE incumbent,
                                    MATRIX_TYPE bound);
static void MIP_print_progress(int node_count, int open_nodes, int cut_count,
                               int has_incumbent, MATRIX_TYPE incumbent,
                               MATRIX_TYPE bound,
                               MATRIX_TYPE objective_display_sign,
                               const char *event);

int MIP_free(M_MIP_struct *mip) {
    if (mip == NULL) {
        return 0;
    }
    if (mip->_matrix_x != NULL) {
        M_free(mip->_matrix_x);
    }
    if (mip->_matrix_c != NULL) {
        M_free(mip->_matrix_c);
    }
    if (mip->_matrix_integer != NULL) {
        M_free(mip->_matrix_integer);
    }
    free(mip);
    return 0;
}

MIP_Options MIP_Default_Options(void) {
    MIP_Options options;
    options.method = MIP_METHOD_CUT_BRANCH_BOUND;
    options.node_limit = MIP_DEFAULT_NODE_LIMIT;
    options.open_node_limit = MIP_DEFAULT_OPEN_NODE_LIMIT;
    options.progress_interval = 0;
    options.root_cut_rounds = MIP_DEFAULT_ROOT_CUT_ROUNDS;
    options.heuristic_frequency = MIP_DEFAULT_HEURISTIC_FREQUENCY;
    options.repair_limit = 0;
    options.branch_rule = MIP_BRANCH_MOST_FRACTIONAL;
    options.node_selection = MIP_NODE_BEST_BOUND;
    options.bound_mode = MIP_BOUND_PARENT;
    options.feasibility_tol = 0;
    options.integer_tol = 0;
    options.objective_display_sign = 1;
    options.time_limit_seconds = 0;
    options.gap_limit = 0;
    return options;
}

static MIP_Options MIP_normalized_options(const MIP_Options *options) {
    MIP_Options normalized = MIP_Default_Options();
    if (options != NULL) {
        normalized = *options;
    }
    if ((normalized.method != MIP_METHOD_BRANCH_BOUND) &&
        (normalized.method != MIP_METHOD_CUT_BRANCH_BOUND)) {
        normalized.method = MIP_METHOD_CUT_BRANCH_BOUND;
    }
    if (normalized.node_limit <= 0) {
        normalized.node_limit = MIP_DEFAULT_NODE_LIMIT;
    }
    if (normalized.open_node_limit <= 0) {
        normalized.open_node_limit = MIP_DEFAULT_OPEN_NODE_LIMIT;
    }
    if (normalized.progress_interval < 0) {
        normalized.progress_interval = 0;
    }
    if (normalized.root_cut_rounds < 0) {
        normalized.root_cut_rounds = MIP_DEFAULT_ROOT_CUT_ROUNDS;
    }
    if (normalized.method == MIP_METHOD_BRANCH_BOUND) {
        normalized.root_cut_rounds = 0;
    }
    if (normalized.heuristic_frequency < 0) {
        normalized.heuristic_frequency = MIP_DEFAULT_HEURISTIC_FREQUENCY;
    }
    if (normalized.repair_limit < 0) {
        normalized.repair_limit = 0;
    }
    if ((normalized.branch_rule != MIP_BRANCH_FIRST_FRACTIONAL) &&
        (normalized.branch_rule != MIP_BRANCH_MOST_FRACTIONAL)) {
        normalized.branch_rule = MIP_BRANCH_MOST_FRACTIONAL;
    }
    if ((normalized.node_selection != MIP_NODE_DEPTH_FIRST) &&
        (normalized.node_selection != MIP_NODE_BEST_BOUND)) {
        normalized.node_selection = MIP_NODE_BEST_BOUND;
    }
    if ((normalized.bound_mode != MIP_BOUND_PARENT) &&
        (normalized.bound_mode != MIP_BOUND_TIGHT)) {
        normalized.bound_mode = MIP_BOUND_PARENT;
    }
    normalized.objective_display_sign =
        (normalized.objective_display_sign < 0) ? -1 : 1;
    if (normalized.time_limit_seconds < 0) {
        normalized.time_limit_seconds = 0;
    }
    if (normalized.gap_limit < 0) {
        normalized.gap_limit = 0;
    }
    return normalized;
}

static double MIP_now_seconds(void) {
#if defined(_WIN32)
    return (double)clock() / (double)CLOCKS_PER_SEC;
#else
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == 0) {
        return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
    }
    return (double)clock() / (double)CLOCKS_PER_SEC;
#endif
}

static int MIP_time_limit_reached(double start_seconds,
                                  MATRIX_TYPE time_limit_seconds) {
    if (time_limit_seconds <= 0) {
        return 0;
    }
    return (MIP_now_seconds() - start_seconds) >= (double)time_limit_seconds;
}

static MATRIX_TYPE MIP_tolerance(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c) {
    MATRIX_TYPE scale = M_fro_norm_raw(mat_A);
    MATRIX_TYPE b_scale = M_fro_norm_raw(mat_b);
    MATRIX_TYPE c_scale = M_fro_norm_raw(mat_c);
    if (b_scale > scale) {
        scale = b_scale;
    }
    if (c_scale > scale) {
        scale = c_scale;
    }
    return M_relative_tolerance(scale);
}

static MATRIX_TYPE MIP_b_get(Matrix *mat_b, int index) {
    return mat_b->data[index];
}

static MATRIX_TYPE MIP_c_get(Matrix *mat_c, int index) {
    if (mat_c->row == 1) {
        return mat_c->data[index];
    }
    return mat_c->data[index * mat_c->column];
}

static int MIP_is_integer_var(Matrix *integer_mask, int index, int n) {
    if (integer_mask == NULL) {
        return 1;
    }
    if ((integer_mask->row == 1) && (integer_mask->column == n)) {
        return fabs(integer_mask->data[index]) > 0.5;
    }
    if ((integer_mask->column == 1) && (integer_mask->row == n)) {
        return fabs(integer_mask->data[index]) > 0.5;
    }
    return 0;
}

static int MIP_validate_input(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                              Matrix *integer_mask) {
    int n;
    if ((mat_A == NULL) || (mat_b == NULL) || (mat_c == NULL)) {
        return 0;
    }
    n = mat_A->column;
    if ((mat_b->row != mat_A->row || mat_b->column != 1) &&
        (mat_b->row != 1 || mat_b->column != mat_A->row)) {
        return 0;
    }
    if ((mat_c->row != 1 || mat_c->column != n) &&
        (mat_c->row != n || mat_c->column != 1)) {
        return 0;
    }
    if ((integer_mask != NULL) &&
        !((integer_mask->row == 1 && integer_mask->column == n) ||
          (integer_mask->row == n && integer_mask->column == 1))) {
        return 0;
    }
    return 1;
}

static MIP_Node MIP_make_node(int n) {
    MIP_Node node;
    int i;
    node.lower = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    node.upper = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    node.relax_x = NULL;
    node.bound = MIP_INF;
    node.depth = 0;
    node.has_relaxation = 0;
    if ((node.lower == NULL) || (node.upper == NULL)) {
        free(node.lower);
        free(node.upper);
        node.lower = NULL;
        node.upper = NULL;
        node.relax_x = NULL;
        node.bound = -MIP_INF;
        node.has_relaxation = 0;
        return node;
    }
    for (i = 0; i < n; i++) {
        node.lower[i] = 0;
        node.upper[i] = MIP_INF;
    }
    return node;
}

static void MIP_free_node(MIP_Node *node) {
    if (node == NULL) {
        return;
    }
    free(node->lower);
    free(node->upper);
    M_free(node->relax_x);
    node->lower = NULL;
    node->upper = NULL;
    node->relax_x = NULL;
    node->bound = -MIP_INF;
    node->depth = 0;
    node->has_relaxation = 0;
}

static int MIP_copy_bounds(MIP_Node *dst, MIP_Node *src, int n) {
    int i;
    dst->lower = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    dst->upper = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    dst->relax_x = NULL;
    dst->bound = src->bound;
    dst->depth = src->depth + 1;
    dst->has_relaxation = 0;
    if ((dst->lower == NULL) || (dst->upper == NULL)) {
        MIP_free_node(dst);
        return 0;
    }
    for (i = 0; i < n; i++) {
        dst->lower[i] = src->lower[i];
        dst->upper[i] = src->upper[i];
    }
    return 1;
}

static int MIP_apply_bound_cuts(Matrix *mat_A, Matrix *mat_b, Matrix *integer_mask,
                                MATRIX_TYPE *upper, MATRIX_TYPE tol) {
    /* Simple valid bound cuts:
     * If a row has nonnegative coefficients and x >= 0, then each integer
     * variable has x_i <= floor(b / a_i). This compact separator keeps the
     * branch-and-bound flow easy to inspect and extend. */
    int row, col, k;
    int cut_count = 0;
    for (row = 0; row < mat_A->row; row++) {
        int nonnegative_row = MIP_b_get(mat_b, row) >= -tol;
        for (k = 0; k < mat_A->column; k++) {
            if (mat_A->data[row * mat_A->column + k] < -tol) {
                nonnegative_row = 0;
                break;
            }
        }
        if (!nonnegative_row) {
            continue;
        }
        for (col = 0; col < mat_A->column; col++) {
            MATRIX_TYPE a = mat_A->data[row * mat_A->column + col];
            if (!MIP_is_integer_var(integer_mask, col, mat_A->column) || (a <= tol)) {
                continue;
            }
            {
                MATRIX_TYPE bound = floor(MIP_b_get(mat_b, row) / a + tol);
                if (bound < upper[col]) {
                    upper[col] = bound;
                    cut_count++;
                }
            }
        }
    }
    return cut_count;
}

static int MIP_build_lp_relaxation(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                   const MATRIX_TYPE *lower, const MATRIX_TYPE *upper,
                                   MATRIX_TYPE tol,
                                   Matrix **lp_A, Matrix **lp_b, Matrix **lp_c,
                                   MATRIX_TYPE *objective_shift) {
    int m = mat_A->row;
    int n = mat_A->column;
    int upper_rows = 0;
    int total_rows;
    int total_cols;
    int row, col;
    Matrix *A = NULL;
    Matrix *b = NULL;
    Matrix *c = NULL;

    *lp_A = NULL;
    *lp_b = NULL;
    *lp_c = NULL;
    *objective_shift = 0;
    for (col = 0; col < n; col++) {
        if (upper[col] < MIP_INF / 2) {
            upper_rows++;
        }
        *objective_shift += MIP_c_get(mat_c, col) * lower[col];
    }

    total_rows = m + upper_rows;
    total_cols = n + total_rows;
    A = M_Zeros(total_rows, total_cols);
    b = M_Zeros(total_rows, 1);
    c = M_Zeros(1, total_cols);
    if ((A == NULL) || (b == NULL) || (c == NULL)) {
        M_free(A);
        M_free(b);
        M_free(c);
        return 0;
    }

    for (row = 0; row < m; row++) {
        MATRIX_TYPE rhs = MIP_b_get(mat_b, row);
        for (col = 0; col < n; col++) {
            MATRIX_TYPE value = mat_A->data[row * n + col];
            A->data[row * total_cols + col] = value;
            rhs -= value * lower[col];
        }
        b->data[row] = (fabs(rhs) <= tol) ? 0 : rhs;
        A->data[row * total_cols + n + row] = 1;
    }

    row = m;
    for (col = 0; col < n; col++) {
        c->data[col] = MIP_c_get(mat_c, col);
        if (upper[col] < MIP_INF / 2) {
            MATRIX_TYPE rhs = upper[col] - lower[col];
            if (rhs < -tol) {
                M_free(A);
                M_free(b);
                M_free(c);
                return 0;
            }
            A->data[row * total_cols + col] = 1;
            A->data[row * total_cols + n + row] = 1;
            b->data[row] = (fabs(rhs) <= tol) ? 0 : rhs;
            row++;
        }
    }

    *lp_A = A;
    *lp_b = b;
    *lp_c = c;
    return 1;
}

static int MIP_solve_relaxation(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                const MATRIX_TYPE *lower, const MATRIX_TYPE *upper,
                                MATRIX_TYPE tol, int lp_progress_interval,
                                Matrix **solution,
                                MATRIX_TYPE *objective, int *status) {
    /* Solve the LP relaxation of one search node.
     * Bounds are handled by x = z + lower, z >= 0. Finite upper bounds become
     * extra rows z_i <= upper_i - lower_i before calling the LP plugin. */
    Matrix *lp_A = NULL, *lp_b = NULL, *lp_c = NULL;
    Matrix *x = NULL;
    M_LP_struct *lp = NULL;
    MATRIX_TYPE objective_shift = 0;
    int row, col, n = mat_A->column;

    *solution = NULL;
    *objective = 0;
    *status = MIP_STATUS_INPUT_ERROR;
    if (!MIP_build_lp_relaxation(mat_A, mat_b, mat_c, lower, upper, tol,
                                 &lp_A, &lp_b, &lp_c, &objective_shift)) {
        *status = MIP_STATUS_INFEASIBLE;
        return 0;
    }

    lp = LP_Solver_With_Progress(lp_A, lp_b, lp_c,
                                 LP_METHOD_REVISED_SIMPLEX, lp_progress_interval);
    if (lp == NULL) {
        M_free(lp_A);
        M_free(lp_b);
        M_free(lp_c);
        return 0;
    }
    if (lp->OPT_STATUS == LP_STATUS_UNBOUNDED) {
        *status = MIP_STATUS_UNBOUNDED;
        LP_free(lp);
        M_free(lp_A);
        M_free(lp_b);
        M_free(lp_c);
        return 0;
    }
    if (lp->OPT_STATUS == LP_STATUS_INFEASIBLE) {
        *status = MIP_STATUS_INFEASIBLE;
        LP_free(lp);
        M_free(lp_A);
        M_free(lp_b);
        M_free(lp_c);
        return 0;
    }
    if (lp->OPT_STATUS == LP_STATUS_ITER_LIMIT) {
        *status = MIP_STATUS_NODE_LIMIT;
        LP_free(lp);
        M_free(lp_A);
        M_free(lp_b);
        M_free(lp_c);
        return 0;
    }
    if (lp->OPT_STATUS != LP_STATUS_OPTIMAL) {
        *status = MIP_STATUS_INPUT_ERROR;
        LP_free(lp);
        M_free(lp_A);
        M_free(lp_b);
        M_free(lp_c);
        return 0;
    }

    x = M_Zeros(n, 1);
    if (x == NULL) {
        LP_free(lp);
        M_free(lp_A);
        M_free(lp_b);
        M_free(lp_c);
        return 0;
    }
    for (col = 0; col < n; col++) {
        x->data[col] = lower[col];
    }
    for (row = 0; row < lp->_matrix_base->column; row++) {
        int basis_index = (int)lp->_matrix_base->data[row];
        if ((basis_index >= 0) && (basis_index < n)) {
            x->data[basis_index] += lp->_matrix_b->data[row];
        }
    }

    *objective = objective_shift + lp->values_opt;
    *solution = x;
    *status = MIP_STATUS_OPTIMAL;
    LP_free(lp);
    M_free(lp_A);
    M_free(lp_b);
    M_free(lp_c);
    return 1;
}

static int MIP_prepare_child_node(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                  Matrix *integer_mask, MIP_Node *child,
                                  MATRIX_TYPE tol, MATRIX_TYPE integer_tol,
                                  int branch_rule, Matrix **best_x,
                                  MATRIX_TYPE *best_value, int *status,
                                  int *numerical_skip_count,
                                  MATRIX_TYPE *skipped_bound) {
    Matrix *child_x = NULL;
    MATRIX_TYPE child_value = 0;
    int child_status = MIP_STATUS_INPUT_ERROR;
    int fractional;

    if (!MIP_solve_relaxation(mat_A, mat_b, mat_c, child->lower, child->upper,
                              tol, 0, &child_x, &child_value,
                              &child_status)) {
        if (child_status == MIP_STATUS_INFEASIBLE) {
            MIP_free_node(child);
            return 0;
        }
        if ((child_status == MIP_STATUS_INPUT_ERROR) && (*best_x != NULL)) {
            if (numerical_skip_count != NULL) {
                (*numerical_skip_count)++;
            }
            if ((skipped_bound != NULL) && (child->bound > *skipped_bound)) {
                *skipped_bound = child->bound;
            }
            MIP_free_node(child);
            return 0;
        }
        *status = child_status;
        MIP_free_node(child);
        return -1;
    }

    child->bound = child_value;
    if ((*best_x != NULL) && (child_value <= *best_value + tol)) {
        M_free(child_x);
        MIP_free_node(child);
        return 0;
    }

    fractional = MIP_find_branch_var(child_x, integer_mask, integer_tol,
                                     branch_rule);
    if (fractional < 0) {
        if ((*best_x == NULL) || (child_value > *best_value + tol)) {
            M_free(*best_x);
            *best_x = child_x;
            *best_value = child_value;
            child_x = NULL;
            *status = MIP_STATUS_OPTIMAL;
        }
        M_free(child_x);
        MIP_free_node(child);
        return 0;
    }

    child->relax_x = child_x;
    child->has_relaxation = 1;
    return 1;
}

static int MIP_find_branch_var(Matrix *x, Matrix *integer_mask,
                               MATRIX_TYPE tol, int branch_rule) {
    int i;
    int best = -1;
    MATRIX_TYPE best_score = -1;
    for (i = 0; i < x->row; i++) {
        if (MIP_is_integer_var(integer_mask, i, x->row)) {
            MATRIX_TYPE value = x->data[i];
            MATRIX_TYPE rounded = floor(value + 0.5);
            MATRIX_TYPE distance = fabs(value - rounded);
            if (distance <= tol) {
                continue;
            }
            if (branch_rule == MIP_BRANCH_FIRST_FRACTIONAL) {
                return i;
            }
            {
                MATRIX_TYPE frac = value - floor(value);
                MATRIX_TYPE score = (frac <= 0.5) ? frac : 1 - frac;
                if (score > best_score + tol) {
                    best = i;
                    best_score = score;
                }
            }
        }
    }
    return best;
}

static MIP_Node MIP_pop_node(MIP_Node *stack, int *stack_size, int node_selection) {
    int last = *stack_size - 1;
    int selected = last;
    MIP_Node node;
    if (node_selection == MIP_NODE_BEST_BOUND) {
        int i;
        MATRIX_TYPE best_bound = stack[0].bound;
        selected = 0;
        for (i = 1; i < *stack_size; i++) {
            if (stack[i].bound > best_bound) {
                best_bound = stack[i].bound;
                selected = i;
            }
        }
    }
    node = stack[selected];
    if (selected != last) {
        stack[selected] = stack[last];
    }
    stack[last].lower = NULL;
    stack[last].upper = NULL;
    stack[last].relax_x = NULL;
    stack[last].bound = -MIP_INF;
    stack[last].depth = 0;
    stack[last].has_relaxation = 0;
    *stack_size = last;
    return node;
}

static MATRIX_TYPE MIP_solution_objective(Matrix *mat_c, Matrix *x) {
    int i;
    MATRIX_TYPE value = 0;
    for (i = 0; i < x->row; i++) {
        value += MIP_c_get(mat_c, i) * x->data[i];
    }
    return value;
}

static int MIP_solution_feasible(Matrix *mat_A, Matrix *mat_b, Matrix *x,
                                 Matrix *integer_mask, const MATRIX_TYPE *lower,
                                 const MATRIX_TYPE *upper, MATRIX_TYPE tol,
                                 MATRIX_TYPE integer_tol) {
    int row, col;
    int n = mat_A->column;
    MATRIX_TYPE check_tol = 100 * tol;
    if (check_tol < integer_tol) {
        check_tol = integer_tol;
    }
    for (col = 0; col < n; col++) {
        MATRIX_TYPE value = x->data[col];
        if (value < lower[col] - check_tol) {
            return 0;
        }
        if ((upper[col] < MIP_INF / 2) && (value > upper[col] + check_tol)) {
            return 0;
        }
        if (MIP_is_integer_var(integer_mask, col, n)) {
            MATRIX_TYPE rounded = floor(value + 0.5);
            if (fabs(value - rounded) > integer_tol) {
                return 0;
            }
        }
    }
    for (row = 0; row < mat_A->row; row++) {
        MATRIX_TYPE activity = 0;
        for (col = 0; col < n; col++) {
            activity += mat_A->data[row * n + col] * x->data[col];
        }
        if (activity > MIP_b_get(mat_b, row) + check_tol) {
            return 0;
        }
    }
    return 1;
}

static void MIP_polish_solution(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                Matrix *integer_mask, const MATRIX_TYPE *lower,
                                const MATRIX_TYPE *upper, Matrix *x,
                                MATRIX_TYPE tol, MATRIX_TYPE integer_tol,
                                int polish_limit) {
    int step, col, row;
    int n = mat_A->column;
    MATRIX_TYPE *activity = NULL;
    if (polish_limit <= 0) {
        polish_limit = 5 * n + 100;
    }
    activity = (MATRIX_TYPE *)calloc((size_t)mat_A->row, sizeof(MATRIX_TYPE));
    if (activity == NULL) {
        return;
    }
    for (row = 0; row < mat_A->row; row++) {
        for (col = 0; col < n; col++) {
            activity[row] += mat_A->data[row * n + col] * x->data[col];
        }
    }
    for (step = 0; step < polish_limit; step++) {
        int best_col = -1;
        int best_delta = 0;
        MATRIX_TYPE best_gain = tol;
        for (col = 0; col < n; col++) {
            int delta_index;
            if (!MIP_is_integer_var(integer_mask, col, n)) {
                continue;
            }
            for (delta_index = 0; delta_index < 2; delta_index++) {
                int delta = (delta_index == 0) ? -1 : 1;
                MATRIX_TYPE new_value = x->data[col] + delta;
                MATRIX_TYPE gain = MIP_c_get(mat_c, col) * delta;
                int feasible = 1;
                if (gain <= best_gain) {
                    continue;
                }
                if (new_value < ceil(lower[col] - integer_tol)) {
                    continue;
                }
                if ((upper[col] < MIP_INF / 2) &&
                    (new_value > floor(upper[col] + integer_tol))) {
                    continue;
                }
                for (row = 0; row < mat_A->row; row++) {
                    MATRIX_TYPE new_activity = activity[row] +
                        mat_A->data[row * n + col] * delta;
                    if (new_activity > MIP_b_get(mat_b, row) + 100 * tol) {
                        feasible = 0;
                        break;
                    }
                }
                if (feasible) {
                    best_col = col;
                    best_delta = delta;
                    best_gain = gain;
                }
            }
        }
        if (best_col < 0) {
            break;
        }
        x->data[best_col] += best_delta;
        for (row = 0; row < mat_A->row; row++) {
            activity[row] += mat_A->data[row * n + best_col] * best_delta;
        }
    }
    free(activity);
}

static Matrix *MIP_round_repair_heuristic(Matrix *mat_A, Matrix *mat_b,
                                          Matrix *mat_c, Matrix *integer_mask,
                                          const MATRIX_TYPE *lower,
                                          const MATRIX_TYPE *upper,
                                          Matrix *x_relax,
                                          MATRIX_TYPE tol,
                                          MATRIX_TYPE integer_tol,
                                          int repair_limit,
                                          MATRIX_TYPE *objective) {
    int mode;
    int m = mat_A->row;
    int n = mat_A->column;
    int max_steps = repair_limit;
    if (max_steps <= 0) {
        max_steps = 20 * n + 5 * m + 100;
    }

    for (mode = 0; mode < 4; mode++) {
        int row, col, step;
        Matrix *x = M_Zeros(n, 1);
        MATRIX_TYPE *activity = NULL;
        if (x == NULL) {
            return NULL;
        }
        for (col = 0; col < n; col++) {
            MATRIX_TYPE value = x_relax->data[col];
            if (mode == 3) {
                value = lower[col];
            }
            if (MIP_is_integer_var(integer_mask, col, n)) {
                if (mode == 0) {
                    value = floor(value + 0.5);
                } else if (mode == 1) {
                    value = floor(value);
                } else if (mode == 2) {
                    value = ceil(value);
                } else {
                    value = ceil(value - integer_tol);
                }
                if (value < ceil(lower[col] - integer_tol)) {
                    value = ceil(lower[col] - integer_tol);
                }
                if ((upper[col] < MIP_INF / 2) &&
                    (value > floor(upper[col] + integer_tol))) {
                    value = floor(upper[col] + integer_tol);
                }
            } else {
                if (value < lower[col]) {
                    value = lower[col];
                }
                if ((upper[col] < MIP_INF / 2) && (value > upper[col])) {
                    value = upper[col];
                }
            }
            x->data[col] = value;
        }

        if (MIP_solution_feasible(mat_A, mat_b, x, integer_mask,
                                  lower, upper, tol, integer_tol)) {
            *objective = MIP_solution_objective(mat_c, x);
            return x;
        }

        activity = (MATRIX_TYPE *)calloc((size_t)m, sizeof(MATRIX_TYPE));
        if (activity == NULL) {
            M_free(x);
            return NULL;
        }
        for (row = 0; row < m; row++) {
            for (col = 0; col < n; col++) {
                activity[row] += mat_A->data[row * n + col] * x->data[col];
            }
        }

        for (step = 0; step < max_steps; step++) {
            MATRIX_TYPE total_violation = 0;
            MATRIX_TYPE max_violation = 0;
            int max_row = -1;
            MATRIX_TYPE best_reduction = 0;
            MATRIX_TYPE best_objective_change = -MIP_INF;
            int best_col = -1;
            int best_delta = 0;

            for (row = 0; row < m; row++) {
                MATRIX_TYPE violation = activity[row] - MIP_b_get(mat_b, row);
                if (violation > tol) {
                    total_violation += violation;
                    if (violation > max_violation) {
                        max_violation = violation;
                        max_row = row;
                    }
                }
            }
            if (total_violation <= 100 * tol &&
                MIP_solution_feasible(mat_A, mat_b, x, integer_mask,
                                      lower, upper, tol, integer_tol)) {
                free(activity);
                *objective = MIP_solution_objective(mat_c, x);
                return x;
            }

            for (col = 0; col < n; col++) {
                int delta_index;
                if (!MIP_is_integer_var(integer_mask, col, n)) {
                    continue;
                }
                for (delta_index = 0; delta_index < 2; delta_index++) {
                    int delta = (delta_index == 0) ? -1 : 1;
                    MATRIX_TYPE new_value = x->data[col] + delta;
                    MATRIX_TYPE reduction = 0;
                    MATRIX_TYPE objective_change;
                    if (new_value < ceil(lower[col] - integer_tol)) {
                        continue;
                    }
                    if ((upper[col] < MIP_INF / 2) &&
                        (new_value > floor(upper[col] + integer_tol))) {
                        continue;
                    }
                    for (row = 0; row < m; row++) {
                        MATRIX_TYPE old_violation =
                            activity[row] - MIP_b_get(mat_b, row);
                        MATRIX_TYPE new_violation =
                            old_violation + mat_A->data[row * n + col] * delta;
                        if (old_violation < 0) {
                            old_violation = 0;
                        }
                        if (new_violation < 0) {
                            new_violation = 0;
                        }
                        reduction += old_violation - new_violation;
                    }
                    objective_change = MIP_c_get(mat_c, col) * delta;
                    if ((reduction > best_reduction + tol) ||
                        ((fabs(reduction - best_reduction) <= tol) &&
                         (reduction > tol) &&
                         (objective_change > best_objective_change))) {
                        best_reduction = reduction;
                        best_objective_change = objective_change;
                        best_col = col;
                        best_delta = delta;
                    }
                }
            }

            if (best_col < 0) {
                MATRIX_TYPE best_total_after = MIP_INF;
                if (max_row < 0) {
                    break;
                }
                for (col = 0; col < n; col++) {
                    int delta;
                    MATRIX_TYPE a = mat_A->data[max_row * n + col];
                    MATRIX_TYPE new_value;
                    MATRIX_TYPE total_after = 0;
                    MATRIX_TYPE objective_change;
                    if (!MIP_is_integer_var(integer_mask, col, n) ||
                        (fabs(a) <= tol)) {
                        continue;
                    }
                    delta = (a > 0) ? -1 : 1;
                    new_value = x->data[col] + delta;
                    if (new_value < ceil(lower[col] - integer_tol)) {
                        continue;
                    }
                    if ((upper[col] < MIP_INF / 2) &&
                        (new_value > floor(upper[col] + integer_tol))) {
                        continue;
                    }
                    for (row = 0; row < m; row++) {
                        MATRIX_TYPE violation = activity[row] -
                            MIP_b_get(mat_b, row) +
                            mat_A->data[row * n + col] * delta;
                        if (violation > 0) {
                            total_after += violation;
                        }
                    }
                    objective_change = MIP_c_get(mat_c, col) * delta;
                    if ((total_after < best_total_after - tol) ||
                        ((fabs(total_after - best_total_after) <= tol) &&
                         (objective_change > best_objective_change))) {
                        best_total_after = total_after;
                        best_objective_change = objective_change;
                        best_col = col;
                        best_delta = delta;
                    }
                }
                if (best_col < 0) {
                    break;
                }
            }
            x->data[best_col] += best_delta;
            for (row = 0; row < m; row++) {
                activity[row] += mat_A->data[row * n + best_col] * best_delta;
            }
        }

        if (MIP_solution_feasible(mat_A, mat_b, x, integer_mask,
                                  lower, upper, tol, integer_tol)) {
            free(activity);
            *objective = MIP_solution_objective(mat_c, x);
            return x;
        }
        free(activity);
        M_free(x);
    }
    return NULL;
}

static Matrix *MIP_cover_greedy_heuristic(Matrix *mat_A, Matrix *mat_b,
                                          Matrix *mat_c, Matrix *integer_mask,
                                          const MATRIX_TYPE *lower,
                                          const MATRIX_TYPE *upper,
                                          MATRIX_TYPE tol,
                                          MATRIX_TYPE integer_tol,
                                          int repair_limit,
                                          MATRIX_TYPE *objective) {
    int row, col, step;
    int m = mat_A->row;
    int n = mat_A->column;
    int max_steps = repair_limit;
    Matrix *x = M_Zeros(n, 1);
    MATRIX_TYPE *activity = NULL;
    if (max_steps <= 0) {
        max_steps = 20 * n + 5 * m + 100;
    }
    if (x == NULL) {
        return NULL;
    }
    activity = (MATRIX_TYPE *)calloc((size_t)m, sizeof(MATRIX_TYPE));
    if (activity == NULL) {
        M_free(x);
        return NULL;
    }

    for (col = 0; col < n; col++) {
        MATRIX_TYPE value = lower[col];
        if (MIP_is_integer_var(integer_mask, col, n)) {
            value = ceil(value - integer_tol);
        }
        x->data[col] = value;
    }
    for (row = 0; row < m; row++) {
        for (col = 0; col < n; col++) {
            activity[row] += mat_A->data[row * n + col] * x->data[col];
        }
    }

    for (step = 0; step < max_steps; step++) {
        MATRIX_TYPE total_violation = 0;
        MATRIX_TYPE best_score = -MIP_INF;
        MATRIX_TYPE best_cover = 0;
        MATRIX_TYPE best_objective_change = -MIP_INF;
        int best_col = -1;

        if (MIP_solution_feasible(mat_A, mat_b, x, integer_mask,
                                  lower, upper, tol, integer_tol)) {
            free(activity);
            *objective = MIP_solution_objective(mat_c, x);
            return x;
        }

        for (row = 0; row < m; row++) {
            MATRIX_TYPE violation = activity[row] - MIP_b_get(mat_b, row);
            if (violation > tol) {
                total_violation += violation;
            }
        }
        if (total_violation <= 100 * tol) {
            break;
        }

        for (col = 0; col < n; col++) {
            MATRIX_TYPE cover = 0;
            MATRIX_TYPE packing_over = 0;
            MATRIX_TYPE objective_change = MIP_c_get(mat_c, col);
            MATRIX_TYPE penalty = (objective_change < 0) ? -objective_change : 0;
            MATRIX_TYPE score;

            if (!MIP_is_integer_var(integer_mask, col, n)) {
                continue;
            }
            if ((upper[col] < MIP_INF / 2) &&
                (x->data[col] + 1 > floor(upper[col] + integer_tol))) {
                continue;
            }

            for (row = 0; row < m; row++) {
                MATRIX_TYPE a = mat_A->data[row * n + col];
                MATRIX_TYPE old_violation = activity[row] - MIP_b_get(mat_b, row);
                MATRIX_TYPE new_violation = old_violation + a;
                if (old_violation > tol) {
                    if (new_violation < 0) {
                        new_violation = 0;
                    }
                    cover += old_violation - new_violation;
                } else if ((a > tol) && (new_violation > tol)) {
                    packing_over += new_violation;
                }
            }
            if (cover <= tol) {
                continue;
            }
            score = cover / (1 + penalty + 1000 * packing_over);
            if ((score > best_score + tol) ||
                ((fabs(score - best_score) <= tol) &&
                 ((cover > best_cover + tol) ||
                  ((fabs(cover - best_cover) <= tol) &&
                   (objective_change > best_objective_change))))) {
                best_score = score;
                best_cover = cover;
                best_objective_change = objective_change;
                best_col = col;
            }
        }

        if (best_col < 0) {
            break;
        }
        x->data[best_col] += 1;
        for (row = 0; row < m; row++) {
            activity[row] += mat_A->data[row * n + best_col];
        }
    }

    for (step = 0; step < max_steps; step++) {
        MATRIX_TYPE max_violation = 0;
        MATRIX_TYPE best_total_after = MIP_INF;
        MATRIX_TYPE best_objective_change = -MIP_INF;
        int max_row = -1;
        int best_col = -1;
        int best_delta = 0;

        if (MIP_solution_feasible(mat_A, mat_b, x, integer_mask,
                                  lower, upper, tol, integer_tol)) {
            MIP_polish_solution(mat_A, mat_b, mat_c, integer_mask,
                                lower, upper, x, tol, integer_tol,
                                max_steps);
            free(activity);
            *objective = MIP_solution_objective(mat_c, x);
            return x;
        }

        for (row = 0; row < m; row++) {
            MATRIX_TYPE violation = activity[row] - MIP_b_get(mat_b, row);
            if (violation > max_violation) {
                max_violation = violation;
                max_row = row;
            }
        }
        if (max_row < 0) {
            break;
        }

        for (col = 0; col < n; col++) {
            int delta_index;
            if (!MIP_is_integer_var(integer_mask, col, n)) {
                continue;
            }
            for (delta_index = 0; delta_index < 2; delta_index++) {
                int delta = (delta_index == 0) ? -1 : 1;
                MATRIX_TYPE new_value = x->data[col] + delta;
                MATRIX_TYPE total_after = 0;
                MATRIX_TYPE objective_change;
                if (mat_A->data[max_row * n + col] * delta >= -tol) {
                    continue;
                }
                if (new_value < ceil(lower[col] - integer_tol)) {
                    continue;
                }
                if ((upper[col] < MIP_INF / 2) &&
                    (new_value > floor(upper[col] + integer_tol))) {
                    continue;
                }
                for (row = 0; row < m; row++) {
                    MATRIX_TYPE violation = activity[row] -
                        MIP_b_get(mat_b, row) +
                        mat_A->data[row * n + col] * delta;
                    if (violation > 0) {
                        total_after += violation;
                    }
                }
                objective_change = MIP_c_get(mat_c, col) * delta;
                if ((total_after < best_total_after - tol) ||
                    ((fabs(total_after - best_total_after) <= tol) &&
                     (objective_change > best_objective_change))) {
                    best_total_after = total_after;
                    best_objective_change = objective_change;
                    best_col = col;
                    best_delta = delta;
                }
            }
        }

        if (best_col < 0) {
            break;
        }
        x->data[best_col] += best_delta;
        for (row = 0; row < m; row++) {
            activity[row] += mat_A->data[row * n + best_col] * best_delta;
        }
    }

    if (MIP_solution_feasible(mat_A, mat_b, x, integer_mask,
                              lower, upper, tol, integer_tol)) {
        MIP_polish_solution(mat_A, mat_b, mat_c, integer_mask,
                            lower, upper, x, tol, integer_tol, max_steps);
        free(activity);
        *objective = MIP_solution_objective(mat_c, x);
        return x;
    }
    free(activity);
    M_free(x);
    return NULL;
}

static MATRIX_TYPE MIP_stack_bound(MIP_Node *stack, int stack_size) {
    int i;
    MATRIX_TYPE bound = -MIP_INF;
    for (i = 0; i < stack_size; i++) {
        if (stack[i].bound > bound) {
            bound = stack[i].bound;
        }
    }
    return bound;
}

static int MIP_bound_is_finite(MATRIX_TYPE bound) {
    return (bound > -MIP_INF / 2) && (bound < MIP_INF / 2);
}

static MATRIX_TYPE MIP_relative_gap(int has_incumbent,
                                    MATRIX_TYPE incumbent,
                                    MATRIX_TYPE bound) {
    MATRIX_TYPE scale;
    if (!has_incumbent || !MIP_bound_is_finite(bound)) {
        return MIP_INF;
    }
    scale = fabs(incumbent);
    if (scale < 1) {
        scale = 1;
    }
    return fabs(bound - incumbent) / scale;
}

static void MIP_print_progress(int node_count, int open_nodes, int cut_count,
                               int has_incumbent, MATRIX_TYPE incumbent,
                               MATRIX_TYPE bound,
                               MATRIX_TYPE objective_display_sign,
                               const char *event) {
    int is_min_display = objective_display_sign < 0;
    int has_bound = MIP_bound_is_finite(bound);
    MATRIX_TYPE display_incumbent = is_min_display ? -incumbent : incumbent;
    MATRIX_TYPE lower_bound = 0;
    MATRIX_TYPE upper_bound = 0;

    if (is_min_display) {
        if (has_bound) {
            lower_bound = -bound;
        }
        if (has_incumbent) {
            upper_bound = display_incumbent;
        }
    } else {
        if (has_incumbent) {
            lower_bound = display_incumbent;
        }
        if (has_bound) {
            upper_bound = bound;
        }
    }

    if (has_incumbent && has_bound) {
        MATRIX_TYPE gap = MIP_relative_gap(has_incumbent, incumbent, bound);
        printf("[MIP] node=%d open=%d cuts=%d incumbent=%.10g lower_bound=%.10g upper_bound=%.10g gap=%.3g event=%s\n",
               node_count, open_nodes, cut_count, display_incumbent,
               lower_bound, upper_bound, gap, event);
    } else if (has_incumbent) {
        if (is_min_display) {
            printf("[MIP] node=%d open=%d cuts=%d incumbent=%.10g lower_bound=-inf upper_bound=%.10g gap=inf event=%s\n",
                   node_count, open_nodes, cut_count, display_incumbent,
                   upper_bound, event);
        } else {
            printf("[MIP] node=%d open=%d cuts=%d incumbent=%.10g lower_bound=%.10g upper_bound=inf gap=inf event=%s\n",
                   node_count, open_nodes, cut_count, display_incumbent,
                   lower_bound, event);
        }
    } else if (has_bound) {
        if (is_min_display) {
            printf("[MIP] node=%d open=%d cuts=%d incumbent=none lower_bound=%.10g upper_bound=none gap=inf event=%s\n",
                   node_count, open_nodes, cut_count, lower_bound, event);
        } else {
            printf("[MIP] node=%d open=%d cuts=%d incumbent=none lower_bound=none upper_bound=%.10g gap=inf event=%s\n",
                   node_count, open_nodes, cut_count, upper_bound, event);
        }
    } else {
        if (is_min_display) {
            printf("[MIP] node=%d open=%d cuts=%d incumbent=none lower_bound=-inf upper_bound=none gap=inf event=%s\n",
                   node_count, open_nodes, cut_count, event);
        } else {
            printf("[MIP] node=%d open=%d cuts=%d incumbent=none lower_bound=none upper_bound=inf gap=inf event=%s\n",
                   node_count, open_nodes, cut_count, event);
        }
    }
}

M_MIP_struct *MIP_Solver(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                         Matrix *integer_mask, int MIP_method) {
    return MIP_Solver_With_Progress(mat_A, mat_b, mat_c, integer_mask, MIP_method, 0);
}

M_MIP_struct *MIP_Solver_With_Progress(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                       Matrix *integer_mask, int MIP_method,
                                       int progress_interval) {
    MIP_Options options = MIP_Default_Options();
    options.method = MIP_method;
    options.progress_interval = progress_interval;
    return MIP_Solver_With_Options(mat_A, mat_b, mat_c, integer_mask, &options);
}

M_MIP_struct *MIP_Solver_With_Options(Matrix *mat_A, Matrix *mat_b, Matrix *mat_c,
                                      Matrix *integer_mask,
                                      const MIP_Options *options_in) {
    /*
     * Teaching MIP solver for:
     *     max c*x
     *     s.t. A*x <= b, x >= 0, selected x_i integer.
     *
     * Current framework:
     *     1. solve LP relaxation by the revised-simplex LP plugin;
     *     2. prune by relaxation bound;
     *     3. accept integer-feasible node as incumbent;
     *     4. branch deterministically on the first fractional integer value.
     *
     * MIP_METHOD_CUT_BRANCH_BOUND adds a small root-node cut pass. This keeps
     * the code readable while leaving clear hooks for stronger separators,
     * node selection, and branching rules later.
     */
    MIP_Options options = MIP_normalized_options(options_in);
    M_MIP_struct *result = NULL;
    MIP_Node *stack = NULL;
    int stack_size = 0;
    int node_count = 0;
    int cut_count = 0;
    int status = MIP_STATUS_INFEASIBLE;
    int abort_search = 0;
    int numerical_skip_count = 0;
    int n, fractional;
    MATRIX_TYPE tol;
    MATRIX_TYPE integer_tol;
    MATRIX_TYPE best_value = -MIP_INF;
    MATRIX_TYPE final_bound = MIP_INF;
    MATRIX_TYPE skipped_bound = -MIP_INF;
    double start_seconds = MIP_now_seconds();
    int final_open_nodes = 0;
    Matrix *best_x = NULL;

    if (!MIP_validate_input(mat_A, mat_b, mat_c, integer_mask)) {
        printf("[ERROR] MIP input should be A(m,n), b(m,1), c(1,n), integer mask(1,n).\n");
        return NULL;
    }

    n = mat_A->column;
    tol = (options.feasibility_tol > 0) ?
        options.feasibility_tol : MIP_tolerance(mat_A, mat_b, mat_c);
    integer_tol = (options.integer_tol > 0) ? options.integer_tol : sqrt(tol);
    if (integer_tol < tol) {
        integer_tol = tol;
    }
    stack = (MIP_Node *)calloc((size_t)options.open_node_limit, sizeof(MIP_Node));
    if (stack == NULL) {
        return NULL;
    }
    stack[stack_size++] = MIP_make_node(n);
    if (stack[0].lower == NULL) {
        free(stack);
        return NULL;
    }
    if ((options.method == MIP_METHOD_CUT_BRANCH_BOUND) &&
        (options.root_cut_rounds > 0)) {
        int round;
        for (round = 0; round < options.root_cut_rounds; round++) {
            int added = MIP_apply_bound_cuts(mat_A, mat_b, integer_mask,
                                            stack[0].upper, tol);
            cut_count += added;
            if (added == 0) {
                break;
            }
        }
    }
    if (options.progress_interval > 0) {
        MIP_print_progress(0, stack_size, cut_count, 0, 0, stack[0].bound,
                           options.objective_display_sign, "start");
    }

    while (stack_size > 0) {
        if (MIP_time_limit_reached(start_seconds, options.time_limit_seconds)) {
            MATRIX_TYPE current_bound = MIP_stack_bound(stack, stack_size);
            if (skipped_bound > current_bound) {
                current_bound = skipped_bound;
            }
            status = MIP_STATUS_TIME_LIMIT;
            if (options.progress_interval > 0) {
                MIP_print_progress(node_count, stack_size, cut_count, best_x != NULL,
                                   best_value, current_bound,
                                   options.objective_display_sign,
                                   "time_limit");
            }
            break;
        }
        if ((options.gap_limit > 0) &&
            (best_x != NULL)) {
            MATRIX_TYPE current_bound = MIP_stack_bound(stack, stack_size);
            if (skipped_bound > current_bound) {
                current_bound = skipped_bound;
            }
            if (MIP_relative_gap(1, best_value, current_bound) <= options.gap_limit) {
                status = MIP_STATUS_GAP_LIMIT;
                if (options.progress_interval > 0) {
                    MIP_print_progress(node_count, stack_size, cut_count, 1,
                                       best_value, current_bound,
                                       options.objective_display_sign,
                                       "gap_limit");
                }
                break;
            }
        }

        MIP_Node node = MIP_pop_node(stack, &stack_size, options.node_selection);
        Matrix *x_relax = NULL;
        MATRIX_TYPE relax_value = 0;
        MATRIX_TYPE global_bound;
        int relax_status = MIP_STATUS_INPUT_ERROR;
        if (node_count >= options.node_limit) {
            MATRIX_TYPE current_bound = MIP_stack_bound(stack, stack_size);
            if (skipped_bound > current_bound) {
                current_bound = skipped_bound;
            }
            status = MIP_STATUS_NODE_LIMIT;
            if (options.progress_interval > 0) {
                MIP_print_progress(node_count, stack_size, cut_count, best_x != NULL,
                                   best_value, current_bound,
                                   options.objective_display_sign, "node_limit");
            }
            MIP_free_node(&node);
            break;
        }
        node_count++;

        if ((options.progress_interval > 0) && ((node_count == 1) ||
            (node_count % options.progress_interval == 0))) {
            MATRIX_TYPE current_bound = MIP_stack_bound(stack, stack_size);
            if (skipped_bound > current_bound) {
                current_bound = skipped_bound;
            }
            if (node.bound > current_bound) {
                current_bound = node.bound;
            }
            MIP_print_progress(node_count, stack_size, cut_count, best_x != NULL,
                               best_value, current_bound,
                               options.objective_display_sign, "solve_relax");
        }

        if (node.has_relaxation && (node.relax_x != NULL)) {
            x_relax = node.relax_x;
            node.relax_x = NULL;
            node.has_relaxation = 0;
            relax_value = node.bound;
            relax_status = MIP_STATUS_OPTIMAL;
        } else if (!MIP_solve_relaxation(mat_A, mat_b, mat_c,
                                         node.lower, node.upper,
                                         tol, 0,
                                         &x_relax, &relax_value,
                                         &relax_status)) {
            if (relax_status == MIP_STATUS_UNBOUNDED) {
                status = MIP_STATUS_UNBOUNDED;
                abort_search = 1;
            } else if (relax_status == MIP_STATUS_NODE_LIMIT) {
                status = relax_status;
                abort_search = 1;
            } else if (relax_status == MIP_STATUS_INPUT_ERROR) {
                if (best_x != NULL) {
                    numerical_skip_count++;
                    if (node.bound > skipped_bound) {
                        skipped_bound = node.bound;
                    }
                } else {
                    status = relax_status;
                    abort_search = 1;
                }
            }
            if ((options.progress_interval > 0) && ((node_count == 1) ||
                (node_count % options.progress_interval == 0))) {
                const char *event = "lp_infeasible";
                MATRIX_TYPE current_bound = MIP_stack_bound(stack, stack_size);
                if (skipped_bound > current_bound) {
                    current_bound = skipped_bound;
                }
                if (node.bound > current_bound) {
                    current_bound = node.bound;
                }
                if (relax_status == MIP_STATUS_UNBOUNDED) {
                    event = "lp_unbounded";
                } else if (relax_status == MIP_STATUS_NODE_LIMIT) {
                    event = "lp_iter_limit";
                } else if (relax_status == MIP_STATUS_INPUT_ERROR) {
                    event = (best_x != NULL) ? "lp_numerical_skip" : "lp_error";
                }
                MIP_print_progress(node_count, stack_size, cut_count, best_x != NULL,
                                   best_value, current_bound,
                                   options.objective_display_sign, event);
            }
            MIP_free_node(&node);
            if (abort_search) {
                break;
            }
            continue;
        }

        global_bound = MIP_stack_bound(stack, stack_size);
        if (skipped_bound > global_bound) {
            global_bound = skipped_bound;
        }
        if (relax_value > global_bound) {
            global_bound = relax_value;
        }
        if ((options.progress_interval > 0) && ((node_count == 1) ||
            (node_count % options.progress_interval == 0))) {
            MIP_print_progress(node_count, stack_size, cut_count, best_x != NULL,
                               best_value, global_bound,
                               options.objective_display_sign, "relax");
        }

        if ((best_x != NULL) && (relax_value <= best_value + tol)) {
            M_free(x_relax);
            MIP_free_node(&node);
            continue;
        }

        if ((options.heuristic_frequency > 0) &&
            ((node_count == 1) ||
             (node_count % options.heuristic_frequency == 0))) {
            MATRIX_TYPE heuristic_value = 0;
            Matrix *heuristic_x = MIP_cover_greedy_heuristic(
                mat_A, mat_b, mat_c, integer_mask, node.lower, node.upper,
                tol, integer_tol, options.repair_limit, &heuristic_value);
            if (heuristic_x == NULL) {
                heuristic_x = MIP_round_repair_heuristic(
                    mat_A, mat_b, mat_c, integer_mask, node.lower, node.upper,
                    x_relax, tol, integer_tol, options.repair_limit,
                    &heuristic_value);
            }
            if (heuristic_x != NULL) {
                if ((best_x == NULL) || (heuristic_value > best_value + tol)) {
                    M_free(best_x);
                    best_x = heuristic_x;
                    best_value = heuristic_value;
                    status = MIP_STATUS_OPTIMAL;
                    if (options.progress_interval > 0) {
                        MIP_print_progress(node_count, stack_size, cut_count, 1,
                                           best_value,
                                           global_bound,
                                           options.objective_display_sign,
                                           "heuristic");
                    }
                } else {
                    M_free(heuristic_x);
                }
            }
            if ((best_x != NULL) && (relax_value <= best_value + tol)) {
                M_free(x_relax);
                MIP_free_node(&node);
                continue;
            }
        }

        fractional = MIP_find_branch_var(x_relax, integer_mask, integer_tol,
                                         options.branch_rule);
        if (fractional < 0) {
            if ((best_x == NULL) || (relax_value > best_value + tol)) {
                M_free(best_x);
                best_x = x_relax;
                best_value = relax_value;
                x_relax = NULL;
                status = MIP_STATUS_OPTIMAL;
                if (options.progress_interval > 0) {
                    MIP_print_progress(node_count, stack_size, cut_count, 1,
                                       best_value, global_bound,
                                       options.objective_display_sign,
                                       "incumbent");
                }
            }
            M_free(x_relax);
            MIP_free_node(&node);
            continue;
        }

        {
            MATRIX_TYPE value = x_relax->data[fractional];
            MATRIX_TYPE floor_value = floor(value);
            MATRIX_TYPE ceil_value = ceil(value);
            MIP_Node left, right;
            if ((options.progress_interval > 0) && ((node_count == 1) ||
                (node_count % options.progress_interval == 0))) {
                printf("[MIP] node=%d branch_var=%d value=%.10g left<=%.10g right>=%.10g\n",
                       node_count, fractional, value, floor_value, ceil_value);
            }
            if ((stack_size + 2) <= options.open_node_limit) {
                if (MIP_copy_bounds(&right, &node, n)) {
                    right.bound = relax_value;
                    right.lower[fractional] = ceil_value;
                    if (right.lower[fractional] <= right.upper[fractional] + tol) {
                        if (options.bound_mode == MIP_BOUND_TIGHT) {
                            int prepared = MIP_prepare_child_node(
                                mat_A, mat_b, mat_c, integer_mask, &right,
                                tol, integer_tol, options.branch_rule,
                                &best_x, &best_value, &status,
                                &numerical_skip_count, &skipped_bound);
                            if (prepared > 0) {
                                stack[stack_size++] = right;
                            } else if (prepared < 0) {
                                abort_search = 1;
                            }
                        } else {
                            stack[stack_size++] = right;
                        }
                    } else {
                        MIP_free_node(&right);
                    }
                }
                if (!abort_search && MIP_copy_bounds(&left, &node, n)) {
                    left.bound = relax_value;
                    left.upper[fractional] = floor_value;
                    if (left.lower[fractional] <= left.upper[fractional] + tol) {
                        if (options.bound_mode == MIP_BOUND_TIGHT) {
                            int prepared = MIP_prepare_child_node(
                                mat_A, mat_b, mat_c, integer_mask, &left,
                                tol, integer_tol, options.branch_rule,
                                &best_x, &best_value, &status,
                                &numerical_skip_count, &skipped_bound);
                            if (prepared > 0) {
                                stack[stack_size++] = left;
                            } else if (prepared < 0) {
                                abort_search = 1;
                            }
                        } else {
                            stack[stack_size++] = left;
                        }
                    } else {
                        MIP_free_node(&left);
                    }
                }
                if (abort_search) {
                    M_free(x_relax);
                    MIP_free_node(&node);
                    break;
                }
            } else {
                MATRIX_TYPE current_bound = MIP_stack_bound(stack, stack_size);
                if (skipped_bound > current_bound) {
                    current_bound = skipped_bound;
                }
                status = MIP_STATUS_NODE_LIMIT;
                if (options.progress_interval > 0) {
                    MIP_print_progress(node_count, stack_size, cut_count, best_x != NULL,
                                       best_value, current_bound,
                                       options.objective_display_sign,
                                       "open_limit");
                }
                M_free(x_relax);
                MIP_free_node(&node);
                break;
            }
        }
        M_free(x_relax);
        MIP_free_node(&node);
    }

    final_open_nodes = stack_size;
    if (stack_size > 0) {
        final_bound = MIP_stack_bound(stack, stack_size);
    } else if (numerical_skip_count > 0) {
        final_bound = skipped_bound;
    }
    if ((numerical_skip_count > 0) && (skipped_bound > final_bound)) {
        final_bound = skipped_bound;
    }
    while (stack_size > 0) {
        MIP_free_node(&stack[--stack_size]);
    }
    free(stack);
    if ((final_open_nodes == 0) && (best_x != NULL) &&
        (numerical_skip_count == 0)) {
        final_bound = best_value;
    }
    if ((status == MIP_STATUS_OPTIMAL) && (numerical_skip_count > 0)) {
        status = MIP_STATUS_INPUT_ERROR;
    }

    result = (M_MIP_struct *)malloc(sizeof(M_MIP_struct));
    if (result == NULL) {
        M_free(best_x);
        return NULL;
    }
    result->_matrix_x = best_x;
    result->_matrix_c = Matrix_copy(mat_c);
    result->_matrix_integer = (integer_mask == NULL) ? NULL : Matrix_copy(integer_mask);
    result->OPT_STATUS = status;
    result->values_opt = (best_x != NULL) ? best_value : 0;
    result->node_num = node_count;
    result->cut_num = cut_count;
    if (options.progress_interval > 0) {
        MIP_print_progress(node_count, final_open_nodes, cut_count, best_x != NULL,
                           (best_x != NULL) ? best_value : 0,
                           final_bound, options.objective_display_sign, "done");
    }
    return result;
}
